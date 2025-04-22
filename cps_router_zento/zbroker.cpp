#include "zbroker.h"



#define CallFunction(name)name##__module_entry
#define CallFunctionDef(name)int name##__module_entry(moduleload_t * p_module);

#define LoadMediaModule(name) \
		p_moduleload = loadModule(OBJECT(p_module),CallFunction(name));\
		if(p_moduleload){ \
			object_attach( OBJECT(p_moduleload), OBJECT(p_module->p_libmodule->p_module_bank) );\
		}


extern "C" {

CallFunctionDef(main);
CallFunctionDef(IPv4);
CallFunctionDef(access_udp);
CallFunctionDef(access_udp_out);

void zento_moduleinit(moduleload_t* p_moduleload);

}




zento_Broker::zento_Broker(void)
{

	
	parser = new rtps_parser(this);

	thread_id = NULL;
	is_end = false;

	//modules_register();

	module_id = moduleCreate();
	p_module = current_object( module_id );
	
	moduleInitBank(OBJECT(p_module));

	LoadMediaModule(main);

	zento_moduleinit(p_moduleload);

	LoadMediaModule(IPv4);
	LoadMediaModule(access_udp);
	LoadMediaModule(access_udp_out);

	configuration(p_module);

	p_module = current_object( module_id );


	p_RemoteBrokerManager = new RemoteBrokerManager();

}


zento_Broker::~zento_Broker(void)
{
	if(thread_id)
	{
		is_end = true;
		p_access->b_end = true;
		thread_id->join();
		access_delete(p_access);
	}

	if(p_accessout)
	{
		access_out_delete(p_accessout);
	}

	modules_unregister(module_id);

	delete p_RemoteBrokerManager;
}


int zento_Broker::open(int port)
{
	this->port = port;

	//modules_register();

	asprintf(&psz_path, "udp:@%s:%d", "239.255.0.1", port);
	trace_msg((module_object_t*)p_module,TRACE_TRACE,"SPDP ACCESS : %s", psz_path);

	p_access = access_new((module_object_t*)p_module, "udp4", psz_path);

	thread_id = new std::thread(Network, this);


	int bind_port = p_access->pf_getBind(p_access);

	printf("zento BIND(%d)\n", bind_port);

	if (p_access)
	{
		char* p_path_temp;
		asprintf(&p_path_temp,"%s:%d","239.255.0.1", port);
		p_accessout = access_out_new((module_object_t*)p_module, "udp", p_path_temp, p_access->fd);
		FREE(p_path_temp);
	}

	return 0;
}



void zento_Broker::Network(zento_Broker *p_zento_Broker)
{

	while(p_zento_Broker->is_end == false)
	{
		p_zento_Broker->getData();
	}
}


void zento_Broker::getData()
{
	struct sockaddr client;
	int l = sizeof(struct sockaddr);
	data_t*	data = p_access->pf_dataFrom(p_access, &client, &l);



	
	bool hasclientaddr = p_RemoteBrokerManager->hasBindAddr(client);

	if(hasclientaddr == false)
	{

	
		hasclientaddr = p_RemoteBrokerManager->hasClientAddr(client);

		if (hasclientaddr == false)
		{
			//printf("data : %d\n", data->i_size);
			if(data) parser->parse(data);
		}
		else {

			//printf("data : %d\n", data->i_size);
		}
	}


	if (data) data_release(data);
}


InParticipant *zento_Broker::getInParticipant(GuidPrefix_t a_guid_prefix)
{

	InParticipant *found_InParticipant = NULL;

	for (std::list<InParticipant*>::iterator li = inParticipants.begin(); li != inParticipants.end(); li++)
	{
		InParticipant *_InParticipant = *li;


		if ((currenTime().sec - _InParticipant->UpdateTime().sec) > 100)
		{
		
			if(_InParticipant->b_using) _InParticipant->setUsing(false);

			continue;
		}

		if (memcmp(&a_guid_prefix, &_InParticipant->guid_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			found_InParticipant = _InParticipant;
			break;
		}
	}

	if (found_InParticipant == NULL)
	{

		
		for (std::list<InParticipant*>::iterator li = inParticipants.begin(); li != inParticipants.end(); li++)
		{
			InParticipant *_InParticipant = *li;


			if (_InParticipant->b_using == false)
			{
				found_InParticipant = _InParticipant;
				break;
			}
		}

		if (found_InParticipant == NULL)
		{

			found_InParticipant = new InParticipant(a_guid_prefix);
			inParticipants.push_back(found_InParticipant);

		}
		else {

			found_InParticipant->change_prefix(a_guid_prefix);
			found_InParticipant->setUsing(true);
		}


		printf(" : %d\n", inParticipants.size());

	}


	found_InParticipant->Update();

	return found_InParticipant;
}


void zento_Broker::send_PDP(uint8_t *p_data, int i_size)
{

	/*data_t *p_senddata = data_new(i_size);


	memcpy(p_senddata->p_data, p_data, i_size);

	p_senddata->i_port = 7400;
	p_senddata->p_address = (uint8_t *)strdup("239.255.0.1");

	

	p_accessout->pf_write(p_accessout, p_senddata);*/
	p_RemoteBrokerManager->sendPDP(p_data, i_size);
}



static zento_Broker *r = NULL;

zento_Broker *getBroker()
{

	return r;
}


void printPrefix(GuidPrefix_t prefix)
{

	for (int i = 0; i < 12; i++)
	{
		printf("%2X ", prefix._guidprefix[i]);
	}
	

}

void main()
{
	r = new zento_Broker();

	r->open(7400);

	while(true)
	{

		int s = getchar();

		if (s == 's')
		{
			getBroker()->getRemoteBrokerManager()->printRemoteBrokers();
		}

		Sleep(1000);
	}

}