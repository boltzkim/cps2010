/*
	Foo 관련 헤더
	작성자 : 
	이력
	2010-08-7 : 시작

*/

#if !defined(__FOO_H)
#define __FOO_H


struct Foo
{
	int32_t dummy;
};

//typedef sequence<Foo> FooSeq;

typedef struct FooSeq
{
	int32_t i_seq;
	Foo** pp_foo;
} FooSeq;


static FooSeq INIT_FOOSEQ = {0, NULL};

////////////////////////////////////////////////////////////////////
typedef struct TypeSupport TypeSupport;

#ifndef FOOTYPESUPPORT_DEFINE
	#define FOOTYPESUPPORT_DEFINE 1
	typedef struct FooTypeSupport FooTypeSupport;
#endif

typedef struct FooDataWriter FooDataWriter;
typedef struct FooDataReader FooDataReader;

////////////////////////////////////////////////////////////////////

//typedef enum SUPPORT_TYPE
//{
//	DDS_STRING_TYPE,
//	DDS_INTEGER32_TYPE,
//	DDS_FLOAT_TYPE,
//	DDS_CHARACTER_TYPE,
//	DDS_STRUCT_TYPE
//} SUPPORT_TYPE;

typedef enum SUPPORT_TYPE
{
	DDS_STRING_TYPE,
	DDS_BYTE_TYPE,
	DDS_INTEGER8_TYPE,
	DDS_INTEGER16_TYPE,
	DDS_INTEGER32_TYPE,
	DDS_INTEGER64_TYPE,
	DDS_UINTEGER8_TYPE,
	DDS_UINTEGER16_TYPE,
	DDS_UINTEGER32_TYPE,
	DDS_UINTEGER64_TYPE,
	DDS_FLOAT_TYPE,
	DDS_DOUBLE_TYPE,
	DDS_BOOLEAN_TYPE,
	DDS_CHARACTER_TYPE,
	DDS_STRUCT_TYPE
} SUPPORT_TYPE;

struct dds_parameter_t
{
	SUPPORT_TYPE	type;
	int				i_size;
	char*			p_field_name;
	union
	{
		char*		string;
		int			integer;
		float		real;
		char		character;
	} value;
	bool			is_key;
	long			id;
	int				offset;
};



#define I_PARAMETER(foots, p_field_name, type, size) \
	{\
		dds_parameter_t *p_para = get_new_parameter(p_field_name, type, size);\
		INSERT_PARAM(##foots->pp_parameters,##foots->i_parameters, ##foots->i_parameters, p_para);\
	}

static inline dds_parameter_t* get_new_parameter(char* p_field_name, SUPPORT_TYPE type, int i_size)
{
	dds_parameter_t *p_parameter = (dds_parameter_t *)malloc(sizeof(dds_parameter_t));
	memset(p_parameter, '\0', sizeof(dds_parameter_t));

	p_parameter->i_size = i_size;
	p_parameter->type = type;

	p_parameter->is_key = false;
	p_parameter->offset = 0;


	if(p_field_name)p_parameter->p_field_name = strdup(p_field_name);
	else p_parameter->p_field_name = NULL;

	return p_parameter;
}


static long PARAMETERKEY = 0;


static inline dds_parameter_t *get_new_parameter_key(char* p_field_name, SUPPORT_TYPE type, int i_size)
{
	dds_parameter_t *p_parameter = (dds_parameter_t *)malloc(sizeof(dds_parameter_t));
	memset(p_parameter, '\0', sizeof(dds_parameter_t));

	p_parameter->i_size = i_size;
	p_parameter->type = type;
	p_parameter->is_key = true;


	if (p_field_name) p_parameter->p_field_name = strdup(p_field_name);
	else p_parameter->p_field_name = NULL;

	p_parameter->id = ++PARAMETERKEY;

	return p_parameter;
}



////////////////////////////////////////////////////////////////////


//interface FooTypeSupport : DDS::TypeSupport {
//	DDS::ReturnCode_t register_type(in_dds DDS::DomainParticipant participant, in_dds string type_name);
//	string get_type_name();
//};

struct FooTypeSupport
{
	TYPESUPPORT_BODY
	ReturnCode_t (*register_type)(FooTypeSupport *p_fooTypeSupport, in_dds DomainParticipant *p_participant, in_dds string type_name);
	string (*get_type_name)(FooTypeSupport *p_fooTypeSupport);
	//
	bool (*insert_parameter)(FooTypeSupport *p_fooTypeSupport, string parameter_name, SUPPORT_TYPE type, bool is_key);
	bool (*insert_parameter_struct)(FooTypeSupport *p_fooTypeSupport, string parameter_name, int size, bool is_key);
};

//interface FooDataWriter : DDS::DataWriter {
//	DDS::InstanceHandle_t register_instance(in_dds Foo instance_data);
//	DDS::InstanceHandle_t register_instance_w_timestamp(in_dds Foo instance_data,in_dds DDS::Time_t source_timestamp);
//	DDS::ReturnCode_t unregister_instance(in_dds Foo instance_data, in_dds DDS::InstanceHandle_t handle);
//	DDS::ReturnCode_t unregister_instance_w_timestamp(in_dds Foo instance_data, in_dds DDS::InstanceHandle_t handle, in_dds DDS::Time_t source_timestamp);
//	DDS::ReturnCode_t write( in_dds Foo instance_data, in_dds DDS::InstanceHandle_t handle);
//	DDS::ReturnCode_t write_w_timestamp( in_dds Foo instance_data, in_dds DDS::InstanceHandle_t handle, in_dds DDS::Time_t source_timestamp);
//	DDS::ReturnCode_t dispose( in_dds Foo instance_data, in_dds DDS::InstanceHandle_t instance_handle);
//	DDS::ReturnCode_t dispose_w_timestamp(in_dds Foo instance_data, in_dds DDS::InstanceHandle_t instance_handle, in_dds DDS::Time_t source_timestamp);
//	DDS::ReturnCode_t get_key_value(inout_dds Foo key_holder, in_dds DDS::InstanceHandle_t handle);
//	DDS::InstanceHandle_t lookup_instance(in_dds Foo key_holder);
//};

struct FooDataWriter
{
	DATAWRITER_BODY
	bool b_write_blocked;//by kki...(reliability)(Reliable로 설정되었을때 HistoryCache가 가득차서, block된 것을 표시하기 위한 플래그)
	InstanceHandle_t (*register_instance)(FooDataWriter *p_fooDataWriter, in_dds Foo *p_instance_data);
	InstanceHandle_t (*register_instance_w_timestamp)(FooDataWriter *p_fooDataWriter,in_dds Foo *instance_data,in_dds Time_t source_timestamp);
	ReturnCode_t (*unregister_instance)(FooDataWriter *p_fooDataWriter, in_dds Foo *instance_data, in_dds InstanceHandle_t handle);
	ReturnCode_t (*unregister_instance_w_timestamp)(FooDataWriter *p_fooDataWriter, in_dds Foo *instance_data, in_dds InstanceHandle_t handle, in_dds Time_t source_timestamp);
	ReturnCode_t (*write)(FooDataWriter *p_fooDataWriter, in_dds Foo *p_instance_data, in_dds InstanceHandle_t handle);
	ReturnCode_t (*write_w_timestamp)(FooDataWriter *p_fooDataWriter, in_dds Foo *instance_data, in_dds InstanceHandle_t handle, in_dds Time_t source_timestamp);
	ReturnCode_t (*dispose)(FooDataWriter *p_fooDataWriter, in_dds Foo *instance_data, in_dds InstanceHandle_t instance_handle);
	ReturnCode_t (*dispose_w_timestamp)(FooDataWriter *p_fooDataWriter, in_dds Foo *instance_data, in_dds InstanceHandle_t instance_handle, in_dds Time_t source_timestamp);
	ReturnCode_t (*get_key_value)(FooDataWriter *p_fooDataWriter,inout_dds Foo *key_holder, in_dds InstanceHandle_t handle);
	InstanceHandle_t (*lookup_instance)(FooDataWriter *p_fooDataWriter,in_dds Foo *key_holder);
};


//interface FooDataReader : DDS::DataReader {
//	DDS::ReturnCode_t read(inout_dds FooSeq data_values, inout_dds DDS::SampleInfoSeq sample_infos, in_dds long max_samples, in_dds DDS::SampleStateMask sample_states, in_dds DDS::ViewStateMask view_states, in_dds DDS::InstanceStateMask instance_states);
//	DDS::ReturnCode_t take(inout_dds FooSeq data_values, inout_dds DDS::SampleInfoSeq sample_infos, in_dds long max_samples, in_dds DDS::SampleStateMask sample_states, in_dds DDS::ViewStateMask view_states, in_dds DDS::InstanceStateMask instance_states);
//	DDS::ReturnCode_t read_w_condition(inout_dds FooSeq data_values,inout_dds DDS::SampleInfoSeq sample_infos, in_dds long max_samples, in_dds DDS::ReadCondition a_condition);
//	DDS::ReturnCode_t take_w_condition(inout_dds FooSeq data_values,inout_dds DDS::SampleInfoSeq sample_infos,in_dds long max_samples,in_dds DDS::ReadCondition a_condition);
//	DDS::ReturnCode_t read_next_sample(inout_dds Foo data_value,inout_dds DDS::SampleInfo sample_info);
//	DDS::ReturnCode_t take_next_sample(inout_dds Foo data_value,inout_dds DDS::SampleInfo sample_info);
//	DDS::ReturnCode_t read_instance(inout_dds FooSeq data_values,inout_dds DDS::SampleInfoSeq sample_infos,in_dds long max_samples,in_dds DDS::InstanceHandle_t a_handle,in_dds DDS::SampleStateMask sample_states,in_dds DDS::ViewStateMask view_states, in_dds DDS::InstanceStateMask instance_states);
//	DDS::ReturnCode_t take_instance(inout_dds FooSeq data_values,inout_dds DDS::SampleInfoSeq sample_infos,in_dds long max_samples,in_dds DDS::InstanceHandle_t a_handle,in_dds DDS::SampleStateMask sample_states,in_dds DDS::ViewStateMask view_states,in_dds DDS::InstanceStateMask instance_states);
//	DDS::ReturnCode_t read_next_instance(inout_dds FooSeq data_values,inout_dds DDS::SampleInfoSeq sample_infos,in_dds long max_samples,in_dds DDS::InstanceHandle_t previous_handle,in_dds DDS::SampleStateMask sample_states,in_dds DDS::ViewStateMask view_states,in_dds DDS::InstanceStateMask instance_states);
//	DDS::ReturnCode_t take_next_instance(inout_dds FooSeq data_values,inout_dds DDS::SampleInfoSeq sample_infos,in_dds long max_samples,in_dds DDS::InstanceHandle_t previous_handle,in_dds DDS::SampleStateMask sample_states,in_dds DDS::ViewStateMask view_states,in_dds DDS::InstanceStateMask instance_states);
//};

struct FooDataReader
{
	DATAREADER_BODY
	ReturnCode_t (*read)(FooDataReader *p_fooDataReader, inout_dds FooSeq *p_data_values, inout_dds SampleInfoSeq *p_sample_infos, in_dds long max_samples, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states);
	ReturnCode_t (*take)(FooDataReader *p_fooDataReader, inout_dds FooSeq *p_data_values, inout_dds SampleInfoSeq *p_sample_infos, in_dds long max_samples, in_dds SampleStateMask sample_states, in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states);
	ReturnCode_t (*read_w_condition)(FooDataReader *p_fooDataReader, inout_dds FooSeq *p_data_values,inout_dds SampleInfoSeq *p_sample_infos, in_dds long max_samples, in_dds ReadCondition *p_condition);
	ReturnCode_t (*take_w_condition)(FooDataReader *p_fooDataReader, inout_dds FooSeq *p_data_values,inout_dds SampleInfoSeq *p_sample_infos,in_dds long max_samples,in_dds ReadCondition *p_condition);
	ReturnCode_t (*read_next_sample)(FooDataReader *p_fooDataReader, inout_dds Foo **data_value,inout_dds SampleInfo **sample_info);
	ReturnCode_t (*take_next_sample)(FooDataReader *p_fooDataReader, inout_dds Foo **data_value,inout_dds SampleInfo **sample_info);
	ReturnCode_t (*read_instance)(FooDataReader *p_fooDataReader, inout_dds FooSeq *data_values,inout_dds SampleInfoSeq *sample_infos,in_dds long max_samples,in_dds InstanceHandle_t a_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states, in_dds InstanceStateMask instance_states);
	ReturnCode_t (*take_instance)(FooDataReader *p_fooDataReader, inout_dds FooSeq *data_values,inout_dds SampleInfoSeq *sample_infos,in_dds long max_samples,in_dds InstanceHandle_t a_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
	ReturnCode_t (*read_next_instance)(FooDataReader *p_fooDataReader, inout_dds FooSeq *data_values,inout_dds SampleInfoSeq *sample_infos,in_dds long max_samples,in_dds InstanceHandle_t *previous_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
	ReturnCode_t (*take_next_instance)(FooDataReader *p_fooDataReader, inout_dds FooSeq *data_values,inout_dds SampleInfoSeq *sample_infos,in_dds long max_samples,in_dds InstanceHandle_t *previous_handle,in_dds SampleStateMask sample_states,in_dds ViewStateMask view_states,in_dds InstanceStateMask instance_states);
	ReturnCode_t (*get_key_value)(FooDataReader *p_fooDataReader, inout_dds Foo *key_holder, in_dds InstanceHandle_t handle);
	InstanceHandle_t (*lookup_instance)(FooDataReader *p_fooDataReader, in_dds Foo *key_holder);
};

#ifdef __cplusplus
extern "C" {
#endif
	Duration_t current_duration();
	Time_t currenTime();

	void init_foo_type_support(FooTypeSupport* p_foo_type_support);
	void init_foo_datawriter(FooDataWriter* p_foo_datawriter);
	void init_foo_datareader(FooDataReader* p_foo_datareader);
	FooTypeSupport* get_new_foo_type_support();

	void  register_type(module_object_t* p_this, FooTypeSupport* p_foo_type_support, in_dds DomainParticipant* p_participant, in_dds string type_name);
	string get_type_name(module_object_t* p_this, FooTypeSupport* p_foo_type_support);
	FooTypeSupport* domain_participant_find_support_type(module_object_t* p_this, DomainParticipant* p_participant, string type_name);
	FooTypeSupport* get_type_support(string type_name);
	void remove_foo_seq(FooSeq* p_fseq, int index);
	bool cpskey__is_same(const cpskey_t key_1, const cpskey_t key_2);

#ifdef __cplusplus
}
#endif

#endif
