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
	Resource Limits QoS Policy를 위한 모듈.

	이력
	2012-10-15
*/
#include "../rtps/rtps.h"


/************************************************************************/
/* Key를 가진 Topic인가?                                                */
/************************************************************************/
static bool is_topic_with_key(const Topic* const p_topic)
{
	// TODO[111005,김경일] 구현해야 함.
	// TODO[111024,김경일] p_data_reader 를 얻어오는 수단 고민하기.
	DataReader* p_data_reader;
	rtps_reader_t* p_rtps_reader;

	p_rtps_reader = (rtps_reader_t*) p_data_reader->p_related_rtps_reader;

	if (p_rtps_reader->topic_kind == WITH_KEY)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/************************************************************************/
/* max_instances 값이 1인가?                                            */
/************************************************************************/
static bool is_single_instance(const ResourceLimitsQosPolicy* const p_resource_limits_qos)
{
	if (p_resource_limits_qos == NULL) 
	{
		return false;
	}

	if (p_resource_limits_qos->max_instances == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/************************************************************************/
/* Resource Limits QoS Policy 관련                                      */
/* DataWriter, DataReader, Topic Entity 의 set_qos() 함수에서 호출하기  */
/* 위한 함수                                                            */
/* 새로운 값으로 설정하기 전, 새로운 값이 유효한 값인지 확인해야 한다.  */
/************************************************************************/
ReturnCode_t qosim_check_resource_limits(qos_policy_t* const p_this, const ResourceLimitsQosPolicy* const p_resource_limits_qos/*, const Topic* const p_topic*/)
{
	if (p_resource_limits_qos->max_instances == LENGTH_UNLIMITED ||
		p_resource_limits_qos->max_samples_per_instance  == LENGTH_UNLIMITED ||
		p_resource_limits_qos->max_samples == LENGTH_UNLIMITED)
	{
		return RETCODE_OK;
	}

	if (p_resource_limits_qos->max_instances * p_resource_limits_qos->max_samples_per_instance != p_resource_limits_qos->max_samples)
	{
		return RETCODE_UNSUPPORTED;
	}

	//if (!is_topic_with_key(p_topic) && !is_single_instance(p_resource_limits_qos))
	//{
	//	return RETCODE_UNSUPPORTED;
	//}

	return RETCODE_OK;
}
