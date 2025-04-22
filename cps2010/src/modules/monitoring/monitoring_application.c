//This file is generated automatically. by edy100.. 
#include "monitoring_application.h"

monitoring_qos_t monitoring_qos;

void monitoring_qos_init()
{
	// monitoring_qos DomainParticipantFactory QoS

	// monitoring_qos DomainParticipant QoS

	static_get_default_participant_qos(&monitoring_qos.m_domainparticipantqos);


	// monitoring_qos Topic QoS

	static_get_default_topic_qos(&monitoring_qos.m_topicqos);


	// monitoring_qos Subscriber QoS

	static_get_default_subscriber_qos(&monitoring_qos.m_subscriberqos);


	// monitoring_qos Publisher QoS

	static_get_default_publisher_qos(&monitoring_qos.m_publisherqos);


	// monitoring_qos DataWriter QoS

	static_get_default_datawriter_qos(&monitoring_qos.m_datawriterqos);

	monitoring_qos.m_datawriterqos.reliability.kind=RELIABLE_RELIABILITY_QOS;
	monitoring_qos.m_datawriterqos.history.kind=KEEP_ALL_HISTORY_QOS;
	monitoring_qos.m_datawriterqos.history.depth=255;

	// monitoring_qos DataReader QoS

	static_get_default_datareader_qos(&monitoring_qos.m_datareaderqos);

	monitoring_qos.m_datareaderqos.reliability.kind=RELIABLE_RELIABILITY_QOS;
	monitoring_qos.m_datareaderqos.history.kind=KEEP_ALL_HISTORY_QOS;
	monitoring_qos.m_datareaderqos.history.depth=255;
}

monitoring2_qos_t monitoring2_qos;

void monitoring2_qos_init()
{
	// monitoring2_qos DomainParticipantFactory QoS

	// monitoring2_qos DomainParticipant QoS

	static_get_default_participant_qos(&monitoring2_qos.m_domainparticipantqos);


	// monitoring2_qos Topic QoS

	static_get_default_topic_qos(&monitoring2_qos.m_topicqos);


	// monitoring2_qos Subscriber QoS

	static_get_default_subscriber_qos(&monitoring2_qos.m_subscriberqos);


	// monitoring2_qos Publisher QoS

	static_get_default_publisher_qos(&monitoring2_qos.m_publisherqos);


	// monitoring2_qos DataWriter QoS

	static_get_default_datawriter_qos(&monitoring2_qos.m_datawriterqos);

	monitoring2_qos.m_datawriterqos.history.kind=KEEP_LAST_HISTORY_QOS;
	monitoring2_qos.m_datawriterqos.history.depth=1;

	// monitoring2_qos DataReader QoS

	static_get_default_datareader_qos(&monitoring2_qos.m_datareaderqos);

	monitoring2_qos.m_datareaderqos.history.kind=KEEP_LAST_HISTORY_QOS;
	monitoring2_qos.m_datareaderqos.history.depth=50;
}



static bool b_is_quit = false;

typedef struct thread_func_arg
{
	void* data_writer_or_reader;
	void* ptr_data;
} thread_func_arg;
#define SAMPLE_DATA_COUNT 20



static thread_func_arg* ParticipantDomain_arg = NULL;
static thread_func_arg* Topic_arg = NULL;
static thread_func_arg* Publisher_arg = NULL;
static thread_func_arg* Subscriber_arg = NULL;
static thread_func_arg* DataWriter_arg = NULL;
static thread_func_arg* DataReader_arg = NULL;
static thread_func_arg* DataBandwidth_arg = NULL;
static thread_func_arg* WriterProxy_arg = NULL;
static thread_func_arg* ReaderProxy_arg = NULL;
static thread_func_arg* WarningErrorConditions_arg = NULL;
static thread_func_arg* SamplesinforamtionReader_arg = NULL;
static thread_func_arg* SamplesinforamtionWriter_arg = NULL;




void write_ParticipantDomain(monitoring_ParticipantDomain a_participantdomain_data)
{	
	Foo* p_foo;
	FooDataWriter* p_datawriter;

	// 함수가 수행될 수 없는 조건을 검사한다.
	if (ParticipantDomain_arg == NULL || ParticipantDomain_arg->data_writer_or_reader == NULL) return;

	if (b_is_quit) return;

	p_datawriter = (FooDataWriter *)ParticipantDomain_arg->data_writer_or_reader;
	
	p_foo = (Foo *)&a_participantdomain_data;
	p_datawriter->write(p_datawriter, p_foo, 0);
}

void thread_writer_ParticipantDomain_func(thread_func_arg* p_arg)
{
	int i;
	Foo* p_foo;
	FooDataWriter* p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		p_foo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter, p_foo, 0);
	}
}


void thread_reader_ParticipantDomain_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet* p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_ParticipantDomain a_ParticipantDomain_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t* p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_ParticipantDomain_data, p_message->v_data, sizeof(monitoring_ParticipantDomain));
				read_ParticipantDomain(a_ParticipantDomain_data);
				//printf(("received data(ParticipantDomain): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}
		FREE(condseq.pp_conditions);
	}
}



void write_Topic(monitoring_Topic a_topic_data)
{
	Foo *fFoo;
	FooDataWriter *p_datawriter;
	
	if (Topic_arg == NULL || Topic_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)Topic_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_topic_data;
	p_datawriter->write(p_datawriter,fFoo,0);
}

void thread_writer_Topic_func(thread_func_arg* p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter *p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}


void thread_reader_Topic_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet* p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_Topic a_Topic_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_Topic_data, p_message->v_data, sizeof(monitoring_Topic));
				read_Topic(a_Topic_data);
				//printf(("received data(Topic): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}

		FREE(condseq.pp_conditions);
	}
}



void write_Publisher(monitoring_Publisher a_Publisher_data)
{
	Foo *fFoo;
	FooDataWriter *p_datawriter;
	
	if (Publisher_arg == NULL || Publisher_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)Publisher_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_Publisher_data;
	p_datawriter->write(p_datawriter,fFoo,0);
}

void thread_writer_Publisher_func(thread_func_arg* p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter *p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}


void thread_reader_Publisher_func(thread_func_arg* arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_Publisher a_Publisher_data;

	if (arg == NULL || arg->data_writer_or_reader == NULL || arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_Publisher_data, p_message->v_data, sizeof(monitoring_Publisher));
				read_Publisher(a_Publisher_data);
				//printf(("received data(Publisher): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}
		FREE(condseq.pp_conditions);
	}
}



void write_Subscriber(monitoring_Subscriber a_Subscriber_data)
{
	Foo *fFoo;
	FooDataWriter *p_datawriter;
	
	if (Subscriber_arg == NULL || Subscriber_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)Subscriber_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_Subscriber_data;
	p_datawriter->write(p_datawriter,fFoo,0);
}

void thread_writer_Subscriber_func(thread_func_arg* p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter *p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}


void thread_reader_Subscriber_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_Subscriber a_Subscriber_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_Subscriber_data, p_message->v_data, sizeof(monitoring_Subscriber));
				read_Subscriber(a_Subscriber_data);
				//printf(("received data(Subscriber): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}

		FREE(condseq.pp_conditions);
	}
}



void write_DataWriter(monitoring_DataWriter a_DataWriter_data)
{
	
	Foo *fFoo;
	FooDataWriter *p_datawriter;
	
	if (DataWriter_arg == NULL || DataWriter_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)DataWriter_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_DataWriter_data;
	p_datawriter->write(p_datawriter,fFoo,0);
}

void thread_writer_DataWriter_func(thread_func_arg* p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter *p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}


void thread_reader_DataWriter_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_DataWriter a_DataWriter_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_DataWriter_data, p_message->v_data, sizeof(monitoring_DataWriter));
				read_DataWriter(a_DataWriter_data);
				//printf(("received data(DataWriter): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}

		FREE(condseq.pp_conditions);
	}
}




void write_DataReader(monitoring_DataReader a_datareader_data)
{
	Foo* fFoo;
	FooDataWriter* p_datawriter;
	
	if (DataReader_arg == NULL || DataReader_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)DataReader_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_datareader_data;
	p_datawriter->write(p_datawriter, fFoo, 0);
}

void thread_writer_DataReader_func(thread_func_arg* p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter *p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}


void thread_reader_DataReader_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_DataReader a_DataReader_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_DataReader_data, p_message->v_data, sizeof(monitoring_DataReader));
				read_DataReader(a_DataReader_data);
				//printf(("received data(DataReader): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}

		FREE(condseq.pp_conditions);
	}
}


void write_DataBandwidth(monitoring_DataBandwidth a_DataBandwidth_data)
{
	
	Foo *fFoo;
	FooDataWriter *p_datawriter;
	
	if (DataBandwidth_arg == NULL || DataBandwidth_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)DataBandwidth_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_DataBandwidth_data;
	p_datawriter->write(p_datawriter,fFoo,0);
}

void thread_writer_DataBandwidth_func(thread_func_arg* p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter *p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}


void thread_reader_DataBandwidth_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_DataBandwidth a_DataBandwidth_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_DataBandwidth_data, p_message->v_data, sizeof(monitoring_DataBandwidth));
				read_DataBandwidth(a_DataBandwidth_data);
				//printf(("received data(DataBandwidth): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}

		FREE(condseq.pp_conditions);
	}
}



void write_WriterProxy(monitoring_WriterProxy a_WriterProxy_data)
{
	
	Foo *fFoo;
	FooDataWriter *p_datawriter;
	
	if (WriterProxy_arg == NULL || WriterProxy_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)WriterProxy_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_WriterProxy_data;
	p_datawriter->write(p_datawriter,fFoo,0);
}

void thread_writer_WriterProxy_func(thread_func_arg *p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter *p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}


#include "../rtps/rtps.h"

void thread_reader_WriterProxy_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_WriterProxy a_WriterProxy_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_WriterProxy_data, p_message->v_data, sizeof(monitoring_WriterProxy));

				if (memcmp(&p_datareader->p_subscriber->p_domain_participant->p_rtps_participant->guid, a_WriterProxy_data.remoteWriterGuid, 12) != 0)
				{
					read_WriterProxy(a_WriterProxy_data);
				}
				else
				{
					//read_WriterProxy(a_WriterProxy_data);
				}

				
				//printf(("received data(WriterProxy): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}

		FREE(condseq.pp_conditions);
	}
}




void write_ReaderProxy(monitoring_ReaderProxy a_ReaderProxy_data)
{
	
	Foo *fFoo;
	FooDataWriter *p_datawriter;
	
	if (ReaderProxy_arg == NULL || ReaderProxy_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)ReaderProxy_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_ReaderProxy_data;
	p_datawriter->write(p_datawriter,fFoo,0);
}

void thread_writer_ReaderProxy_func(thread_func_arg* p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter *p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}


void thread_reader_ReaderProxy_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_ReaderProxy a_ReaderProxy_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_ReaderProxy_data, p_message->v_data, sizeof(monitoring_ReaderProxy));

				if (memcmp(&p_datareader->p_subscriber->p_domain_participant->p_rtps_participant->guid, a_ReaderProxy_data.remoteReaderGuid, 12) != 0)
				{
					read_ReaderProxy(a_ReaderProxy_data);
				}
				else
				{
					//read_ReaderProxy(a_ReaderProxy_data);
				}


				
				//printf(("received data(ReaderProxy): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}

		FREE(condseq.pp_conditions);
	}
}




void write_WarningErrorConditions(monitoring_WarningErrorConditions a_WarningErrorConditions_data)
{
	
	Foo *fFoo;
	FooDataWriter *p_datawriter;
	
	if (WarningErrorConditions_arg == NULL || WarningErrorConditions_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)WarningErrorConditions_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_WarningErrorConditions_data;
	p_datawriter->write(p_datawriter,fFoo,0);
}

void thread_writer_WarningErrorConditions_func(thread_func_arg* p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter *p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}


void thread_reader_WarningErrorConditions_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_WarningErrorConditions a_WarningErrorConditions_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_WarningErrorConditions_data, p_message->v_data, sizeof(monitoring_WarningErrorConditions));
				read_WarningErrorConditions(a_WarningErrorConditions_data);
				//printf(("received data(WarningErrorConditions): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}

		FREE(condseq.pp_conditions);
	}
}




void write_SamplesinforamtionReader(monitoring_SamplesinforamtionReader a_SamplesinforamtionReader_data)
{
	
	Foo *fFoo;
	FooDataWriter *p_datawriter;
	
	if (SamplesinforamtionReader_arg == NULL || SamplesinforamtionReader_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)SamplesinforamtionReader_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_SamplesinforamtionReader_data;
	p_datawriter->write(p_datawriter,fFoo,0);
}

void thread_writer_SamplesinforamtionReader_func(thread_func_arg* p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter *p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if(b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}


void thread_reader_SamplesinforamtionReader_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader *p_datareader;
	monitoring_SamplesinforamtionReader a_SamplesinforamtionReader_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t *p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_SamplesinforamtionReader_data, p_message->v_data, sizeof(monitoring_SamplesinforamtionReader));
				read_SamplesinforamtionReader(a_SamplesinforamtionReader_data);
				//printf(("received data(SamplesinforamtionReader): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}

		FREE(condseq.pp_conditions);
	}
}




void write_SamplesinforamtionWriter(monitoring_SamplesinforamtionWriter a_SamplesinforamtionWriter_data)
{
	
	Foo *fFoo;
	FooDataWriter *p_datawriter;
	
	if (SamplesinforamtionWriter_arg == NULL || SamplesinforamtionWriter_arg->data_writer_or_reader == NULL) return;
	
	if (b_is_quit) return;
	p_datawriter = (FooDataWriter *)SamplesinforamtionWriter_arg->data_writer_or_reader;
	
	fFoo = (Foo *)&a_SamplesinforamtionWriter_data;
	p_datawriter->write(p_datawriter,fFoo,0);
}

void thread_writer_SamplesinforamtionWriter_func(thread_func_arg* p_arg)
{
	int i;
	Foo *fFoo;
	FooDataWriter* p_datawriter;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datawriter = (FooDataWriter *)p_arg->data_writer_or_reader;

	for (i = 0; i < SAMPLE_DATA_COUNT; i++)
	{
		msleep(100000);
		fFoo = (Foo *)p_arg->ptr_data;
		if (b_is_quit) return;
		p_datawriter->write(p_datawriter,fFoo,0);
	}
}

void thread_reader_SamplesinforamtionWriter_func(thread_func_arg* p_arg)
{
	ReadCondition *p_readCondition;
	StatusCondition *p_statusCondition;
	WaitSet * p_waitset = waitset_new();
	Duration_t waittime = {100,100};
	ConditionSeq condseq = INIT_CONDITIONSEQ;
	FooSeq fseq = INIT_FOOSEQ;
	SampleInfoSeq sSeq = INIT_SAMPLEINFOSEQ;
	FooDataReader* p_datareader;
	monitoring_SamplesinforamtionWriter a_SamplesinforamtionWriter_data;

	if (p_arg == NULL || p_arg->data_writer_or_reader == NULL || p_arg->ptr_data == NULL) return;

	p_datareader = (FooDataReader *)p_arg->data_writer_or_reader;

	p_statusCondition = p_datareader->get_statuscondition((Entity*)p_datareader);
	p_statusCondition->set_enabled_statuses(p_statusCondition,DATA_AVAILABLE_STATUS);

	p_readCondition = p_datareader->create_readcondition((DataReader *)p_datareader, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

	p_waitset->attach_condition(p_waitset, (Condition *)p_readCondition);
	p_waitset->attach_condition(p_waitset, (Condition *)p_statusCondition);

	memset(&condseq,0, sizeof(ConditionSeq));

	while (!b_is_quit)
	{
		//p_waitset->wait(p_waitset, &condseq, waittime);
		p_datareader->take(p_datareader, &fseq, &sSeq, 1, ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		if (fseq.i_seq)
		{
			message_t* p_message = NULL;

			while (fseq.i_seq)
			{
				p_message = (message_t *)fseq.pp_foo[0];
				////
				//memcpy in "p_message->v_data"
				memcpy(&a_SamplesinforamtionWriter_data, p_message->v_data, sizeof(monitoring_SamplesinforamtionWriter));
				read_SamplesinforamtionWriter(a_SamplesinforamtionWriter_data);
				//printf(("received data(SamplesinforamtionWriter): %d\r\n", fseq.i_seq);

				///
				message_release(p_message);
				REMOVE_ELEM(fseq.pp_foo, fseq.i_seq, 0);
			}

			if (b_is_quit) break;
		}

		FREE(condseq.pp_conditions);
	}
}

static int module_id = -800;

static bool reader_thread_enabled = true;

static bool writer_thread_enabled = false;

void monitoring_quit()
{
	b_is_quit = true;
}

void ParticipantDomain_Wrapper()
{
	DomainParticipantFactory* p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant* p_participant;
	Topic* p_topic;
	Publisher* p_publisher;
	FooDataWriter* p_datawriter;
	Subscriber* p_subscriber;
	FooDataReader* p_datareader;
	StatusMask mask = -1;
	thread_func_arg* p_arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring_qos_init();
	p_dpf->set_qos(&monitoring_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant, "ParticipantDomain", "monitoring:ParticipantDomain", &monitoring_qos.m_topicqos, NULL, mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &monitoring_qos.m_datawriterqos, NULL, 0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber, p_topic, &monitoring_qos.m_datareaderqos, NULL, 0);
	}
	if (writer_thread_enabled)
	{
		ParticipantDomain_arg = p_arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		p_arg->data_writer_or_reader = p_datawriter;
		p_arg->ptr_data = malloc(sizeof(monitoring_ParticipantDomain));
		memset(p_arg->ptr_data, '\0', sizeof(monitoring_ParticipantDomain));
#ifdef _MSC_VER
		//_beginthread( thread_writer_ParticipantDomain_func, 0, p_arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_ParticipantDomain_func, p_arg );
#endif
	}
	if (reader_thread_enabled)
	{
		p_arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		p_arg->data_writer_or_reader = p_datareader;
		p_arg->ptr_data = malloc(sizeof(monitoring_ParticipantDomain));
		memset(p_arg->ptr_data,'\0',sizeof(monitoring_ParticipantDomain));
#ifdef _MSC_VER
		_beginthread( thread_reader_ParticipantDomain_func, 0, p_arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_ParticipantDomain_func, p_arg );
#endif
	}
}



void Topic_Wrapper()
{
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant* p_participant;
	Topic* p_topic;
	Publisher* p_publisher;
	FooDataWriter* p_datawriter;
	Subscriber* p_subscriber;
	FooDataReader* p_datareader;
	StatusMask mask = -1;
	thread_func_arg* p_arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring_qos_init();
	p_dpf->set_qos(&monitoring_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant,"Topic","monitoring:Topic",&monitoring_qos.m_topicqos,NULL,mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_topic,&monitoring_qos.m_datawriterqos,NULL,0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_topic,&monitoring_qos.m_datareaderqos,NULL,0);
	}
	if (writer_thread_enabled)
	{
		Topic_arg = p_arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		p_arg->data_writer_or_reader = p_datawriter;
		p_arg->ptr_data = malloc(sizeof(monitoring_Topic));
		memset(p_arg->ptr_data,'\0',sizeof(monitoring_Topic));
#ifdef _MSC_VER
		//_beginthread( thread_writer_Topic_func, 0, p_arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_Topic_func, p_arg );
#endif
	}
	if (reader_thread_enabled)
	{
		p_arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		p_arg->data_writer_or_reader = p_datareader;
		p_arg->ptr_data = malloc(sizeof(monitoring_Topic));
		memset(p_arg->ptr_data,'\0',sizeof(monitoring_Topic));
#ifdef _MSC_VER
		_beginthread( thread_reader_Topic_func, 0, p_arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_Topic_func, p_arg );
#endif
	}
}



void Publisher_Wrapper()
{
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant *p_participant;
	Topic	*p_topic;
	Publisher *p_publisher;
	FooDataWriter *p_datawriter;
	Subscriber *p_subscriber;
	FooDataReader *p_datareader;
	StatusMask mask = -1;
	thread_func_arg *arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring_qos_init();
	p_dpf->set_qos(&monitoring_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant,"Publisher","monitoring:Publisher",&monitoring_qos.m_topicqos,NULL,mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_topic,&monitoring_qos.m_datawriterqos,NULL,0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_topic,&monitoring_qos.m_datareaderqos,NULL,0);
	}
	if (writer_thread_enabled)
	{
		Publisher_arg = arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datawriter;
		arg->ptr_data = malloc(sizeof(monitoring_Publisher));
		memset(arg->ptr_data,'\0',sizeof(monitoring_Publisher));
#ifdef _MSC_VER
		//_beginthread( thread_writer_Publisher_func, 0, arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_Publisher_func, arg );
#endif
	}
	if (reader_thread_enabled)
	{
		arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datareader;
		arg->ptr_data = malloc(sizeof(monitoring_Publisher));
		memset(arg->ptr_data,'\0',sizeof(monitoring_Publisher));
#ifdef _MSC_VER
		_beginthread( thread_reader_Publisher_func, 0, arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_Publisher_func, arg );
#endif
	}
}



void Subscriber_Wrapper()
{
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant *p_participant;
	Topic	*p_topic;
	Publisher *p_publisher;
	FooDataWriter *p_datawriter;
	Subscriber *p_subscriber;
	FooDataReader *p_datareader;
	StatusMask mask = -1;
	thread_func_arg *arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring_qos_init();
	p_dpf->set_qos(&monitoring_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant,"Subscriber","monitoring:Subscriber",&monitoring_qos.m_topicqos,NULL,mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_topic,&monitoring_qos.m_datawriterqos,NULL,0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_topic,&monitoring_qos.m_datareaderqos,NULL,0);
	}
	if (writer_thread_enabled)
	{
		Subscriber_arg = arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datawriter;
		arg->ptr_data = malloc(sizeof(monitoring_Subscriber));
		memset(arg->ptr_data,'\0',sizeof(monitoring_Subscriber));
#ifdef _MSC_VER
		//_beginthread( thread_writer_Subscriber_func, 0, arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_Subscriber_func, arg );
#endif
	}
	if (reader_thread_enabled)
	{
		arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datareader;
		arg->ptr_data = malloc(sizeof(monitoring_Subscriber));
		memset(arg->ptr_data,'\0',sizeof(monitoring_Subscriber));
#ifdef _MSC_VER
		_beginthread( thread_reader_Subscriber_func, 0, arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_Subscriber_func, arg );
#endif
	}
}



void DataWriter_Wrapper()
{
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant *p_participant;
	Topic	*p_topic;
	Publisher *p_publisher;
	FooDataWriter *p_datawriter;
	Subscriber *p_subscriber;
	FooDataReader *p_datareader;
	StatusMask mask = -1;
	thread_func_arg *arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring_qos_init();
	p_dpf->set_qos(&monitoring_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant,"DataWriter","monitoring:DataWriter",&monitoring_qos.m_topicqos,NULL,mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_topic,&monitoring_qos.m_datawriterqos,NULL,0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_topic,&monitoring_qos.m_datareaderqos,NULL,0);
	}
	if (writer_thread_enabled)
	{
		DataWriter_arg = arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datawriter;
		arg->ptr_data = malloc(sizeof(monitoring_DataWriter));
		memset(arg->ptr_data,'\0',sizeof(monitoring_DataWriter));
#ifdef _MSC_VER
		//_beginthread( thread_writer_DataWriter_func, 0, arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_DataWriter_func, arg );
#endif
	}
	if (reader_thread_enabled)
	{
		arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datareader;
		arg->ptr_data = malloc(sizeof(monitoring_DataWriter));
		memset(arg->ptr_data,'\0',sizeof(monitoring_DataWriter));
#ifdef _MSC_VER
		_beginthread( thread_reader_DataWriter_func, 0, arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_DataWriter_func, arg );
#endif
	}
}



void DataReader_Wrapper()
{
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant *p_participant;
	Topic	*p_topic;
	Publisher *p_publisher;
	FooDataWriter *p_datawriter;
	Subscriber *p_subscriber;
	FooDataReader *p_datareader;
	StatusMask mask = -1;
	thread_func_arg *arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring_qos_init();
	p_dpf->set_qos(&monitoring_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant,"DataReader","monitoring:DataReader",&monitoring_qos.m_topicqos,NULL,mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_topic,&monitoring_qos.m_datawriterqos,NULL,0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_topic,&monitoring_qos.m_datareaderqos,NULL,0);
	}
	if (writer_thread_enabled)
	{
		DataReader_arg = arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datawriter;
		arg->ptr_data = malloc(sizeof(monitoring_DataReader));
		memset(arg->ptr_data,'\0',sizeof(monitoring_DataReader));
#ifdef _MSC_VER
		//_beginthread( thread_writer_DataReader_func, 0, arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_DataReader_func, arg );
#endif
	}
	if (reader_thread_enabled)
	{
		arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datareader;
		arg->ptr_data = malloc(sizeof(monitoring_DataReader));
		memset(arg->ptr_data,'\0',sizeof(monitoring_DataReader));
#ifdef _MSC_VER
		_beginthread( thread_reader_DataReader_func, 0, arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_DataReader_func, arg );
#endif
	}
}



void DataBandwidth_Wrapper()
{
	DomainParticipantFactory* p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant* p_participant;
	Topic* p_topic;
	Publisher* p_publisher;
	FooDataWriter* p_datawriter;
	Subscriber* p_subscriber;
	FooDataReader* p_datareader;
	StatusMask mask = -1;
	thread_func_arg* p_arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring2_qos_init();
	p_dpf->set_qos(&monitoring2_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring2_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring2_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring2_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant, "DataBandwidth", "monitoring:DataBandwidth", &monitoring2_qos.m_topicqos, NULL, mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher, p_topic, &monitoring2_qos.m_datawriterqos, NULL, 0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber, p_topic, &monitoring2_qos.m_datareaderqos, NULL, 0);
	}
	if (writer_thread_enabled)
	{
		DataBandwidth_arg = p_arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		p_arg->data_writer_or_reader = p_datawriter;
		p_arg->ptr_data = malloc(sizeof(monitoring_DataBandwidth));
		memset(p_arg->ptr_data,'\0',sizeof(monitoring_DataBandwidth));
#ifdef _MSC_VER
		//_beginthread( thread_writer_DataBandwidth_func, 0, arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_DataBandwidth_func, arg );
#endif
	}
	if (reader_thread_enabled)
	{
		p_arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		p_arg->data_writer_or_reader = p_datareader;
		p_arg->ptr_data = malloc(sizeof(monitoring_DataBandwidth));
		memset(p_arg->ptr_data,'\0',sizeof(monitoring_DataBandwidth));
#ifdef _MSC_VER
		_beginthread( thread_reader_DataBandwidth_func, 0, p_arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_DataBandwidth_func, p_arg );
#endif
	}
}



void WriterProxy_Wrapper()
{
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant* p_participant;
	Topic* p_topic;
	Publisher* p_publisher;
	FooDataWriter* p_datawriter;
	Subscriber *p_subscriber;
	FooDataReader *p_datareader;
	StatusMask mask = -1;
	thread_func_arg *arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring_qos_init();
	p_dpf->set_qos(&monitoring_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant,"WriterProxy","monitoring:WriterProxy",&monitoring_qos.m_topicqos,NULL,mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_topic,&monitoring_qos.m_datawriterqos,NULL,0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_topic,&monitoring_qos.m_datareaderqos,NULL,0);
	}
	if (writer_thread_enabled)
	{
		WriterProxy_arg = arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datawriter;
		arg->ptr_data = malloc(sizeof(monitoring_WriterProxy));
		memset(arg->ptr_data,'\0',sizeof(monitoring_WriterProxy));
#ifdef _MSC_VER
		//_beginthread( thread_writer_WriterProxy_func, 0, arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_WriterProxy_func, arg );
#endif
	}
	if (reader_thread_enabled)
	{
		arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datareader;
		arg->ptr_data = malloc(sizeof(monitoring_WriterProxy));
		memset(arg->ptr_data,'\0',sizeof(monitoring_WriterProxy));
#ifdef _MSC_VER
		_beginthread( thread_reader_WriterProxy_func, 0, arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_WriterProxy_func, arg );
#endif
	}
}



void ReaderProxy_Wrapper()
{
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant *p_participant;
	Topic	*p_topic;
	Publisher *p_publisher;
	FooDataWriter *p_datawriter;
	Subscriber *p_subscriber;
	FooDataReader *p_datareader;
	StatusMask mask = -1;
	thread_func_arg *arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring_qos_init();
	p_dpf->set_qos(&monitoring_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant,"ReaderProxy","monitoring:ReaderProxy",&monitoring_qos.m_topicqos,NULL,mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_topic,&monitoring_qos.m_datawriterqos,NULL,0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_topic,&monitoring_qos.m_datareaderqos,NULL,0);
	}
	if (writer_thread_enabled)
	{
		ReaderProxy_arg = arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datawriter;
		arg->ptr_data = malloc(sizeof(monitoring_ReaderProxy));
		memset(arg->ptr_data,'\0',sizeof(monitoring_ReaderProxy));
#ifdef _MSC_VER
		//_beginthread( thread_writer_ReaderProxy_func, 0, arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_ReaderProxy_func, arg );
#endif
	}
	if (reader_thread_enabled)
	{
		arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datareader;
		arg->ptr_data = malloc(sizeof(monitoring_ReaderProxy));
		memset(arg->ptr_data,'\0',sizeof(monitoring_ReaderProxy));
#ifdef _MSC_VER
		_beginthread( thread_reader_ReaderProxy_func, 0, arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_ReaderProxy_func, arg );
#endif
	}
}



void WarningErrorConditions_Wrapper()
{
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant *p_participant;
	Topic	*p_topic;
	Publisher *p_publisher;
	FooDataWriter *p_datawriter;
	Subscriber *p_subscriber;
	FooDataReader *p_datareader;
	StatusMask mask = -1;
	thread_func_arg *arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring2_qos_init();
	p_dpf->set_qos(&monitoring2_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring2_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring2_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring2_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant,"WarningErrorConditions","monitoring:WarningErrorConditions",&monitoring2_qos.m_topicqos,NULL,mask);
	if (writer_thread_enabled)
	{
		monitoring2_qos.m_datawriterqos.history.depth = 100;
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_topic,&monitoring2_qos.m_datawriterqos,NULL,0);
	}
	if (reader_thread_enabled)
	{
		monitoring2_qos.m_datareaderqos.history.depth = 100;
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_topic,&monitoring2_qos.m_datareaderqos,NULL,0);
	}
	if (writer_thread_enabled)
	{
		WarningErrorConditions_arg = arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datawriter;
		arg->ptr_data = malloc(sizeof(monitoring_WarningErrorConditions));
		memset(arg->ptr_data,'\0',sizeof(monitoring_WarningErrorConditions));
#ifdef _MSC_VER
		//_beginthread( thread_writer_WarningErrorConditions_func, 0, arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_WarningErrorConditions_func, arg );
#endif
	}
	if (reader_thread_enabled)
	{
		arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datareader;
		arg->ptr_data = malloc(sizeof(monitoring_WarningErrorConditions));
		memset(arg->ptr_data,'\0',sizeof(monitoring_WarningErrorConditions));
#ifdef _MSC_VER
		_beginthread( thread_reader_WarningErrorConditions_func, 0, arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_WarningErrorConditions_func, arg );
#endif
	}
}



void SamplesinforamtionReader_Wrapper()
{
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant *p_participant;
	Topic	*p_topic;
	Publisher *p_publisher;
	FooDataWriter *p_datawriter;
	Subscriber *p_subscriber;
	FooDataReader *p_datareader;
	StatusMask mask = -1;
	thread_func_arg *arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring2_qos_init();
	p_dpf->set_qos(&monitoring2_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring2_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring2_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring2_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant,"SamplesinforamtionReader","monitoring:SamplesinforamtionReader",&monitoring2_qos.m_topicqos,NULL,mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_topic,&monitoring2_qos.m_datawriterqos,NULL,0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_topic,&monitoring2_qos.m_datareaderqos,NULL,0);
	}
	if (writer_thread_enabled)
	{
		SamplesinforamtionReader_arg = arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datawriter;
		arg->ptr_data = malloc(sizeof(monitoring_SamplesinforamtionReader));
		memset(arg->ptr_data,'\0',sizeof(monitoring_SamplesinforamtionReader));
#ifdef _MSC_VER
		//_beginthread( thread_writer_SamplesinforamtionReader_func, 0, arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_SamplesinforamtionReader_func, arg );
#endif
	}
	if (reader_thread_enabled)
	{
		arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		arg->data_writer_or_reader = p_datareader;
		arg->ptr_data = malloc(sizeof(monitoring_SamplesinforamtionReader));
		memset(arg->ptr_data,'\0',sizeof(monitoring_SamplesinforamtionReader));
#ifdef _MSC_VER
		_beginthread( thread_reader_SamplesinforamtionReader_func, 0, arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_SamplesinforamtionReader_func, arg );
#endif
	}
}



void SamplesinforamtionWriter_Wrapper()
{
	DomainParticipantFactory *p_dpf = DomainParticipantFactory_get_instance();
	DomainParticipant *p_participant;
	Topic	*p_topic;
	Publisher *p_publisher;
	FooDataWriter *p_datawriter;
	Subscriber *p_subscriber;
	FooDataReader *p_datareader;
	StatusMask mask = -1;
	thread_func_arg *p_arg;
#ifndef _MSC_VER
	pthread_t thread_id;
#endif


	monitoring2_qos_init();
	p_dpf->set_qos(&monitoring2_qos.m_domainparticipantfactoryqos);
	p_participant = p_dpf->create_participant(module_id, &monitoring2_qos.m_domainparticipantqos, NULL, mask);


	register_type_module_monitoring(p_participant);


	p_publisher = p_participant->create_publisher(p_participant, &monitoring2_qos.m_publisherqos, NULL, mask);
	p_subscriber = p_participant->create_subscriber(p_participant, &monitoring2_qos.m_subscriberqos, NULL, mask);
	p_topic = p_participant->create_topic(p_participant,"SamplesinforamtionWriter","monitoring:SamplesinforamtionWriter",&monitoring2_qos.m_topicqos,NULL,mask);
	if (writer_thread_enabled)
	{
		p_datawriter = (FooDataWriter *)p_publisher->create_datawriter(p_publisher,p_topic,&monitoring2_qos.m_datawriterqos,NULL,0);
	}
	if (reader_thread_enabled)
	{
		p_datareader = (FooDataReader *)p_subscriber->create_datareader(p_subscriber,p_topic,&monitoring2_qos.m_datareaderqos,NULL,0);
	}
	if (writer_thread_enabled)
	{
		SamplesinforamtionWriter_arg = p_arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		p_arg->data_writer_or_reader = p_datawriter;
		p_arg->ptr_data = malloc(sizeof(monitoring_SamplesinforamtionWriter));
		memset(p_arg->ptr_data,'\0',sizeof(monitoring_SamplesinforamtionWriter));
#ifdef _MSC_VER
		//_beginthread( thread_writer_SamplesinforamtionWriter_func, 0, p_arg );
#else
		//pthread_create( &thread_id, NULL, (void *)thread_writer_SamplesinforamtionWriter_func, p_arg );
#endif
	}
	if (reader_thread_enabled)
	{
		p_arg = (thread_func_arg *)malloc(sizeof(thread_func_arg));
		p_arg->data_writer_or_reader = p_datareader;
		p_arg->ptr_data = malloc(sizeof(monitoring_SamplesinforamtionWriter));
		memset(p_arg->ptr_data,'\0',sizeof(monitoring_SamplesinforamtionWriter));
#ifdef _MSC_VER
		_beginthread( thread_reader_SamplesinforamtionWriter_func, 0, p_arg );
#else
		pthread_create( &thread_id, NULL, (void *)thread_reader_SamplesinforamtionWriter_func, p_arg );
#endif
	}
}





void monitoring_wrapper_application()
{
	trace_msg(NULL, TRACE_LOG, "ParticipantDomain_Wrapper..");
	ParticipantDomain_Wrapper();
	trace_msg(NULL, TRACE_LOG, "Topic_Wrapper..");
	Topic_Wrapper();
	trace_msg(NULL, TRACE_LOG, "Publisher_Wrapper..");
	Publisher_Wrapper();
	trace_msg(NULL, TRACE_LOG, "Subscriber_Wrapper..");
	Subscriber_Wrapper();
	trace_msg(NULL, TRACE_LOG, "DataWriter_Wrapper..");
	DataWriter_Wrapper();
	trace_msg(NULL, TRACE_LOG, "DataReader_Wrapper..");
	DataReader_Wrapper();
	trace_msg(NULL, TRACE_LOG, "DataBandwidth_Wrapper..");
	DataBandwidth_Wrapper();
	trace_msg(NULL, TRACE_LOG, "WriterProxy_Wrapper..");
	WriterProxy_Wrapper();
	trace_msg(NULL, TRACE_LOG, "ReaderProxy_Wrapper..");
	ReaderProxy_Wrapper();
	trace_msg(NULL, TRACE_LOG, "WarningErrorConditions_Wrapper..");
	WarningErrorConditions_Wrapper();
	trace_msg(NULL, TRACE_LOG, "SamplesinforamtionReader_Wrapper..");
	SamplesinforamtionReader_Wrapper();
	trace_msg(NULL, TRACE_LOG, "SamplesinforamtionWriter_Wrapper..");
	SamplesinforamtionWriter_Wrapper();
}

