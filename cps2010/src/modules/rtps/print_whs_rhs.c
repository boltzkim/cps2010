/*******************************************************************************
 * Copyright (c) 2014 Electronics and Telecommunications Research Institute
 * (ETRI) All Rights Reserved.
 * 
 * Following acts are STRICTLY PROHIBITED except when a specific prior written
 * permission is obtained from ETRI or a separate written agreement with ETRI
 * stipulates such permission specifically:
 * a) Selling, distributing, sublicensing, renting, leasing, transmitting,
 * redistributing or otherwise transferring this software to a third party;
 * b) Copying, transforming, modifying, creating any derivatives of, reverse
 * engineering, decompiling, disassembling, translating, making any attempt to
 * discover the source code of, the whole or part of this software in source or
 * binary form;
 * c) Making any copy of the whole or part of this software other than one copy
 * for backup purposes only; and
 * d) Using the name, trademark or logo of ETRI or the names of contributors in
 * order to endorse or promote products derived from this software.
 * 
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. IN NO EVENT WILL ETRI (OR ITS
 * LICENSORS, IF ANY) BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES,
 * HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN
 * IF ETRI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * 
 * Any permitted redistribution of this software must retain the copyright
 * notice, conditions, and disclaimer as specified above.
 ******************************************************************************/

/*
	ST 를 위해 WHS 와 RHS 를 출력하기 위한 함수 구현

	2014-04-23 : 작성, 김경일
	2014-04-28 : set_whs_file(), set_rhs_file() 작성, 김경일
*/

#include "print_whs_rhs.h"


static FILE* p_whs_file = NULL;
static FILE* p_rhs_file = NULL;


/************************************************************************/
/*                                                                      */
/************************************************************************/
void set_whs_file(FILE* const p_file)
{
	p_whs_file = p_file;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
void set_rhs_file(FILE* const p_file)
{
	p_rhs_file = p_file;
}


extern void caculate_message_data_rtps_to_type_support(message_t* p_message, FooTypeSupport* p_type_support, char* support_value, int i_typesupport_size, char* p_value, int i_size);
extern Time_t time_subtraction(const Time_t minuend_time, const Time_t subtrahend_time);
/************************************************************************/
/*                                                                      */
/************************************************************************/
static void print_seq_key(FILE* const p_file, FooTypeSupport* const p_typesupport, char* const p_value, const int i_size)
{
	message_t* p_message = NULL;
	int32_t i = 0;
	dds_parameter_t* p_parameter = NULL;
	int32_t seq = 0;
	int32_t key = 0;
	int32_t offset = 0;
	Time_t current_time = {0, 0};
	Time_t interval_time = {0, 0};
	static Time_t before_time = {0,0};

	p_message = malloc(sizeof(message_t));
	memset(p_message, 0, sizeof(message_t));

	p_message->v_data = malloc(p_typesupport->i_size);
	memset(p_message->v_data, 0, p_typesupport->i_size);

	caculate_message_data_rtps_to_type_support(p_message, p_typesupport, (char*)p_message->v_data, p_message->i_datasize, p_value, i_size);

	for (i = 0; i < p_typesupport->i_parameters; i++)
	{
		p_parameter = p_typesupport->pp_parameters[i];

		if (strcmp(p_parameter->p_field_name, "seq") == 0)
		{
			memcpy(&seq, (char*)p_message->v_data+offset, p_parameter->i_size);
		}
		else if (strcmp(p_parameter->p_field_name, "key") == 0)
		{
			memcpy(&key, (char*)p_message->v_data+offset, p_parameter->i_size);
		}

		offset += p_parameter->i_size;
	}

	current_time = currenTime();

	if (before_time.sec != 0 && before_time.nanosec != 0)
	{
		interval_time = time_subtraction(current_time, before_time);
	}

	fprintf(p_file, ">>>>>>>>>> t[%11d.%-9u] i[%11d.%-9u] s[%5d] k[%5d] \r\n", current_time.sec, current_time.nanosec, interval_time.sec, interval_time.nanosec, seq, key);

	if (current_time.sec != 0 && current_time.nanosec != 0)
	{
		before_time = current_time;
	}

	message_release(p_message);
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
void print_whs(const rtps_writer_t* const p_rtps_writer, const SerializedPayloadForWriter* const p_serialized)
{
	module_t* p_module = NULL;
	Topic* p_topic = NULL;
	FooTypeSupport* p_typesupport = NULL;

	if (!p_whs_file || !p_rtps_writer || p_rtps_writer->is_builtin || !p_serialized) return;

	if ((p_module = current_object(get_domain_participant_factory_module_id())) == NULL) return;
	if ((p_topic = p_rtps_writer->p_datawriter->get_topic(p_rtps_writer->p_datawriter)) == NULL) return;
	if ((p_typesupport = domain_participant_find_support_type(OBJECT(p_module), p_topic->get_participant(p_topic), p_topic->type_name)) == NULL) return;

	print_seq_key(p_whs_file, p_typesupport, (char*)p_serialized->p_value+4, p_serialized->i_size);
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
void print_rhs(const rtps_reader_t* const p_rtps_reader, const SerializedPayloadForReader* const p_serialized)
{
	module_t* p_module = NULL;
	TopicDescription* p_topicdescription = NULL;
	FooTypeSupport* p_typesupport = NULL;

	if (!p_rhs_file || !p_rtps_reader || p_rtps_reader->is_builtin || !p_serialized) return;

	if ((p_module = current_object(get_domain_participant_factory_module_id())) == NULL) return;
	if ((p_topicdescription = p_rtps_reader->p_datareader->get_topicdescription(p_rtps_reader->p_datareader)) == NULL) return;
	if ((p_typesupport = domain_participant_find_support_type(OBJECT(p_module), p_topicdescription->p_domain_participant, p_topicdescription->type_name)) == NULL) return;

	print_seq_key(p_rhs_file, p_typesupport, (char*)p_serialized->p_value+4, p_serialized->i_size-4);
}


extern int BoundarySize(int i_size);
/************************************************************************/
/*                                                                      */
/************************************************************************/
void print_whs2(const data_t* const p_pk)
{
	int i_temp = 0;
	SerializedPayloadForWriter* p_serialized = NULL;

	if (!p_whs_file || !p_pk->p_rtps_writer || p_pk->p_rtps_writer->is_builtin) return;

	i_temp = BoundarySize(sizeof(Header))
			//		+ BoundarySize(sizeof(SubmessageHeader)+sizeof(GuidPrefix_t))
			+ BoundarySize(sizeof(SubmessageHeader)+sizeof(Duration_t))
			+ (sizeof(SubmessageHeader))
			+ sizeof(Data)
			+ 36;
	p_serialized = malloc(sizeof(SerializedPayloadForWriter));
	memset(p_serialized, 0, sizeof(SerializedPayloadForWriter));

	p_serialized->i_size = p_pk->i_size - i_temp;
	p_serialized->p_value = p_pk->p_data + i_temp;

	print_whs(p_pk->p_rtps_writer, (SerializedPayloadForWriter*)p_serialized);

	FREE(p_serialized);
}
