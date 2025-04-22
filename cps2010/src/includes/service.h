/*
	서비스 관련 헤더
	작성자 : 
	이력
	2010-07-24 : 시작 일단 윈도우만 우선 선언 어느 정도 되면 다른 플랫폼 추가

*/

#if !defined(__SERVICE_H)
#define __SERVICE_H

typedef struct service_sys_t service_sys_t;

#ifndef ENTITY_DEFINE
        #define ENTITY_DEFINE 1
        typedef struct Entity Entity;
#endif

#ifndef DOMAINPARTICIPANT_DEFINE
        #define DOMAINPARTICIPANT_DEFINE 1
        typedef struct DomainParticipant DomainParticipant;
#endif

#ifndef RTPS_WRITER_T_DEFINE
        #define RTPS_WRITER_T_DEFINE 1
        typedef struct rtps_writer_t rtps_writer_t;
#endif

#ifndef HANDLE_TYPE_NATIVE
#ifdef _M_IX86
#	define HANDLE_TYPE_NATIVE int32_t
#else
#	define HANDLE_TYPE_NATIVE int64_t
#endif
#endif
#ifndef HANDLE_TYPE_NATIVE_DEFINE
        #define HANDLE_TYPE_NATIVE_DEFINE 1
        typedef HANDLE_TYPE_NATIVE InstanceHandle_t;
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

enum SERVICE_ACCESS_OUT_TYPE
{
	SPDP_OUT_TYPE,
	SEDP_OUT_TYPE,
	DEFAULT_OUT_TYPE
};

#ifndef DCPS_STATE_DEFINE
#define DCPS_STATE_DEFINE 1

// Sample states to support reads
//typedef uint32_t SampleStateKind;
typedef enum SampleStateKind
{
	READ_SAMPLE_STATE = 0x0001 << 0,
	 NOT_READ_SAMPLE_STATE = 0x0001 << 1,
}SampleStateKind;
// This is a bit-mask SampleStateKind
//typedef uint32_t SampleStateMask;
typedef enum SampleStateMask
{
	ANY_SAMPLE_STATE = 0xffff
}SampleStateMask;
// View states to support reads
//typedef uint32_t ViewStateKind;

typedef enum ViewStateKind
{
	NEW_VIEW_STATE = 0x0001 << 0,
	NOT_NEW_VIEW_STATE = 0x0001 << 1,

}ViewStateKind;
// This is a bit-mask ViewStateKind
//typedef uint32_t ViewStateMask;
typedef enum ViewStateMask
{
	ANY_VIEW_STATE = 0xffff
}ViewStateMask;
// Instance states to support reads
//typedef uint32_t InstanceStateKind;
typedef enum InstanceStateKind
{
	ALIVE_INSTANCE_STATE = 0x0001 << 0,
	NOT_ALIVE_DISPOSED_INSTANCE_STATE = 0x0001 << 1,
	NOT_ALIVE_NO_WRITERS_INSTANCE_STATE = 0x0001 << 2
}InstanceStateKind;
// This is a bit-mask InstanceStateKind
//typedef uint32_t InstanceStateMask;
typedef enum InstanceStateMask
{
	ANY_INSTANCE_STATE = 0xffff,
	NOT_ALIVE_INSTANCE_STATE = 0x006
}InstanceStateMask;

//static const StatusMask ANY_STATUS_STATE = 0xffff;

#endif

struct service_t
{
    MODULE_COMMON_FIELD

	moduleload_t*	p_moduleload;
	
	int				(*add_entity)(module_object_t* p_this, Entity* p_entity);
	int				(*remove_entity)(module_object_t* p_this, Entity* p_entity);
	void*			(*write)(module_object_t * p_this, rtps_writer_t *p_writer, message_t *p_message);
	void*			(*dispose)(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle);
	void*			(*unregister)(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle, bool b_dispose);
	access_out_t*	(*getaccessout)(module_object_t * p_this, int type);
	message_t**		(*read)(module_object_t * p_this, DataReader *p_datareader, int32_t max_samples, int *p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);
	message_t**		(*take)(module_object_t * p_this, DataReader *p_datareader, int32_t max_samples, int *p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);
	message_t**		(*read_instance)(module_object_t * p_this, DataReader *p_datareader, InstanceHandle_t handle, int32_t max_samples, int *p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);
	message_t**		(*take_instance)(module_object_t * p_this, DataReader *p_datareader, InstanceHandle_t handle, int32_t max_samples, int *p_size, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);
	void*			(*write_timestamp)(module_object_t * p_this, rtps_writer_t *p_writer, message_t *p_message, Time_t source_timestamp);
	void*			(*dispose_timestamp)(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle, Time_t source_timestamp);
	void*			(*unregister_timestamp)(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle, Time_t source_timestamp);
	
	int				(*add_thread)(module_object_t* p_this, Entity* p_entity);

	data_fifo_t*	p_network_fifo;
	service_sys_t*	p_sys;
	data_t			*p_networkdata[3];
};

#ifdef __cplusplus
extern "C" {
#endif

	int launch_service_module(module_object_t* p_this);
	void add_entity_to_service(Entity* p_entity);
	void remove_entity_to_service(Entity* p_entity);
	void write_to_service(rtps_writer_t* p_rtps_writer, message_t* p_message);
	void dispose_to_service(rtps_writer_t* p_rtps_writer, void* v_data, InstanceHandle_t handle);
	void dispose_to_service_timestamp(rtps_writer_t* p_rtps_writer, void* v_data, InstanceHandle_t handle, Time_t source_timestamp);
	void unregister_to_service(rtps_writer_t* p_rtps_writer, void* v_data, InstanceHandle_t handle, bool b_dispose);
	void unregister_to_service_timestamp(rtps_writer_t* p_rtps_writer, void* v_data, InstanceHandle_t handle, Time_t source_timestamp);
	

	message_t** message_read_from_service(DataReader* p_datareader, int32_t max_samples, int* p_count, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);
	message_t** message_take_from_service(DataReader* p_datareader, int32_t max_samples, int* p_count, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);
	message_t** message_read_instance_from_service(DataReader* p_datareader, InstanceHandle_t handle, int32_t max_samples, int* p_count, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);
	message_t** message_take_instance_from_service(DataReader* p_datareader, InstanceHandle_t handle, int32_t max_samples, int* p_count, SampleStateMask sample_states, ViewStateMask view_states, InstanceStateMask instance_states);

	void add_thread_to_service(Entity* p_entity);

#ifdef __cplusplus
}
#endif

#define DEFAULT_MTU 1500


#endif
