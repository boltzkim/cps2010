/*
	RTSP Entity class
	작성자 : 
	이력
	2010-08-10 : 처음 시작
*/

/*
Base class for all RTPS entities. RTPS Entity represents the class of objects that are
visible to other RTPS Entities on the network. As such, RTPS Entity objects have a
globally-unique identifier (GUID) and can be referenced inside RTPS messages.
*/

#include "rtps.h"

static Time_t *p_time = NULL;
static Time_t SystemStartedTime;
static int32_t l_entitynum = 1;
static char *local_ip = NULL;
static unsigned long local_addr = 0;


static int16_t a_port = 0;
static int16_t b_port = 0;


static octet default_guidprefix[12];

void init_rtps_entity( rtps_entity_t* p_rtps_entity )
{
	if(p_time == NULL){
		SystemStartedTime = currenTime();

		SystemStartedTime.nanosec += (uint32_t)&SystemStartedTime;
		p_time = &SystemStartedTime;
		srand( (unsigned)time( NULL ) );
		//l_entitynum = rand();

		a_port = get_default_port();
		b_port = get_metatraffic_port();

		if (local_ip == NULL)
		{
			local_ip = get_default_local_address();
			local_addr = inet_addr(local_ip);
		}

		memcpy(default_guidprefix, p_time, sizeof(Time_t));
		memcpy(&default_guidprefix[8], &a_port, sizeof(int16_t));
		memcpy(&default_guidprefix[10], &b_port, sizeof(int16_t));
		memcpy(default_guidprefix, &local_addr, sizeof(local_addr));

		
	}

	

	memcpy(p_rtps_entity->guid.guid_prefix._guidprefix, default_guidprefix,sizeof(default_guidprefix));


	p_rtps_entity->guid.entity_id.entity_key[0] = 0;
	p_rtps_entity->guid.entity_id.entity_key[1] = 0;
	p_rtps_entity->guid.entity_id.entity_key[2] = 1;
	p_rtps_entity->guid.entity_id.entity_kind = KIND_OF_ENTITY_UNKNOWN_BUILTIN;



}

GuidPrefix_t getDefaultPrefix()
{
	GuidPrefix_t prefix;

	if(p_time == NULL) {
		SystemStartedTime = currenTime();

		SystemStartedTime.nanosec += (uint32_t)&SystemStartedTime;
		p_time = &SystemStartedTime;
		srand((unsigned)time(NULL));
		l_entitynum = rand();
	}

	memcpy(prefix._guidprefix, p_time, sizeof(Time_t));
	memcpy(&prefix._guidprefix[8], &l_entitynum, sizeof(int32_t));

	return prefix;
}


void destroyRtpsEntity(rtps_entity_t *p_rtpsEntity)
{

}

int32_t rtps_add_entity( module_object_t* p_this, Entity* p_entity )
{
	switch(p_entity->i_entity_type)
	{
		case DOMAINPARTICIPANT_ENTITY:
			rtps_add_participant(p_this, (DomainParticipant*)p_entity);
			break;
		case DATAREADER_ENTITY:
			rtps_add_datareader(p_this, (DataReader*)p_entity);
			break;
		case DATAWRITER_ENTITY:
			rtps_add_datawriter(p_this, (DataWriter*)p_entity);
			break;
		case TOPIC_ENTITY:
			rtps_add_topic(p_this, (Topic*)p_entity);
			break;
		default:
			trace_msg(p_this, TRACE_ERROR,"RTPS addEntity UNKNOWN_ENTITY");
			return MODULE_ERROR_UNKNOWN_ENTITY;
			break;
	}

	return MODULE_SUCCESS;
}

int32_t rtps_remove_entity( module_object_t* p_this, Entity* p_entity )
{
	switch(p_entity->i_entity_type)
	{
		case DOMAINPARTICIPANT_ENTITY:
			rtps_remove_participant(p_this, (DomainParticipant*)p_entity);
			break;
		case DATAREADER_ENTITY:
			rtps_remove_datareader(p_this, (DataReader*)p_entity);
			break;
		case DATAWRITER_ENTITY:
			rtps_remove_datawriter(p_this, (DataWriter*)p_entity);
			break;
		case TOPIC_ENTITY:
			rtps_remove_topic(p_this, (Topic*)p_entity);
			break;
		default:
			trace_msg(p_this, TRACE_ERROR,"RTPS removeEntity UNKNOWN_ENTITY");
			return MODULE_ERROR_UNKNOWN_ENTITY;
			break;
	}

	return MODULE_SUCCESS;
}

rtps_entity_t* rtps_find_all_entity( GuidPrefix_t a_prefix )
{
	int i_size;
	int i;
	int j;

	rtps_participant_t	**pp_rtps_participants;

	pp_rtps_participants = get_rtps_participants(&i_size);
	for(i=0; i < i_size && pp_rtps_participants; i++)
	{
		
		if (memcmp(&pp_rtps_participants[i]->guid.guid_prefix, &a_prefix, sizeof(GuidPrefix_t)) == 0)
		{
			return	(rtps_entity_t *) pp_rtps_participants[i];
		}

		for(j=0; j < pp_rtps_participants[i]->i_endpoint; j++)
		{
			
			if (memcmp(&pp_rtps_participants[i]->pp_endpoint[j]->guid.guid_prefix, &a_prefix, sizeof(GuidPrefix_t)) == 0)
			{
				return	(rtps_entity_t *) pp_rtps_participants[i]->pp_endpoint[j];
			}
		}

	}

	return NULL;
}