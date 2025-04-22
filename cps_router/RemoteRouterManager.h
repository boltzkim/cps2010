#pragma once

#include "cps_router.h"
#include "RemoteRouter.h"
#include "RemoteParticipant.h"

#include <thread>
#include <list>
#include <mutex>


class sRouter
{
public:
	sRouter(std::string ip, int port)
	{
		this->ip = ip;
		this->port = port;
	}

	std::string ip;
	int port;

};

class RemoteRouterManager
{
public:
	RemoteRouterManager();
	virtual ~RemoteRouterManager();

	void getData();


	static void Live(RemoteRouterManager *p_RemoteRouterManager);
	static void Network(RemoteRouterManager *p_RemoteRouterManager);
	void sendPDP(uint8_t *p_data, int i_size);
	void sendNACK(struct sockaddr client, RemoteRouter *p_RemoteRouter);
	void sendReNACK(struct sockaddr client, GuidPrefix_t remote_prefix);


	bool hasClientAddr(struct sockaddr client);
	bool hasBindAddr(struct sockaddr client);
	void procedureNotify(uint8_t *p_data);
	void printRemoteRouters();

private:
	void sendLive(sRouter a_sRouter);
	void notifyNewRemoteRouterToOtherRemoteRouter(GuidPrefix_t new_remote_prefix, struct sockaddr_in new_remote_addr, struct sockaddr new_mapped_remote_addr);
	void notifyOtherRemoteRouterToNewRemoteRouter(GuidPrefix_t	new_remote_prefix, struct sockaddr_in new_remote_addr, struct sockaddr new_mapped_remote_addr);

	access_t		*p_access;
	access_out_t	*p_accessout;

	std::thread		*thread_id;
	std::thread		*thread_id2;
	bool			is_end;
	GuidPrefix_t		prefix;


	std::list<sRouter> sRouters;
	std::list<RemoteRouter*> remoteRouters;

};



void receiveNotify(uint8_t* p_data);
