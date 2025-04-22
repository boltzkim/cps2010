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
 * OF OR in CONNECTION WITH THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN
 * IF ETRI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * 
 * Any permitted redistribution of this software must retain the copyright
 * notice, conditions, and disclaimer as specified above.
 ******************************************************************************/

/*
	QoS Policy를 위한 모듈.

	이력
	2012-11-06
*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include "qos.h"
#include "../rtps/rtps.h"
#include <service.h>


static int  Open  ( module_object_t * );
static void Close ( module_object_t * );


module_define_start(qos)
	set_category( CAT_QOS_POLICY );
	set_description( "Quality of Service" );
	set_capability( "qos_policy", 200 );
	set_callbacks( Open, Close );
	add_shortcut( "qos" );
module_define_end()


static int  Open(module_object_t *p_this)
{
	qos_policy_t* p_qos_policy = (qos_policy_t*) p_this;
	module_t* p_module = p_this->p_module;

	if (!p_module)
	{
		trace_msg(OBJECT(p_this), TRACE_ERROR, "Can't Use Module \"QoS Policy \" module. p_module is NULL.");
		return MODULE_ERROR_CREATE;
	}

	p_qos_policy->check_immutable_policy_before_change_publisher_qos = qosim_check_immutable_policy_before_change_publisher_qos;
	p_qos_policy->check_immutable_policy_before_change_subscriber_qos = qosim_check_immutable_policy_before_change_subscriber_qos;
	p_qos_policy->check_immutable_policy_before_change_topic_qos = qosim_check_immutable_policy_before_change_topic_qos;
	p_qos_policy->check_immutable_policy_before_change_datawriter_qos = qosim_check_immutable_policy_before_change_datawriter_qos;
	p_qos_policy->check_immutable_policy_before_change_datareader_qos = qosim_check_immutable_policy_before_change_datareader_qos;
	p_qos_policy->check_inconsistent_policy_before_change_topic_qos = qosim_check_inconsistent_policy_before_change_topic_qos;
	p_qos_policy->check_inconsistent_policy_before_change_datawriter_qos = qosim_check_inconsistent_policy_before_change_datawriter_qos;
	p_qos_policy->check_inconsistent_policy_before_change_datareader_qos = qosim_check_inconsistent_policy_before_change_datareader_qos;
	p_qos_policy->send_publication_info_after_change_datawriter_qos = qosim_send_publication_info_after_change_datawriter_qos;
	p_qos_policy->send_subscription_info_after_change_datareader_qos = qosim_send_subscription_info_after_change_datareader_qos;

	//////////////////////////////////////////////////////////////////////////
	// Entity Factory QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->is_enabled = qosim_is_enabled;
	p_qos_policy->create_ext_entity_factory_qos = qosim_create_ext_entity_factory_qos;
	p_qos_policy->enable_topic = qosim_enable_topic;
	p_qos_policy->enable_datawriter = qosim_enable_datawriter;
	p_qos_policy->enable_datareader = qosim_enable_datareader;
	p_qos_policy->enable_publisher = qosim_enable_publisher;
	p_qos_policy->enable_subscriber = qosim_enable_subscriber;
	p_qos_policy->enable_domainparticipant = qosim_enable_domainparticipant;

	//////////////////////////////////////////////////////////////////////////
	// History QoS Policy and Reliability QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->get_history_max_length = qosim_get_history_max_length;
	p_qos_policy->check_historycache_for_writer = qosim_check_historycache_for_writer;
	p_qos_policy->check_historycache_for_reader = qosim_check_historycache_for_reader;

	//////////////////////////////////////////////////////////////////////////
	// Liveliness QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->b_operation_called = false;
	p_qos_policy->b_operation_called_lock_initialize = false;
	p_qos_policy->pp_automatic_liveliness_writers = NULL;
	p_qos_policy->i_automatic_liveliness_writers  = 0;
	p_qos_policy->pp_manual_liveliness_writers = NULL;
	p_qos_policy->i_manual_liveliness_writers  = 0;
	p_qos_policy->sent_time = -1;
	p_qos_policy->b_end_liveliness = false;
	p_qos_policy->operation_called_for_liveliness_qos = qosim_operation_called_for_liveliness_qos;
	p_qos_policy->set_stop_liveliness = qosim_set_stop_liveliness;
	p_qos_policy->liveliness_thread = qosim_liveliness_thread;
	p_qos_policy->add_liveliness_writer = qosim_add_liveliness_writer;
	p_qos_policy->send_heartbeat_for_topic_liveliness = qosim_send_heartbeat_for_topic_liveliness;
	p_qos_policy->writerproxy_set_liveliness_kind_and_lease_duration = qosim_writerproxy_set_liveliness_kind_and_lease_duration;
	p_qos_policy->writer_set_liveliness_kind_and_lease_duration = qosim_writer_set_liveliness_kind_and_lease_duration;
	p_qos_policy->participant_liveliness_set_alive = qosim_participant_liveliness_set_alive;
	p_qos_policy->liveliness_check_wakeup_time_for_reader = qosim_liveliness_check_wakeup_time_for_reader;
	p_qos_policy->liveliness_check_wakeup_time_for_writer = qosim_liveliness_check_wakeup_time_for_writer;
	p_qos_policy->manual_by_topic_liveliness_set_alive = qosim_manual_by_topic_liveliness_set_alive;
	p_qos_policy->liveliness_writer_set_alive = qosim_liveliness_writer_set_alive;
	p_qos_policy->is_liveliness_job = qosim_is_liveliness_job;
	p_qos_policy->add_liveliness_job_for_datareader = qosim_add_liveliness_job_for_datareader;
	p_qos_policy->add_liveliness_job_for_datawriter = qosim_add_liveliness_job_for_datawriter;
	p_qos_policy->remove_liveliness_job_for_datareader = qosim_remove_liveliness_job_for_datareader;
	p_qos_policy->remove_liveliness_job_for_datawriter = qosim_remove_liveliness_job_for_datawriter;

	//////////////////////////////////////////////////////////////////////////
	// Ownership QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->has_exclusive_ownership = qosim_has_exclusive_ownership;
	p_qos_policy->insert_exclusive_writer = qosim_insert_exclusive_writer;
	p_qos_policy->find_exclusive_writer = qosim_find_exclusive_writer;
	p_qos_policy->set_exclusive_writer_key = qosim_set_exclusive_writer_key;
	p_qos_policy->change_owner_writer = qosim_change_owner_writer;
	p_qos_policy->remove_exclusive_writer = qosim_remove_exclusive_writer;

	//////////////////////////////////////////////////////////////////////////
	// Reader Data Lifecycle QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->remove_disposed_instance = qosim_remove_disposed_instance;
	p_qos_policy->rtps_reader_set_instance_state_of_cachechanges_with_guid = qosim_rtps_reader_set_instance_state_of_cachechanges_with_guid;

	//////////////////////////////////////////////////////////////////////////
	// Resource Limits QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->check_resource_limits = qosim_check_resource_limits;

	//////////////////////////////////////////////////////////////////////////
	// Writer Data Lifecycle QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->is_autodispose_unregistered_instances = qosim_is_autodispose_unregistered_instances;

	//////////////////////////////////////////////////////////////////////////
	// Writer Data Lifecycle QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->durability_service_load = qosim_durability_service_load;
	p_qos_policy->durability_service_save = qosim_durability_service_save;

	//////////////////////////////////////////////////////////////////////////
	// Deadline QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->compare_deadline = qosim_compare_deadline;

	//////////////////////////////////////////////////////////////////////////
	// Time Based Filter QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->compare_timebasedfilter = qosim_compare_timebasedfilter;

	//////////////////////////////////////////////////////////////////////////
	// Lifespan QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->find_lifespan_qos_from_parameterlist = qosim_find_lifespan_qos_from_parameterlist;
	p_qos_policy->input_lifespan = qosim_input_lifespan;

	//////////////////////////////////////////////////////////////////////////
	// User Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->find_user_data_qos_from_parameterlist = qosim_find_user_data_qos_from_parameterlist;
	p_qos_policy->init_user_data_qos = qosim_init_user_data_qos;
	p_qos_policy->copy_user_data_qos = qosim_copy_user_data_qos;
	p_qos_policy->set_qos_about_user_data_qos_from_datareader = qosim_set_qos_about_user_data_qos_from_datareader;
	p_qos_policy->get_qos_about_user_data_qos_from_datareader = qosim_get_qos_about_user_data_qos_from_datareader;
	p_qos_policy->set_qos_about_user_data_qos_from_datawriter = qosim_set_qos_about_user_data_qos_from_datawriter;
	p_qos_policy->get_qos_about_user_data_qos_from_datawriter = qosim_get_qos_about_user_data_qos_from_datawriter;
	p_qos_policy->set_qos_about_user_data_qos_from_domainparticipant = qosim_set_qos_about_user_data_qos_from_domainparticipant;
	p_qos_policy->print_user_data_qos = qosim_print_user_data_qos;
	p_qos_policy->generate_user_data = qosim_generate_user_data;

	//////////////////////////////////////////////////////////////////////////
	// Topic Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->find_topic_data_qos_from_parameterlist = qosim_find_topic_data_qos_from_parameterlist;
	p_qos_policy->print_topic_data_qos = qosim_print_topic_data_qos;
	p_qos_policy->init_topic_data_qos = qosim_init_topic_data_qos;
	p_qos_policy->copy_topic_data_qos = qosim_copy_topic_data_qos;
	p_qos_policy->set_qos_about_topic_data_qos_from_topic = qosim_set_qos_about_topic_data_qos_from_topic;
	p_qos_policy->get_qos_about_topic_data_qos_from_topic = qosim_get_qos_about_topic_data_qos_from_topic;
	p_qos_policy->generate_topic_data = qosim_generate_topic_data;

	//////////////////////////////////////////////////////////////////////////
	// Group Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->find_group_data_qos_from_parameterlist = qosim_find_group_data_qos_from_parameterlist;
	p_qos_policy->print_group_data_qos = qosim_print_group_data_qos;
	p_qos_policy->init_group_data_qos = qosim_init_group_data_qos;
	p_qos_policy->copy_group_data_qos = qosim_copy_group_data_qos;
	p_qos_policy->set_qos_about_group_data_qos_from_publisher = qosim_set_qos_about_group_data_qos_from_publisher;
	p_qos_policy->get_qos_about_group_data_qos_from_publisher = qosim_get_qos_about_group_data_qos_from_publisher;
	p_qos_policy->set_qos_about_group_data_qos_from_subscriber = qosim_set_qos_about_group_data_qos_from_subscriber;
	p_qos_policy->get_qos_about_group_data_qos_from_subscriber = qosim_get_qos_about_group_data_qos_from_subscriber;
	p_qos_policy->generate_group_data = qosim_generate_group_data;

	//////////////////////////////////////////////////////////////////////////
	// Partition Data QoS Policy
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->compare_partition = qosim_compare_partition;
	p_qos_policy->find_partition_qos_from_parameterlist = qosim_find_partition_qos_from_parameterlist;
	p_qos_policy->generate_partition = qosim_generate_partition;

	//////////////////////////////////////////////////////////////////////////
	// Find Entity
	//////////////////////////////////////////////////////////////////////////
	p_qos_policy->find_topic_qos_from_parameterlist = qosim_find_topic_qos_from_parameterlist;
	p_qos_policy->find_publication_qos_from_parameterlist = qosim_find_publication_qos_from_parameterlist;
	p_qos_policy->find_subscription_qos_from_parameterlist = qosim_find_subscription_qos_from_parameterlist;

	return MODULE_SUCCESS;
}


static void Close(module_object_t *p_this)
{
}
