/*
	core에서 하는 DataWriter 관련...

	이력
	2010-08-8 : 시작
*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


//timestamp by jun
extern void writeToServiceTimestamp(rtps_writer_t *p_writer, message_t *p_message, Time_t source_timestamp);
void datawriter_write_timestamp( DataWriter* p_datawriter, void* v_data, InstanceHandle_t handle, Time_t source_timestamp )
{
	message_t* p_message = message_new(p_datawriter, v_data);
	//만들 메세지 작업.

	if (is_quit())
	{
		msleep(100000);
		if (is_terminated())
		{

			exit(0);
		}
	}

	if (p_message == NULL)
		return;

	p_message->handle = handle;
	p_message->sequence = ++p_datawriter->last_sequencenum;

	//RTPS에 보내기...
	if (p_datawriter && p_datawriter->p_related_rtps_writer)
	{
		writeToServiceTimestamp((rtps_writer_t*)p_datawriter->p_related_rtps_writer, p_message, source_timestamp);
	}

	//insert_instanceset_datawriter(p_DataWriter, NULL, p_message);


	/*
	
	if(strcmp(p_message->topic_name, PARTICIPANTINFO_NAME)
	&& strcmp(p_message->topic_name, TOPICINFO_NAME)
	&& strcmp(p_message->topic_name, PUBLICATIONINFO_NAME)
	&& strcmp(p_message->topic_name, SUBSCRIPTIONINFO_NAME))
	{
	messageRelease(p_message);
	}else{
	messageRelease(p_message);
	}
	*/



}

void datawriter_write(DataWriter* p_datawriter, void* v_data, InstanceHandle_t handle, Time_t message_time)
{
	message_t* p_message = message_new(p_datawriter, v_data);

	//added by kyy(Presentation QoS Writer Side)
	//Coherent set이 끝났다는 Message 생성
	//message_t *p_message_coh = message_new(p_datawriter, NULL);
	//만들 메세지 작업.

	if (is_quit())
	{
		msleep(100000);
		if (is_terminated())
		{
			exit(0);
		}
	}

	if (p_message == NULL)
		return;

	p_message->handle = handle;
	p_message->sequence = ++p_datawriter->last_sequencenum;

//time by jun
	p_message->source_time = message_time;

	//RTPS에 보내기...
	if (p_datawriter && p_datawriter->p_related_rtps_writer)
	{
		write_to_service((rtps_writer_t*)p_datawriter->p_related_rtps_writer, p_message);
	}

	//insert_instanceset_datawriter(p_DataWriter, NULL, p_message);


	/*
	
	if(strcmp(p_message->topic_name, PARTICIPANTINFO_NAME)
	&& strcmp(p_message->topic_name, TOPICINFO_NAME)
	&& strcmp(p_message->topic_name, PUBLICATIONINFO_NAME)
	&& strcmp(p_message->topic_name, SUBSCRIPTIONINFO_NAME))
	{
	messageRelease(p_message);
	}else{
	messageRelease(p_message);
	}
	*/



}


void datawriter_dispose(DataWriter* p_datawriter, void* v_data, InstanceHandle_t handle)
{
	//RTPS에 보내기...
	if (p_datawriter && p_datawriter->p_related_rtps_writer)
	{
		dispose_to_service((rtps_writer_t*)p_datawriter->p_related_rtps_writer, v_data, handle);
	}
}

void datawriter_dispose_timestamp(DataWriter* p_datawriter, void* v_data, InstanceHandle_t handle, Time_t source_timestamp)
{
	//RTPS에 보내기...
	if (p_datawriter && p_datawriter->p_related_rtps_writer)
	{
		dispose_to_service_timestamp((rtps_writer_t*)p_datawriter->p_related_rtps_writer, v_data, handle, source_timestamp);
	}
}

void datawriter_unregister(DataWriter* p_datawriter, void* v_data, InstanceHandle_t handle, bool b_dispose)
{
	//RTPS에 보내기...
	if (p_datawriter && p_datawriter->p_related_rtps_writer)
	{
		unregister_to_service((rtps_writer_t*)p_datawriter->p_related_rtps_writer, v_data, handle, b_dispose);
	}
}



//timestamp by jun
void datawriter_unregister_timestamp(DataWriter* p_datawriter, void* v_data, InstanceHandle_t handle, Time_t source_timestamp)
{
	//RTPS에 보내기...
	if (p_datawriter && p_datawriter->p_related_rtps_writer)
	{
		unregister_to_service_timestamp((rtps_writer_t*)p_datawriter->p_related_rtps_writer, v_data, handle, source_timestamp);
	}
}
