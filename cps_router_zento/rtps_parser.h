#pragma once

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <../modules/rtps/rtps.h>

class zento_Broker;
class InParticipant;

class rtps_parser
{
public:
	rtps_parser(zento_Broker *p_Broker);
	virtual ~rtps_parser();


public:

	void parse(data_t *data);

private:
	bool rtps_data_parse(InParticipant *inParticipant, uint8_t *p_data, ushort i_size, octet flag);
	zento_Broker *p_Broker;
};

