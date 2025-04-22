#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

/*
#ifdef PERFTEST_KEYED_TYPE_DDS43
const long MAX_BINDATA_SIZE = 1600;
#else
const long MAX_BINDATA_SIZE = 63000;
#endif

const long KEY_SIZE = 4;

struct TestData_t {

#if defined(PERFTEST_KEYED_TYPE) || defined(PERFTEST_KEYED_TYPE_DDS43)
    octet key[KEY_SIZE]; //@key
#else
    octet key[KEY_SIZE];
#endif

    long entity_id;
    unsigned long seq_num;
    long timestamp_sec;
    unsigned long timestamp_usec;
    long latency_ping;
    sequence<octet, MAX_BINDATA_SIZE> bin_data;
};
*/

#ifndef _KEY
//#define _KEY
#endif

#define TEST_KEY_LENGTH		4
#define TEST_DATA_LENGTH	(100 - 56)

typedef struct TestData_t{
    char key[TEST_KEY_LENGTH];
    long entity_id;
    unsigned long seq_num;
    long timestamp_sec;
    unsigned long timestamp_usec;
    long latency_ping;
	char bin_data[TEST_DATA_LENGTH];
	long sent_flag; // replacement for message size
}TestData_t;

class TestData_tTypeSupport
{
public:
	FooTypeSupport	*fooTS;

public:
	void register_topic_type(DomainParticipant *p_Participant)
	{
		if(p_Participant == NULL) return;
		{
			fooTS = get_new_foo_type_support();
			insert_topic_parameters(fooTS);
			//fooTS->i_size = sizeof(struct TestData_t);
			fooTS->register_type(fooTS,p_Participant,"TestData_t");
		}
	}

	void insert_topic_parameters(FooTypeSupport *fooTS)
	{   
#ifdef _KEY
		fooTS->insert_parameter(fooTS, "key", DDS_INTEGER32_TYPE, true);
#else
		fooTS->insert_parameter(fooTS, "key", DDS_INTEGER32_TYPE, false);
#endif

		fooTS->insert_parameter(fooTS, "entity_id", DDS_INTEGER32_TYPE, false);
		fooTS->insert_parameter(fooTS, "seq_num", DDS_INTEGER32_TYPE, false);
		fooTS->insert_parameter(fooTS, "timestamp_sec", DDS_INTEGER32_TYPE, false);
		fooTS->insert_parameter(fooTS, "timestamp_usec", DDS_INTEGER32_TYPE, false);
		fooTS->insert_parameter(fooTS, "latency_ping", DDS_INTEGER32_TYPE, false);
		fooTS->insert_parameter(fooTS, "bin_data", DDS_INTEGER32_TYPE, false);
		fooTS->insert_parameter_struct(fooTS, "bin_data", (sizeof(char) * TEST_DATA_LENGTH), false);
		fooTS->insert_parameter(fooTS, "sent_flag", DDS_INTEGER32_TYPE, false);


//#ifdef _KEY
//		{
//			dds_parameter_t *p_para = get_new_parameter_key("key", DDS_CHARACTER_TYPE, (sizeof(char) * TEST_KEY_LENGTH));
//			INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
//		}
//#else
//		{
//			dds_parameter_t *p_para = get_new_parameter("key", DDS_CHARACTER_TYPE, (sizeof(char) * TEST_KEY_LENGTH));
//			INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
//		}
//#endif
//		{
//			dds_parameter_t *p_para = get_new_parameter("entity_id", DDS_INTEGER32_TYPE,sizeof(int32_t));
//			INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
//		}
//		{
//			dds_parameter_t *p_para = get_new_parameter("seq_num", DDS_INTEGER32_TYPE,sizeof(int32_t));
//			INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
//		}
//		{
//			dds_parameter_t *p_para = get_new_parameter("timestamp_sec", DDS_INTEGER32_TYPE,sizeof(int32_t));
//			INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
//		}
//		{
//			dds_parameter_t *p_para = get_new_parameter("timestamp_usec", DDS_INTEGER32_TYPE,sizeof(int32_t));
//			INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
//		}
//		{
//			dds_parameter_t *p_para = get_new_parameter("latency_ping", DDS_INTEGER32_TYPE,sizeof(int32_t));
//			INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
//		}
//		{
//			dds_parameter_t *p_para = get_new_parameter("bin_data", DDS_CHARACTER_TYPE, (sizeof(char) * TEST_DATA_LENGTH));
//			INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
//		}
//		{
//			dds_parameter_t *p_para = get_new_parameter("sent_flag", DDS_INTEGER32_TYPE,sizeof(int32_t));
//			INSERT_PARAM(fooTS->pp_parameters,fooTS->i_parameters, fooTS->i_parameters, p_para);
//		}
	}
};
