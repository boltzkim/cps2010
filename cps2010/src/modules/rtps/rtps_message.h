/*
	RTSP message 해더
	이력
	2010-08-10 : 처음 시작
*/


#if !defined(__RTPS_MESSAGE_H)
#define __RTPS_MESSAGE_H
#ifndef __APPLE__
typedef uint16_t ushort;
#endif
typedef enum SubmessageKind
{
	PAD = 0x01, /* Pad */
	ACKNACK = 0x06, /* AckNack */
	HEARTBEAT = 0x07, /* Heartbeat */
	GAP = 0x08, /* Gap */
	INFO_TS = 0x09, /* InfoTimestamp */
	INFO_SRC = 0x0c, /* InfoSource */
	INFO_REPLY_IP4 = 0x0d, /* InfoReplyIp4 */
	INFO_DST = 0x0e, /* InfoDestination */
	INFO_REPLY = 0x0f, /* InfoReply */
	NACK_FRAG = 0x12, /* NackFrag */
	HEARTBEAT_FRAG = 0x13, /* HeartbeatFrag */
	DATA = 0x15, /* Data */
	DATA_FRAG = 0x16, /* DataFrag */
} SubmessageKind;

/*
Enumeration used to identify the protocol.
The following values are reserved by the protocol:
PROTOCOL_RTPS
*/
typedef struct ProtocolId_t ProtocolId_t;

/*
Type used to specify a Submessage flag.
A Submessage flag takes a boolean value and affects the parsing of the Submessage by
the receiver.
*/
typedef bool SubmessageFlag;

/*
Enumeration used to identify the kind of Submessage.
The following values are reserved by this version of the protocol:
DATA, GAP, HEARTBEAT, ACKNACK, PAD, INFO_TS, INFO_REPLY,
INFO_DST, INFO_SRC, DATA_FRAG, NACK_FRAG, HEARTBEAT_FRAG
*/
typedef octet SubmessageKind1;


/*
Type used to hold a timestamp.
Should have at least nano-second resolution.
The following values are reserved by the protocol:
TIME_ZERO
TIME_INVALID
TIME_INFINITE
*/
//Time_t

/*
Type used to encapsulate a count that is incremented monotonically, used to identify
message duplicates.
*/
typedef int32_t Count_t;

/*
Type used to uniquely identify a parameter in a parameter list.
Used extensively by the Discovery Module mainly to define QoS Parameters. A range
of values is reserved for protocol-defined parameters, while another range can be used
for vendor-defined parameters, see Section 8.3.5.9.
*/
typedef int16_t ParameterId_t;

/*
Type used to hold fragment numbers.
Must be possible to represent using 32 bits.
*/
typedef int32_t FragmentNumber_t;


///////////////////////////////////////////////////////////////////////////////////////
struct ProtocolId_t
{
	char	_rtps[4];
};


///////////////////////////////////////////////////////////////////////////////////////
typedef struct Header Header;
typedef struct SubmessageHeader SubmessageHeader;
typedef struct Receiver Receiver;

struct Header
{
	ProtocolId_t		protocol;
	ProtocolVersion_t	version;
	VendorId_t			vendor_id;
	GuidPrefix_t		guid_prefix;
};

struct SubmessageHeader
{
	SubmessageKind1		submessage_id;
	octet				flags;
	ushort				submessage_length;
};

struct Receiver
{
	ProtocolVersion_t	source_version;
	VendorId_t			source_vendor_id;
	GuidPrefix_t		source_guid_prefix;
	GuidPrefix_t		dest_guid_prefix;
	Locator_t**			pp_unicast_reply_locator_list;
	Locator_t**			pp_multicast_reply_locator_list;
	bool				have_timestamp;
	Time_t				timestamp;
};


typedef struct SubmessageElement SubmessageElement;
typedef struct GuidPrefix GuidPrefix;
typedef struct EntityId EntityId;
typedef struct Flags Flags;
typedef struct SequenceNumber SequenceNumber;
typedef struct SequenceNumberSet SequenceNumberSet;
typedef struct SerializedPayload SerializedPayload;
typedef struct SerializedPayloadFragment SerializedPayloadFragment;
typedef struct Timestamp Timestamp;
typedef struct Count Count;
typedef struct VendorId VendorId;
typedef struct ProtocolVersion ProtocolVersion;
typedef struct LocatorList LocatorList;
typedef struct ParameterList ParameterList;
typedef struct Parameter Parameter;
typedef struct FragmentNumber FragmentNumber;
typedef struct FragmentNumberSet FragmentNumberSet;

typedef struct SerializedPayloadForWriter SerializedPayloadForWriter;
typedef struct ParameterWithValue ParameterWithValue;


#define SUBMESSAGEELEMENT_BODY	

//struct SubmessageElement
//{
//	SUBMESSAGEELEMENT_BODY
//};

struct GuidPrefix
{
	SUBMESSAGEELEMENT_BODY
	GuidPrefix_t	value;
};

struct EntityId
{
	SUBMESSAGEELEMENT_BODY
	EntityId_t		value;
};

struct Flags
{
	SUBMESSAGEELEMENT_BODY
	SubmessageFlag	value;
};

struct SequenceNumber
{
	SequenceNumber_t	value;
};

struct SequenceNumberSet
{
	SequenceNumber_t	bitmap_base;
	uint32_t			numbits;
	uint32_t			bitmap[8];
};

struct SerializedPayload
{
	uint8_t*	p_value;
};

struct SerializedPayloadForWriter
{
	uint8_t*	p_value;
	uint16_t	i_size;
	GUID_t		entity_key_guid;
};

typedef struct SerializedPayloadForReader
{
	uint8_t*	p_value;
	uint16_t	i_size;
	GUID_t		entity_key_guid;
} SerializedPayloadForReader;

struct SerializedPayloadFragment
{
	uint8_t*	p_value;
};

struct Timestamp
{
	Time_t		value;
};

struct Count
{
	Count_t		value;
};

struct VendorId
{
	VendorId_t	value;
};

struct ProtocolVersion
{
	ProtocolVersion_t		value;
};

struct LocatorList
{
	Locator_t**		pp_value;
};

struct ParameterList
{
	_LINKED_LIST_HEAD
};

struct Parameter
{
	ParameterId_t	parameter_id;
	uint16_t		length;
//	uint8_t*		p_value;
};

struct ParameterWithValue
{
	ParameterId_t			parameter_id;
	uint16_t				length;
	uint8_t*				p_value;
	linked_list_atom_t		a_tom;
};


struct FragmentNumber
{
	FragmentNumber_t	value;
};

struct FragmentNumberSet
{
	FragmentNumber_t	base;
	FragmentNumber_t**	pp_set;
};


///////////////////////////////////////////////////////////////////////////////////////

//typedef struct Submessage Submessage;
//struct Submessage
//{
//
//};

typedef struct Data Data;
typedef struct DataFrag DataFrag;
typedef struct Gap Gap;
typedef struct Heartbeat Heartbeat;
typedef struct AckNack AckNack;
typedef struct HeartbeatFrag HeartbeatFrag;
typedef struct NackFrag NackFrag;
typedef struct Pad Pad;
typedef struct InfoTimestamp InfoTimestamp;
typedef struct InfoReply InfoReply;
typedef struct InfoDestination InfoDestination;
typedef struct InfoSource InfoSource;
typedef struct DataFull DataFull;
typedef struct DataFragFull DataFragFull;

#define DATA_BODY_T	\
	uint16_t			extra_flags;			\
	short				octets_to_inline_qos;	\
	EntityId			reader_id;				\
	EntityId			writer_id;				\
	SequenceNumber		writer_sn;				\


struct Data
{
	DATA_BODY_T
	//ParameterList		inline_qos;
	//SerializedPayload	p_serialized_data;
};

struct DataFull
{
	DATA_BODY_T
	ParameterList		inline_qos;
	SerializedPayload*	p_serialized_data;
};


typedef struct DataFullList
{
	DATA_BODY_T
	ParameterList		inline_qos;
	ParameterList		list;
}DataFullList;


#define DATAFRAG_BODY_T	\
	DATA_BODY_T \
	FragmentNumber		fragment_starting_num;\
	ushort				fragments_in_submessage;\
	ushort				fragment_size;\
	uint32_t			data_size;\

struct DataFrag
{
	DATAFRAG_BODY_T
	//ParameterList		inline_qos;
	//SerializedPayload	*p_serialized_data;
};

struct DataFragFull
{
	DATAFRAG_BODY_T
	ParameterList		inline_qos;
	SerializedPayload*	p_serialized_data;
};

struct Gap
{
	EntityId			reader_id;
	EntityId			writer_id;
	SequenceNumber		gap_start;
	SequenceNumberSet	gap_list;
};

struct Heartbeat
{
	EntityId			reader_id;
	EntityId			writer_id;
	SequenceNumber		first_sn;
	SequenceNumber		last_sn;
	Count				count;
};

struct AckNack
{
	EntityId			reader_id;
	EntityId			writer_id;
	SequenceNumberSet	reader_sn_state;
	Count				count;
};

struct HeartbeatFrag
{
	EntityId			reader_id;
	EntityId			writer_id;
	SequenceNumber		writer_sn;
	FragmentNumber		last_fragment_num;
	Count				count;
};

struct NackFrag
{
	EntityId			reader_id;
	EntityId			writer_id;
	SequenceNumber		writer_sn;
	SequenceNumberSet	fragment_number_state;
	Count				count;
};

struct Pad
{
	int umm;/////
};

struct InfoTimestamp
{
	Timestamp	timestamp;
};

struct InfoReply
{
	LocatorList		multicast_locator_list;
	LocatorList		unicast_locator_list;
};

struct InfoDestination
{
	GuidPrefix		guid_prefix;
};

struct InfoSource
{
	ProtocolVersion	protocol_version;
	VendorId		vendor_id;
	GuidPrefix		guid_prefix;
};


///////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif



#define FOUND_DATA				0x00000001 
#define FOUND_DATAFRAG			0x00000001 << 1
#define FOUND_GAP				0x00000001 << 2
#define FOUND_HEARTBEAT			0x00000001 << 3
#define FOUND_ACKNACK			0x00000001 << 4
#define FOUND_HEARTBEATFRAG		0x00000001 << 5
#define FOUND_NACKFRAG			0x00000001 << 6
#define FOUND_PAD				0x00000001 << 7
#define FOUND_INFOTIMESTAMP		0x00000001 << 8
#define FOUND_INFOREPLY			0x00000001 << 9
#define FOUND_INFODESTINATION	0x00000001 << 10
#define FOUND_INFOSOURCE		0x00000001 << 11
#define FOUND_INFOREPLY_IP4		0x00000001 << 12



#endif


