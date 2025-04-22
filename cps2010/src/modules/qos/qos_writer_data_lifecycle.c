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
	Writer Data Lifecycle QoS Policy를 위한 모듈.

	이력
	2012-11-09
*/
#include <core.h>
#include <cpsdcps.h>

/************************************************************************/
/* Writer Data Lifecycle QoS Policy의                                   */
/* autodispose_unregistered_instances == TRUE인가?                      */
/*                                                                      */
/* DataWriter 가 unregister() 될 때, 인스턴스를 dispose() 함.           */
/* DataWriter 의 unregister() 함수에 넣어야 함.                         */
/************************************************************************/
bool qosim_is_autodispose_unregistered_instances(qos_policy_t* const p_this, const WriterDataLifecycleQosPolicy* const p_writer_data_lifecycle_qos)
{
	if (p_writer_data_lifecycle_qos == NULL)
	{
		return false;
	}

	if (p_writer_data_lifecycle_qos->autodispose_unregistered_instances)
	{
		return true;
	}
	else
	{
		return false;
	}
}
