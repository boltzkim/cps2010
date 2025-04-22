/*
	RTSP Discovery 해더
	이력
	2010-09-27 : 처음 시작
*/


#if !defined(__RTPS_DISCOVERY_H)
#define __RTPS_DISCOVERY_H

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include "../../../src/modules/rtps/rtps.h"


typedef struct SPDPdiscoveredParticipantData SPDPdiscoveredParticipantData;
typedef struct DiscoveredWriterData	DiscoveredWriterData;
typedef struct DiscoveredReaderData DiscoveredReaderData;
typedef struct DiscoveredTopicData DiscoveredTopicData;

typedef struct ParticipantProxy ParticipantProxy;


#define PARTICIPANTPROXY_BODY \
	ProtocolVersion_t	protocol_version;\
	GuidPrefix_t		guid_prefix;\
	VendorId_t			vendor_id;\
	bool				expects_inline_qos;\


struct ParticipantProxy
{
	PARTICIPANTPROXY_BODY
	/*+availableBuiltinEndpoints : BuiltinEndpointSet_t[]
	+metatrafficMulticastLocatorList : Locator_t[]
	+metatrafficUnicastLocatorList : Locator_t[]
	+defaultMulticastLocatorList : Locator_t[]
	+defaultUnicastLocatorList : Locator_t[]
	+@protocolVersion : ProtocolVersion_t
	+manualLivelinessCount : Count_t*/
};


struct SPDPdiscoveredParticipantData {
	ParticipantBuiltinTopicData dds_participant_data;
	ParticipantProxy participant_proxy;
	Duration_t lease_duration;
};

struct DiscoveredWriterData {
	PublicationBuiltinTopicData dds_publication_data;
	rtps_writerproxy_t writer_proxy;
};

struct DiscoveredReaderData {
	SubscriptionBuiltinTopicData dds_subscription_data;
	rtps_readerproxy_t reader_proxy;
	ContentFilterProperty_t content_filter_property;
};

struct DiscoveredTopicData {
	TopicBuiltinTopicData dds_topic_data;
};


#ifdef __cplusplus
extern "C" {
#endif
	int rtps_period_thread_start(discovery_t *p_discovery);
	int rtps_period_thread_end(discovery_t *p_discovery);

#ifdef __cplusplus
}
#endif


#endif