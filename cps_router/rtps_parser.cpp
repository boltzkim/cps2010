#include "rtps_parser.h"
#include "InParticipant.h"
#include "cps_router.h"



rtps_parser::rtps_parser(cps_router *p_router)
{
	this->p_router = p_router;
}


rtps_parser::~rtps_parser()
{
}



void rtps_parser::parse(data_t *data)
{
	Header		a_header;
	int32_t		ret;
	int32_t		i_pos = 0;
	uint8_t		E; // EndiannessFlag
	int32_t		i_remind = data->i_size;
	SubmessageHeader a_submessageHeader;
	uint8_t*	p_data = data->p_data;
	InParticipant *inParticipant;
	bool isPDP;

	

	if (ret = rtps_Header((char*)p_data, data->i_size, &a_header))
	{
		return;
	}

	a_header = *(Header*)p_data;

	i_pos += sizeof(Header);
	i_remind -= sizeof(Header);


	if (memcmp(&getDefaultPrefix(), &a_header.guid_prefix, sizeof(GuidPrefix_t)) == 0)
	{
		return ;
	}

	inParticipant = p_router->getInParticipant(a_header.guid_prefix);

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
			break;
		case HEARTBEAT:
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
			isPDP = rtps_data_parse(inParticipant, &p_data[i_pos], a_submessageHeader.submessage_length, a_submessageHeader.flags);
			break;
		case DATA_FRAG:
			break;
		default:
			break;
		}




		i_pos += a_submessageHeader.submessage_length;
		i_remind -= a_submessageHeader.submessage_length;
	}

	if (isPDP)
	{
		p_router->send_PDP(p_data, data->i_size);
	}

}

bool rtps_parser::rtps_data_parse(InParticipant *inParticipant, uint8_t *p_data, ushort i_size, octet flag)
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
	}else if (memcmp(&a_data.reader_id, &ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER, sizeof(a_data.writer_id)) == 0)
	{
		is_pdp = true;
	}


	do {
		Parameter a_parameter = *((Parameter*)&p_data[i_used]);


		switch (a_parameter.parameter_id)
		{
		case PID_SENTINEL:
			 break;
		case PID_UNICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used+sizeof(Parameter)]);
			if(is_pdp) inParticipant->setUNICAST_LOCATOR(a_Locator);
			break;
		case PID_MULTICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp) inParticipant->setMULTICAST_LOCATOR(a_Locator);
			break;
		case PID_DEFAULT_UNICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp) inParticipant->setDEFAULT_UNICAST_LOCATOR(a_Locator);
			break;
		case PID_DEFAULT_MULTICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp) inParticipant->setDEFAULT_MULTICAST_LOCATOR(a_Locator);
			break;
		case PID_METATRAFFIC_UNICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp) inParticipant->setMETATRAFFIC_UNICAST_LOCATOR(a_Locator);
			break;
		case PID_METATRAFFIC_MULTICAST_LOCATOR:
			a_Locator = *((Locator_t*)&p_data[i_used + sizeof(Parameter)]);
			if (is_pdp) inParticipant->setMETATRAFFIC_MULTICAST_LOCATOR(a_Locator);
			break;
		}


		i_remind -= sizeof(Parameter);
		i_used += sizeof(Parameter);
		i_remind -= a_parameter.length;
		i_used += a_parameter.length;

	} while (i_remind > 0);


	if (is_pdp)
	{
		inParticipant->print();
		
	}

	return is_pdp;
}

