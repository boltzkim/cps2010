#include "RemoteParticipant.h"
#include "RemoteRouter.h"



RemoteParticipant::RemoteParticipant(RemoteRouter *p_remoteRouter, GuidPrefix_t guid_prefix)
{
	b_end = false;
	this->p_remoteRouter = p_remoteRouter;
	this->guid_prefix = guid_prefix;


	isChanged = false;
	this->guid_prefix = guid_prefix;

	printf("New RemoteParticipant :");

	for (int i = 0; i < sizeof(GuidPrefix_t); i++)
	{
		printf("%03d ", guid_prefix._guidprefix[i]);
	}

	//printf("\n");


	memset(&UNICAST_LOCATOR, 0, sizeof(Locator_t));
	memset(&MULTICAST_LOCATOR, 0, sizeof(Locator_t));
	memset(&DEFAULT_UNICAST_LOCATOR, 0, sizeof(Locator_t));
	memset(&DEFAULT_MULTICAST_LOCATOR, 0, sizeof(Locator_t));
	memset(&METATRAFFIC_UNICAST_LOCATOR, 0, sizeof(Locator_t));
	memset(&METATRAFFIC_MULTICAST_LOCATOR, 0, sizeof(Locator_t));




	char *psz_path;
	bind_ip = get_default_local_address();

	module_object_t *p_module = (module_object_t *)current_object(0);

	asprintf(&psz_path, "udp:@%s:%d", "239.255.0.1", 0);

	p_access = access_new((module_object_t*)p_module, "udp4", psz_path);
	bind_port = p_access->pf_getBind(p_access);

	printf("Bind RemoteParticipant(%d)", bind_port);
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

	b_using = true;
}

void RemoteParticipant::change_prefix(GuidPrefix_t _guid_prefix)
{
	this->guid_prefix = _guid_prefix;
}

void RemoteParticipant::setUsing(bool use)
{

	b_using = use;


	if (use)
	{
		printf("setUsing(true) RemoteParticipant :");
	}
	else {

		printf("setUsing(false) RemoteParticipant :");
	}

	for (int i = 0; i < sizeof(GuidPrefix_t); i++)
	{
		printf("%03d ", guid_prefix._guidprefix[i]);
	}

	if (use == false) {
		printf("\n");
	}
}


RemoteParticipant::~RemoteParticipant()
{
	b_end = true;
	printf("Delete RemoteParticipant \n");

	if (thread_id)
	{
		b_end = true;
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

	printf("Delete Good RemoteParticipant \n");
}


bool RemoteParticipant::rtps_data_parse(uint8_t *p_org_data, int i_org_size, uint8_t *p_data, ushort i_size, octet flag)
{
	Data a_data = *((Data*)p_data);
	uint8_t Q, D, K;
	int		i_remind = i_size;
	int		i_used = 0;

	encapsulation_schemes_t   a_encapsulation_schemes;
	Locator_t a_Locator;



	if (i_size < sizeof(Data))
	{
		return false;
	}

	Q = flag & 0x02;	//InlineQosFlag
	D = flag & 0x04;	//DataFlag
	K = flag & 0x08;	//KeyFlag


	i_remind -= sizeof(Data);
	i_used += sizeof(Data);


	



	if (D == 0x00 && K == 0x00)
	{

	}
	else if (D && K == 0x00)
	{

	}
	else
	{
		return false;
	}


	if (Q)
	{
		char *p_started_qos = (char *)p_data;
		int i_inline_qos_size = sizeof(Parameter);
		int ret = 0;
		//inline_qos
		do {

			ret = rtps_parse_Parameter(*(Parameter*)&p_started_qos[i_used], &p_started_qos[i_used], NULL);
			i_remind -= ret;
			i_used += ret;
		} while (ret != i_inline_qos_size && i_used < i_size && i_remind >= 0);
	}


	a_encapsulation_schemes = *((encapsulation_schemes_t*)&p_data[i_used]);


	i_remind -= sizeof(encapsulation_schemes_t);
	i_used += sizeof(encapsulation_schemes_t);



	bool is_pdp = false;


	if (memcmp(&a_data.writer_id, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER, sizeof(a_data.writer_id)) == 0)
	{
		is_pdp = true;
	}
	else if (memcmp(&a_data.reader_id, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(a_data.writer_id)) == 0)
	{
		is_pdp = true;
	}

	do {
		Parameter a_parameter = *((Parameter*)&p_data[i_used]);
		Parameter *p_parameter = ((Parameter*)&p_data[i_used]);
		bool isIPv6 = false;

		switch (a_parameter.parameter_id)
		{
		case PID_SENTINEL:
			break;
		case PID_UNICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp)
			{
				setUNICAST_LOCATOR(a_Locator);
				isIPv6 = changeUnicast((Locator_t*)&p_data[i_used + sizeof(Parameter)], bind_port);
			}
			break;
		case PID_MULTICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp) {
				setMULTICAST_LOCATOR(a_Locator);
				isIPv6 = changeMulticast((Locator_t*)&p_data[i_used + sizeof(Parameter)], bind_port);
			}
			break;
		case PID_DEFAULT_UNICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp) {
				setDEFAULT_UNICAST_LOCATOR(a_Locator);
				isIPv6 = changeUnicast((Locator_t*)&p_data[i_used + sizeof(Parameter)], bind_port);
			}
			break;
		case PID_DEFAULT_MULTICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp) {
				setDEFAULT_MULTICAST_LOCATOR(a_Locator);
				isIPv6 = changeMulticast((Locator_t*)&p_data[i_used + sizeof(Parameter)], bind_port);
			}
			break;
		case PID_METATRAFFIC_UNICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp) {
				setMETATRAFFIC_UNICAST_LOCATOR(a_Locator);
				isIPv6 = changeUnicast((Locator_t*)&p_data[i_used + sizeof(Parameter)], bind_port);
			}
			break;
		case PID_METATRAFFIC_MULTICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp) {
				setMETATRAFFIC_MULTICAST_LOCATOR(a_Locator);
				isIPv6 = changeMulticast((Locator_t*)&p_data[i_used + sizeof(Parameter)], p_remoteRouter->bind_port);
			}
			break;
		case PID_VENDORID:
			/*char *vendoer = (char *)&p_data[i_used + sizeof(Parameter)];
			vendoer[0] = 2;
			vendoer[1] = 2;*/
			break;
		}


		if (isIPv6)
		{
			
			//p_parameter->parameter_id = 0x0099;
		}

		i_remind -= sizeof(Parameter);
		i_used += sizeof(Parameter);
		i_remind -= a_parameter.length;
		i_used += a_parameter.length;

	} while (i_remind > 0);


	

	return is_pdp;

}



static void PrintLocator(char *name, Locator_t a_locator)
{
	if (a_locator.kind == 0) return;

	printf("%s : ", name);
	if (a_locator.kind != 0)
	{
		if (a_locator.kind == LOCATOR_KIND_UDPv4)
		{
			printf("v4 ");
			printf(", IP : %d.%d.%d.%d, Port : %d\n", a_locator.address[12], a_locator.address[13], a_locator.address[14], a_locator.address[15], a_locator.port);
		}
		else {
			printf("v6 ");
			printf(", IP : %s, Port : %d\n", a_locator.address, a_locator.port);
		}
	}
}


#ifdef __cplusplus
extern "C" {
#endif

	void pause_system();


#ifdef __cplusplus
}
#endif

bool RemoteParticipant::changeUnicast(Locator_t *p_locator,int port)
{

	bool ret;

	unsigned long address = inet_addr(bind_ip.c_str());

	if(p_locator->kind == 1){
		p_locator->port = port;

		if(p_locator->address[12] == 127)
		{
			pause_system();
		}

		p_locator->address[12] = (octet)address;
		p_locator->address[13] = (octet)(address >> 8);
		p_locator->address[14] = (octet)(address >> 16);
		p_locator->address[15] = (octet)(address >> 24);



		/*p_locator->address[12] = (octet)127;
		p_locator->address[13] = (octet)0;
		p_locator->address[14] = (octet)0;
		p_locator->address[15] = (octet)1;*/


		/*p_locator->address[12] = (octet)127;
		p_locator->address[13] = (octet)0;
		p_locator->address[14] = (octet)0;
		p_locator->address[15] = (octet)1;

		p_locator->address[12] = (octet)192;
		p_locator->address[13] = (octet)168;
		p_locator->address[14] = (octet)0;
		p_locator->address[15] = (octet)4;*/

		ret = false;
	}
	else{
		p_locator->port = port;

		memset(p_locator->address, 0, sizeof(p_locator->address));

		if(p_locator->address[12] == 127)
		{
			pause_system();
		}


		//Ipv6 not support
		
		
		/*p_locator->address[12] = (octet)address;
		p_locator->address[13] = (octet)(address >> 8);
		p_locator->address[14] = (octet)(address >> 16);
		p_locator->address[15] = (octet)(address >> 24);*/

		ret = true;
	}

	


	if (p_locator->address[12] == 127)
	{
		pause_system();
	}

	return ret;
}

bool RemoteParticipant::changeMulticast(Locator_t *p_locator, int port)
{

	bool ret;

	if (p_locator->kind == 1)
	{ 
		p_locator->port = port;
		ret = false;
	}
	else{
		p_locator->port = port;
		ret = true;
	}

	if(p_locator->kind == 1){
		unsigned long address = inet_addr(bind_ip.c_str());

		memset(p_locator->address, 0, sizeof(p_locator->address));

		p_locator->address[12] = (octet)239;
		p_locator->address[13] = (octet)255;
		p_locator->address[14] = (octet)0;
		p_locator->address[15] = (octet)1;
	}


	return ret;
}



void RemoteParticipant::sendPDP(uint8_t *p_org_data, int i_org_size)
{
	data_t *p_senddata = data_new(i_org_size);


	memcpy(p_senddata->p_data, p_org_data, i_org_size);

	p_senddata->i_port = 7400;
	p_senddata->p_address = (uint8_t *)strdup("239.255.0.1");

	p_remoteRouter->send(p_senddata);

	
}



void RemoteParticipant::Network(RemoteParticipant *p_RemoteParticipant)
{
	while (p_RemoteParticipant->b_end == false)
	{
		p_RemoteParticipant->getData();
	}
}


void RemoteParticipant::getData()
{
	struct sockaddr client;
	int l = sizeof(struct sockaddr);
	RemoteRouter *found_remoteRouter = NULL;

	data_t*	data = p_access->pf_dataFrom(p_access, &client, &l);

	if (data == NULL)  return;


	data_t *p_senddata = data_new(data->i_size+6+sizeof(GuidPrefix_t));

	if (data->i_size > 1480)
	{
		printf("Maybe can't send data to remote router.\n");
	}


	memcpy(p_senddata->p_data, "TOPAR:", 6);
	memcpy(p_senddata->p_data + 6, &guid_prefix, sizeof(GuidPrefix_t));
	memcpy(p_senddata->p_data + 6 + sizeof(GuidPrefix_t), data->p_data, data->i_size);




	p_remoteRouter->sendToRemoteRouter(p_senddata);


	if (data) data_release(data);
}
