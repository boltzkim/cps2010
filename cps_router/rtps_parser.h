#pragma once

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <../modules/rtps/rtps.h>

class cps_router;
class InParticipant;

class rtps_parser
{
public:
	rtps_parser(cps_router *p_router);
	virtual ~rtps_parser();


public:

	void parse(data_t *data);

private:
	bool rtps_data_parse(InParticipant *inParticipant, uint8_t *p_data, ushort i_size, octet flag);
	cps_router *p_router;
};

