/*
	RTSP 해더
	이력
	2010-08-10 : 처음 시작
*/


#if !defined(__RTPS_H)
#define __RTPS_H

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

typedef uint8_t	octet;

/*
Type used to hold globally-unique RTPS-entity identifiers. These are identifiers used
to uniquely refer to each RTPS Entity in the system.
Must be possible to represent using 16 octets.
The following values are reserved by the protocol: GUID_UNKNOWN
*/
typedef struct GUID_t GUID_t;

/*
Type used to hold the prefix of the globally-unique RTPS-entity identifiers. The
GUIDs of entities belonging to the same participant all have the same prefix (see
Section 8.2.4.3).
Must be possible to represent using 12 octets.
The following values are reserved by the protocol: GUIDPREFIX_UNKNOWN
*/
typedef struct GuidPrefix_t GuidPrefix_t;


/*
Type used to hold the suffix part of the globally-unique RTPS-entity identifiers. The
EntityId_t uniquely identifies an Entity within a Participant.
Must be possible to represent using 4 octets.
The following values are reserved by the protocol: ENTITYID_UNKNOWN
Additional pre-defined values are defined by the Discovery module in Section 8.5.
*/
typedef struct EntityId_t EntityId_t;

/*
Type used to hold sequence numbers.
Must be possible to represent using 64 bits.
The following values are reserved by the protocol:
SEQUENCENUMBER_UNKNOWN
*/
typedef struct SequenceNumber_t SequenceNumber_t;

/*
Type used to represent the addressing information needed to send a message to an
RTPS Endpoint using one of the supported transports.
Should be able to hold a discriminator identifying the kind of transport, an address,
and a port number. It must be possible to represent the discriminator and port
number using 4 octets, the address using 16 octets.
The following values are reserved by the protocol:
LOCATOR_INVALID
LOCATOR_KIND_INVALID
LOCATOR_KIND_RESERVED
LOCATOR_KIND_UDPv4
LOCATOR_KIND_UDPv6
LOCATOR_ADDRESS_INVALID
LOCATOR_PORT_INVALID
*/
typedef struct Locator_t Locator_t;

/*
Enumeration used to distinguish whether a Topic has defined some fields within to
be used as the ‘key’ that identifies data-instances within the Topic. See the DDS
specification for more details on keys.
The following values are reserved by the protocol:
NO_KEY
WITH_KEY
*/
typedef int32_t TopicKind_t;

/*
Enumeration used to distinguish the kind of change that was made to a data-object.
Includes changes to the data or the lifecycle of the data-object.
It can take the values:
ALIVE, NOT_ALIVE_DISPOSED, NOT_ALIVE_UNREGISTERED
*/
typedef int32_t ChangeKind_t;

/*
Enumeration used to indicate the level of the reliability used for communications.
It can take the values:
BEST_EFFORT, RELIABLE.
*/
typedef int32_t ReliabilityKind_t;

/*
Type used to represent the identity of a data-object whose changes in value are
communicated by the RTPS protocol.
*/
//typedef struct InstanceHandle_t InstanceHandle_t;

/*
Type used to represent the version of the RTPS protocol. The version is composed
of a major and a minor version number. See also Section 8.6.
The following values are reserved by the protocol:
PROTOCOLVERSION
PROTOCOLVERSION_1_0
PROTOCOLVERSION_1_1
PROTOCOLVERSION_2_0
PROTOCOLVERSION_2_1
PROTOCOLVERSION is an alias for the most recent version, in this case
PROTOCOLVERSION_2_1
*/
typedef struct ProtocolVersion_t ProtocolVersion_t;

/*
Type used to represent the vendor of the service implementing the RTPS protocol.
The possible values for the vendorId are assigned by the OMG.
The following values are reserved by the protocol:
VENDORID_UNKNOWN
*/
typedef struct VendorId_t VendorId_t;

struct VendorId_t
{
	octet vendor_id[2];
};

///////////////////////////////////////////////////////////////////////////////////////////





static VendorId_t VENDORID_UNKNOWN = {0,0};
static VendorId_t VENDORID_ETRI = {1,9};

#define SEQUENCENUMBER_UNKNOWN {-1,0}

#define LOCATOR_INVALID {LOCATOR_KIND_INVALID, LOCATOR_PORT_INVALID, LOCATOR_ADDRESS_INVALID}
#define LOCATOR_KIND_INVALID -1
#define LOCATOR_ADDRESS_INVALID {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define LOCATOR_PORT_INVALID 0
#define LOCATOR_KIND_RESERVED 0
#define LOCATOR_KIND_UDPv4 1
#define LOCATOR_KIND_UDPv6 2

#define NO_KEY		1
#define WITH_KEY	2

#define BEST_EFFORT 1
#define RELIABLE	3

#define PROTOCOLVERSION_1_0 {1,0}
#define PROTOCOLVERSION_1_1 {1,1}
#define PROTOCOLVERSION_2_0 {2,0}
#define PROTOCOLVERSION_2_1 {2,1}
#define PROTOCOLVERSION		PROTOCOLVERSION_2_1



#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_ANNOUNCER 0x00000001			<< 0
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_DETECTOR 0x00000001			<< 1
#define DISC_BUILTIN_ENDPOINT_PUBLICATION_ANNOUNCER 0x00000001			<< 2
#define DISC_BUILTIN_ENDPOINT_PUBLICATION_DETECTOR 0x00000001			<< 3
#define DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER 0x00000001			<< 4
#define DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR 0x00000001			<< 5
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_PROXY_ANNOUNCER 0x00000001	<< 6
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_PROXY_DETECTOR 0x00000001		<< 7
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_STATE_ANNOUNCER 0x00000001	<< 8
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_STATE_DETECTOR 0x00000001		<< 9;
#define BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_WRITER 0x00000001		<< 10
#define BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_READER 0x00000001		<< 11

#define LOCATORUDPv4_INVALID {0, 0}

static char PARTICIPANT_MESSAGE_DATA_KIND_UNKNOWN[] = {0x00, 0x00, 0x00, 0x00};
static char PARTICIPANT_MESSAGE_DATA_KIND_AUTOMATIC_LIVELINESS_UPDATE[] = {0x00, 0x00, 0x00, 0x01};
static char PARTICIPANT_MESSAGE_DATA_KIND_AUTOMATIC_LIVELINESS_UPDATE_FOR_TEST[] = {0x01, 0x00, 0x00, 0x00};
static char PARTICIPANT_MESSAGE_DATA_KIND_MANUAL_LIVELINESS_UPDATE[] = {0x00, 0x00, 0x00, 0x02};

//ParameterId Values
#define PID_PAD								0x0000
#define PID_SENTINEL						0x0001
#define PID_USER_DATA						0x002c	//UserDataQosPolicy
#define PID_TOPIC_NAME						0x0005	//string<256>
#define PID_TYPE_NAME						0x0007	//string<256>
#define PID_GROUP_DATA						0x002d
#define PID_TOPIC_DATA						0x002e
#define PID_DURABILITY						0x001d
#define PID_DURABILITY_SERVICE				0x001e
#define PID_DEADINE							0x0023
#define PID_LATENCY_BUDGET					0x0027
#define PID_LIVELINESS						0x001b
#define PID_RELIABILITY						0x001a
#define PID_LIFESPAN						0x002b
#define PID_DESTINATION_ORDER				0x0025
#define PID_HISTORY							0x0040
#define PID_RESOURCE_LIMITS					0x0041
#define PID_OWNERSHIP						0x001f
#define PID_OWNERSHIP_STRENGTH				0x0006
#define PID_PRESENTATION					0x0021
#define PID_PARTITION						0x0029
#define PID_TIME_BASED_FILTER				0x0004
#define PID_TRANSPORT_PRIORITY				0x0049
#define PID_PROTOCOL_VERSION				0x0015
#define PID_VENDORID						0x0016
#define PID_UNICAST_LOCATOR					0x002f
#define PID_MULTICAST_LOCATOR				0x0030
#define PID_MULTICAST_IPADDRESS				0x0011
#define PID_DEFAULT_UNICAST_LOCATOR			0x0031
#define PID_DEFAULT_MULTICAST_LOCATOR		0x0048
#define PID_METATRAFFIC_UNICAST_LOCATOR		0x0032
#define PID_METATRAFFIC_MULTICAST_LOCATOR	0x0033
#define PID_DEFAULT_UNICAST_IPADDRESS		0x000c
#define PID_DEFAULT_UNICAST_PORT			0x000e
#define PID_METATRAFFIC_UNICAST_IPADDRESS	0x0045
#define PID_METATRAFFIC_UNICAST_PORT		0x000d
#define PID_METATRAFFIC_MULTICAST_IPADDRESS	0x000b
#define PID_METATRAFFIC_MULTICAST_PORT		0x0046
#define PID_EXPECTS_INLINE_QOS				0x0043
#define PID_PARTICIPANT_MANUAL_LIVENESS_COUNT	0x0034
#define PID_PARTICIPANT_BUILTIN_ENDPOINTS	0x0044
#define PID_PARTICIPANT_LEASE_DURATION		0x0002
#define PID_CONTENT_FILTER_PROPERTY			0x0035
#define PID_PARTICIPANT_GUID				0x0050
#define PID_PARTICIPANT_ENTITYID			0x0051
#define PID_GROUP_GUID						0x0052
#define PID_GROUP_ENTITYID					0x0053
#define PID_COHERENT_SET					0x0056	//added by kyy(Presetation QoS)
#define PID_BUILTIN_ENDPOINT_SET			0x0058
#define PID_PROPERTY_LIST					0x0059
#define PID_TYPE_MAX_SIZE_SERIALIZED		0x0060
#define PID_ENTITY_NAME						0x0062
#define PID_KEY_HASH						0x0070
#define PID_STATUS_INFO						0x0071

#define PID_PERSISTENCE						0x0003
#define PID_TYPE_CHECKSUM					0x0008
#define PID_TYPE2_NAME						0x0009
#define PID_TYPE2_CHECKSUM					0x000a
#define PID_EXPECTS_ACK						0x0010
#define PID_MANAGER_KEY						0x0012
#define PID_SEND_QUEUE_SIZE					0x0013
#define PID_RELIABILITY_ENABLED				0x0014
#define PID_VARGAPPS_SEQUENCE_NUMBER_LIST	0x0017
#define PID_RECV_QUEUE_SIZE					0x0018
#define PID_RELIABILITY_OFFERED				0x0019

//
#define PID_PRODUCT_VERSION					0x8000
#define PID_ENDPOINT_GUID					0x005a


#define CDR_BE		0x0000		//Big Endian
#define CDR_LE		0x0001		//Little Endian
#define PL_CDR_BE	0x0002		//Both the parameter list and its parameters are encapsulated using OMG CDR Big Endian.
#define PL_CDR_LE	0x0003		//Both the parameter list and its parameters are encapsulated using OMG CDR Little Endian.

#define	KIND_OF_ENTITY_UNKNOWN_USER_DEFINED			0x00
#define	KIND_OF_ENTITY_WRITER_WITH_KEY_USER_DEFINED	0x02
#define	KIND_OF_ENTITY_WRITER_NO_KEY_USER_DEFINED	0x03
#define	KIND_OF_ENTITY_READER_WITH_KEY_USER_DEFINED	0x07
#define	KIND_OF_ENTITY_READER_NO_KEY_USER_DEFINED	0x04

#define	KIND_OF_ENTITY_UNKNOWN_BUILTIN				0xc0
#define	KIND_OF_ENTITY_PARTICIPANT_BUILTIN			0xc1
#define	KIND_OF_ENTITY_WRITER_WITH_KEY_BUILTIN		0xc2
#define	KIND_OF_ENTITY_WRITER_NO_KEY_BUILTIN		0xc3
#define	KIND_OF_ENTITY_READER_WITH_KEY_BUILTIN		0xc4
#define	KIND_OF_ENTITY_READER_NO_KEY_BUILTIN		0xc7

struct EntityId_t
{
	octet	entity_key[3];
	octet	entity_kind;
};


static EntityId_t ENTITYID_PARTICIPANT  = {0x00,0x00,0x01,0xc1};
static EntityId_t ENTITYID_SEDP_BUILTIN_TOPIC_WRITER = {0x00,0x00,0x02,0xc2};
static EntityId_t ENTITYID_SEDP_BUILTIN_TOPIC_READER = {0x00,0x00,0x02,0xc7};
static EntityId_t ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER = {0x00,0x00,0x03,0xc2};
static EntityId_t ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER = {0x00,0x00,0x03,0xc7};
static EntityId_t ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER = {0x00,0x00,0x04,0xc2};
static EntityId_t ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER = {0x00,0x00,0x04,0xc7};
static EntityId_t ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER = {0x00,0x01,0x00,0xc2};
static EntityId_t ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER = {0x00,0x01,0x00,0xc7};
static EntityId_t ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER = {0x00,0x02,0x00,0xC2};
static EntityId_t ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER = {0x00,0x02,0x00,0xC7};

#define IS_SAME_ENTITYID(x,y)	(x[0] == y[0] && x[1] == y[1] && x[2] == y[2] && x[3] == y[3])



///////////////////////////////////////////////////////////////////////////////////////////

typedef struct KeyHash_t
{
	octet value[16];
} KeyHash_t;

typedef struct StatusInfo_t
{
	octet value[4];
} StatusInfo_t;

typedef struct TypeWithShortKey
{
	int32_t id;
	char name[6];
} TypeWithShortKey;

///////////////////////////////////////////////////////////////////////////////////////////

struct	GuidPrefix_t
{
	octet _guidprefix[12];
};


struct GUID_t
{
	GuidPrefix_t guid_prefix;
	EntityId_t	 entity_id;
} ;

#define GUIDPREFIX_UNKNOWN {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
static  EntityId_t ENTITYID_UNKNOWN = {0x00, 0x00, 0x00, 0x00};
static GUID_t GUID_UNKNOWN = { GUIDPREFIX_UNKNOWN, {0x00, 0x00, 0x00, 0x00} };

struct Locator_t
{
	int32_t kind;
	uint32_t port;
	octet address[16];
};





struct ProtocolVersion_t
{
	octet major;
	octet minor;
};


enum CHANGE_KIND
{
	ALIVE,
	NOT_ALIVE_DISPOSED,
	NOT_ALIVE_UNREGISTERED
};




struct SequenceNumber_t
{
	int32_t high;
	uint32_t low;
};


///////////////////////////////////////////////////////////////////////////////////////////

#include "rtps_message.h"

///////////////////////////////////////////////////////////////////////////////////////////
typedef struct rtps_entity_t rtps_entity_t;
typedef struct rtps_endpoint_t rtps_endpoint_t;

#ifndef RTPS_PARTICIPANT_T_DEFINE
#	define RTPS_PARTICIPANT_T_DEFINE 1
	typedef struct rtps_participant_t rtps_participant_t;
#endif

typedef struct rtps_historycache_t rtps_historycache_t;

#ifndef RTPS_WRITER_T_DEFINE
	#define RTPS_WRITER_T_DEFINE 1
	typedef struct rtps_writer_t rtps_writer_t;
#endif

#ifndef RTPS_CACHECHANGE_T_DEFINE
	#define RTPS_CACHECHANGE_T_DEFINE 1
	typedef struct rtps_cachechange_t rtps_cachechange_t;
#endif

typedef struct rtps_statelesswriter_t rtps_statelesswriter_t;

#ifndef RTPS_STATEFULWRITER_T_DEFINE
	#define RTPS_STATEFULWRITER_T_DEFINE 1
	typedef struct rtps_statefulwriter_t rtps_statefulwriter_t;
#endif

#ifndef RTPS_READER_T_DEFINE
	#define RTPS_READER_T_DEFINE 1
	typedef struct rtps_reader_t rtps_reader_t;
#endif

typedef struct rtps_statelessreader_t rtps_statelessreader_t;

#ifndef RTPS_STATEFULREADER_T_DEFINE
	#define RTPS_STATEFULREADER_T_DEFINE 1
	typedef struct rtps_statefulreader_t rtps_statefulreader_t;
#endif

typedef struct rtps_readerlocator_t rtps_readerlocator_t;
typedef struct rtps_change_for_reader_t rtps_change_for_reader_t;

#ifndef RTPS_READERPROXY_T_DEFINE
	#define RTPS_READERPROXY_T_DEFINE 1
	typedef struct rtps_readerproxy_t rtps_readerproxy_t;
#endif

#ifndef RTPS_WRITERPROXY_T_DEFINE
	#define RTPS_WRITERPROXY_T_DEFINE 1
	typedef struct rtps_writerproxy_t rtps_writerproxy_t;
#endif

typedef struct rtps_change_from_writer_t rtps_change_from_writer_t;
///////////////////////////////////////////////////////////////////////////////////////////

enum CHANGE_FOR_READER_STATUS_KIND
{
	UNSENT,
	UNACKNOWLEDGED,
	REQUESTED,
	ACKNOWLEDGED,
	UNDERWAY
};

enum CHANGE_FROM_WRITER_STATUS_KIND
{
	LOST,
	MISSING,
	RECEIVED,
	UNKNOWN
};
typedef int ChangeForReaderStatusKind;

typedef struct cache_for_guid
{
	_LINKED_LIST_ATOM

	GUID_t	guid;
	ChangeForReaderStatusKind	status;
	bool	is_relevant;
	

}cache_for_guid;

struct rtps_cachechange_t
{
	_LINKED_LIST_ATOM

	ChangeKind_t		kind;
	GUID_t				writer_guid;
	InstanceHandle_t	instance_handle;
	SequenceNumber_t	sequence_number;
	SerializedPayload*	p_data_value;
	//이 부분은 statefulWriter에서만 사용되는 데, 여기 넣어도 문제 없음
	//ChangeForReaderStatusKind* p_status;
	//bool						is_relevant;
	///
	bool				b_read;
	/// test_for_reliability
	int32_t				is_blockcount;
	//added by kyy(Presentation QoS)///////
	Time_t				source_timestamp;
	Time_t				lifespan_duration;
	bool				is_coherent_set;
	SequenceNumber_t	coherent_set_number;
	//////////////////////////////////////
	//added by kyy(Destination Order QoS)
	Time_t				received_time;
	//////////////////////////////////////
	bool				is_same;
	linked_list_head_t  cache_for_guids;
	ChangeForReaderStatusKind	tmp_status;
	//
	bool				b_isdisposed;
	bool				b_ref_rtps;
	bool				b_ref_dcps;
	message_t*			p_org_message;
	uint8_t				key_guid_for_reader[16];
	rtps_writerproxy_t	*p_rtps_writerproxy;
	

};







struct rtps_historycache_t  //HistoryCache
{
	_LINKED_LIST_HEAD

	//rtps_cachechange_t**	pp_changes;
	//추가
	//uint16_t				i_changes;
	mutex_t					object_lock;
	rtps_reader_t*			p_rtps_reader;
	rtps_writer_t*			p_rtps_writer;
	HistoryQosPolicyKind	history_kind;
	int32_t					history_depth;
	int32_t					 history_max_length; //by kki...(history)
	ReliabilityQosPolicyKind reliability_kind; //by kki...(reliability)
	//added by kyy(Presentation QoS)
	SequenceNumber_t	coherent_set_number;
	//////////////////////////////////////
	

};

#define RTPS_ENTITY_T_BODY	\
	GUID_t				guid;

struct rtps_entity_t
{
	RTPS_ENTITY_T_BODY
};




struct rtps_participant_t
{
	RTPS_ENTITY_T_BODY
	ProtocolVersion_t	protocol_version;
	VendorId_t			vendor_id;
	Locator_t**			pp_default_unicast_locator_list;
	Locator_t**			pp_default_multicast_locator_list;
	//추가
	DomainParticipant*	p_participant;
	int32_t				i_endpoint;
	rtps_endpoint_t**	pp_endpoint;
	rtps_statelesswriter_t*		p_spdp_builtin_participant_writer;
	rtps_statelessreader_t*		p_spdp_builtin_participant_reader;
};

#define	RTPS_ENDPOINT_T_BODY						\
	RTPS_ENTITY_T_BODY								\
	TopicKind_t			topic_kind;					\
	ReliabilityKind_t	reliability_level;			\
	Locator_t**			pp_unicast_locator_list;	\
	Locator_t**			pp_multicast_locator_list;	\
	/*추가*/										\
	rtps_participant_t*	p_rtps_participant;			\
	int32_t				behavior_type;				\
	int32_t				i_unicast_locator;			\
	int32_t				i_multicast_locator;		\
	int32_t				i_entity_type;				\

struct rtps_endpoint_t
{
	RTPS_ENDPOINT_T_BODY
};

typedef struct rtps_topic_t
{
	RTPS_ENDPOINT_T_BODY
	Topic*	p_topic;
}rtps_topic_t;

#define RTPS_WRITER_T_BODY	\
	RTPS_ENDPOINT_T_BODY	\
	DataWriter*				p_datawriter;\
	/**/\
	/*Configures the mode in which the Writer operates. If pushMode==true, then the Writer will push changes to the*/\
	/*reader. If pushMode==false, changes will only be announced via heartbeats and only be sent as response to the request of a reader.*/\
	/*(automatically configured).*/\
	/**/\
	bool				push_mode;\
	/**/\
	/*Protocol tuning parameter that allows the RTPS Writer to repeatedly announce the availability of data by sending a Heartbeat Message.*/\
	/*(automatically configured).*/\
	/**/\
	Duration_t			heartbeat_period;\
	/**/\
	/*Protocol tuning parameter that allows the RTPS Writer to delay the response to a request for data from a negative acknowledgment.*/\
	/*(automatically configured).*/\
	/**/\
	Duration_t			nack_response_delay;\
	/**/\
	/*Protocol tuning parameter that allows the RTPS Writer to ignore requests for data from negative acknowledgments that*/\
	/*arrive ‘too soon’ after the corresponding change is sent.*/\
	/*(automatically configured)
	/**/\
	Duration_t			nack_suppression_duration;\
	/**/\
	/*Internal counter used to assign increasing sequence number to each change made by the Writer.*/\
	/*(used as part of the logic of the virtual machine)*/\
	/**/\
	SequenceNumber_t	last_change_sequence_number;\
	/**/\
	/*Contains the history of CacheChange changes for this Writer.*/\
	/**/\
	rtps_historycache_t*		p_writer_cache;\
	/*추가*/\
	mutex_t						object_lock;\
    cond_t						object_wait;\
	bool						b_end;\
	access_out_t*				p_accessout;\
	bool						b_has_liveliness;\
	int32_t						fragment_size;\
	int32_t						i_heartbeat_frag_count;\
	bool						is_builtin;\
	module_thread_t				thread_id;\
	int32_t						send_count;\
	int64_t						send_bytes;\
	int32_t						send_sample_count;\
	int64_t						send_sample_bytes;\
	int32_t						send_count_throughput;\
	int32_t						send_bytes_throughput;\
	int32_t						send_sample_count_throughput;\
	int32_t						send_sample_bytes_throughput;\
	bool 						b_thread_first;\
	LivelinessQosPolicyKind		liveliness_kind;/*by kki (liveliness)*/ \
	Time_t						liveliness_lease_duration;/*by kki (liveliness)*/ \
	Time_t						liveliness_base_time;/*by kki (liveliness)*/ \
	bool						b_liveliness_alive;/*by kki (liveliness)*/ \
	data_t*						p_liveliness_job;/*by kki (liveliness)*/ \
	Time_t						checked_time; \
	


struct rtps_writer_t
{
RTPS_WRITER_T_BODY
};

#define RTPS_READER_T_BODY	\
	RTPS_ENDPOINT_T_BODY\
	DataReader*			p_datareader;\
	/**/\
	/*Protocol tuning parameter that allows the RTPS Reader to delay the sending of a*/\
	/*positive or negative acknowledgment (see Section 8.4.12.2)*/\
	/**/\
	Duration_t			heartbeat_response_delay;\
	/**/\
	/*Protocol tuning parameter that allows the RTPS Reader to ignore HEARTBEATs */\
	/*that arrive ‘too soon’ after a previous HEARTBEAT was received.*/\
	/**/\
	Duration_t			heartbeat_suppression_duration;\
	/**/\
	/*Contains the history of CacheChange changes for this RTPS Reader.*/\
	/**/\
	rtps_historycache_t*	p_reader_cache;\
	/**/\
	/*Specifies whether the RTPS Reader expects in-line QoS to be sent along with any data.*/\
	/**/\
	bool				expects_inline_qos;\
	/*추가*/\
	mutex_t						object_lock;\
    cond_t						object_wait;\
	bool						b_end;\
	data_fifo_t*				p_data_fifo;\
	module_thread_t				thread_id;\
	Count_t						count;\
	DataFragFull**				pp_datafragfull;\
	int32_t						i_datafragfull;\
	bool						is_builtin;\
	/*added by kyy*/\
	Duration_t					source_timestamp;\
	int32_t						received_count;\
	int64_t						received_bytes;\
	int32_t						received_sample_count;\
	int64_t						received_sample_bytes;\
	int32_t						received_count_throughput;\
	int32_t						received_bytes_throughput;\
	int32_t						received_sample_count_throughput;\
	int32_t						received_sample_bytes_throughput;\
	/*//by kki (ownership) */\
	rtps_exclusive_writer_t**	pp_exclusive_writers;\
	int32_t						i_exclusive_writers;\
	Time_t						checked_time;\

//by kki (ownership)
typedef struct rtps_exclusive_writer_t
{
	rtps_writerproxy_t*	p_writerproxy;
	uint32_t			i_key;
	cpskey_t			keylist[MAX_KEY_NUM];
	bool				b_has_key;
	bool				b_is_owner;
} rtps_exclusive_writer_t;

struct rtps_reader_t
{
	RTPS_READER_T_BODY
};

struct rtps_statelesswriter_t
{
	RTPS_WRITER_T_BODY
	/*
	Protocol tuning parameter that indicates that the StatelessWriter resends all the changes in the writer’s
	HistoryCache to all the Locators periodically each resendPeriod.
	(Automatically configured)
	*/
	Duration_t				resend_data_period;
	/*
	The StatelessWriter maintains the list of locators to which it sends the
	CacheChanges. This list may include both unicast and multicast locators.
	(Automatically configured)
	*/
	rtps_readerlocator_t**	pp_reader_locators;
	//추가
	int32_t					i_reader_locators;

	Time_t					last_send;
};

struct rtps_statefulwriter_t
{
	RTPS_WRITER_T_BODY
	/*
	The StatefulWriter keeps track of all the RTPS Readers matched with it. Each
	matched reader is represented by an instance of the ReaderProxy class.
	(Automatically configured)
	*/
	rtps_readerproxy_t** pp_matched_readers;
	//추가
	int					i_matched_readers;
	Time_t				last_send_heartbeat;
	Time_t				last_nack_time;
	Count_t				heatbeat_count;
	bool				b_enable_multicast;
	bool				b_new_matched_readers;
};






struct rtps_readerlocator_t
{
	/*
	A list of changes in the writer’s HistoryCache that were requested by remote Readers at this ReaderLocator.
	(Automatically configured)
	*/
	linked_list_head_t		requested_changes;
	/*
	A list of changes in the writer’s HistoryCache that have not been sent yet to this ReaderLocator.
	(Automatically configured)
	*/
	linked_list_head_t		unsent_changes;
	/*
	Unicast or multicast locator through which the readers represented by this ReaderLocator can be reached.
	*/
	Locator_t				locator;
	/*
	Specifies whether the readers represented by this ReaderLocator expect inline QoS to be sent with every Data Message.
	*/
	bool					expects_inline_qos;
	//추가
	/*int32_t					i_unsent_changes;*/
	int32_t					i_sent_changes;
	int32_t					i_for_writer_state;
};



struct rtps_change_for_reader_t
{
	ChangeForReaderStatusKind	status;
	bool						is_relevant;
};


struct rtps_readerproxy_t
{
	GUID_t				remote_reader_guid;
	Locator_t**			pp_unicast_locator_list;
	Locator_t**			pp_multicast_locator_list;
	linked_list_head_t	changes_for_reader;
	bool				expects_inline_qos;
	bool				b_is_active;
	//추가
	//int32_t				i_changes_for_reader;
	int32_t				i_unicast_locator_list;
	int32_t				i_multicast_locator_list;
	int32_t				i_for_writer_state;
	int32_t				i_for_writer_fromReader_state;
	mutex_t				object_lock;
	rtps_writer_t*		p_rtps_writer;
	GUID_t				remote_endpoint_guid;

	//added by kyy
	Duration_t			deadline_base_time;
	int32_t				start;
	Duration_t			lifespan;
	//
	octet				flags;
	bool				is_zero; //ack의 base가 0인지 판별
	bool				is_ignore;
	bool				is_heartbeatsend_and_wait_for_ack;
	uint32_t			not_ack_count;
	ReliabilityQosPolicyKind reliablility_kind;
	DurabilityQosPolicyKind durability_kind;//by kki (durability)


	bool					is_destroy;
};


struct rtps_statelessreader_t
{
	RTPS_READER_T_BODY
};

struct rtps_statefulreader_t
{
	RTPS_READER_T_BODY
	/*
	Used to maintain state on the remote Writers matched up with the Reader.
	*/
	rtps_writerproxy_t**	pp_matched_writers;
	//추가
	int32_t					i_matched_writers;
};


struct rtps_writerproxy_t
{
	/*
	Identifies the matched Writer.
	Configured by discovery
	*/
	GUID_t				remote_writer_guid;
	/*
	List of unicast (address, port) combinations that can be used to send messages 
	to the matched Writer or Writers. The list may be empty.
	Configured by discovery
	*/
	Locator_t**			pp_unicast_locator_list;
	/*
	List of multicast (address, port) combinations that can be used to send
	messages to the matched Writer or Writers. The list may be empty.
	Configured by discovery
	*/
	Locator_t**			pp_multicast_locator_list;
	/*
	Used to maintain state on the remote Writers matched up with the Reader.
	List of CacheChange changes received or expected from the matched RTPS Writer.
	implement the behavior of the RTPS protocol.
	*/
	//linked_list_head_t		changes_from_writers;
	//추가
	int32_t					i_unicast_locator_list;
	int32_t					i_multicast_locator_list;
	rtps_cachechange_t      **pp_changes_from_writers;
	int32_t					i_changes_from_writer;
	mutex_t					object_lock;
	cond_t					object_wait;//by kki (liveliness)
	int32_t					i_for_reader_state;
	Time_t					ackcheced_time;
	AckNack					last_acknack;
	Heartbeat				a_remote_heartbeat;
	Count_t					i_ack_count;
	Count_t					i_nack_count;
	rtps_reader_t*			p_rtps_reader;
	//rtps_historycache_t	*p_reader_cache_for_writerproxy;
	SequenceNumber_t		last_available_seq_num;

	//added by kyy
	Duration_t				base_time;
	Duration_t				deadline_base_time;
	int32_t					start;
	Duration_t				lifespan;
	int32_t					i_for_reader_state_for_ack;
	bool					is_first_ack;
	int32_t					ownership_strength;
	//
	int32_t					i_ref_count;
	bool					is_ignore;


	LivelinessQosPolicyKind	liveliness_kind;//by kki (liveliness)
	Time_t					liveliness_lease_duration;//by kki (liveliness)
	Time_t					liveliness_base_time;//by kki (liveliness)
	bool					b_liveliness_alive;//by kki (liveliness)
	
	bool					b_is_deadline_on;//by kki (ownership)

	SequenceNumber_t		**pp_missing_seq_num;
	uint32_t				i_missing_seq_num;
	SequenceNumber_t		max_seq_num;
	SequenceNumber_t		min_seq_num;

	SequenceNumber_t		received_first_seq_num;
	bool					received_first_packet;

	bool					is_destroy;
};

typedef int32_t ChangeFromWriterStatusKind;


struct rtps_change_from_writer_t
{
	ChangeFromWriterStatusKind		status;
	bool							is_relevant;
};

//////////////////////////////////////////////////////////////////////////////////////////
typedef struct ContentFilterProperty_t
{
	char	content_filtered_topic_name[256];
	char	related_topic_name[256];
	char*	p_filter_expression;
	char**	pp_expression_parameters;
} ContentFilterProperty_t;

typedef struct _filterSignature_t
{
	int32_t	value[4];
} _filterSignature_t;

typedef int32_t** FilterResult_t;
typedef _filterSignature_t FilterSignature_t;

typedef struct ContentFilterInfo_t
{
	FilterResult_t		filter_result;
	FilterSignature_t**	pp_filter_signatures;
} ContentFilterInfo_t;

typedef struct Property_t
{
	char* p_name;
	char* p_value;
} Property_t;


typedef struct EntityName_t
{
	int32_t	i_length;
	char	name[12];
} EntityName_t;

typedef struct OriginalWriterInfo_t
{
	GUID_t original_writer_guid;
	SequenceNumber_t original_writer_sn;
	ParameterList original_writer_qos;
} OriginalWriterInfo_t;

typedef uint32_t BuiltinEndpointSet_t;

typedef struct LocatorUDPv4_t
{
	uint32_t address;
	uint32_t port;
} LocatorUDPv4_t;

//////////////////////////////////////////////////////////////////////////////////////////


typedef struct ParticipantMessageData
{
	GuidPrefix_t	participant_guid_prefix;
	octet			kind[4];
	//sequce
	octet   sequence_length[4];
	octet			data[4];

} ParticipantMessageData;




#ifdef __cplusplus
extern "C" {
#endif
	////////////////////////////////////
	void	*rtps_write(module_object_t * p_this, rtps_writer_t *p_writer, message_t *p_message);
	void	*rtps_dispose(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle);
	void	*rtps_unregister(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle, bool b_dispose);
	void 	*rtps_write_timestamp(module_object_t * p_this, rtps_writer_t *p_writer, message_t *p_message, Time_t source_timestamp);
	void	*rtps_dispose_timestamp(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle, Time_t soruce_timestamp);
	void	*rtps_unregister_timestamp(module_object_t * p_this, rtps_writer_t *p_writer, void *v_data, InstanceHandle_t handle, Time_t source_timestamp);

	///////////////////////////////////

	DataFull* rtps_message_parser(module_object_t* p_this, char* p_data, int32_t i_size, GuidPrefix_t** pp_remote_guid_prefix, GuidPrefix_t** pp_dst_guid_prefix, EntityId_t** pp_reader_entity_id, EntityId_t** pp_writer_entity_id);
	int32_t rtps_add_entity(module_object_t* p_this, Entity* p_entity);
	int32_t rtps_remove_entity(module_object_t* p_this, Entity* p_entity);

	int32_t rtps_add_participant(module_object_t* p_this, DomainParticipant* p_participant);
	int32_t rtps_remove_participant(module_object_t* p_this, DomainParticipant* p_participant);

	int32_t rtps_add_datareader(module_object_t* p_this, DataReader* p_datareader);
	int32_t rtps_remove_datareader(module_object_t* p_this, DataReader* p_datareader);

	int32_t rtps_add_datawriter(module_object_t* p_this, DataWriter* p_datawriter);
	int32_t rtps_remove_datawriter(module_object_t* p_this, DataWriter* p_datawriter);

	int32_t rtps_add_topic(module_object_t* p_this, Topic* p_topic);
	int32_t rtps_remove_topic(module_object_t* p_this, Topic* p_topic);

	void init_rtps_entity(rtps_entity_t* p_rtps_entity);
	void init_rtps_endpoint(rtps_endpoint_t* p_endpoint);
	GuidPrefix_t getDefaultPrefix();

	int32_t rtps_add_endpoint_to_participant(module_object_t* p_this, rtps_endpoint_t* p_rtps_endpoint, DomainParticipant* p_participant);
	int32_t rtps_remove_endpoint_from_participant(module_object_t* p_this, Entity* p_entity);

	void destroy_endpoint(rtps_endpoint_t* p_endpoint);


	int32_t rtps_parse_serializedData(uint8_t *p_serializedData,int32_t i_size, DataFull *p_retData);
	int32_t rtps_parse_serializedDataFrag(uint8_t *p_serializedData,int32_t i_size, DataFragFull *p_retData);
	int32_t rtps_parse_Parameter(Parameter a_parameter, char* p_data, ParameterList *);

	void rtps_participant_lock();
	void rtps_participant_unlock();
	rtps_participant_t** get_rtps_participants(int32_t* p_size);
	rtps_participant_t* get_rtps_builtin_participant();

	void rtps_writer_init(rtps_writer_t* p_rtps_writer);
	void rtps_reader_init(rtps_reader_t* p_rtps_reader);
	void rtps_writer_destroy(rtps_writer_t* p_rtps_writer);

	rtps_statelessreader_t *rtps_statelessreader_new();
	void rtps_statelessreader_destory(rtps_statelessreader_t *p_rtps_statelessreader);
	rtps_statelesswriter_t* rtps_statelesswriter_new(module_object_t* p_this, bool expects_inline_qos);
	void rtps_statelesswriter_destory(rtps_statelesswriter_t *p_rtps_statelesswriter);
	void rtps_statelesswriter_unsent_changes_reset(rtps_statelesswriter_t *p_rtps_statelesswriter);

	rtps_historycache_t	*rtps_historycache_new();
	void rtps_historycache_remove_change(rtps_historycache_t *p_historycache, rtps_cachechange_t *p_change);
	void rtps_historycache_destory(rtps_historycache_t *p_historycache);
	SequenceNumber_t rtps_historycache_get_seq_num_max(rtps_historycache_t *p_historycache);
	SequenceNumber_t rtps_historycache_get_seq_num_min(rtps_historycache_t *p_historycache);
	void  trace_historycache(module_object_t* p_this, int mode, rtps_historycache_t* p_historycache);

	rtps_readerlocator_t *rtps_readerlocator_new(Locator_t a_locator, bool expectsInlineQos);
	void rtps_readerlocator_destory(rtps_readerlocator_t *p_rtps_readerlocator);

	void rtps_statelesswriter_reader_locator_add(rtps_statelesswriter_t *p_rtps_statelesswriter, rtps_readerlocator_t *p_rtps_readerlocator);
	rtps_cachechange_t *rtps_readerlocator_next_unsent_change(rtps_readerlocator_t *p_rtps_readerlocator);
	linked_list_head_t rtps_readerlocator_unsent_changes(rtps_readerlocator_t *p_rtps_readerlocator, int32_t *p_size);


	rtps_cachechange_t* rtps_cachechange_new(ChangeKind_t kind, GUID_t guid, SequenceNumber_t seq, SerializedPayload* p_serialized_data, InstanceHandle_t handle);
	void rtps_cachechange_destory(rtps_cachechange_t *p_rtps_cachechange);
	cache_for_guid *rtps_cachechange_get_associated_cache_for_guid(GUID_t guid, rtps_cachechange_t *p_rtps_cachechange);
	rtps_cachechange_t *rtps_historycache_add_change(rtps_historycache_t *p_historycache, rtps_cachechange_t *p_change);
	void rtps_historycache_change_dispose(rtps_historycache_t *p_historycache, rtps_cachechange_t *p_change, GUID_t key_guid);
	void rtps_historycache_change_unregister(rtps_historycache_t *p_historycache, rtps_cachechange_t *p_change, GUID_t key_guid);

	void insert_missing_num(rtps_writerproxy_t	*p_rtps_writerproxy, SequenceNumber_t missing_num);

	DataFull *rtps_data_new(rtps_cachechange_t *p_rtps_cachechange);
	int32_t rtps_send_data_to(rtps_writer_t* p_rtps_writer, Locator_t locator, DataFull* p_data, Time_t message_time);
	int32_t rtps_send_data_with_dst_to(rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, DataFull* p_data, bool is_multicast, Time_t message_time);
	int32_t rtps_send_data_and_heartbeat_with_dst_to(rtps_writer_t* p_rtps_writer, rtps_readerproxy_t *p_rtps_readerproxy, Locator_t locator, DataFull* p_data, bool is_multicast, bool is_final, Time_t message_time);
	int32_t rtps_send_data_with_dst_to_for_remove(rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, DataFull* p_data);
	int32_t rtps_send_data_frag_with_dst_to(rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, DataFull* p_data, Time_t message_time);
	int32_t rtps_send_acknack_with_dst_to(rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, AckNack a_acknack);
	int32_t rtps_send_heartbeat_with_dst_to(rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, Heartbeat a_heartbeat_data, bool is_final);
	int32_t rtps_send_gap_with_dst_to(rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, Gap a_gap, SequenceNumber_t a_sequence_number);
	SerializedPayloadForWriter* rtps_generate_serialized_payload(rtps_writer_t* p_rtps_writer, message_t* p_message);
	int32_t rtps_send_data_frag_to(rtps_writer_t* p_rtps_writer, Locator_t locator, DataFull* p_data, Time_t message_time);
	int32_t rtps_send_heartbeat_frag_with_dst_to(rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, HeartbeatFrag a_heartbeat_frag);
	int32_t rtps_send_nack_frag_with_dst_to(rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, NackFrag a_nack);
	int rtps_send_dispose_unregister_with_dst_to(rtps_writer_t* p_rtps_writer, GUID_t remote_reader_guid, Locator_t locator, bool is_dispose, bool is_unregister);
	SerializedPayloadForWriter* rtps_generate_dispose_unregister(DataWriter* p_datawriter, rtps_writer_t* p_rtps_writer, rtps_writer_t* p_writer, bool is_dispose, bool is_unregister);

	void rtps_get_spdp_multicast_locator(service_t *p_service, Locator_t *p_locator);
	void rtps_get_spdp_unicast_locator(service_t *p_service, Locator_t *p_locator);
	void rtps_get_sedp_multicast_locator(service_t *p_service, Locator_t *p_locator);
	void rtps_get_sedp_unicast_locator(service_t *p_service, Locator_t *p_locator);
	void rtps_get_default_multicast_locator(service_t *p_service, Locator_t *p_locator);
	void rtps_get_default_unicast_locator(service_t *p_service, Locator_t *p_locator);
	


	ParameterWithValue* rtps_make_parameter(ParameterId_t pid, int32_t i_size, void* v_value);
	void destory_datafull(DataFull* p_data);
	void destory_datafull_all(DataFull* p_data);
	void destory_datafull_except_serialized_data(DataFull* p_data);
	void destory_data_frag_full(DataFragFull* p_datafrag);
	void destory_data_frag_full_all(DataFragFull* p_datafrag);
	void destory_data_frag_full_except_serialized_data(DataFragFull* p_datafrag);
	void destory_data_frag_full_all_except_inline_qos(DataFragFull* p_datafrag);
	rtps_endpoint_t* rtps_get_spdp_builtin_participant_reader();
	rtps_endpoint_t* rtps_get_spdp_builtin_participant_writer();
	rtps_endpoint_t* rtps_get_spdp_Builtin_publication_reader();
	rtps_endpoint_t* rtps_get_spdp_builtin_publication_writer();
	rtps_endpoint_t* rtps_get_spdp_builtin_subscription_reader();
	rtps_endpoint_t* rtps_get_spdp_builtin_subscription_writer();
	rtps_endpoint_t* rtps_get_spdp_builtin_participant_message_reader();
	rtps_endpoint_t* rtps_get_spdp_builtin_participant_message_writer();
	rtps_endpoint_t* rtps_get_spdp_builtin_topic_reader();
	rtps_endpoint_t* rtps_get_spdp_builtin_topic_writer();
	rtps_endpoint_t* rtps_get_liveliness_writer();
	rtps_endpoint_t* rtps_get_liveliness_reader();
	rtps_endpoint_t** rtps_compare_reader(GUID_t a_guid, char* p_topic_name, char* p_topic_type, int* p_size);
	rtps_endpoint_t** rtps_compare_writer(GUID_t a_guid, char* p_topic_name, char* p_topic_type, int* p_size);
	rtps_endpoint_t* rtps_find_topic(char* p_topic_name);

	rtps_endpoint_t* rtps_get_endpoint(GUID_t a_matched_guid);

	rtps_statefulwriter_t *rtps_statefulwriter_new(module_object_t * p_this);
	void rtps_statefulwriter_destory(rtps_statefulwriter_t *p_statefulwriter);
	bool rtps_statefulwriter_matched_reader_add(rtps_statefulwriter_t *p_statefulwriter, rtps_readerproxy_t *p_reader_proxy);
	void rtps_statefulwriter_matched_reader_remove(rtps_statefulwriter_t *p_statefulwriter, rtps_readerproxy_t *p_reader_proxy);
	rtps_readerproxy_t *rtps_statefulwriter_matched_reader_lookup(rtps_statefulwriter_t *p_statefulwriter, GUID_t a_reader_guid);

	void set_unicast_and_multicast_spdp(module_object_t* p_this, rtps_endpoint_t* p_endpoint);
	void set_unicast_and_multicast_sedp(module_object_t* p_this, rtps_endpoint_t* p_endpoint);
	void set_unicast_and_multicast_default(module_object_t* p_this, rtps_endpoint_t* p_endpoint);

	rtps_readerproxy_t* rtps_readerproxy_new(rtps_writer_t* p_rtps_writer, GUID_t guid, Locator_t* p_metatraffic_unicast_locator, Locator_t* p_metatraffic_multicast_locator, bool expects_inline_qos, DurabilityQosPolicyKind reader_durability_kind);
	linked_list_head_t rtps_readerproxy_unsent_changes(rtps_readerproxy_t* p_rtps_readerproxy, int32_t* p_size, bool is_just_exist);
	linked_list_head_t rtps_readerproxy_unsent_changes_nolock(rtps_readerproxy_t *p_rtps_readerproxy, int *p_size, bool is_just_exist);
	linked_list_head_t rtps_readerproxy_requested_changes(rtps_readerproxy_t *p_rtps_readerproxy, int32_t *p_size);
	linked_list_head_t rtps_readerproxy_requested_changes_unlock(rtps_readerproxy_t *p_rtps_readerproxy, int32_t *p_size);
	linked_list_head_t rtps_readerproxy_unacked_changes(rtps_readerproxy_t *p_rtps_readerproxy, int32_t *p_size, bool is_just_exist);
	void rtps_readerproxy_destroy(rtps_readerproxy_t *p_rtps_readerproxy);
	void rtps_readerproxy_destroy2(rtps_readerproxy_t *p_rtps_readerproxy);
	void rtps_readerproxy_acked_changes_set(rtps_readerproxy_t *p_rtps_readerproxy, SequenceNumber_t committed_seq_num);
	void rtps_readerproxy_requested_chanages_set(rtps_readerproxy_t *p_rtps_readerproxy, SequenceNumberSet req_seq_num_set);
	rtps_cachechange_t *rtps_readerproxy_next_unsent_chage(rtps_readerproxy_t *p_rtps_readerproxy);
	rtps_cachechange_t *rtps_readerproxy_next_requested_change(rtps_readerproxy_t *p_rtps_readerproxy);

	void check_remote_participant_leaseDuration();
	void add_remote_participant(GUID_t a_remote_participant_guid, Locator_t a_locator, Locator_t a_default_locator, Locator_t a_mlocator, Locator_t a_default_mlocator, Duration_t a_duration);
	void remote_remote_participant(GUID_t a_remote_participant_guid);
	void remove_Participant_and_etc(GUID_t remote_pariticpant_guid);
	Locator_t* get_remote_participant_locator(GUID_t a_remote_guid);
	Locator_t* get_default_remote_participant_locator(GUID_t a_remote_guid);
	Locator_t* get_remote_participant_multicast_locator(GUID_t a_remote_guid);
	Locator_t* get_default_remote_participant_multicast_locator(GUID_t a_remote_guid);
	bool is_exist_participant(GUID_t a_remote_participant_guid);
	bool is_ignore_participant(GuidPrefix_t guid_prefix);
	void ignoreRemoteParticipant(GUID_t a_remoteParticipantGuid);
	void ignoreTopic(DomainParticipant *p_domainparticipant, GUID_t key, char *topic_name, char *type_name);
	void rtps_receive_heartbeat_procedure2(InfoDestination a_info_descript, Heartbeat a_heartbeat, octet flags);
	void heartbeat_wakeup_participant(GuidPrefix_t a_guid_prefix);
	rtps_entity_t* rtps_find_all_entity(GuidPrefix_t a_prefix);

	void reliable_statefulwriter_behavior(rtps_statefulwriter_t* p_rtps_statefulwriter);
	void best_effort_statefulwriter_behavior(rtps_statefulwriter_t* p_rtps_statefulwriter);
	void best_effort_statelesswriter_behavior(rtps_statelesswriter_t* p_rtps_statelesswriter);
	rtps_writerproxy_t* rtps_writerproxy_new(rtps_reader_t* p_rtps_reader, GUID_t guid, Locator_t* p_metatraffic_unicast_locator, Locator_t* p_metatraffic_multicast_locator);
	void rtps_writerproxy_missing_changes_update(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t last_available_seq_num);
	void rtps_writerproxy_lost_changes_update(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t first_available_seq_num);
	SequenceNumber_t rtps_writerproxy_available_changes_max(rtps_writerproxy_t *p_rtps_writerproxy);
	SequenceNumber_t rtps_writerproxy_available_changes_max_for_acknack(rtps_writerproxy_t *p_rtps_writerproxy);
	void rtps_writerproxy_irrelevant_change_set(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t a_seq_num);
	linked_list_head_t rtps_writerproxy_missing_changes(rtps_writerproxy_t *p_rtps_writerproxy, int32_t *p_size);
	rtps_writerproxy_t *rtps_statefulreader_matched_writer_lookup(rtps_statefulreader_t *p_statefulreader, GUID_t a_writer_guid);
	void rtps_writerproxy_received_change_set(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t a_seq_num);
	bool rtps_writerproxy_is_exist_cache(rtps_writerproxy_t *p_rtps_writerproxy, SequenceNumber_t a_seq_num);
	void rtps_writerproxy_destroy(rtps_writerproxy_t *p_rtps_writerproxy);
	void rtps_writerproxy_destroy2(rtps_writerproxy_t *p_rtps_writerproxy);
	void rtps_writerproxy_ref(rtps_writerproxy_t *p_rtps_writerproxy);
	void rtps_writerproxy_unref(rtps_writerproxy_t *p_rtps_writerproxy);

	bool sequnce_number_left_bigger_and_equal(SequenceNumber_t a_left_sequnce_number, SequenceNumber_t a_right_sequnce_number);
	bool sequnce_number_left_bigger(SequenceNumber_t a_left_sequnce_number, SequenceNumber_t a_right_sequnce_number);
	bool sequnce_number_equal(SequenceNumber_t a_left_sequnce_number, SequenceNumber_t a_right_sequnce_number);
	bool is_same_guid(const GUID_t* const p_guid1, const GUID_t* const p_guid2);

	rtps_statefulreader_t *rtps_statefulreader_new();
	void rtps_statefulreader_destory(rtps_statefulreader_t *p_rtps_statefulreader);
	bool rtps_statefulreader_matched_writer_add(rtps_statefulreader_t *p_statefulreader, rtps_writerproxy_t *p_writer_proxy);
	void rtps_statefulreader_matched_writer_remove(rtps_statefulreader_t *p_statefulreader, rtps_writerproxy_t *p_writer_proxy);
	rtps_writerproxy_t *rtps_statefulreader_matched_writer_lookup(rtps_statefulreader_t *p_statefulreader, GUID_t a_writer_guid);

	int32_t find_parameter(Parameter a_parameter, char* p_data, ParameterId_t pid, char** pp_ret, int32_t* p_found);
	void find_in_parameter_list_pl_cdr_le(char* p_buf, int32_t i_size, ParameterId_t pid, char** pp_ret, int32_t* p_found);
	void find_parameter_list(char* p_buf, int32_t i_size, ParameterId_t pid, char** pp_ret, int* p_found);
	void find_ParameterList_for_remove(char *p_buf, int32_t i_size, ParameterId_t pid, char **pp_ret, int32_t *i_found);
	void best_effort_statelessreader_behavior(rtps_statelessreader_t* p_rtps_statelessreader, int32_t state, DataFull** pp_datafull);
	void reliable_statefulreader_behavior(rtps_statefulreader_t* p_statefulreader, DataFull* p_datafull);
	void best_effort_statefulreader_behavior(rtps_statefulreader_t* p_statefulreader, DataFull** pp_datafull, data_t* p_data);
	rtps_endpoint_t** rtps_matched_writerproxy_reader(GUID_t a_matched_guid, EntityId_t reader_id, int* p_size);
	rtps_endpoint_t** rtps_matched_readerproxy_writer(GUID_t a_matched_guid, EntityId_t reader_id, int* p_size);
	void remove_matched_endpoint_proxy(GUID_t remote_pariticpant_guid);

	int rtps_Header(char* p_data, int i_size, Header *p_header);
	int32_t rtps_submessage_header(char* p_data, int32_t i_size, SubmessageHeader* p_header);
	DataFull* rtps_submessage_data(Data a_data, char* p_data, int32_t i_size, uint8_t flag);
	DataFragFull* rtps_submessage_datafrag(DataFrag a_datafrag, char* p_data, int32_t i_size, uint8_t flag);

	void rtps_receive_heartbeat_procedure(rtps_reader_t* p_rtps_reader, GuidPrefix_t a_guid_prefix, /*InfoDestination a_infoDescript,*/ Heartbeat a_heartbeat, octet flags);

	void rtps_receive_acknack_procedure(rtps_reader_t* p_rtps_reader, GuidPrefix_t a_guid_prefix, InfoDestination a_info_descript, AckNack a_acknack, octet flags);
	void rtps_receive_data_procedure(rtps_reader_t* p_rtps_reader, GuidPrefix_t a_guid_prefix, Data a_data, SerializedPayload* p_serialized_data, octet flags, bool b_remove_serialized_data, DataFull* p_datafull);
	void rtps_receive_gap_procedure(rtps_reader_t* p_rtps_reader, GuidPrefix_t a_guid_prefix, InfoDestination a_info_descript, Gap a_gap, octet flags);
	void rtps_receive_heartbeat_frag_procedure(rtps_reader_t* p_rtps_reader, GuidPrefix_t a_guid_prefix, InfoDestination a_info_descript, HeartbeatFrag a_heartbeat_frag, octet flags);
	SequenceNumber_t sequnce_number_inc(SequenceNumber_t a_sequnce_number);
	SequenceNumber_t sequnce_number_dec(SequenceNumber_t a_sequnce_number);
	int32_t sequnce_number_difference(SequenceNumber_t a_last_sequnce_number, SequenceNumber_t a_first_sequnce_number);
	int32_t sequnce_number_difference2(SequenceNumber_t a_last_sequnce_number, SequenceNumber_t a_first_sequnce_number);
	void* rtps_write_from_builtin(rtps_writer_t* p_writer, SerializedPayloadForWriter* p_serialized_data, InstanceHandle_t handle);
	void* rtps_write_from_builtin_rapidly(rtps_writer_t* p_writer, SerializedPayloadForWriter* p_serialized_data, InstanceHandle_t handle);
	void gererate_parameter(char* p_serialized, int32_t* p_size, int16_t pid, int16_t length, void* v_parameter_data);
	

	void destory_liveliness();

	SequenceNumberSet rtps_writerproxy_missing_changes_and_missing_sequencenumberset(rtps_writerproxy_t *p_rtps_writerproxy, int32_t *p_size, SequenceNumberSet missing_seq_num_set);
	int rtps_get_fragmentsize(module_object_t * p_this);

	bool have_topic_name_topic_type_reader(rtps_reader_t* p_rtps_reader, char* p_topic_name, char* p_topic_type);
	bool have_topic_name_topic_type_writer(rtps_writer_t* p_rtps_writer, char* p_topic_name, char* p_topic_type);

	//added by kyy
	//void rxo_compare_pub_writer(rtps_writer_t *p_rtps_writer, rtps_reader_t *p_rtps_reader, SerializedPayload *p_serialized);
	//Time_t get_source_timestamp(message_t* p_message);
	bool compare_timebasedfilter(Duration_t receive_time, Duration_t* base_time, Duration_t minimum_seperation);
	void process_lifespan_for_historycache(rtps_statefulreader_t *p_statefulreader);
	void input_lifespan(rtps_cachechange_t *p_rtps_cachechange, rtps_statefulreader_t *p_statefulreader, rtps_writerproxy_t *p_rtps_writerproxy);
	bool compare_deadline(Duration_t receive_time, Duration_t *base_time, Duration_t deadline_period);
	void find_rxo_qos_from_parameterlist(RxOQos *p_rxo_qos, char *value, int32_t i_size);
	void find_publisher_qos_from_parameterlist(PublisherQos *p_pqos, char *value, int32_t i_size);
	bool compare_rxo(RxOQos *p_offered_qos, RxOQos *p_requested_qos);
	void get_requested_qos(RxOQos *p_requested_qos, rtps_reader_t *p_rtps_reader);
	void find_subscriber_qos_from_parameterlist(SubscriberQos *p_sqos, char *value, int32_t i_size);
	void get_offered_qos(RxOQos *p_offered_qos, rtps_writer_t *p_rtps_writer);
	void process_lifespan_for_writer_historycache(rtps_statefulwriter_t *p_statefulwriter);
	void find_lifespan_qos_from_parameterlist(rtps_writerproxy_t *p_writerproxy, char *value, int32_t i_size);
	int32_t find_exclusive_writer(const rtps_reader_t* const p_rtps_reader, const GUID_t* const p_writer_guid);
	void set_default_port(int port);
	int get_default_port();
	void set_metatraffic_port(int port);
	int get_metatraffic_port();

	bool rtps_remove_remote_entity(DataFull *p_datafull);
	void update_writerproxy_from_rtps_reader(rtps_writerproxy_t** p_rtps_writerproxy);
	void update_readerproxy_from_rtps_writer(rtps_readerproxy_t** p_rtps_readerproxy);

	bool get_define_user_entity_lock();


	/////
	int reader_thread2(DomainParticipant *p_domainparticipant);
	int writer_thread2(DomainParticipant *p_domainparticipant);


	void set_enable_multicast(service_t* p_service, rtps_statefulwriter_t* p_statefulwriter);
	void rtps_cachechange_ref(rtps_cachechange_t *p_rtps_cachechange, bool isrtps, bool isref);


	void HISTORYCACHE_LOCK(rtps_historycache_t *p_historycache);
	void HISTORYCACHE_UNLOCK(rtps_historycache_t *p_historycache);
	void READER_LOCK(rtps_reader_t *p_reader);
	void READER_UNLOCK(rtps_reader_t *p_reader);
	bool have_topic_name_topic_type_topic(Topic* p_topic, char* p_topic_name, char* p_topic_type);

	////////////////////////////////

	void qosim_dcps_message_ordering_source_timestamp(message_t **pp_message, int i_count);
	int generate_user_data_for_domainparticipant(DomainParticipant* p_participant, char* userdata);
	void qosim_process_coherent_set(rtps_readerproxy_t *p_rtps_readerproxy, rtps_cachechange_t *p_change);
	void qosim_make_coherent_set_parameter(DataFull* p_data, SequenceNumber_t* p_sequence_number);


	rtps_cachechange_t*	check_key_guid(rtps_statefulreader_t* p_statefulreader,  GUID_t writer_guid, DataFull *p_datafull, GUID_t *key_guid, bool *found_key_guid, bool *found_state_info);
	rtps_cachechange_t*	check_key_guid_for_reliability(rtps_statefulreader_t* p_statefulreader,  GUID_t writer_guid, DataFull *p_datafull, SerializedPayload* p_serialized_data, GUID_t *key_guid, bool *found_key_guid, bool *found_state_info);


	void SendHeartBeat(rtps_statefulwriter_t *p_rtps_statefulwriter, rtps_readerproxy_t *p_rtps_readerproxy, bool is_final);
	void check_HeatBeat_Count(rtps_readerproxy_t *p_rtps_readerproxy);
	int *Is_exist_in_cachechange_for_writer(GUID_t a_guid, rtps_cachechange_t *p_change, bool pushmode, bool *p_b_found);
	int *Is_exist_in_cachechange(GUID_t a_guid, rtps_cachechange_t *p_change, ReliabilityKind_t level);
	void ChangeStatefulwriterMulticast(rtps_statefulwriter_t *p_rtps_statefulwriter, rtps_cachechange_t *p_rtps_cachechange, Locator_t *p_matched_locator);
	uint32_t get_millisecond(const Duration_t duration);

	void block_write_for_waiting_ack(rtps_statefulwriter_t *p_rtps_statefulwriter);


	bool all_standby();
	void set_domainparticipant(DomainParticipant *p_domainparticipant);
	void set_participant_writer(rtps_writer_t *p_writer);
	void set_pub_writer(rtps_writer_t *p_writer);
	void set_sub_writer(rtps_writer_t *p_writer);
	void set_topic_writer(rtps_writer_t *p_writer);
	void set_liveness_writer(rtps_writer_t *p_writer);
	void set_participant_reader(rtps_reader_t *p_reader);
	void set_pub_reader(rtps_reader_t *p_reader);
	void set_sub_reader(rtps_reader_t *p_reader);
	void set_topic_reader(rtps_reader_t *p_reader);
	void set_liveness_reader(rtps_reader_t *p_reader);


#ifdef __cplusplus
}
#endif



enum MESSAGE_RETURN_TYPE
{
	MESSAGE_OK,
	MESSAGE_ERROR,
	MESSAGE_ERROR_HEADER,
	MESSAGE_ERROR_SUBMESSAGE_HEADER,
	MESSAGE_ERROR_SIZE,
	MESSAGE_ERROR_INVALID_ENCAPSULATION_SCHEME_IDENTIFIER
};


typedef struct encapsulation_schemes_t
{
	uint8_t Encapsulation_Scheme_Identifier1;
	uint8_t Encapsulation_Scheme_Identifier2;
	uint16_t Encapsulation_Scheme_Identifier_options;
} encapsulation_schemes_t;




// Entity들의 Conditin Polling time

#define POLLING_TIME 10 //10ms

enum TRANSITIOIN_STATE
{
	INITIAL_STATE,
	IDLE_STATE,
	PUSHING_STATE,
	ANY_STATE,
	WAITING_STATE,
	ANNOUNCING_STATE,
	FINAL_STATE,
	MUST_SEND_ACK_STATE,
	MAY_SEND_ACK_STATE,
	MUST_REPAIR_STATE,
	REPAIRING_STATE,
	READY_STATE
};


static Duration_t DefaultleaseDuration = {100, 0};

enum RTPS_ENTITY_TYPE
{
	PARTICIPANT_ENTITY,
	READER_ENTITY,
	WRITER_ENTITY,
	RTPS_TOPIC_ENTITY
};


#define NOT_ACK_COUNT 5

#include "../qos/qos.h"


static inline ParameterWithValue *get_real_parameter(linked_list_atom_t *p_atom)
{
	int size = sizeof(ParameterWithValue) - sizeof(linked_list_atom_t);

	if(p_atom == NULL) return NULL;


	return (ParameterWithValue *)(((char*)p_atom)-size);
}


#endif
