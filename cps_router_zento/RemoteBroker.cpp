#include "RemoteBroker.h"
#include "zbroker.h"
#include "InParticipant.h"




static int i_message_size = 0;


int getMessageSize()
{
	if (i_message_size == 0)
	{
		char *livemessage = "ALIVE:";
		int i_alive = strlen(livemessage);
		int i_prefix = sizeof(GuidPrefix_t);
		i_message_size = i_alive;
		i_message_size += i_prefix;
		i_message_size += sizeof(struct sockaddr_in);

	}
	return i_message_size;

}


static int i_notify_message_size = 0;

int getNotifyMessageSize()
{
	if (i_notify_message_size == 0)
	{
		char *notifymessage = "NOTIF:";

		int i_alive = strlen(notifymessage);
		int i_prefix = sizeof(GuidPrefix_t);
		int i_sockaddr_in = sizeof(struct sockaddr_in);

		int i_size = i_alive;
		i_size += i_prefix;
		i_size += i_sockaddr_in;   // bind - address
		i_size += i_sockaddr_in;	// mapp = remote - address
		i_notify_message_size = i_size;
	}


	return i_notify_message_size;
}

RemoteBroker::RemoteBroker(GuidPrefix_t _prefix, struct sockaddr_in addr)
{
	remote_prefix = _prefix;
	is_recv_notify_addr_diff = false;

	is_end = false;
	bind_port = -1;

	this->remote_addr = addr;

	port = 0;

	mapped_ip = inet_ntoa(addr.sin_addr);
	mapped_port = ntohs(addr.sin_port);

	printPrefix(_prefix);
	printf("\nNew RemoteBroker(Mapped) : %s:%d\n", mapped_ip.c_str(), mapped_port);
	isMapped = true;
	alive();
}

RemoteBroker::RemoteBroker(struct sockaddr_in addr)
{

	memset(&remote_prefix, NULL, sizeof(GuidPrefix_t));
	memset(&prefix, NULL, sizeof(GuidPrefix_t));
	
	is_recv_notify_addr_diff = false;
	is_end = false;
	bind_port = -1;

	
	this->remote_addr = addr;

	port = 0;

	mapped_ip = inet_ntoa(addr.sin_addr);
	mapped_port = ntohs(addr.sin_port);

	printf("New RemoteBroker(Mapped) : %s:%d\n", mapped_ip.c_str(), mapped_port);
	isMapped = true;
	alive();

}

RemoteBroker::RemoteBroker(std::string ip, int port)
{

	memset(&remote_prefix, NULL, sizeof(GuidPrefix_t));
	memset(&prefix, NULL, sizeof(GuidPrefix_t));

	is_recv_notify_addr_diff = false;
	is_end = false;

	mapped_port = 0;

	this->ip = ip;
	this->port = port;


	this->remote_addr.sin_family = AF_INET;
	this->remote_addr.sin_port = htons(port);
	this->remote_addr.sin_addr.s_addr = inet_addr(ip.c_str());

	printf("New RemoteBroker : %s, %d\n", ip.c_str(), port);
	
	isMapped = false;

	alive();
}

void RemoteBroker::setLocalAddr(struct sockaddr_in addr)
{
	ip = inet_ntoa(addr.sin_addr);
	port = ntohs(addr.sin_port);

	if (memcmp(&remote_addr, &addr, sizeof(struct sockaddr_in)) != 0)
	{
		is_recv_notify_addr_diff = true;
		printf("setLocalAddr : is_recv_notify_addr_diff is true.\r\n");
	}
}

RemoteBroker::~RemoteBroker()
{
	printf("Delete RemoteBroker\n");

	for (std::list<RemoteParticipant*>::iterator li = remoteParticipants.begin(); li != remoteParticipants.end(); li++)
	{
		RemoteParticipant *_remoteParticipant = *li;

		delete _remoteParticipant;

	}

	if (thread_id)
	{
		is_end = true;
		p_access->b_end = true;
		thread_id->join();
		access_delete(p_access);
		p_access = NULL;
	}

	if (p_accessout)
	{
		access_out_delete(p_accessout);
		p_accessout = NULL;
	}


	if (thread_id2)
	{
		is_end = true;
		thread_id2->join();
	}

	printf("Delete GOOD RemoteBroker\n");

}




void RemoteBroker::alive()
{
	char *psz_path;

	isrecvNACK = false;

	module_object_t *p_module = (module_object_t *)current_object(0);

	asprintf(&psz_path, "udp:@%s:%d", "239.255.0.1", 0);
	trace_msg((module_object_t*)p_module, TRACE_TRACE, "SEDP ACCESS : %s", psz_path);

	prefix = getDefaultPrefix();
	bind_ip = get_default_local_address();


	p_access = access_new((module_object_t*)p_module, "udp4", psz_path);
	bind_port = p_access->pf_getBind(p_access);

	//printf("New RemoteBroker(%d) : %s:%d\n", bind_port, ip.c_str(), port);
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(bind_port);
	bind_addr.sin_addr.s_addr = inet_addr(bind_ip.c_str());


	thread_id = new std::thread(Network, this);

	if (p_access)
	{
		char* p_path_temp;
		asprintf(&p_path_temp, "%s:%d", "239.255.0.1", 0);
		p_accessout = access_out_new((module_object_t*)p_module, "udp", p_path_temp, p_access->fd);
		FREE(p_path_temp);
	}


	FREE(psz_path);


	printf("RemoteBroker Bind(%s:%d)\n\n", bind_ip.c_str(), bind_port);


	thread_id2 = new std::thread(Live, this);

}



void RemoteBroker::Network(RemoteBroker *p_RemoteBroker)
{
	while (p_RemoteBroker->is_end == false)
	{
		p_RemoteBroker->getData();
	}
}

void RemoteBroker::Live(RemoteBroker *p_RemoteBroker)
{
	while (p_RemoteBroker->is_end == false)
	{
		Sleep(LIVESLEEPTIME);

		p_RemoteBroker->sendLive();
	}
}

void RemoteBroker::sendLive()
{

	char *livemessage = "ALIVE:";
	int i_alive = strlen(livemessage);
	int i_prefix = sizeof(GuidPrefix_t);

	int i_size = i_alive;
	i_size += i_prefix;
	i_size += sizeof(struct sockaddr_in);

	data_t *p_senddata = data_new(i_size);

	memcpy(p_senddata->p_data, livemessage, i_alive);
	memcpy(p_senddata->p_data + i_alive, &prefix, sizeof(prefix));
	memcpy(p_senddata->p_data + i_alive + i_prefix, &bind_addr, sizeof(bind_addr));


	if ((currenTime().sec - acked_time.sec) > 10)
	{
		if (acked_time.sec != 0)
		{
			if (isMapped)
			{
				is_end = true;
				printf("Mapped is (is_end = true)....\n");
				return;
			}
		}
		mapped_port = 0;

	}


	if (is_recv_notify_addr_diff)
	{
		bool is_send = false;
		if (mapped_port == 0)
		{
			is_send = true;
			p_senddata->i_port = port;
			p_senddata->p_address = (uint8_t *)strdup(ip.c_str());
			p_accessout->pf_write(p_accessout, p_senddata);
		}


		if (port == 0)
		{
			if (is_send)
			{
				p_senddata = data_duplicate(p_senddata);
			}

			p_senddata->i_port = mapped_port;
			p_senddata->p_address = (uint8_t *)strdup(mapped_ip.c_str());
			p_accessout->pf_write(p_accessout, p_senddata);
		}

	}
	else {

		if (mapped_port == 0)
		{
			p_senddata->i_port = port;
			p_senddata->p_address = (uint8_t *)strdup(ip.c_str());
		}
		else
		{
			p_senddata->i_port = mapped_port;
			p_senddata->p_address = (uint8_t *)strdup(mapped_ip.c_str());
		}
#if PRINT_NETWORK
		printf("(S) ALIVE(%d) %s:%d\n", bind_port, p_senddata->p_address, p_senddata->i_port);
#endif
		p_accessout->pf_write(p_accessout, p_senddata);
	}


	
}

void RemoteBroker::sendACK()
{
	char *livemessage = "ACK  :";
	int i_alive = strlen(livemessage);
	int i_prefix = sizeof(GuidPrefix_t);

	int i_size = i_alive;
	i_size += i_prefix;
	i_size += sizeof(struct sockaddr_in);

	data_t *p_senddata = data_new(i_size);

	memcpy(p_senddata->p_data, livemessage, i_alive);
	memcpy(p_senddata->p_data + i_alive, &prefix, sizeof(prefix));
	memcpy(p_senddata->p_data + i_alive + i_prefix, &bind_addr, sizeof(bind_addr));


	if (mapped_port == 0)
	{
		p_senddata->i_port = port;
		p_senddata->p_address = (uint8_t *)strdup(ip.c_str());
	}
	else
	{
		p_senddata->i_port = mapped_port;
		p_senddata->p_address = (uint8_t *)strdup(mapped_ip.c_str());
	}
#if PRINT_NETWORK
	printf("(S) ACK %s:%d\n", p_senddata->p_address, p_senddata->i_port);
#endif
	p_accessout->pf_write(p_accessout, p_senddata);
}


void RemoteBroker::getData()
{
	struct sockaddr client;
	int l = sizeof(struct sockaddr);
	RemoteBroker *found_remoteBroker = NULL;

	data_t*	data = p_access->pf_dataFrom(p_access, &client, &l);

	if (data == NULL)  return;

	if (data->i_size == getMessageSize()
		|| data->i_size == getNotifyMessageSize())
	{
		if (memcmp(data->p_data, "ALIVE:", 6) == 0)
		{
			GuidPrefix_t		remote_prefix;
			struct sockaddr_in	remote_addr;

			remote_prefix = *((GuidPrefix_t *)&data->p_data[6]);
			remote_addr = *((struct sockaddr_in *)&data->p_data[18]);

#if PRINT_NETWORK
			printf("(R) ALIVE \n");
#endif
			if (memcmp(&remote_addr, &client, sizeof(client)) != 0)
			{
	
				if (memcmp(&remote_prefix, &this->remote_prefix, sizeof(GuidPrefix_t)) != 0)
				{
					printf("(R) 1 PREFIX WRONG. \n");
				}
				else {
					if(memcmp(&this->remote_addr, &client, sizeof(client)) != 0)
					{
						printf("Change address mapped.");
						changeAddr(remote_prefix, *((struct sockaddr_in*)&client));
					}

					sendACK();
				}
			}else if(memcmp(&remote_prefix, &this->remote_prefix, sizeof(GuidPrefix_t)) != 0)
			{
				printf("(R) 2 PREFIX WRONG. \n");
				getBroker()->getRemoteBrokerManager()->sendReNACK(client, remote_prefix);
			}
			else {
				sendACK();
			}
		}else if (memcmp(data->p_data, "NACK_:", 6) == 0)
		{
			GuidPrefix_t		remote_prefix;
			struct sockaddr_in	remote_addr;

			remote_prefix = *((GuidPrefix_t *)&data->p_data[6]);
			remote_addr = *((struct sockaddr_in *)&data->p_data[18]);

			
#if PRINT_NETWORK
			printf("(R) NACK_ \n");
#endif

			isrecvNACK = true;
			
			changeAddr(remote_prefix, remote_addr);
			acked_time = currenTime();
			sendACK();
		}
		else if (memcmp(data->p_data, "ACK  :", 6) == 0)
		{
#if PRINT_NETWORK
			printf("(R) ACK \n");
#endif
			acked_time = currenTime();
		}
		else if (memcmp(data->p_data, "NOTIF:", 6) == 0)
		{

			printf("(R:2) NOTIF \n");
			receiveNotify(data->p_data);
		}
	}
	else {


		if (memcmp(data->p_data, "TOPAR:", 6) == 0)
		{
			acked_time = currenTime();

			//Remote Broker로 부터 데이터가 전송됨...
			GuidPrefix_t in_guid_prefix = *((GuidPrefix_t*)&data->p_data[6]);

			sendDataToInParticipant(in_guid_prefix, data->p_data + 6 + sizeof(GuidPrefix_t), data->i_size - 6 - sizeof(GuidPrefix_t));
		}
		else {
			bool isfromremoteBroker = false;

			int client_port = ntohs(((struct sockaddr_in*)&client)->sin_port);
			char *client_address = (char *)inet_ntoa(((struct sockaddr_in*)&client)->sin_addr);


			char *remote_address = inet_ntoa(this->remote_addr.sin_addr);
			int remote_port = ntohs(this->remote_addr.sin_port);

			if (memcmp(&this->remote_addr, &client, sizeof(client)) == 0)
			{
				isfromremoteBroker = true;
				acked_time = currenTime();
			}

			if (isfromremoteBroker == false)
			{
				if (client_port == remote_port && strcmp(client_address, remote_address) == 0)
				{
					isfromremoteBroker = true;
				}
			}

			parse(data, isfromremoteBroker);

		}
	}

	data_release(data);
}


void RemoteBroker::changeAddr(GuidPrefix_t prefix, struct sockaddr_in addr)
{

	char *mapped_ip1 = inet_ntoa(remote_addr.sin_addr);
	int mapped_port1 = ntohs(remote_addr.sin_port);

	char *mapped_ip2 = inet_ntoa(addr.sin_addr);
	int mapped_port2 = ntohs(addr.sin_port);

	printf("Change ADDRESS From %s:%d To %s:%d\n", mapped_ip1, mapped_port1, mapped_ip2, mapped_port2);

	this->remote_prefix = prefix;
	this->remote_addr = addr;


	

	mapped_ip = inet_ntoa(addr.sin_addr);
	mapped_port = ntohs(addr.sin_port);
}


void RemoteBroker::parse(data_t	*data, bool isfromremoteBroker)
{
	Header		a_header;
	int32_t		ret;
	int32_t		i_pos = 0;
	uint8_t		E; // EndiannessFlag
	int32_t		i_remind = data->i_size;
	SubmessageHeader a_submessageHeader;
	uint8_t*	p_data = data->p_data;
	bool isPDP = false;
	RemoteParticipant *p_RemoteParticipant;



	if (ret = rtps_Header((char*)p_data, data->i_size, &a_header))
	{
		return;
	}

	a_header = *(Header*)p_data;

	if (memcmp(&getDefaultPrefix(), &a_header.guid_prefix, sizeof(GuidPrefix_t)) == 0)
	{
		return;
	}

	i_pos += sizeof(Header);
	i_remind -= sizeof(Header);



	if (isfromremoteBroker)
	{

	
		p_RemoteParticipant = getRemoteParticipant(a_header.guid_prefix);


		while (i_remind > 0) {
			a_submessageHeader = *(SubmessageHeader*)&p_data[i_pos];

			E = a_submessageHeader.flags & 0x01;	// Endianness bit


			i_pos += sizeof(SubmessageHeader);
			i_remind -= sizeof(SubmessageHeader);

			switch (a_submessageHeader.submessage_id)
			{
			case PAD:
				break;
			case GAP:
				break;
			case ACKNACK:
				//printf("ACKNACK 2\n");
				break;
			case HEARTBEAT:
				//printf("HEARTBEAT 2\n");
				break;
			case INFO_TS:
				break;
			case INFO_SRC:
				break;
			case INFO_REPLY_IP4:
				break;
			case INFO_DST:
				break;
			case INFO_REPLY:
				break;
			case NACK_FRAG:
				break;
			case HEARTBEAT_FRAG:
				break;
			case DATA:
				isPDP = p_RemoteParticipant->rtps_data_parse(p_data, data->i_size, &p_data[i_pos], a_submessageHeader.submessage_length, a_submessageHeader.flags);
				break;
			case DATA_FRAG:
				break;
			default:
				break;
			}

			if (isPDP) break;

			i_pos += a_submessageHeader.submessage_length;
			i_remind -= a_submessageHeader.submessage_length;
		}

		if (isPDP)
		{
			p_RemoteParticipant->sendPDP(p_data, data->i_size);
		}
		else {


			//누구에게 전달을 해야하는 문제가 존재.
			for (std::list<InParticipant*>::iterator li = getBroker()->inParticipants.begin(); li != getBroker()->inParticipants.end(); li++)
			{
				InParticipant *_InParticipant = *li;


				//if (_InParticipant->b_end)
				//{
				//	/*getBroker()->inParticipants.remove(_InParticipant);
				//	delete _InParticipant;

				//	if (getBroker()->inParticipants.size() == 0) break;
				//	li = getBroker()->inParticipants.begin();*/

				//	continue;
				//}

				if (_InParticipant->b_using == false) continue;

				data_t *p_senddata = data_new(data->i_size);
				memcpy(p_senddata->p_data, p_data, data->i_size);


				if (_InParticipant->DEFAULT_UNICAST_LOCATOR.kind == 1)
				{
					p_senddata->i_port = _InParticipant->DEFAULT_UNICAST_LOCATOR.port;

					char *ip = NULL;
					asprintf(&ip, "%d.%d.%d.%d", _InParticipant->DEFAULT_UNICAST_LOCATOR.address[12], _InParticipant->DEFAULT_UNICAST_LOCATOR.address[13], _InParticipant->DEFAULT_UNICAST_LOCATOR.address[14], _InParticipant->DEFAULT_UNICAST_LOCATOR.address[15]);

					p_senddata->p_address = (uint8_t *)ip;
					send(p_senddata);
				}
			
			}
		}
	}
	else {

		
	}
	
	if (isfromremoteBroker == false)
	{
		//리모트 라우터로 보내기...
		data_t *p_senddata = data_new(data->i_size);
		memcpy(p_senddata->p_data, p_data, data->i_size);
		sendToRemoteBroker(p_senddata);
	}

}


RemoteParticipant *RemoteBroker::getRemoteParticipant(GuidPrefix_t a_guid_prefix)
{

	RemoteParticipant *found_RemoteParticipant = NULL;

	for (std::list<RemoteParticipant*>::iterator li = remoteParticipants.begin(); li != remoteParticipants.end(); li++)
	{
		RemoteParticipant *_remoteParticipant = *li;


		if ((currenTime().sec - _remoteParticipant->UpdateTime().sec) > 100)
		{

			if (_remoteParticipant->b_using) _remoteParticipant->setUsing(false);
			/*remoteParticipants.remove(_remoteParticipant);

			delete _remoteParticipant;

			if (remoteParticipants.size() == 0) break;
			li = remoteParticipants.begin();
			continue;*/

			continue;
		}

		if (memcmp(&a_guid_prefix, &_remoteParticipant->guid_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			found_RemoteParticipant = _remoteParticipant;
			break;
		}
	}

	if (found_RemoteParticipant == NULL)
	{

		for (std::list<RemoteParticipant*>::iterator li = remoteParticipants.begin(); li != remoteParticipants.end(); li++)
		{
			RemoteParticipant *_remoteParticipant = *li;

			if (_remoteParticipant->b_using == false)
			{
				found_RemoteParticipant = _remoteParticipant;
				break;
			}
		}

		if (found_RemoteParticipant == NULL)
		{
			found_RemoteParticipant = new RemoteParticipant(this, a_guid_prefix);
			remoteParticipants.push_back(found_RemoteParticipant);

		}
		else {
			found_RemoteParticipant->change_prefix(a_guid_prefix);
			found_RemoteParticipant->setUsing(true);

		}

		printf(" : %d\n", remoteParticipants.size());

	}

	found_RemoteParticipant->Update();

	return found_RemoteParticipant;
}


void RemoteBroker::send(data_t *p_data)
{
	p_accessout->pf_write(p_accessout, p_data);
}



void RemoteBroker::sendToRemoteBroker(data_t *p_data)
{
	//리모트 라우터로 보내기...

	if (mapped_port == 0)
	{
		p_data->i_port = port;
		p_data->p_address = (uint8_t *)strdup(ip.c_str());
	}
	else
	{
		p_data->i_port = mapped_port;
		p_data->p_address = (uint8_t *)strdup(mapped_ip.c_str());
	}

	send(p_data);
}






void RemoteBroker::sendDataToInParticipant(GuidPrefix_t in_prefix, uint8_t *p_org_data, int i_org_size)
{

	bool is_send = false;
	for (std::list<InParticipant*>::iterator li = getBroker()->inParticipants.begin(); li != getBroker()->inParticipants.end(); li++)
	{
		InParticipant *_InParticipant = *li;


		if (_InParticipant->b_using == false)
		{
			
			continue;
		}


		if (memcmp(&in_prefix, &_InParticipant->guid_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			data_t *p_senddata = data_new(i_org_size);
			memcpy(p_senddata->p_data, p_org_data, i_org_size);


			if (_InParticipant->DEFAULT_UNICAST_LOCATOR.kind == 1)
			{
				p_senddata->i_port = _InParticipant->DEFAULT_UNICAST_LOCATOR.port;

				char *ip = NULL;
				asprintf(&ip, "%d.%d.%d.%d", _InParticipant->DEFAULT_UNICAST_LOCATOR.address[12], _InParticipant->DEFAULT_UNICAST_LOCATOR.address[13], _InParticipant->DEFAULT_UNICAST_LOCATOR.address[14], _InParticipant->DEFAULT_UNICAST_LOCATOR.address[15]);

				p_senddata->p_address = (uint8_t *)ip;
				send(p_senddata);
			}

			is_send = true;
			break;

		}
	}

	if (is_send == false)
	{

		printf("can't find a inParticipant\n");
	}
}



void RemoteBroker::print()
{
	printPrefix(remote_prefix);
	printf(" IP(%s:%d), Map(%s:%d), Bind(%s,%d)\r\n", ip.c_str(), port, mapped_ip.c_str(), mapped_port, bind_ip.c_str(), bind_port);
}