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
Time Based Filter QoS Policy를 위한 모듈.

이력
2012-11-26
*/
#include <core.h>
#include <cpsdcps.h>
#include "../rtps/rtps.h"

extern bool time_left_bigger_d(const Duration_t time_1, const Duration_t time_2);
extern Duration_t time_addition_d(const Duration_t augend_time, const Duration_t addend_time);

/************************************************************************/
/* Time Based Filter QoS Policy                                         */
/************************************************************************/

//time_based_filter 비교 함수
bool qosim_compare_timebasedfilter(qos_policy_t* const p_this, Duration_t receive_time, Duration_t* p_base_time, Duration_t minimum_separation)
{
	if (minimum_separation.nanosec / 0xffffffff >= 1)
	{
		minimum_separation.sec += (minimum_separation.nanosec / 0xffffffff);
		minimum_separation.nanosec -= 0xffffffff;
	}

	if (minimum_separation.sec == 0 && minimum_separation.nanosec == 0)
	{
		p_base_time->sec = receive_time.sec;
		p_base_time->nanosec = receive_time.nanosec;

	}
/*	if (receive_time.sec < p_base_time->sec + minimum_separation.sec)
	{
		return false;
	}
	else if(receive_time.sec == p_base_time->sec && receive_time.nanosec < p_base_time->nanosec + minimum_separation.nanosec)
	{
		return false;
	}
*/
	if (time_left_bigger_d(time_addition_d(*p_base_time, minimum_separation), receive_time))
	{
		return false;
	}
	else
	{
		p_base_time->sec = receive_time.sec;
		p_base_time->nanosec = receive_time.nanosec;
		//printf("Timebasedfilter next basetime : %d\n", p_base_time->sec);
		//printf("Timebasedfilter next basetime : %d\n", p_base_time->nanosec);
	}

	return true;
}
