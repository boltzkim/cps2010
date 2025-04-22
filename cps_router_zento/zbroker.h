#pragma once

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

#include <thread>
#include <list>

#include "rtps_parser.h"
#include "InParticipant.h"
#include "RemoteBrokerManager.h"


void printPrefix(GuidPrefix_t prefix);


class RemoteBrokerManager;

class zento_Broker
{
public:
	zento_Broker(void);
	virtual ~zento_Broker(void);
	int open(int port);
	void getData();

	static void Network(zento_Broker *p_zento_Broker);



	InParticipant *getInParticipant(GuidPrefix_t a_guid_prefix);
	void send_PDP(uint8_t *p_data, int i_size);


	RemoteBrokerManager *getRemoteBrokerManager()
	{
		return p_RemoteBrokerManager;
	}


	std::list<InParticipant *> inParticipants;

private:

	int module_id;
	int port;

	module_t		*p_module;
	moduleload_t	*p_moduleload;

	char *psz_path;
	access_t *p_access;
	access_out_t*	p_accessout;



	std::thread  *thread_id;
	bool is_end;

	rtps_parser *parser;
	RemoteBrokerManager *p_RemoteBrokerManager;
	
};

zento_Broker *getBroker();

