/*
	Monitoring 관련 헤더
	작성자 : 
	이력
	2012-08-8 : 시작

*/

#if !defined(__MONITORING_H)
#define __MONITORING_H

typedef struct monitoring_sys_t monitoring_sys_t;

#ifndef DATAREADER_DEFINE
	#define DATAREADER_DEFINE 1
	typedef struct DataReader DataReader;
#endif

#ifndef PUBLISHER_DEFINE
#	define PUBLISHER_DEFINE 1
	typedef struct Publisher Publisher;
#endif

#ifndef SUBSCRIBER_DEFINE
#	define SUBSCRIBER_DEFINE 1
	typedef struct Subscriber Subscriber;
#endif

#ifndef RTPS_STATEFULREADER_T_DEFINE
	#define RTPS_STATEFULREADER_T_DEFINE 1
	typedef struct rtps_statefulreader_t rtps_statefulreader_t;
#endif

#ifndef RTPS_WRITERPROXY_T_DEFINE
	#define RTPS_WRITERPROXY_T_DEFINE 1
	typedef struct rtps_writerproxy_t rtps_writerproxy_t;
#endif

#ifndef RTPS_STATEFULWRITER_T_DEFINE
	#define RTPS_STATEFULWRITER_T_DEFINE 1
	typedef struct rtps_statefulwriter_t rtps_statefulwriter_t;
#endif

#ifndef RTPS_READERPROXY_T_DEFINE
	#define RTPS_READERPROXY_T_DEFINE 1
	typedef struct rtps_readerproxy_t rtps_readerproxy_t;
#endif

#ifndef RTPS_READER_T_DEFINE
	#define RTPS_READER_T_DEFINE 1
	typedef struct rtps_reader_t rtps_reader_t;
#endif

#ifndef RTPS_WRITER_T_DEFINE
	#define RTPS_WRITER_T_DEFINE 1
	typedef struct rtps_writer_t rtps_writer_t;
#endif

#ifndef HANDLE_TYPE_NATIVE
	#define HANDLE_TYPE_NATIVE int32_t
#endif
#ifndef HANDLE_TYPE_NATIVE_DEFINE
        #define HANDLE_TYPE_NATIVE_DEFINE 1
        typedef HANDLE_TYPE_NATIVE InstanceHandle_t;
#endif
#ifndef DATAWRITER_DEFINE
        #define DATAWRITER_DEFINE 1
        typedef struct DataWriter DataWriter;
#endif

#ifndef DATAREADER_DEFINE
        #define DATAREADER_DEFINE 1
        typedef struct DataReader DataReader;
#endif

#ifndef TIME_STRUCT_T
	#define TIME_STRUCT_T
	typedef struct Time_t Time_t;
	struct Time_t {
		int32_t sec;
		uint32_t nanosec;
	};
	typedef struct Duration_t {
		int32_t sec;
		uint32_t nanosec;
	}Duration_t;
#endif

#ifndef SAMPLEINFO_DEFINE
	typedef struct SampleInfo SampleInfo;
#	define SAMPLEINFO_DEFINE 1
#endif

struct monitoring_t
{
	MODULE_COMMON_FIELD
	moduleload_t *p_moduleload;
	
	void  (*addParticipant)(monitoring_t *p_monitoring, DomainParticipant *p_participant);
	void  (*deleteParticipant)(monitoring_t *p_monitoring, DomainParticipant *p_participant);

	void  (*addPublisher)(monitoring_t *p_monitoring, Publisher *p_publisher);
	void  (*deletePublisher)(monitoring_t *p_monitoring, Publisher *p_publisher);

	void  (*addSubscriber)(monitoring_t *p_monitoring, Subscriber *p_subscriber);
	void  (*deleteSubscriber)(monitoring_t *p_monitoring, Subscriber *p_subscriber);

	void  (*addTopic)(monitoring_t *p_monitoring, Topic *p_topic);
	void  (*deleteTopic)(monitoring_t *p_monitoring, Topic *p_topic);

	void  (*addDataWriter)(monitoring_t *p_monitoring, DataWriter *p_datawriter);
	void  (*deleteDataWriter)(monitoring_t *p_monitoring, DataWriter *p_datawriter);

	void  (*addDataReader)(monitoring_t *p_monitoring, DataReader *p_datareader);
	void  (*deleteDataReader)(monitoring_t *p_monitoring, DataReader *p_datareader);

	void (*add_writerproxy)(monitoring_t* p_monitoring, rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy);
	void  (*deleteWriterProxy)(monitoring_t *p_monitoring, rtps_statefulreader_t *p_statefulreader, rtps_writerproxy_t *p_writer_proxy);

	void  (*addReaderProxy)(monitoring_t *p_monitoring, rtps_statefulwriter_t *p_statefulwriter, rtps_readerproxy_t *p_reader_proxy);
	void  (*deleteReaderProxy)(monitoring_t *p_monitoring, rtps_statefulwriter_t *p_statefulwriter, rtps_readerproxy_t *p_reader_proxy);

	void  (*warningcodition)(monitoring_t *p_monitoring, int32_t entity, char *condition, int32_t warningorerror, int32_t level, int32_t code);

	monitoring_sys_t	*p_sys;
};



enum WARNINGLEVEL
{
	ERROR1,
	ERROR2,
	ERROR3,
	WARNING1,
	WARNING2,
	WARNING3
};


#ifdef __cplusplus
extern "C" {
#endif

	int launch_monitoring_module(module_object_t* p_this);
	void  monitoring__add_participant(DomainParticipant* p_participant);
	void  monitoring__delete_participant(DomainParticipant* p_participant);

	void  monitoring__add_publisher(Publisher* p_publisher);
	void  monitoring__delete_publisher(Publisher* p_publisher);
	void  monitoring__add_subscriber(Subscriber* p_subscriber);
	void  monitoring__delete_subscriber(Subscriber* p_subscriber);
	void  monitoring__add_topic(Topic* p_topic);
	void  monitoring__delete_topic(Topic* p_topic);
	void  monitoring__add_datareader(DataReader* p_datareader);
	void  monitoring__delete_datareader(DataReader* p_datareader);
	void  monitoring__add_datawriter(DataWriter* p_datawriter);
	void  monitoring__delete_datawriter(DataWriter* p_datawriter);


	void monitoring__add_writerproxy(rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy);
	void monitoring__delete_writerproxy(rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy);


	void monitoring__add_readerproxy(rtps_statefulwriter_t* p_statefulwriter, rtps_readerproxy_t* p_readerproxy);
	void monitoring__delete_readerproxy(rtps_statefulwriter_t* p_statefulwriter, rtps_readerproxy_t* p_readerproxy);


	void monitoring__add_writerproxy_for_builtin(rtps_reader_t* p_rtps_reader);
	void monitoring__add_readerproxy_for_builtin(rtps_writer_t* p_rtps_writer);


	void monitoring__warning_condtion(int32_t entity, char* p_condition, int32_t warning_or_error, int32_t level, int32_t code);

#ifdef __cplusplus
}
#endif

#endif
