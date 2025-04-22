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
	Entity Factory QoS Policy�� ���� ���.

	�̷�
	2012-11-08
*/
#include "../rtps/rtps.h"


/************************************************************************/
/* Entity�� ���°� enable �����ΰ�?                                     */
/************************************************************************/
bool qosim_is_enabled(qos_policy_t* const p_this, const Entity* const p_entity)
{
	if (p_entity == NULL)
	{
		return false;
	}

	return (p_entity->b_enable);
}


/************************************************************************/
/* Entity Factory QoS Policy �� autoenable_created_entities == TRUE�ΰ�?*/
/************************************************************************/
bool is_auto_enabled_created_entities(const EntityFactoryQosPolicy* const p_entity_factory_qos)
{
	if (p_entity_factory_qos == NULL)
	{
		return false;
	}

	return (p_entity_factory_qos->autoenable_created_entities);
}


/************************************************************************/
/* �����Ǵ� Entity�� ���� �ʱⰪ�� disable�� �Ѵ�.                      */
/* Entity Factory QoS Policy �� autoenable_created_entities == TRUE �̰�*/
/* Factory �� enable �����̸�,                                          */ 
/* �����Ǵ� Entity �� enable() �Լ��� ȣ���Ѵ�.                         */
/************************************************************************/
void qosim_create_ext_entity_factory_qos(qos_policy_t* const p_this, Entity* const p_entity, const Entity* const p_factory, const EntityFactoryQosPolicy* const p_entity_factory_qos)
{
	p_entity->b_enable = false;

	if (is_auto_enabled_created_entities(p_entity_factory_qos) && qosim_is_enabled(p_this, p_factory))
	{
		p_entity->enable(p_entity);
	}
}


/************************************************************************/
/* �Ķ���� p_entity �� Factory Entity �ΰ�?                            */
/************************************************************************/
static bool is_factory_entity(const Entity* const p_entity)
{
	if (p_entity == NULL)
	{
		return false;
	}

	switch (p_entity->i_entity_type)
	{
		case DOMAINPARTICIPANT_ENTITY:
		case PUBLISHER_ENTITY:
		case SUBSCRIBER_ENTITY:
			return true;
		default:
			return false;
	}
}


/************************************************************************/
/* �ش� Entity �� �̹� enable �����̸�, �ƹ��� ���۾��� OK�� �����Ѵ�.  */
/* �ش� Entity �� �θ� Factory �� disable �����̸�,                     */
/* �Լ��� �����ϰ� PRECONDITION_NOT_MET�� �����Ѵ�.                     */
/* Factory Entity �� enable() �Լ��� ȣ���� ���                        */
/* ���� autoenable_created_entities == TRUE �̸�,                       */
/* �ش� Entity �� enable �ϰ� �ڽ� Entity ���� enable() �Լ��� ȣ���Ѵ�.*/
/* ���� autoenable_created_entities == FALSE �̸�,                      */
/* �ش� Entity�� enable �ȴ�.                                           */
/************************************************************************/
/*
// ���� ���� ������ ������ enable() �Լ��� �ϳ��� �����Ϸ��� �ϴٰ� �ߴ���.
// �ϳ��� �Լ��� ���� ���⵵�� �������� ����. 
// ���߿� �ٽ� �õ��غ����� �ڵ�� �ּ�ó���� ���ܵ�.
static ReturnCode_t enable(Entity* p_this)
{
	void* p_factory;

	if (!p_this) return RETCODE_BAD_PARAMETER;

	if (qosim_is_enabled(p_this))
	{
		return RETCODE_OK;
	}

	switch (p_this->i_entityType)
	{
		case PUBLISHER_ENTITY:
			p_factory = (DomainParticipant*) ((Publisher*) p_this)->get_participant((Publisher*) p_this);
			break;
		case SUBSCRIBER_ENTITY:
			p_factory = (DomainParticipant*) ((Subscriber*) p_this)->get_participant((Subscriber*) p_this);
			break;
		case TOPIC_ENTITY:
			p_factory = (DomainParticipant*) ((Topic*) p_this)->get_participant((Topic*) p_this);
			break;
		case DATAREADER_ENTITY:
			p_factory = (Subscriber*) ((DataReader*) p_this)->get_subscriber((DataReader*) p_this);
			break;
		case DATAWRITER_ENTITY:
			p_factory = (Publisher*) ((DataWriter*) p_this)->get_publisher((DataWriter*) p_this);
			break;
	}

	if (p_this->i_entityType != DOMAINPARTICIPANT_ENTITY && !qosim_is_enabled((Entity*) p_factory))
	{
		return RETCODE_PRECONDITION_NOT_MET;
	}
	p_this->b_enable = true;

	if (is_factory_entity(p_this))
	{
		PublisherQos* p_qos;
		int i;

		switch (p_this->i_entityType)
		{
			case PUBLISHER_ENTITY:
				((Publisher*) p_this)->get_qos((Publisher*) p_this, p_qos);
				if (is_auto_enabled_created_entities(&p_qos->entity_factory))
				{
					for (i = 0; i < ((Publisher*) p_this)->i_datawriters; i++)
					{
						DataWriter* child = ((Publisher*) p_this)->pp_datawriters[i];
						child->enable((Entity*) child);
					}
				}
				break;
			case SUBSCRIBER_ENTITY:
				p_this->get_qos(p_this, p_qos);
				if (is_auto_enabled_created_entities(&p_qos->entity_factory))
				{
					for (i = 0; i < p_this->i_datareaders; i++)
					{
						DataReader* child = p_this->pp_datareaders[i];
						child->enable((Entity*) child);
					}
				}

		}



		// TODO:[111005,�����] Entity�� QoS ������ ���;� �Ѵ�.
		// TODO:[111005,�����] Entity ����ü�� get_qos(), set_qos() �Լ��� �����Ǿ� ���� �ʴ�.
		// TODO:[111006,�����] Entity�� �� �ڽĵ�(DataReader, DataWriter, Domainparticipant, Publisher, Subscriber, Topic)���� �л�Ǿ� �����Ǿ� �ִ�.
//		p_entity->get_qos();
//		if (is_auto_enabled_created_entities()) 
		{
			// TODO:[111005,�����] child entities�� ã�ƿ� ������ �ʿ��ϴ�.
///			for each child
///			{
///				child->enable();
///			}
		}


	}

	return RETCODE_OK;
}
*/


// [111006,�����] ������ Entity���� enable �Լ��� �������.
ReturnCode_t qosim_enable_topic(qos_policy_t* const p_this, Entity* p_entity)
{
	Topic* p_this_entity = (Topic*) p_entity;
	DomainParticipant* p_factory;

	if (!p_this_entity || p_this_entity->i_entity_type != TOPIC_ENTITY)
	{
		return RETCODE_BAD_PARAMETER;
	}

	if (qosim_is_enabled(p_this, (Entity*) p_this_entity))
	{
		return RETCODE_OK;
	}

	p_factory = p_this_entity->get_participant(p_this_entity);

	if (!qosim_is_enabled(p_this, (Entity*) p_factory))
	{
		return RETCODE_PRECONDITION_NOT_MET;
	}
	p_this_entity->b_enable = true;

	return RETCODE_OK;
}
ReturnCode_t qosim_enable_datawriter(qos_policy_t* const p_this, Entity* p_entity)
{
	module_t* p_module = current_object(get_domain_participant_factory_module_id());
	DataWriter* p_this_entity = (DataWriter*) p_entity;
	Publisher* p_factory;

	if (!p_this_entity || p_this_entity->i_entity_type != DATAWRITER_ENTITY)
	{
		return RETCODE_BAD_PARAMETER;
	}

	if (qosim_is_enabled(p_this, (Entity*) p_this_entity))
	{
		return RETCODE_OK;
	}

	p_factory = p_this_entity->get_publisher(p_this_entity);

	if (!qosim_is_enabled(p_this, (Entity*) p_factory))
	{
		return RETCODE_PRECONDITION_NOT_MET;
	}

	add_entity_to_service(p_entity);
	p_this_entity->b_enable = true;

	return RETCODE_OK;
}
ReturnCode_t qosim_enable_datareader(qos_policy_t* const p_this, Entity* p_entity)
{
	DataReader* p_this_entity = (DataReader*) p_entity;
	Subscriber* p_factory;

	if (!p_this_entity || p_this_entity->i_entity_type != DATAREADER_ENTITY)
	{
		return RETCODE_BAD_PARAMETER;
	}

	if (qosim_is_enabled(p_this, (Entity*) p_this_entity))
	{
		return RETCODE_OK;
	}

	p_factory = p_this_entity->get_subscriber(p_this_entity);

	if (!qosim_is_enabled(p_this, (Entity*) p_factory))
	{
		return RETCODE_PRECONDITION_NOT_MET;
	}

	add_entity_to_service(p_entity);
	p_this_entity->b_enable = true;

	return RETCODE_OK;
}
ReturnCode_t qosim_enable_publisher(qos_policy_t* const p_this, Entity* p_entity)
{
	Publisher* p_this_entity = (Publisher*) p_entity;
	DomainParticipant* p_factory;
	int32_t i;

	if (!p_this_entity || p_this_entity->i_entity_type != PUBLISHER_ENTITY)
	{
		return RETCODE_BAD_PARAMETER;
	}

	if (qosim_is_enabled(p_this, (Entity*) p_this_entity))
	{
		return RETCODE_OK;
	}

	p_factory = p_this_entity->get_participant(p_this_entity);

	if (!qosim_is_enabled(p_this, (Entity*) p_factory))
	{
		return RETCODE_PRECONDITION_NOT_MET;
	}
	p_this_entity->b_enable = true;

	if (is_auto_enabled_created_entities(&p_this_entity->publisher_qos.entity_factory))
	{
		for (i = 0; i < p_this_entity->i_datawriters; i++)
		{
			DataWriter* child = p_this_entity->pp_datawriters[i];
			child->enable((Entity*) child);
		}
	}

	return RETCODE_OK;
}
ReturnCode_t qosim_enable_subscriber(qos_policy_t* const p_this, Entity* p_entity)
{
	Subscriber* p_this_entity = (Subscriber*) p_entity;
	DomainParticipant* p_factory;
	int32_t i;

	if (!p_this_entity || p_this_entity->i_entity_type != SUBSCRIBER_ENTITY)
	{
		return RETCODE_BAD_PARAMETER;
	}

	if (qosim_is_enabled(p_this, (Entity*) p_this_entity))
	{
		return RETCODE_OK;
	}

	p_factory = p_this_entity->get_participant(p_this_entity);

	if (!qosim_is_enabled(p_this, (Entity*) p_factory))
	{
		return RETCODE_PRECONDITION_NOT_MET;
	}
	p_this_entity->b_enable = true;

	if (is_auto_enabled_created_entities(&p_this_entity->subscriber_qos.entity_factory))
	{
		for (i = 0; i < p_this_entity->i_datareaders; i++)
		{
			DataReader* child = p_this_entity->pp_datareaders[i];
			child->enable((Entity*) child);
		}
	}

	return RETCODE_OK;
}
/************************************************************************/
/* �Ķ���� p_entity �� �־��� DomainParticipant �� enable �Ѵ�.        */
/************************************************************************/
ReturnCode_t qosim_enable_domainparticipant(qos_policy_t* const p_this, Entity* p_entity)
{
	DomainParticipant* p_this_entity = (DomainParticipant*) p_entity;
	int32_t i;

	// �߸��� �Ķ���� ���� �˻��Ѵ�.
	if (!p_this_entity || p_this_entity->i_entity_type != DOMAINPARTICIPANT_ENTITY)
	{
		return RETCODE_BAD_PARAMETER;
	}

	// �̹� enable ���¶��, �ٽ� enable �� �ʿ����.
	if (qosim_is_enabled(p_this, (Entity*) p_this_entity))
	{
		return RETCODE_OK;
	}

	// enable ���·� �ٲٰ�, RTPS ��ü�� �����Ѵ�.
	add_entity_to_service(p_entity);
	p_this_entity->b_enable = true;

	if (is_auto_enabled_created_entities(&p_this_entity->qos.entity_factory))
	{
		for (i = 0; i < p_this_entity->i_publisher; i++)
		{
			Publisher* child = p_this_entity->pp_publisher[i];
			qosim_enable_publisher(p_this, (Entity*) child);
		}
		for (i = 0; i < p_this_entity->i_subscriber; i++)
		{
			Subscriber* child = p_this_entity->pp_subscriber[i];
			qosim_enable_subscriber(p_this, (Entity*) child);//child->enable((Entity*) child);
		}
		for (i = 0; i < p_this_entity->i_topic; i++)
		{
			Topic* child = p_this_entity->pp_topic[i];
			qosim_enable_topic(p_this, (Entity*) child);//child->enable((Entity*) child);
		}

		for (i = 0; i < p_this_entity->i_builtin_topics; i++)
		{
			Topic* child = p_this_entity->pp_builtin_topics[i];
			qosim_enable_topic(p_this, (Entity*) child);//child->enable((Entity*) child);
		}
		qosim_enable_publisher(p_this, (Entity*) p_this_entity->p_builtin_publisher);
	}

	return RETCODE_OK;
}
