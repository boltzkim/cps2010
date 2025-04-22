/*******************************************************************************
* Copyright (c) 2011 Electronics and Telecommunications Research Institute
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
Destination Order QoS Policy를 위한 모듈.

이력
2012-12-12
*/
#include <core.h>
#include <cpsdcps.h>
#include "../rtps/rtps.h"


/************************************************************************/
/* Presentation QoS Policy                                              */
/* compare 하는 부분 필요                                               */
/* 원격지의 Data SubMessage에서 Presentation에 대한 parsing 필요        */
/************************************************************************/

static PresentationQosPolicyAccessScopeKind qosim_get_presentation_access_scope_kind(PresentationQosPolicy* p_presentation_qos)
{
	return p_presentation_qos->access_scope;
}


static bool qosim_get_coherent_access(PresentationQosPolicy* p_presentation_qos) //NOT USED FUNCTION
{
	return p_presentation_qos->coherent_access;
}


static bool qosim_get_ordered_access(PresentationQosPolicy* p_presentation_qos) //NOT USED FUNCTION
{
	return p_presentation_qos->ordered_access;
}


static void qosim_set_sequence_number_unknown(SequenceNumber_t* p_sequence_number)
{
	p_sequence_number->high = -1;
	p_sequence_number->low = 0;
}


//Coherent_set Parameter를 생성하여 Data Message에 추가
void qosim_make_coherent_set_parameter(DataFull* p_data, SequenceNumber_t* p_sequence_number)
{
	ParameterWithValue*	p_parameter;
	p_parameter = rtps_make_parameter(PID_COHERENT_SET, sizeof(SequenceNumber_t), p_sequence_number);
	insert_linked_list((linked_list_head_t *)&p_data->inline_qos, &p_parameter->a_tom);
	//INSERT_ELEM(p_data->inline_qos.pp_parameters, p_data->inline_qos.i_parameters,	p_data->inline_qos.i_parameters, p_parameter);
}


//Coherent_set Parameter를 Data Message에서 추출
//static ParameterWithValue* qosim_generate_coherent_set_parameter(DataFull* p_datafull) //NOT USED FUNCTION
//{
//	ParameterWithValue*	p_parameter;
//	int i;
//
//
//	for (i = 0; i < p_datafull->inline_qos.i_parameters; i++)
//	{
//		if (p_datafull->inline_qos.pp_parameters[i]->parameter_id == PID_COHERENT_SET && p_datafull->inline_qos.pp_parameters[i]->length == 8)
//		{
//			p_parameter = p_datafull->inline_qos.pp_parameters[i];
//		}
//	}
//	return p_parameter;
//}


//Coherent_set Parameter를 Data Message에서 추출
//static ParameterWithValue* qosim_generate_coherent_set_parameter_i(DataFull* p_datafull, int i) //NOT USED FUNCTION
//{
//	ParameterWithValue*	p_parameter;
//
//	if (p_datafull->inline_qos.pp_parameters[i]->parameter_id == PID_COHERENT_SET && p_datafull->inline_qos.pp_parameters[i]->length == 8)
//	{
//		p_parameter = p_datafull->inline_qos.pp_parameters[i];
//	}
//
//	return p_parameter;
//}


static void qosim_set_coherent_set_number(rtps_historycache_t* p_historycache, SequenceNumber_t* p_sequence_number)
{
	p_historycache->coherent_set_number.high = p_sequence_number->high;
	p_historycache->coherent_set_number.low = p_sequence_number->low;
}


void qosim_process_coherent_set(rtps_readerproxy_t* p_rtps_readerproxy, rtps_cachechange_t* p_change)
{
	if (p_rtps_readerproxy->p_rtps_writer->p_datawriter->p_publisher->begin_coherent == true && p_rtps_readerproxy->p_rtps_writer->p_datawriter->begin_coherent == false)
	{
		qosim_set_coherent_set_number(p_rtps_readerproxy->p_rtps_writer->p_writer_cache, &p_change->sequence_number);
		p_rtps_readerproxy->p_rtps_writer->p_datawriter->begin_coherent = true;
		//printf("The First\n");
	}

	else if (p_rtps_readerproxy->p_rtps_writer->p_datawriter->p_publisher->begin_coherent == false && p_change->p_data_value->p_value == NULL)
	{
		p_rtps_readerproxy->p_rtps_writer->p_datawriter->begin_coherent = false;
		qosim_set_sequence_number_unknown(&p_rtps_readerproxy->p_rtps_writer->p_writer_cache->coherent_set_number);
		//printf("Coherent Set Number is changed!");
	}
}


/*
void qosim_writer_side_presentation(rtps_writer_t* p_rtps_writer)
{
	int i;

	switch(qosim_get_presentation_access_scope_kind((&p_rtps_writer->p_dataWriter->p_publisher->publisherQos.presentation)))
		{
		case INSTANCE_PRESENTATION_QOS:
			printf("INSTANCE_PRESENTATION_QOS");
			
			break;
		case TOPIC_PRESENTATION_QOS:
			printf("TOPIC_PRESENTATION_QOS");
			
			for(i = 0; i < p_rtps_writer->writer_cache->i_changes; i++)
			{
//				p_rtps_writer->writer_cache->changes[i]->coherent_set_number;
			}
			
			break;
		case GROUP_PRESENTATION_QOS:
			printf("GROUP_PRESENTATION_QOS");
			//추후 구현(2013)
			break;
		default :
			break;
		}
}


//추가 구현
void qosim_find_presentation_qos_from_parameterlist(PresentationQosPolicy* p_presentation_qos, char* value, int32_t i_size)
{
	char* p_value = NULL;
	int32_t	i_found = 0;

	find_ParameterList(value, i_size, PID_PRESENTATION, &p_value, &i_found);
	if (i_found && p_value)
	{
		p_presentation_qos->access_scope = *p_value;
		p_presentation_qos->coherent_access = *(p_value+4);
		p_presentation_qos->ordered_access = *(p_value+5);
	}
	i_found = 0;
}
/*
void qosim_writer_side_presentation(rtps_writer_t* p_rtps_writer, bool is_end_coherent_change, PresentationQosPolicy* p_presentation_qos)
{
	SequenceNumber_t* p_coherent_set_number = NULL;

	int i;
	int j, k;

	for (i = 0; i < p_rtps_writer->writer_cache->i_changes; i++)
	{
		//PID_COHERENT_SET Parameter에 sequenceNumber를 입력한다.
		ParameterWithValue*	p_parameter = rtps_make_parameter(PID_COHERENT_SET, sizeof(SequenceNumber_t), &p_rtps_writer->writer_cache->changes[i]->sequence_number);

		switch (qosim_get_presentation_access_scope_kind((&p_rtps_writer->p_dataWriter->p_publisher->publisherQos.presentation)))//p_presentation_qos))
		{
		case INSTANCE_PRESENTATION_QOS:
			printf("INSTANCE_PRESENTATION_QOS");
			for (j = 0; j < p_rtps_writer->p_dataWriter->i_instanceset; j++)
				for (k = 0; j<p_rtps_writer->p_dataWriter->pp_instanceset[j]->i_message; k++)
				{
					p_rtps_writer->p_dataWriter->pp_instanceset[j]->pp_message[k];
				}
			break;
		case TOPIC_PRESENTATION_QOS:
			printf("TOPIC_PRESENTATION_QOS");
			if (p_parameter->parameter_id == PID_COHERENT_SET)
			{
				//추후 publisher에서 end_coherent_change가 수행 되었을 때 이 값이 true가 됨
				//true가 되었을 때 coherent_set_number가 바뀌면서 RTPS write가 되면서 coherent_set이 끝났음을 알림
				
				if (is_end_coherent_change == true)
				{
					p_coherent_set_number->high = p_rtps_writer->writer_cache->changes[0]->sequence_number.high;
					p_coherent_set_number->low = p_rtps_writer->writer_cache->changes[0]->sequence_number.low;
					//RTPS write() 함수에 coherent_set number 추가, SerializedData==Null로 하여 전송
				}
				else
				{
				}
			}
			break;
		case GROUP_PRESENTATION_QOS:
			printf("GROUP_PRESENTATION_QOS");
			//추후 구현(2013)
			break;
		default :
			break;
		}
	}
}

void qosim_reader_side_presentation(rtps_reader_t* p_rtps_reader, PresentationQosPolicy* p_presentation_qos)
{
	//PID_COHERENT_SET이 존재하는지 확인하고 존재하면 coherent_set이 끝날 때까지 read()함수 waiting
	//DataFlag == 0 and InlineQosFlag == 1 and PID_COHERENT_SET Number == SEQUENCENUMBER_UNKNOWN / DataFlag == 0 and InlineQosFlag == 0 일 때
	//coherent_set의 데이터를 다 받았는지 확인하고 다 받지 못했으면 AckNack을 보냄
	//coherent_set이 끝나서 read 할 수 있다는 것을 알림 
}


void qosim_process_ordering(DataReader* p_datareader, PresentationQosPolicy* p_presentation_qos)
{
	switch (qosim_get_presentation_access_scope_kind(p_presentation_qos))
	{
	case INSTANCE_PRESENTATION_QOS:
		printf("INSTANCE_PRESENTATION_QOS");
		//instnace 단위로 ordering
		rtps_historycache_instance_destination_ordering_instances(p_datareader->pp_instanceset[0]);			
		break;
	case TOPIC_PRESENTATION_QOS:
		printf("INSTANCE_PRESENTATION_QOS");
		rtps_historycache_topic_destination_ordering_instances(p_datareader);			
		break;
	case GROUP_PRESENTATION_QOS:
		printf("GROUP_PRESENTATION_QOS");
		break;
	default :
		break;
	}
}
*/
