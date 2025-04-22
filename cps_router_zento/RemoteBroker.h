#pragma once


#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <../modules/rtps/rtps.h>

#include <winsock.h>
#include <string>
#include <thread>
#include <list>

#include "RemoteParticipant.h"

#define LIVESLEEPTIME 3000

#define PRINT_NETWORK 0

class RemoteBroker
{
public:
	RemoteBroker(GuidPrefix_t prefix, struct sockaddr_in addr);
	RemoteBroker(struct sockaddr_in addr);
	RemoteBroker(std::string ip, int port);
	virtual ~RemoteBroker();


	void getData();
	void send(data_t *p_data);
	void sendToRemoteBroker(data_t *p_data);
	void sendDataToInParticipant(GuidPrefix_t in_prefix, uint8_t *p_org_data, int i_org_size);

	void print();
	void setLocalAddr(struct sockaddr_in addr);

	static void Network(RemoteBroker *p_RemoteBroker);
	static void Live(RemoteBroker *p_RemoteBroker);



	struct sockaddr_in	remote_addr;
	
	std::string ip;
	int port;

	std::string mapped_ip;
	int mapped_port;


	std::string			bind_ip;
	int					bind_port;
	
	bool				is_end;
	bool				isMapped;
private:
	
	void alive();
	void sendLive();
	void sendACK();
	void changeAddr(GuidPrefix_t prefix, struct sockaddr_in addr);
	void parse(data_t	*data, bool isfromremoteBroker);
	RemoteParticipant *getRemoteParticipant(GuidPrefix_t a_guid_prefix);

	access_t			*p_access;
	access_out_t		*p_accessout;

	std::thread			*thread_id;
	std::thread			*thread_id2;
	

	
	
	

	Time_t				acked_time;
	
	bool				isrecvNACK;


	std::list<RemoteParticipant *> remoteParticipants;

	bool				is_recv_notify_addr_diff;

public:
	GuidPrefix_t		remote_prefix;
	GuidPrefix_t		prefix;
	struct sockaddr_in	bind_addr;
};

int getMessageSize();
int getNotifyMessageSize();

