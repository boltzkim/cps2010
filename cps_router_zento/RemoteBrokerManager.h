#pragma once

#include "zbroker.h"
#include "RemoteBroker.h"
#include "RemoteParticipant.h"

#include <thread>
#include <list>
#include <mutex>


class sBroker
{
public:
	sBroker(std::string ip, int port)
	{
		this->ip = ip;
		this->port = port;
	}

	std::string ip;
	int port;

};

class RemoteBrokerManager
{
public:
	RemoteBrokerManager();
	virtual ~RemoteBrokerManager();

	void getData();


	static void Live(RemoteBrokerManager *p_RemoteBrokerManager);
	static void Network(RemoteBrokerManager *p_RemoteBrokerManager);
	void sendPDP(uint8_t *p_data, int i_size);
	void sendNACK(struct sockaddr client, RemoteBroker *p_RemoteBroker);
	void sendReNACK(struct sockaddr client, GuidPrefix_t remote_prefix);


	bool hasClientAddr(struct sockaddr client);
	bool hasBindAddr(struct sockaddr client);
	void procedureNotify(uint8_t *p_data);
	void printRemoteBrokers();

private:
	void sendLive(sBroker a_sBroker);
	void notifyNewRemoteBrokerToOtherRemoteBroker(GuidPrefix_t new_remote_prefix, struct sockaddr_in new_remote_addr, struct sockaddr new_mapped_remote_addr);
	void notifyOtherRemoteBrokerToNewRemoteBroker(GuidPrefix_t	new_remote_prefix, struct sockaddr_in new_remote_addr, struct sockaddr new_mapped_remote_addr);

	access_t		*p_access;
	access_out_t	*p_accessout;

	std::thread		*thread_id;
	std::thread		*thread_id2;
	bool			is_end;
	GuidPrefix_t		prefix;


	std::list<sBroker> sBrokers;
	std::list<RemoteBroker*> remoteBrokers;

};



void receiveNotify(uint8_t* p_data);
