#include "RemoteRouterManager.h"



#define REMOTE_ROUTER_PORT 8301








RemoteRouterManager::RemoteRouterManager()
{

	prefix = getDefaultPrefix();
	is_end = false;

	char *psz_path;

	module_object_t *p_module = (module_object_t *)current_object(0);

	asprintf(&psz_path, "udp:@%s:%d", "239.255.0.1", REMOTE_ROUTER_PORT);
	trace_msg((module_object_t*)p_module, TRACE_TRACE, "SEDP ACCESS : %s", psz_path);

	p_access = access_new((module_object_t*)p_module, "udp4", psz_path);


	int bind_port = p_access->pf_getBind(p_access);

	printPrefix(this->prefix);
	printf("\r\nRemoteRouterManager BIND(%d)\n", bind_port);


	thread_id = new std::thread(Network, this);

	if (p_access)
	{
		char* p_path_temp;
		asprintf(&p_path_temp, "%s:%d", "239.255.0.1", REMOTE_ROUTER_PORT);
		p_accessout = access_out_new((module_object_t*)p_module, "udp", p_path_temp, p_access->fd);
		FREE(p_path_temp);
	}


	FREE(psz_path);



	char *p_routers = NULL;

	p_module = (module_object_t *)current_object(0);

	p_routers = var_create_get_string((module_object_t*)p_module, "routers");

	if (p_routers)
	{
		while (p_routers) {
			char *psz = strchr(p_routers, ',');
			char *psz_realhost;
			int i_realport;

			

			if (psz == NULL)
			{
				if (p_routers)
				{
					char *psz2;

					psz2 = strchr(p_routers, ':');

					if (psz2) {
						*psz2++ = '\0';
					}

					psz_realhost = p_routers;
					i_realport = (psz2 != NULL) ? atoi(psz2) : 8301;

					RemoteRouter* p_remoteRouter = new RemoteRouter(psz_realhost, i_realport);
					remoteRouters.push_back(p_remoteRouter);
					
				}
				break;
			}


			if (psz) {
				*psz++ = '\0';
			}

			if (p_routers)
			{
				char *psz2;

				psz2 = strchr(p_routers, ':');

				if (psz2) {
					*psz2++ = '\0';
				}

				psz_realhost = p_routers;
				i_realport = (psz2 != NULL) ? atoi(psz2) : 8301;

				RemoteRouter* p_remoteRouter = new RemoteRouter(psz_realhost, i_realport);
				remoteRouters.push_back(p_remoteRouter);

				/*sRouter srouter(psz_realhost, i_realport);

				sRouters.push_back(srouter);*/
			}

			p_routers = psz;

		}
	}

	thread_id2 = NULL;

	//thread_id2 = new std::thread(Live, this);
}


RemoteRouterManager::~RemoteRouterManager()
{
	if (thread_id)
	{
		is_end = true;
		p_access->b_end = true;
		thread_id->join();
		access_delete(p_access);
	}

	if (p_accessout)
	{
		access_out_delete(p_accessout);
	}


	if (thread_id2)
	{
		is_end = true;
		thread_id2->join();
	}
}


void RemoteRouterManager::Live(RemoteRouterManager *p_RemoteRouterManager)
{
	while (p_RemoteRouterManager->is_end == false)
	{
		Sleep(LIVESLEEPTIME);

		for (std::list<sRouter>::iterator li = p_RemoteRouterManager->sRouters.begin(); li != p_RemoteRouterManager->sRouters.end(); li++)
		{
			sRouter a_sRouter = *li;
			p_RemoteRouterManager->sendLive(a_sRouter);
		}
	}
}

void RemoteRouterManager::Network(RemoteRouterManager *p_RemoteRouterManager)
{
	while (p_RemoteRouterManager->is_end == false)
	{
		p_RemoteRouterManager->getData();
	}
}




void RemoteRouterManager::getData()
{

	struct sockaddr client;
	int l = sizeof(struct sockaddr);
	RemoteRouter *found_remoteRouter = NULL;

	data_t*	data = p_access->pf_dataFrom(p_access, &client, &l);

	if (data == NULL) return;
	bool is_found = false;
	bool is_ALIVE_OR_NACK = true;

	GuidPrefix_t		remote_prefix;
	struct sockaddr_in	remote_addr;

	if (data->i_size != getMessageSize()
		&& data->i_size != getNotifyMessageSize())
	{
		data_release(data);
		return;
	}

	if (memcmp(data->p_data, "ALIVE:", 6) == 0)
	{
		remote_prefix = *((GuidPrefix_t *)&data->p_data[6]);
		remote_addr = *((struct sockaddr_in *)&data->p_data[18]);
#if PRINT_NETWORK
		printf("(R) ALIVE \n");
#endif
	}else if (memcmp(data->p_data, "NACK_:", 6) == 0)
	{
		remote_prefix = *((GuidPrefix_t *)&data->p_data[6]);
		remote_addr = *((struct sockaddr_in *)&data->p_data[18]);
#if PRINT_NETWORK
		printf("(R) NACK_ \n");
#endif
	}
	else if (memcmp(data->p_data, "NOTIF:", 6) == 0) {
		///

		printf("(R:1) NOTIF\n");
	}
	else {
//#if PRINT_NETWORK
		printf("NOT (R) %s \n", data->p_data);
//#endif
		is_ALIVE_OR_NACK = false;
		data_release(data);
		return;
	}


	if (memcmp(&remote_prefix, &this->prefix, sizeof(GuidPrefix_t)) == 0)
	{
		//printf("Manager Prefix equal prefix...\n");
		return;
	}

	for (std::list<RemoteRouter*>::iterator li = remoteRouters.begin(); li != remoteRouters.end(); li++)
	{
		RemoteRouter *_remoteRouter = *li;

		if (_remoteRouter->is_end)
		{
			remoteRouters.remove(_remoteRouter);
			delete _remoteRouter;

			if (remoteRouters.size() == 0) break;
			li = remoteRouters.begin();
			continue;
		}

		if (memcmp(&remote_prefix, &_remoteRouter->remote_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			is_found = true;
			found_remoteRouter = _remoteRouter;
#if PRINT_NETWORK
			printf("found same prefix...\n");
#endif
			if (memcmp(&remote_addr, &_remoteRouter->remote_addr, sizeof(client)) == 0)
			{
				is_found = true;
				found_remoteRouter = _remoteRouter;
				printf("1 found same remote_addr...\n");
			}

			break;
		}

		if (memcmp(&remote_addr, &_remoteRouter->remote_addr, sizeof(client)) == 0)
		{
			is_found = true;
			found_remoteRouter = _remoteRouter;
			printf("2 found same remote_addr...\n");
			break;
		}
	}

	 
	if (is_found == false && is_ALIVE_OR_NACK)
	{

		printf("\r\n(%d)Add RemoteRouter\n", remoteRouters.size());
		printPrefix(remote_prefix);
		printf("\n");
		found_remoteRouter = new RemoteRouter(remote_prefix, remote_addr);

		
		
		remoteRouters.push_back(found_remoteRouter);
		sendNACK(client, found_remoteRouter);

		//// Notify to Other Remote Router about new Router..
		//notifyNewRemoteRouterToOtherRemoteRouter(remote_prefix, remote_addr, client);
		//// Notify remote Routers's information to New Router.. 
		//notifyOtherRemoteRouterToNewRemoteRouter(remote_prefix, remote_addr, client);


	}
	else if (is_found == true && is_ALIVE_OR_NACK) {

	}

	data_release(data);
}


void RemoteRouterManager::sendLive(sRouter a_sRouter)
{

	/*char *livemessage = "ALIVE:";
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

	printf("(S) ALIVE %s:%d\n", p_senddata->p_address, p_senddata->i_port);
	p_accessout->pf_write(p_accessout, p_senddata);*/
}

void RemoteRouterManager::sendNACK(struct sockaddr client, RemoteRouter *p_RemoteRouter)
{
	char *livemessage = "NACK_:";
	int i_alive = strlen(livemessage);
	int i_prefix = sizeof(GuidPrefix_t);

	int i_size = i_alive;
	i_size += i_prefix;
	i_size += sizeof(struct sockaddr_in);

	data_t *p_senddata = data_new(i_size);

	memcpy(p_senddata->p_data, livemessage, i_alive);
memcpy(p_senddata->p_data + i_alive, &p_RemoteRouter->prefix, sizeof(p_RemoteRouter->prefix));
memcpy(p_senddata->p_data + i_alive + i_prefix, &p_RemoteRouter->bind_addr, sizeof(p_RemoteRouter->bind_addr));


p_senddata->i_port = ntohs(((struct sockaddr_in*)&client)->sin_port);
p_senddata->p_address = (uint8_t *)strdup(inet_ntoa(((struct sockaddr_in*)&client)->sin_addr));

#if PRINT_NETWORK
char *mapped_ip = inet_ntoa(p_RemoteRouter->bind_addr.sin_addr);
int mapped_port = ntohs(p_RemoteRouter->bind_addr.sin_port);

printf("(S) NACK_ %s:%d , %s:%d\n", p_senddata->p_address, p_senddata->i_port, mapped_ip, mapped_port);
#endif
p_accessout->pf_write(p_accessout, p_senddata);


}


void RemoteRouterManager::sendPDP(uint8_t *p_data, int i_size)
{
	for (std::list<RemoteRouter*>::iterator li = remoteRouters.begin(); li != remoteRouters.end(); li++)
	{
		RemoteRouter *_remoteRouter = *li;

		if(_remoteRouter->mapped_port == 0) continue;

		data_t *p_senddata = data_new(i_size);

		memcpy(p_senddata->p_data, p_data, i_size);

		if (_remoteRouter->mapped_port == 0)
		{
			p_senddata->i_port = _remoteRouter->port;
			p_senddata->p_address = (uint8_t *)strdup(_remoteRouter->ip.c_str());
		}
		else
		{
			p_senddata->i_port = _remoteRouter->mapped_port;
			p_senddata->p_address = (uint8_t *)strdup(_remoteRouter->mapped_ip.c_str());
		}


		_remoteRouter->send(p_senddata);

	}

}


bool RemoteRouterManager::hasBindAddr(struct sockaddr client)
{

	int remote_port = ntohs(((struct sockaddr_in*)&client)->sin_port);
    char *remote_address = (char *)inet_ntoa(((struct sockaddr_in*)&client)->sin_addr);

	for (std::list<RemoteRouter*>::iterator li = remoteRouters.begin(); li != remoteRouters.end(); li++)
	{
		RemoteRouter *_remoteRouter = *li;

		if (strcmp(remote_address, _remoteRouter->bind_ip.c_str()) == 0 && 
			remote_port ==  _remoteRouter->bind_port)
		{
			return true;
		}

	}

	return false;
}

bool RemoteRouterManager::hasClientAddr(struct sockaddr client)
{
	for (std::list<RemoteRouter*>::iterator li = remoteRouters.begin(); li != remoteRouters.end(); li++)
	{
		RemoteRouter *_remoteRouter = *li;

		if (memcmp(&client, &_remoteRouter->bind_addr, sizeof(client)) == 0)
		{
			return true;
		}

	}

	return false;
}


static GuidPrefix notPrefix;
static bool is_init = false;



void RemoteRouterManager::notifyNewRemoteRouterToOtherRemoteRouter(GuidPrefix_t new_remote_prefix, struct sockaddr_in new_remote_addr, struct sockaddr new_mapped_remote_addr)
{
	if (is_init == false)
	{
		memset(&notPrefix, NULL, sizeof(GuidPrefix));
		is_init = true;
	}
	printf("(S) NOTIF in notifyNewRemoteRouterToOtherRemoteRouter \r\n");
	char *notifymessage = "NOTIF:";

	int i_alive = strlen(notifymessage);
	int i_prefix = sizeof(GuidPrefix_t);
	int i_sockaddr_in = sizeof(struct sockaddr_in);

	int i_size = i_alive;
	i_size += i_prefix;
	i_size += i_sockaddr_in;   // bind - address
	i_size += i_sockaddr_in;	// mapp = remote - address


	int i = 0;



	for (std::list<RemoteRouter*>::iterator li = remoteRouters.begin(); li != remoteRouters.end(); li++)
	{
		RemoteRouter *_remoteRouter = *li;


		if (memcmp(&notPrefix, &_remoteRouter->remote_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			continue;
		}

		if (memcmp(&new_remote_prefix, &_remoteRouter->remote_prefix, sizeof(GuidPrefix_t)) != 0)
		{
			data_t *p_senddata = data_new(i_size);

			memcpy(p_senddata->p_data, notifymessage, i_alive);
			memcpy(p_senddata->p_data + i_alive, &new_remote_prefix, sizeof(new_remote_prefix));
			memcpy(p_senddata->p_data + i_alive + i_prefix, &new_remote_addr, sizeof(new_remote_addr));
			memcpy(p_senddata->p_data + i_alive + i_prefix+ i_sockaddr_in, &new_mapped_remote_addr, sizeof(new_mapped_remote_addr));



			p_senddata->i_port = ntohs(((struct sockaddr_in*)&_remoteRouter->remote_addr)->sin_port);
			p_senddata->p_address = (uint8_t *)strdup(inet_ntoa(((struct sockaddr_in*)&_remoteRouter->remote_addr)->sin_addr));


			printf("(%d) ", i);
			printPrefix(new_remote_prefix);
			
			printf(" to ");
			printPrefix(_remoteRouter->remote_prefix);

			printf("\r\n");
			
			i++;
			p_accessout->pf_write(p_accessout, p_senddata);
		}
	}
}

void RemoteRouterManager::notifyOtherRemoteRouterToNewRemoteRouter(GuidPrefix_t	new_remote_prefix, struct sockaddr_in new_remote_addr, struct sockaddr new_mapped_remote_addr)
{
	char *notifymessage = "NOTIF:";

	int i_alive = strlen(notifymessage);
	int i_prefix = sizeof(GuidPrefix_t);
	int i_sockaddr_in = sizeof(struct sockaddr_in);

	int i_size = i_alive;
	i_size += i_prefix;
	i_size += i_sockaddr_in;   // bind - address
	i_size += i_sockaddr_in;	// mapp = remote - address


	int i = 0;
	printf("(S) NOTIF in notifyOtherRemoteRouterToNewRemoteRouter \r\n");

	for (std::list<RemoteRouter*>::iterator li = remoteRouters.begin(); li != remoteRouters.end(); li++)
	{
		RemoteRouter *_remoteRouter = *li;

		if (memcmp(&notPrefix, &_remoteRouter->remote_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			continue;
		}

		if (memcmp(&new_remote_prefix, &_remoteRouter->remote_prefix, sizeof(GuidPrefix_t)) != 0)
		{
			data_t *p_senddata = data_new(i_size);

			memcpy(p_senddata->p_data, notifymessage, i_alive);
			memcpy(p_senddata->p_data + i_alive, &_remoteRouter->remote_prefix, sizeof(_remoteRouter->remote_prefix));
			memcpy(p_senddata->p_data + i_alive + i_prefix, &_remoteRouter->remote_addr, sizeof(_remoteRouter->remote_addr));
			memcpy(p_senddata->p_data + i_alive + i_prefix + i_sockaddr_in, &_remoteRouter->remote_addr, sizeof(_remoteRouter->remote_addr));

			p_senddata->i_port = ntohs(((struct sockaddr_in*)&new_mapped_remote_addr)->sin_port);
			p_senddata->p_address = (uint8_t *)strdup(inet_ntoa(((struct sockaddr_in*)&new_mapped_remote_addr)->sin_addr));


			printf("(%d) ", i);
			printPrefix(_remoteRouter->remote_prefix);
			printf(" to ");
			printPrefix(new_remote_prefix);
			printf("\r\n");

			i++;
			
			p_accessout->pf_write(p_accessout, p_senddata);

		}

	}
}


void RemoteRouterManager::procedureNotify(uint8_t *p_data)
{
	GuidPrefix_t		remote_prefix = *((GuidPrefix_t *)&p_data[6]);
	struct sockaddr_in	remote_addr = *((struct sockaddr_in *)&p_data[6 + sizeof(GuidPrefix_t)]);
	struct sockaddr_in	mapped_addr = *((struct sockaddr_in *)&p_data[6 + sizeof(GuidPrefix_t) + sizeof(struct sockaddr_in)]);
	
	bool is_found = false;
	RemoteRouter *found_remoteRouter = NULL;

	for (std::list<RemoteRouter*>::iterator li = remoteRouters.begin(); li != remoteRouters.end(); li++)
	{
		RemoteRouter *_remoteRouter = *li;

		if (memcmp(&notPrefix, &_remoteRouter->remote_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			continue;
		}

		if (memcmp(&remote_prefix, &_remoteRouter->remote_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			is_found = true;
			break;
		}
	}


	if (is_found == false)
	{

		found_remoteRouter = new RemoteRouter(remote_prefix, mapped_addr);
		found_remoteRouter->setLocalAddr(remote_addr);

		printf("Add Notify RemoteRouter ");
		printPrefix(remote_prefix);
		printf("\r\n");

		remoteRouters.push_back(found_remoteRouter);
	}

}


void RemoteRouterManager::printRemoteRouters()
{
	int i = 0;

	printf("---------------------------------------------------------\r\n\r\n\r\n");

	for (std::list<RemoteRouter*>::iterator li = remoteRouters.begin(); li != remoteRouters.end(); li++)
	{
		RemoteRouter *_remoteRouter = *li;

		
		_remoteRouter->print();

		i++;
	}

	printf("\r\n\r\n---------------------------------------------------------\r\n\r\n");

}

void RemoteRouterManager::sendReNACK(struct sockaddr client, GuidPrefix_t remote_prefix)
{

	for (std::list<RemoteRouter*>::iterator li = remoteRouters.begin(); li != remoteRouters.end(); li++)
	{
		RemoteRouter *_remoteRouter = *li;
		if (memcmp(&remote_prefix, &_remoteRouter->remote_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			

			char *livemessage = "NACK_:";
			int i_alive = strlen(livemessage);
			int i_prefix = sizeof(GuidPrefix_t);

			int i_size = i_alive;
			i_size += i_prefix;
			i_size += sizeof(struct sockaddr_in);

			data_t *p_senddata = data_new(i_size);

			memcpy(p_senddata->p_data, livemessage, i_alive);
			memcpy(p_senddata->p_data + i_alive, &_remoteRouter->prefix, sizeof(_remoteRouter->prefix));
			memcpy(p_senddata->p_data + i_alive + i_prefix, &_remoteRouter->bind_addr, sizeof(_remoteRouter->bind_addr));


			p_senddata->i_port = ntohs(((struct sockaddr_in*)&client)->sin_port);
			p_senddata->p_address = (uint8_t *)strdup(inet_ntoa(((struct sockaddr_in*)&client)->sin_addr));

#if PRINT_NETWORK
			char *mapped_ip = inet_ntoa(p_RemoteRouter->bind_addr.sin_addr);
			int mapped_port = ntohs(p_RemoteRouter->bind_addr.sin_port);

			printf("(S) NACK_ %s:%d , %s:%d\n", p_senddata->p_address, p_senddata->i_port, mapped_ip, mapped_port);
#endif

			printf("send renack\r\n");
			p_accessout->pf_write(p_accessout, p_senddata);




			break;
		}
	}
}

void receiveNotify(uint8_t* p_data)
{
	
	if (getRouter() && getRouter()->getRemoteRouterManager())
	{
		getRouter()->getRemoteRouterManager()->procedureNotify(p_data);

	}
}


