#pragma once

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

#include <thread>
#include <list>

#include "rtps_parser.h"
#include "InParticipant.h"
#include "RemoteRouterManager.h"


void printPrefix(GuidPrefix_t prefix);


class RemoteRouterManager;

class cps_router
{
public:
	cps_router(void);
	virtual ~cps_router(void);
	int open(int port);
	void getData();

	static void Network(cps_router *p_cps_router);



	InParticipant *getInParticipant(GuidPrefix_t a_guid_prefix);
	void send_PDP(uint8_t *p_data, int i_size);


	RemoteRouterManager *getRemoteRouterManager()
	{
		return p_RemoteRouterManager;
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
	RemoteRouterManager *p_RemoteRouterManager;
	
};

cps_router *getRouter();

