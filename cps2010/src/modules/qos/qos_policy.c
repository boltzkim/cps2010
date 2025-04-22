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
	QoS Policy를 위한 모듈.

	이력
	2012-11-06
*/
#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>
#include <qos_policy.h>




static qos_policy_t* p_static_qos_policy = NULL;

int	launchQosPolicyModule(module_object_t *p_this)
{
	module_list_t* p_list = NULL;
	int i_index = -1;
	qos_policy_t* p_qos_policy = NULL;
	moduleload_t* p_parser;

	p_list = moduleListFind(OBJECT(p_this), OBJECT_MODULE_LOAD, FIND_ANYWHERE);

	for (i_index = 0; i_index < p_list->i_count; i_index++)
	{
		p_parser = (moduleload_t *)p_list->p_values[i_index].p_object;

		if ((p_parser->psz_capability && strcmp("qos_policy", p_parser->psz_capability)))
		{
			continue;
		}

		p_qos_policy = object_create(p_this, OBJECT_QOS_POLICY);

		if (p_qos_policy && (p_parser->pf_activate(OBJECT(p_qos_policy)) == MODULE_SUCCESS))
		{
			p_qos_policy->p_moduleload = p_parser;
			object_attach(OBJECT(p_qos_policy), p_this);
			p_static_qos_policy = p_qos_policy;
		}
		else
		{
			object_destroy(OBJECT(p_qos_policy));
			p_static_qos_policy = NULL;
		}
	}

	if (p_list) moduleListRelease(p_list);

	return MODULE_SUCCESS;
}


ReturnCode_t qos_check_immutable_policy_before_change_publisher_qos(const Publisher* const p_publisher, const PublisherQos* const p_qos)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->check_immutable_policy_before_change_publisher_qos(p_static_qos_policy, p_publisher, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_publisher && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->check_immutable_policy_before_change_publisher_qos(p_qos_policy, p_publisher, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


ReturnCode_t qos_check_immutable_policy_before_change_subscriber_qos(const Subscriber* const p_subscriber, const SubscriberQos* const p_qos)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->check_immutable_policy_before_change_subscriber_qos(p_static_qos_policy, p_subscriber, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_subscriber && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->check_immutable_policy_before_change_subscriber_qos(p_qos_policy, p_subscriber, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


ReturnCode_t qos_check_immutable_policy_before_change_topic_qos(const Topic* const p_topic, const TopicQos* const p_qos)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->check_immutable_policy_before_change_topic_qos(p_static_qos_policy, p_topic, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_topic && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->check_immutable_policy_before_change_topic_qos(p_qos_policy, p_topic, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


ReturnCode_t qos_check_immutable_policy_before_change_datawriter_qos(const DataWriter* const p_datawriter, const DataWriterQos* const p_qos)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->check_immutable_policy_before_change_datawriter_qos(p_static_qos_policy, p_datawriter, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datawriter && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->check_immutable_policy_before_change_datawriter_qos(p_qos_policy, p_datawriter, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


ReturnCode_t qos_check_immutable_policy_before_change_datareader_qos(const DataReader* const p_datareader, const DataReaderQos* const p_qos)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->check_immutable_policy_before_change_datareader_qos(p_static_qos_policy, p_datareader, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datareader && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->check_immutable_policy_before_change_datareader_qos(p_qos_policy, p_datareader, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}

ReturnCode_t qos_check_inconsistent_policy_before_change_topic_qos(const TopicQos* const p_new_qos)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->check_inconsistent_policy_before_change_topic_qos(p_static_qos_policy, p_new_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_new_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->check_inconsistent_policy_before_change_topic_qos(p_qos_policy, p_new_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


ReturnCode_t qos_check_inconsistent_policy_before_change_datawriter_qos(const DataWriterQos* const p_new_qos)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->check_inconsistent_policy_before_change_datawriter_qos(p_static_qos_policy, p_new_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_new_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->check_inconsistent_policy_before_change_datawriter_qos(p_qos_policy, p_new_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


ReturnCode_t qos_check_inconsistent_policy_before_change_datareader_qos(const DataReaderQos* const p_new_qos)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->check_inconsistent_policy_before_change_datareader_qos(p_static_qos_policy, p_new_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_new_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->check_inconsistent_policy_before_change_datareader_qos(p_qos_policy, p_new_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


void qos_send_publication_info_after_change_datawriter_qos(DataWriter* const p_datawriter)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->send_publication_info_after_change_datawriter_qos(p_static_qos_policy, p_datawriter);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datawriter)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->send_publication_info_after_change_datawriter_qos(p_qos_policy, p_datawriter);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_send_subscription_info_after_change_datareader_qos(DataReader* const p_datareader)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->send_subscription_info_after_change_datareader_qos(p_static_qos_policy, p_datareader);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datareader)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->send_subscription_info_after_change_datareader_qos(p_qos_policy, p_datareader);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Entity Factory QoS Policy
//////////////////////////////////////////////////////////////////////////


bool qos_is_enabled(const Entity* const p_entity)
{

	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->is_enabled(p_static_qos_policy, p_entity);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_entity)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->is_enabled(p_qos_policy, p_entity);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


void qos_create_ext_entity_factory_qos(Entity* const p_entity, const Entity* const p_factory, const EntityFactoryQosPolicy* const p_entity_factory_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->create_ext_entity_factory_qos(p_static_qos_policy, p_entity, p_factory, p_entity_factory_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_entity && p_factory && p_entity_factory_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->create_ext_entity_factory_qos(p_qos_policy, p_entity, p_factory, p_entity_factory_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


ReturnCode_t qos_enable_topic(Entity* p_entity)
{
	ReturnCode_t ret = RETCODE_OK;

	if (p_static_qos_policy)
	{
		ret = p_static_qos_policy->enable_topic(p_static_qos_policy, p_entity);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());
		

		if (p_module && p_entity)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				ret = p_qos_policy->enable_topic(p_qos_policy, p_entity);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return ret;
}


ReturnCode_t qos_enable_datawriter(Entity* p_entity)
{
	ReturnCode_t ret = RETCODE_OK;

	if (p_static_qos_policy)
	{
		ret = p_static_qos_policy->enable_datawriter(p_static_qos_policy, p_entity);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_entity)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				ret = p_qos_policy->enable_datawriter(p_qos_policy, p_entity);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return ret;
}


ReturnCode_t qos_enable_datareader(Entity* p_entity)
{
	ReturnCode_t ret = RETCODE_OK;

	if (p_static_qos_policy)
	{
		ret = p_static_qos_policy->enable_datareader(p_static_qos_policy, p_entity);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_entity)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				ret = p_qos_policy->enable_datareader(p_qos_policy, p_entity);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return ret;
}


ReturnCode_t qos_enable_publisher(Entity* p_entity)
{
	ReturnCode_t ret = RETCODE_OK;

	if (p_static_qos_policy)
	{
		ret = p_static_qos_policy->enable_publisher(p_static_qos_policy, p_entity);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_entity)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				ret = p_qos_policy->enable_publisher(p_qos_policy, p_entity);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return ret;
}


ReturnCode_t qos_enable_subscriber(Entity* p_entity)
{
	ReturnCode_t ret = RETCODE_OK;

	if (p_static_qos_policy)
	{
		ret = p_static_qos_policy->enable_subscriber(p_static_qos_policy, p_entity);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_entity)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				ret = p_qos_policy->enable_subscriber(p_qos_policy, p_entity);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return ret;
}


ReturnCode_t qos_enable_domainparticipant(Entity* p_entity)
{
	ReturnCode_t ret = RETCODE_OK;

	if (p_static_qos_policy)
	{
		ret = p_static_qos_policy->enable_domainparticipant(p_static_qos_policy, p_entity);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_entity)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				ret = p_qos_policy->enable_domainparticipant(p_qos_policy, p_entity);
			}

			if (p_list) moduleListRelease(p_list);
		}

	}

	return ret;
}


//////////////////////////////////////////////////////////////////////////
// History QoS Policy and Reliability QoS Policy
//////////////////////////////////////////////////////////////////////////


int32_t qos_get_history_max_length(const HistoryQosPolicy* const p_history_qos, 
								   const ResourceLimitsQosPolicy* const p_resource_limits_qos)
{

	int32_t i_ret = 0;

	if(p_static_qos_policy)
	{
		i_ret = p_static_qos_policy->get_history_max_length(p_static_qos_policy, p_history_qos, p_resource_limits_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());
		

		if (p_module && p_history_qos && p_resource_limits_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				i_ret = p_qos_policy->get_history_max_length(p_qos_policy, p_history_qos, p_resource_limits_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}

	}

	return i_ret;
}

bool qos_check_historycache_for_writer(rtps_writer_t *p_writer, rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->check_historycache_for_writer(p_static_qos_policy, p_writer, p_historycache, p_cachechange);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_historycache && p_cachechange)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->check_historycache_for_writer(p_qos_policy, p_writer, p_historycache, p_cachechange);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


bool qos_check_historycache_for_reader(rtps_historycache_t* const p_historycache, const rtps_cachechange_t* const p_cachechange)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->check_historycache_for_reader(p_static_qos_policy, p_historycache, p_cachechange);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_historycache && p_cachechange)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->check_historycache_for_reader(p_qos_policy, p_historycache, p_cachechange);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


//////////////////////////////////////////////////////////////////////////
// Liveliness QoS Policy
//////////////////////////////////////////////////////////////////////////


void qos_operation_called_for_liveliness_qos(const DataWriter* const p_data_writer)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->operation_called_for_liveliness_qos(p_static_qos_policy, p_data_writer);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_data_writer)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->operation_called_for_liveliness_qos(p_qos_policy, p_data_writer);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_set_stop_liveliness(const bool state)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->set_stop_liveliness(p_static_qos_policy, state);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->set_stop_liveliness(p_qos_policy, state);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_liveliness_thread(rtps_writer_t* const p_rtps_writer)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->liveliness_thread(p_static_qos_policy, p_rtps_writer);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_writer)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->liveliness_thread(p_qos_policy, p_rtps_writer);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_add_liveliness_writer(const DataWriter* const p_data_writer)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->add_liveliness_writer(p_static_qos_policy, p_data_writer);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_data_writer)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->add_liveliness_writer(p_qos_policy, p_data_writer);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_send_heartbeat_for_topic_liveliness(const DataWriter* const p_data_writer)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->send_heartbeat_for_topic_liveliness(p_static_qos_policy, p_data_writer);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_data_writer)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->send_heartbeat_for_topic_liveliness(p_qos_policy, p_data_writer);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_writerproxy_set_liveliness_kind_and_lease_duration(rtps_writerproxy_t* const p_writerproxy, const RxOQos rxo_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->writerproxy_set_liveliness_kind_and_lease_duration(p_static_qos_policy, p_writerproxy, rxo_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_writerproxy)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->writerproxy_set_liveliness_kind_and_lease_duration(p_qos_policy, p_writerproxy, rxo_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}

	}
}


void qos_writer_set_liveliness_kind_and_lease_duration(rtps_writer_t* const p_rtps_writer)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->writer_set_liveliness_kind_and_lease_duration(p_static_qos_policy, p_rtps_writer);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_writer)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->writer_set_liveliness_kind_and_lease_duration(p_qos_policy, p_rtps_writer);
			}

			if (p_list) moduleListRelease(p_list);
		}

	}
}


void qos_participant_liveliness_set_alive(const SerializedPayloadForReader* const p_serialized)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->participant_liveliness_set_alive(p_static_qos_policy, p_serialized);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_serialized)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->participant_liveliness_set_alive(p_qos_policy, p_serialized);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_liveliness_check_wakeup_time_for_reader(data_t* const p_job)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->liveliness_check_wakeup_time_for_reader(p_static_qos_policy, p_job);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_job)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->liveliness_check_wakeup_time_for_reader(p_qos_policy, p_job);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_liveliness_check_wakeup_time_for_writer(data_t* const p_job)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->liveliness_check_wakeup_time_for_writer(p_static_qos_policy, p_job);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_job)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->liveliness_check_wakeup_time_for_writer(p_qos_policy, p_job);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_manual_by_topic_liveliness_set_alive(rtps_writerproxy_t* const p_writerproxy)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->manual_by_topic_liveliness_set_alive(p_static_qos_policy, p_writerproxy);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_writerproxy)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->manual_by_topic_liveliness_set_alive(p_qos_policy, p_writerproxy);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_liveliness_writer_set_alive(rtps_writer_t* const p_rtps_writer)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->liveliness_writer_set_alive(p_static_qos_policy, p_rtps_writer);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_writer)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->liveliness_writer_set_alive(p_qos_policy, p_rtps_writer);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


bool qos_is_liveliness_job(const data_t* const p_job)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->is_liveliness_job(p_static_qos_policy, p_job);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_job)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->is_liveliness_job(p_qos_policy, p_job);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


data_t* qos_add_liveliness_job_for_datareader(const DataReader* const p_datareader)
{
	data_t* b_ret = NULL;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->add_liveliness_job_for_datareader(p_static_qos_policy, p_datareader);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datareader)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->add_liveliness_job_for_datareader(p_qos_policy, p_datareader);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


data_t* qos_add_liveliness_job_for_datawriter(const DataWriter* const p_datawriter)
{
	data_t* b_ret = NULL;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->add_liveliness_job_for_datawriter(p_static_qos_policy, p_datawriter);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datawriter)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->add_liveliness_job_for_datawriter(p_qos_policy, p_datawriter);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


data_t* qos_remove_liveliness_job_for_datareader(const DataReader* const p_datareader)
{
	data_t* b_ret = NULL;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->remove_liveliness_job_for_datareader(p_static_qos_policy, p_datareader);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datareader)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->remove_liveliness_job_for_datareader(p_qos_policy, p_datareader);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


data_t* qos_remove_liveliness_job_for_datawriter(const DataWriter* const p_datawriter)
{
	data_t* b_ret = NULL;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->remove_liveliness_job_for_datawriter(p_static_qos_policy, p_datawriter);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datawriter)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->remove_liveliness_job_for_datawriter(p_qos_policy, p_datawriter);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


//////////////////////////////////////////////////////////////////////////
// Ownership QoS Policy
//////////////////////////////////////////////////////////////////////////


bool qos_has_exclusive_ownership(const rtps_reader_t* const p_rtps_reader)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->has_exclusive_ownership(p_static_qos_policy, p_rtps_reader);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_reader)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->has_exclusive_ownership(p_qos_policy, p_rtps_reader);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


void qos_insert_exclusive_writer(rtps_reader_t* const p_rtps_reader, rtps_writerproxy_t* const p_rtps_writerproxy)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->insert_exclusive_writer(p_static_qos_policy, p_rtps_reader, p_rtps_writerproxy);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_reader && p_rtps_writerproxy)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->insert_exclusive_writer(p_qos_policy, p_rtps_reader, p_rtps_writerproxy);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


int32_t qos_find_exclusive_writer(const rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy)
{
	int32_t i_ret = 0;

	if (p_static_qos_policy)
	{
		i_ret = p_static_qos_policy->find_exclusive_writer(p_static_qos_policy, p_rtps_reader, p_rtps_writerproxy);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());
		
		if (p_module && p_rtps_reader && p_rtps_writerproxy)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				i_ret = p_qos_policy->find_exclusive_writer(p_qos_policy, p_rtps_reader, p_rtps_writerproxy);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return i_ret;
}


int32_t qos_set_exclusive_writer_key(rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy, const SerializedPayloadForReader* const p_serialized)
{
	int32_t i_ret = 0;

	if (p_static_qos_policy)
	{
		i_ret = p_static_qos_policy->set_exclusive_writer_key(p_static_qos_policy, p_rtps_reader, p_rtps_writerproxy, p_serialized);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());
		int32_t i_ret = 0;

		if (p_module && p_rtps_reader && p_rtps_writerproxy && p_serialized)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				i_ret = p_qos_policy->set_exclusive_writer_key(p_qos_policy, p_rtps_reader, p_rtps_writerproxy, p_serialized);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return i_ret;
}


int qos_change_owner_writer(rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy, const bool b_is_include)
{
	int32_t i_ret = 0;

	if (p_static_qos_policy)
	{
		i_ret = p_static_qos_policy->change_owner_writer(p_static_qos_policy, p_rtps_reader, p_rtps_writerproxy, b_is_include);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_reader && p_rtps_writerproxy)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				i_ret = p_qos_policy->change_owner_writer(p_qos_policy, p_rtps_reader, p_rtps_writerproxy, b_is_include);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return i_ret;
}


void qos_remove_exclusive_writer(rtps_reader_t* const p_rtps_reader, const rtps_writerproxy_t* const p_rtps_writerproxy)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->remove_exclusive_writer(p_static_qos_policy, p_rtps_reader, p_rtps_writerproxy);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_reader && p_rtps_writerproxy)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->remove_exclusive_writer(p_qos_policy, p_rtps_reader, p_rtps_writerproxy);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Reader Data Lifecycle QoS Policy
//////////////////////////////////////////////////////////////////////////


void qos_remove_disposed_instance(DataReader* p_datareader)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->remove_disposed_instance(p_static_qos_policy, p_datareader);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datareader)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->remove_disposed_instance(p_qos_policy, p_datareader);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_rtps_reader_set_instance_state_of_cachechanges_with_guid(const rtps_reader_t* const p_rtps_reader, InstanceStateKind instance_state, const GUID_t* const guid)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->rtps_reader_set_instance_state_of_cachechanges_with_guid(p_static_qos_policy, p_rtps_reader, instance_state, guid);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_reader)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->rtps_reader_set_instance_state_of_cachechanges_with_guid(p_qos_policy, p_rtps_reader, instance_state, guid);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Resource Limits QoS Policy
//////////////////////////////////////////////////////////////////////////
ReturnCode_t qos_check_resource_limits(const ResourceLimitsQosPolicy* const p_resource_limits_qos/*, const Topic* const p_topic*/)
{
	ReturnCode_t ret = RETCODE_OK;

	if (p_static_qos_policy)
	{
		ret = p_static_qos_policy->check_resource_limits(p_static_qos_policy, p_resource_limits_qos/*, p_topic*/);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());		

		if (p_module && p_resource_limits_qos/* && p_topic*/)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				ret = p_qos_policy->check_resource_limits(p_qos_policy, p_resource_limits_qos/*, p_topic*/);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return ret;
}


//////////////////////////////////////////////////////////////////////////
// Writer Data Lifecycle QoS Policy
//////////////////////////////////////////////////////////////////////////


bool qos_is_autodispose_unregistered_instances(const WriterDataLifecycleQosPolicy* const p_writer_data_lifecycle_qos)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->is_autodispose_unregistered_instances(p_static_qos_policy, p_writer_data_lifecycle_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());		

		if (p_module && p_writer_data_lifecycle_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->is_autodispose_unregistered_instances(p_qos_policy, p_writer_data_lifecycle_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


//////////////////////////////////////////////////////////////////////////
// Durability Service QoS Policy
//////////////////////////////////////////////////////////////////////////


void qos_durability_service_load(const rtps_writer_t* const p_rtps_writer)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->durability_service_load(p_static_qos_policy, p_rtps_writer);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_writer)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->durability_service_load(p_qos_policy, p_rtps_writer);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_durability_service_save(const rtps_writer_t* const p_rtps_writer)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->durability_service_save(p_static_qos_policy, p_rtps_writer);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_writer)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->durability_service_save(p_qos_policy, p_rtps_writer);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Deadline QoS Policy
//////////////////////////////////////////////////////////////////////////


bool qos_compare_deadline(Duration_t receive_time, Duration_t* p_base_time, Duration_t deadline_period)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->compare_deadline(p_static_qos_policy, receive_time, p_base_time, deadline_period);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());		

		if (p_module && p_base_time)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->compare_deadline(p_qos_policy, receive_time, p_base_time, deadline_period);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


//////////////////////////////////////////////////////////////////////////
// Time Based Filter QoS Policy
//////////////////////////////////////////////////////////////////////////


bool qos_compare_timebasedfilter(Duration_t receive_time, Duration_t* p_base_time, Duration_t minimum_separation)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->compare_timebasedfilter(p_static_qos_policy, receive_time, p_base_time, minimum_separation);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());		

		if (p_module && p_base_time)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->compare_timebasedfilter(p_qos_policy, receive_time, p_base_time, minimum_separation);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


//////////////////////////////////////////////////////////////////////////
// Lifespan QoS Policy
//////////////////////////////////////////////////////////////////////////


void qos_find_lifespan_qos_from_parameterlist(rtps_writerproxy_t* p_writerproxy, char* p_value, int32_t i_size)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->find_lifespan_qos_from_parameterlist(p_static_qos_policy, p_writerproxy, p_value, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_writerproxy && p_value)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->find_lifespan_qos_from_parameterlist(p_qos_policy, p_writerproxy, p_value, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_input_lifespan(rtps_cachechange_t* p_rtps_cachechange, rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->input_lifespan(p_static_qos_policy, p_rtps_cachechange, p_statefulreader, p_writerproxy);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_rtps_cachechange && p_statefulreader && p_writerproxy)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->input_lifespan(p_qos_policy, p_rtps_cachechange, p_statefulreader, p_writerproxy);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// User Data QoS Policy
//////////////////////////////////////////////////////////////////////////


void qos_find_user_data_qos_from_parameterlist(UserDataQosPolicy* p_user_data_qos, char* p_value, int32_t i_size)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->find_user_data_qos_from_parameterlist(p_static_qos_policy, p_user_data_qos, p_value, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_user_data_qos && p_value)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->find_user_data_qos_from_parameterlist(p_qos_policy, p_user_data_qos, p_value, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_init_user_data_qos(UserDataQosPolicy* p_user_data)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->init_user_data_qos(p_static_qos_policy, p_user_data);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_user_data)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->init_user_data_qos(p_qos_policy, p_user_data);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_copy_user_data_qos(UserDataQosPolicy* p_dst_user_data, UserDataQosPolicy* p_src_user_data)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->copy_user_data_qos(p_static_qos_policy, p_dst_user_data, p_src_user_data);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_dst_user_data && p_src_user_data)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->copy_user_data_qos(p_qos_policy, p_dst_user_data, p_src_user_data);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_set_qos_about_user_data_qos_from_datareader(DataReader* p_datareader, DataReaderQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->set_qos_about_user_data_qos_from_datareader(p_static_qos_policy, p_datareader, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datareader && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->set_qos_about_user_data_qos_from_datareader(p_qos_policy, p_datareader, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_get_qos_about_user_data_qos_from_datareader(DataReader* p_datareader, DataReaderQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->get_qos_about_user_data_qos_from_datareader(p_static_qos_policy, p_datareader, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datareader && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->get_qos_about_user_data_qos_from_datareader(p_qos_policy, p_datareader, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_set_qos_about_user_data_qos_from_datawriter(DataWriter* p_datawriter, DataWriterQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->set_qos_about_user_data_qos_from_datawriter(p_static_qos_policy, p_datawriter, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datawriter && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->set_qos_about_user_data_qos_from_datawriter(p_qos_policy, p_datawriter, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_get_qos_about_user_data_qos_from_datawriter(DataWriter* p_datawriter, DataWriterQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->get_qos_about_user_data_qos_from_datawriter(p_static_qos_policy, p_datawriter, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_datawriter && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->get_qos_about_user_data_qos_from_datawriter(p_qos_policy, p_datawriter, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_set_qos_about_user_data_qos_from_domainparticipant(DomainParticipant* p_domainparticipant, DomainParticipantQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->set_qos_about_user_data_qos_from_domainparticipant(p_static_qos_policy, p_domainparticipant, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_domainparticipant && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->set_qos_about_user_data_qos_from_domainparticipant(p_qos_policy, p_domainparticipant, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_print_user_data_qos(UserDataQosPolicy user_data_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->print_user_data_qos(p_static_qos_policy, user_data_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->print_user_data_qos(p_qos_policy, user_data_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


int qos_generate_user_data(char* p_serialized, UserDataQosPolicy a_user_data_qos, int i_size)
{
	int32_t i_ret = 0;

	if (p_static_qos_policy)
	{
		i_ret = p_static_qos_policy->generate_user_data(p_static_qos_policy, p_serialized, a_user_data_qos, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_serialized)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				i_ret = p_qos_policy->generate_user_data(p_qos_policy, p_serialized, a_user_data_qos, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return i_ret;
}


//////////////////////////////////////////////////////////////////////////
// Topic Data QoS Policy
//////////////////////////////////////////////////////////////////////////


void qos_find_topic_data_qos_from_parameterlist(TopicDataQosPolicy* p_topic_data_qos, char* p_value, int32_t i_size)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->find_topic_data_qos_from_parameterlist(p_static_qos_policy, p_topic_data_qos, p_value, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_topic_data_qos && p_value)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->find_topic_data_qos_from_parameterlist(p_qos_policy, p_topic_data_qos, p_value, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_print_topic_data_qos(TopicDataQosPolicy topic_data_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->print_topic_data_qos(p_static_qos_policy, topic_data_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->print_topic_data_qos(p_qos_policy, topic_data_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_init_topic_data_qos(TopicDataQosPolicy* p_topic_data)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->init_topic_data_qos(p_static_qos_policy, p_topic_data);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_topic_data)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->init_topic_data_qos(p_qos_policy, p_topic_data);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_copy_topic_data_qos(TopicDataQosPolicy* p_dst_topic_data, TopicDataQosPolicy* p_src_topic_data)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->copy_topic_data_qos(p_static_qos_policy, p_dst_topic_data, p_src_topic_data);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_dst_topic_data && p_src_topic_data)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->copy_topic_data_qos(p_qos_policy, p_dst_topic_data, p_src_topic_data);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_set_qos_about_topic_data_qos_from_topic(Topic* p_topic, TopicQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->set_qos_about_topic_data_qos_from_topic(p_static_qos_policy, p_topic, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_topic && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->set_qos_about_topic_data_qos_from_topic(p_qos_policy, p_topic, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_get_qos_about_topic_data_qos_from_topic(Topic* p_topic, TopicQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->get_qos_about_topic_data_qos_from_topic(p_static_qos_policy, p_topic, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_topic && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->get_qos_about_topic_data_qos_from_topic(p_qos_policy, p_topic, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


int qos_generate_topic_data(char* p_serialized, TopicDataQosPolicy a_topic_data_qos, int i_size)
{
	int32_t i_ret = 0;

	if (p_static_qos_policy)
	{
		i_ret = p_static_qos_policy->generate_topic_data(p_static_qos_policy, p_serialized, a_topic_data_qos, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_serialized)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				i_ret = p_qos_policy->generate_topic_data(p_qos_policy, p_serialized, a_topic_data_qos, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return i_ret;
}


//////////////////////////////////////////////////////////////////////////
// Group Data QoS Policy
//////////////////////////////////////////////////////////////////////////


void qos_find_group_data_qos_from_parameterlist(GroupDataQosPolicy* p_group_data_qos, char* p_value, int32_t i_size)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->find_group_data_qos_from_parameterlist(p_static_qos_policy, p_group_data_qos, p_value, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_group_data_qos && p_value)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->find_group_data_qos_from_parameterlist(p_qos_policy, p_group_data_qos, p_value, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_print_group_data_qos(GroupDataQosPolicy group_data_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->print_group_data_qos(p_static_qos_policy, group_data_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->print_group_data_qos(p_qos_policy, group_data_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_init_group_data_qos(GroupDataQosPolicy* p_group_data)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->init_group_data_qos(p_static_qos_policy, p_group_data);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_group_data)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->init_group_data_qos(p_qos_policy, p_group_data);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_copy_group_data_qos(GroupDataQosPolicy* p_dst_group_data, GroupDataQosPolicy* p_src_group_data)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->copy_group_data_qos(p_static_qos_policy, p_dst_group_data, p_src_group_data);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_dst_group_data && p_src_group_data)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->copy_group_data_qos(p_qos_policy, p_dst_group_data, p_src_group_data);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_set_qos_about_group_data_qos_from_publisher(Publisher* p_publisher, PublisherQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->set_qos_about_group_data_qos_from_publisher(p_static_qos_policy, p_publisher, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_publisher && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->set_qos_about_group_data_qos_from_publisher(p_qos_policy, p_publisher, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_get_qos_about_group_data_qos_from_publisher(Publisher* p_publisher, PublisherQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->get_qos_about_group_data_qos_from_publisher(p_static_qos_policy, p_publisher, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_publisher && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->get_qos_about_group_data_qos_from_publisher(p_qos_policy, p_publisher, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}

void qos_set_qos_about_group_data_qos_from_subscriber(Subscriber* p_subscriber, SubscriberQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->set_qos_about_group_data_qos_from_subscriber(p_static_qos_policy, p_subscriber, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_subscriber && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->set_qos_about_group_data_qos_from_subscriber(p_qos_policy, p_subscriber, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_get_qos_about_group_data_qos_from_subscriber(Subscriber* p_subscriber, SubscriberQos* p_qos)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->get_qos_about_group_data_qos_from_subscriber(p_static_qos_policy, p_subscriber, p_qos);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_subscriber && p_qos)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->get_qos_about_group_data_qos_from_subscriber(p_qos_policy, p_subscriber, p_qos);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


int qos_generate_group_data(char* p_serialized, GroupDataQosPolicy a_group_data_qos, int i_size)
{
	int32_t i_ret = 0;

	if (p_static_qos_policy)
	{
		i_ret = p_static_qos_policy->generate_group_data(p_static_qos_policy, p_serialized, a_group_data_qos, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_serialized)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				i_ret = p_qos_policy->generate_group_data(p_qos_policy, p_serialized, a_group_data_qos, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return i_ret;
}


//////////////////////////////////////////////////////////////////////////
// Partition Data QoS Policy
//////////////////////////////////////////////////////////////////////////


bool qos_compare_partition(PartitionQosPolicy a_partition, PartitionQosPolicy b_partition)
{
	bool b_ret = false;

	if (p_static_qos_policy)
	{
		b_ret = p_static_qos_policy->compare_partition(p_static_qos_policy, a_partition, b_partition);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());		

		if (p_module)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				b_ret = p_qos_policy->compare_partition(p_qos_policy, a_partition, b_partition);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return b_ret;
}


void qos_find_partition_qos_from_parameterlist(PartitionQosPolicy* p_partition_qos, char* p_value, int32_t i_size)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->find_partition_qos_from_parameterlist(p_static_qos_policy, p_partition_qos, p_value, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_partition_qos && p_value)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->find_partition_qos_from_parameterlist(p_qos_policy, p_partition_qos, p_value, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


int qos_generate_partition(char* p_serialized, PartitionQosPolicy a_partition_qos, int i_size)
{
	int32_t i_ret = 0;

	if (p_static_qos_policy)
	{
		i_ret = p_static_qos_policy->generate_partition(p_static_qos_policy, p_serialized, a_partition_qos, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_serialized)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				i_ret = p_qos_policy->generate_partition(p_qos_policy, p_serialized, a_partition_qos, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}

	return i_ret;
}


//////////////////////////////////////////////////////////////////////////
// Find Entity
//////////////////////////////////////////////////////////////////////////


void qos_find_topic_qos_from_parameterlist(TopicBuiltinTopicData* p_topic_data, char* p_value, int32_t i_size)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->find_topic_qos_from_parameterlist(p_static_qos_policy, p_topic_data, p_value, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_topic_data && p_value)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->find_topic_qos_from_parameterlist(p_qos_policy, p_topic_data, p_value, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_find_publication_qos_from_parameterlist(PublicationBuiltinTopicData* p_publication_data, char* p_value, int32_t i_size)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->find_publication_qos_from_parameterlist(p_static_qos_policy, p_publication_data, p_value, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_publication_data && p_value)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->find_publication_qos_from_parameterlist(p_qos_policy, p_publication_data, p_value, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}


void qos_find_subscription_qos_from_parameterlist(SubscriptionBuiltinTopicData* p_subscription_data, char* p_value, int32_t i_size)
{
	if (p_static_qos_policy)
	{
		p_static_qos_policy->find_subscription_qos_from_parameterlist(p_static_qos_policy, p_subscription_data, p_value, i_size);
	}
	else
	{
		module_list_t* p_list = NULL;
		qos_policy_t* p_qos_policy = NULL;
		int i_index = -1;
		module_t* p_module = current_object(get_domain_participant_factory_module_id());

		if (p_module && p_subscription_data && p_value)
		{
			p_list = moduleListFind(OBJECT(p_module), OBJECT_QOS_POLICY, FIND_CHILD);

			for (i_index = 0; i_index < p_list->i_count; i_index++)
			{
				p_qos_policy = (qos_policy_t*)p_list->p_values[i_index].p_object;
				p_qos_policy->find_subscription_qos_from_parameterlist(p_qos_policy, p_subscription_data, p_value, i_size);
			}

			if (p_list) moduleListRelease(p_list);
		}
	}
}
