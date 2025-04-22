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
2012-11-26
*/
#include <core.h>
#include <cpsdcps.h>
#include "../rtps/rtps.h"

extern bool time_left_bigger(const Time_t time_1, const Time_t time_2);

/************************************************************************/
/* Destination Order QoS Policy                                         */
/* get 하는 부분 필요                                                   */
/* 원격지의 Data SubMessage에서 destination order에 대한 parsing 필요   */
/************************************************************************/


static void swap_instances(message_t* a, message_t* b)
{ 
	message_t temp; 
	//printf("before : %d, %d\n",a->source_timestamp.sec, b->source_timestamp.sec);
	temp = *a;
	*a = *b;
	*b = temp;
	//	printf("after : %d, %d\n\n",a->source_timestamp.sec, b->source_timestamp.sec);
}


void qosim_dcps_message_ordering_change(DataReader* p_datareader)//<--- NEED MODULATION
{
	int i, j;
	int i_cachechange_size = p_datareader->i_message_order;
	bool a;
	rtps_reader_t* p_rtps_reader = p_datareader->p_related_rtps_reader;

	if (p_rtps_reader&&!p_rtps_reader->is_builtin)
	{
		if (i_cachechange_size >= 2)
		{
			for (i = i_cachechange_size-1; i >= 1; i--)
			{
				for (j = i-1; j >= 0; j--)
				{
					a = time_left_bigger(p_datareader->pp_message_order[i]->sampleInfo.source_timestamp, p_datareader->pp_message_order[j]->sampleInfo.source_timestamp);

					if (a == false)
					{
						swap_instances(p_datareader->pp_message_order[i], p_datareader->pp_message_order[j]);
					}
					else if (a == true)
					{
					}
				}
			}
		}
	}
}


void qosim_dcps_message_ordering_source_timestamp(message_t** pp_message, int i_count)//<--- NEED MODULATION
{
	int i, j;
	
	bool a;
	
	if (i_count >= 2)
	{
		for (i = i_count-1; i >= 1; i--)
		{
			for (j = i-1; j >= 0; j--)
			{
				a = time_left_bigger(pp_message[i]->sampleInfo.source_timestamp, pp_message[j]->sampleInfo.source_timestamp);

				if (a == false)
				{
					//sorting algorithm 추가(교체 가능)					
					swap_instances(pp_message[i], pp_message[j]);
				}
				else
				{					
				}
			}

		}
	}
}
