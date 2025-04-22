#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


int launch_monitoring_module( module_object_t* p_this )
{
	module_list_t *p_list = NULL;
	int i_index = -1;
	monitoring_t *p_monitoring = NULL;
	moduleload_t *p_parser;

	p_list = moduleListFind(OBJECT(p_this),OBJECT_MODULE_LOAD, FIND_ANYWHERE);

	for( i_index = 0; i_index < p_list->i_count; i_index++ )
    {
		p_parser = (moduleload_t *)p_list->p_values[i_index].p_object;

		if((p_parser->psz_capability && strcmp( "monitoring", p_parser->psz_capability )) )
        {
            continue;
        }

		p_monitoring = object_create( p_this ,OBJECT_MONITORING);

		if(p_monitoring && (p_parser->pf_activate(OBJECT(p_monitoring)) == MODULE_SUCCESS))
		{
			p_monitoring->p_moduleload = p_parser;
			object_attach( OBJECT(p_monitoring), p_this );	
		}else{
			object_destroy(OBJECT(p_monitoring));
		}
	}

	if(p_list)moduleListRelease( p_list );

	return MODULE_SUCCESS;
}


void  monitoring__add_participant(DomainParticipant* p_participant)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->addParticipant(p_monitoring, p_participant);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

void  monitoring__delete_participant(DomainParticipant* p_participant)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->deleteParticipant(p_monitoring, p_participant);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

void  monitoring__add_publisher(Publisher* p_publisher)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->addPublisher(p_monitoring, p_publisher);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

void  monitoring__delete_publisher(Publisher* p_publisher)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->deletePublisher(p_monitoring, p_publisher);
		}

		if (p_list) moduleListRelease(p_list);
	}
}


void monitoring__add_subscriber(Subscriber* p_subscriber)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for( i_index = 0; i_index < p_list->i_count; i_index++ )
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->addSubscriber(p_monitoring, p_subscriber);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

void monitoring__delete_subscriber(Subscriber* p_subscriber)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->deleteSubscriber(p_monitoring, p_subscriber);
		}

		if (p_list) moduleListRelease(p_list);
	}
}


void monitoring__add_topic(Topic* p_topic)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->addTopic(p_monitoring, p_topic);
		}


		if (p_list) moduleListRelease(p_list);
	}
}

void monitoring__delete_topic(Topic* p_topic)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->deleteTopic(p_monitoring, p_topic);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

void  monitoring__add_datareader(DataReader* p_datareader)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->addDataReader(p_monitoring, p_datareader);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

void  monitoring__delete_datareader(DataReader* p_datareader)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->deleteDataReader(p_monitoring, p_datareader);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

void  monitoring__add_datawriter(DataWriter* p_datawriter)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->addDataWriter(p_monitoring, p_datawriter);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

void monitoring__delete_datawriter(DataWriter* p_datawriter)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->deleteDataWriter(p_monitoring, p_datawriter);
		}

		if(p_list)moduleListRelease( p_list );
	}
}

void monitoring__add_writerproxy(rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->add_writerproxy(p_monitoring, p_statefulreader, p_writerproxy);
		}


		if (p_list) moduleListRelease(p_list);
	}
}

void monitoring__delete_writerproxy(rtps_statefulreader_t* p_statefulreader, rtps_writerproxy_t* p_writerproxy)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->deleteWriterProxy(p_monitoring, p_statefulreader, p_writerproxy);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

void monitoring__add_readerproxy(rtps_statefulwriter_t* p_statefulwriter, rtps_readerproxy_t* p_readerproxy)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->addReaderProxy(p_monitoring, p_statefulwriter, p_readerproxy);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

void monitoring__delete_readerproxy(rtps_statefulwriter_t* p_statefulwriter, rtps_readerproxy_t* p_readerproxy)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->deleteReaderProxy(p_monitoring, p_statefulwriter, p_readerproxy);
		}

		if(p_list)moduleListRelease( p_list );
	}
}


void monitoring__warning_condtion(int32_t entity, char* p_condition, int32_t warning_or_error, int32_t level, int32_t code)
{
	module_list_t *p_list = NULL;
	monitoring_t *p_monitoring = NULL;
	int i_index = -1;
	module_t *p_module = current_object(get_domain_participant_factory_module_id());

	if (p_condition == NULL) return;

	if (p_module)
	{
		p_list = moduleListFind(OBJECT(p_module), OBJECT_MONITORING, FIND_CHILD);

		for (i_index = 0; i_index < p_list->i_count; i_index++)
		{
			p_monitoring = (monitoring_t *)p_list->p_values[i_index].p_object;
			p_monitoring->warningcodition(p_monitoring, entity, p_condition, warning_or_error, level, code);
		}

		if (p_list) moduleListRelease(p_list);
	}
}

