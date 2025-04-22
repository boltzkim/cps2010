// status by jun

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>

void init_OfferedDeadlineMissed(OfferedDeadlineMissedStatus* status)
{
	status->total_count = 0;
	status->total_count_change = 0;
	status->last_instance_handle = 0;
}

ReturnCode_t change_OfferedDeadlineMissed_status(DataWriter * p_datawriter,InstanceHandle_t handle)
{
	Publisher *p_publisher;
	DomainParticipant *p_domainParticipant;

	p_publisher = p_datawriter->p_publisher;
	p_domainParticipant = p_datawriter->p_publisher->p_domain_participant;
	
	p_datawriter->status.offered_deadline_missed.total_count++;
	p_datawriter->status.offered_deadline_missed.total_count_change++;
	p_datawriter->status.offered_deadline_missed.last_instance_handle = handle;
	
	p_datawriter->status_changed_flag.b_offered_deadline_missed_status_changed_flag = true;

	p_datawriter->l_status_changes += OFFERED_DEADLINE_MISSED_STATUS;

	if ((p_datawriter->l_status_mask & OFFERED_DEADLINE_MISSED_STATUS )&&(p_datawriter->p_datawriter_listener != NULL)&&(p_datawriter->p_datawriter_listener->on_offered_deadline_missed != NULL)) {

		p_datawriter->p_datawriter_listener->on_offered_deadline_missed(p_datawriter, &p_datawriter->status.offered_deadline_missed);
		p_datawriter->status.offered_deadline_missed.total_count_change = 0;
		p_datawriter->status_changed_flag.b_offered_deadline_missed_status_changed_flag = false;
		p_datawriter->l_status_changes -= OFFERED_DEADLINE_MISSED_STATUS;

	} else if ((p_publisher->l_status_mask & OFFERED_DEADLINE_MISSED_STATUS )&&(p_publisher->p_publisher_listener != NULL)&&(p_publisher->p_publisher_listener->on_offered_deadline_missed != NULL)) {

		p_publisher->p_publisher_listener->on_offered_deadline_missed(p_datawriter, &p_datawriter->status.offered_deadline_missed);
		p_datawriter->status.offered_deadline_missed.total_count_change = 0;
		p_datawriter->status_changed_flag.b_offered_deadline_missed_status_changed_flag = false;
		p_datawriter->l_status_changes -= OFFERED_DEADLINE_MISSED_STATUS;
	} else if ((p_domainParticipant->l_status_mask & OFFERED_DEADLINE_MISSED_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_offered_deadline_missed != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_offered_deadline_missed(p_datawriter, &p_datawriter->status.offered_deadline_missed);
		p_datawriter->status.offered_deadline_missed.total_count_change = 0;
		p_datawriter->status_changed_flag.b_offered_deadline_missed_status_changed_flag = false;
		p_datawriter->l_status_changes -= OFFERED_DEADLINE_MISSED_STATUS;

	}

	status_condition_plain_status_set( (Entity *) p_datawriter, OFFERED_DEADLINE_MISSED_STATUS );

	return RETCODE_OK;
}

void init_OfferedIncompatibleQoS(OfferedIncompatibleQosStatus* status)
{
	int i;
	status->total_count = 0;
	status->total_count_change = 0;
	status->last_policy_id = 0; 
	//check by jun   check all the QoS
	status->policies = (QosPolicyCount *)malloc(sizeof(QosPolicyCount)*22);
	memset(status->policies, '\0', sizeof(QosPolicyCount) * 22);

	for(i=0; i<22; i++){
		status->policies[i].policy_id = i; //QoS Policy 값 
		status->policies[i].count = 0;	
	}
}

ReturnCode_t change_OfferedIncompatibleQoS_status(DataWriter * p_datawriter, QosPolicyId_t id)
{
	int i;
	Publisher *p_publisher;
	DomainParticipant *p_domainParticipant;

	p_publisher = p_datawriter->p_publisher;
	p_domainParticipant = p_datawriter->p_publisher->p_domain_participant;
	
	p_datawriter->status.offered_incompatible_qos.total_count++;
	p_datawriter->status.offered_incompatible_qos.total_count_change++;
	p_datawriter->status.offered_incompatible_qos.last_policy_id = id;

	for(i=0; i<22 ; i++){
		if(p_datawriter->status.offered_incompatible_qos.policies[i].policy_id == id){
			p_datawriter->status.offered_incompatible_qos.policies[i].count++;
			break;
		}		
	}

//	check by jun
//	p_datawriter->status.offered_incompatible_qos.policies 
	
	p_datawriter->status_changed_flag.b_offered_incompatible_qos_status_changed_flag = true;
	p_datawriter->l_status_changes += OFFERED_INCOMPATIBLE_QOS_STATUS;
	if ((p_datawriter->l_status_mask & OFFERED_INCOMPATIBLE_QOS_STATUS )&&(p_datawriter->p_datawriter_listener != NULL)&&(p_datawriter->p_datawriter_listener->on_offered_incompatible_qos != NULL)) {

		p_datawriter->p_datawriter_listener->on_offered_incompatible_qos(p_datawriter, &p_datawriter->status.offered_incompatible_qos);
		p_datawriter->status.offered_incompatible_qos.total_count_change = 0;
		p_datawriter->status_changed_flag.b_offered_incompatible_qos_status_changed_flag = false;
		p_datawriter->l_status_changes -= OFFERED_INCOMPATIBLE_QOS_STATUS;
	} else if ((p_publisher->l_status_mask & OFFERED_INCOMPATIBLE_QOS_STATUS )&&(p_publisher->p_publisher_listener != NULL)&&(p_publisher->p_publisher_listener->on_offered_incompatible_qos != NULL)) {

		p_publisher->p_publisher_listener->on_offered_incompatible_qos(p_datawriter, &p_datawriter->status.offered_incompatible_qos);
		p_datawriter->status.offered_incompatible_qos.total_count_change = 0;
		p_datawriter->status_changed_flag.b_offered_incompatible_qos_status_changed_flag = false;
		p_datawriter->l_status_changes -= OFFERED_INCOMPATIBLE_QOS_STATUS;
	} else if ((p_domainParticipant->l_status_mask & OFFERED_INCOMPATIBLE_QOS_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_offered_incompatible_qos != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_offered_incompatible_qos(p_datawriter, &p_datawriter->status.offered_incompatible_qos);
		p_datawriter->status.offered_incompatible_qos.total_count_change = 0;
		p_datawriter->status_changed_flag.b_offered_incompatible_qos_status_changed_flag = false;
		p_datawriter->l_status_changes -= OFFERED_INCOMPATIBLE_QOS_STATUS;
	}

	status_condition_plain_status_set( (Entity *) p_datawriter, OFFERED_INCOMPATIBLE_QOS_STATUS );
	
	return RETCODE_OK;
}


void init_LivelinessLost(LivelinessLostStatus *status)
{
	status->total_count = 0;
	status->total_count_change = 0; 
}

ReturnCode_t change_LivelinessLost_status(DataWriter * p_datawriter)
{
	Publisher *p_publisher;
	DomainParticipant *p_domainParticipant;

	p_publisher = p_datawriter->p_publisher;
	p_domainParticipant = p_datawriter->p_publisher->p_domain_participant;

	p_datawriter->status.liveliness_lost.total_count++;
	p_datawriter->status.liveliness_lost.total_count_change++;

	p_datawriter->status_changed_flag.b_liveliness_lost_status_changed_flag = true;
	p_datawriter->l_status_changes += LIVELINESS_LOST_STATUS;
	if ((p_datawriter->l_status_mask & LIVELINESS_LOST_STATUS )&&(p_datawriter->p_datawriter_listener != NULL)&&(p_datawriter->p_datawriter_listener->on_liveliness_lost != NULL)) {

		p_datawriter->p_datawriter_listener->on_liveliness_lost(p_datawriter, &p_datawriter->status.liveliness_lost);
		p_datawriter->status.liveliness_lost.total_count_change = 0;
		p_datawriter->status_changed_flag.b_liveliness_lost_status_changed_flag = false;
		p_datawriter->l_status_changes -= LIVELINESS_LOST_STATUS;
	} else if((p_publisher->l_status_mask & LIVELINESS_LOST_STATUS )&&(p_publisher->p_publisher_listener != NULL)&&(p_publisher->p_publisher_listener->on_liveliness_lost != NULL)){

		p_publisher->p_publisher_listener->on_liveliness_lost(p_datawriter, &p_datawriter->status.liveliness_lost);
		p_datawriter->status.liveliness_lost.total_count_change = 0;
		p_datawriter->status_changed_flag.b_liveliness_lost_status_changed_flag = false;
		p_datawriter->l_status_changes -= LIVELINESS_LOST_STATUS;
	} else if((p_domainParticipant->l_status_mask & LIVELINESS_LOST_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_liveliness_lost != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_liveliness_lost(p_datawriter, &p_datawriter->status.liveliness_lost);
		p_datawriter->status.liveliness_lost.total_count_change = 0;
		p_datawriter->status_changed_flag.b_liveliness_lost_status_changed_flag = false;
		p_datawriter->l_status_changes -= LIVELINESS_LOST_STATUS;
	}
// 추가 필요	
//	status_condition_plain_status_set( (Entity *) p_datawriter, LIVELINESS_LOST_STATUS );
	
	return RETCODE_OK;
}



void init_PublicationMathced(PublicationMatchedStatus* status)
{
	status->total_count = 0;
	status->total_count_change = 0; 
	status->current_count = 0;
	status->current_count_change = 0;
	status->last_subscription_handle = 0;
}

ReturnCode_t change_PublicationMathced_status(DataWriter * p_datawriter, int current_count, InstanceHandle_t handle)
{
	Publisher *p_publisher;
	DomainParticipant *p_domainParticipant;

	p_publisher = p_datawriter->p_publisher;
	p_domainParticipant = p_datawriter->p_publisher->p_domain_participant;
	
	p_datawriter->status.publication_matched.total_count++;
	p_datawriter->status.publication_matched.total_count_change++;
	p_datawriter->status.publication_matched.current_count = current_count;
	p_datawriter->status.publication_matched.current_count_change++;
	p_datawriter->status.publication_matched.last_subscription_handle = handle;
//	check by jun
//	p_datawriter->status.offered_incompatible_qos.policies 
	
	p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = true;
	p_datawriter->l_status_changes += PUBLICATION_MATCHED_STATUS;
	
	if ((p_datawriter->l_status_mask & PUBLICATION_MATCHED_STATUS )&&(p_datawriter->p_datawriter_listener != NULL)&&(p_datawriter->p_datawriter_listener->on_publication_matched != NULL)){
		p_datawriter->p_datawriter_listener->on_publication_matched(p_datawriter, &p_datawriter->status.publication_matched);
		p_datawriter->status.publication_matched.total_count_change = 0;
		p_datawriter->status.publication_matched.current_count_change = 0;
		p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = false;
		p_datawriter->l_status_changes -= PUBLICATION_MATCHED_STATUS;

	} else if((p_publisher->l_status_mask & PUBLICATION_MATCHED_STATUS )&&(p_publisher->p_publisher_listener != NULL)&&(p_publisher->p_publisher_listener->on_publication_matched != NULL)){
		p_publisher->p_publisher_listener->on_publication_matched(p_datawriter, &p_datawriter->status.publication_matched);
		p_datawriter->status.publication_matched.total_count_change = 0;
		p_datawriter->status.publication_matched.current_count_change = 0;
		p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = false;
		p_datawriter->l_status_changes -= PUBLICATION_MATCHED_STATUS;
		
	} else if((p_domainParticipant->l_status_mask & PUBLICATION_MATCHED_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_publication_matched != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_publication_matched(p_datawriter, &p_datawriter->status.publication_matched);
		p_datawriter->status.publication_matched.total_count_change = 0;
		p_datawriter->status.publication_matched.current_count_change = 0;
		p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = false;
		p_datawriter->l_status_changes -= PUBLICATION_MATCHED_STATUS;
		
	}

	status_condition_plain_status_set( (Entity *) p_datawriter, PUBLICATION_MATCHED_STATUS );
	return RETCODE_OK;
}

ReturnCode_t change_publication_mathced_current_count_status( DataWriter* p_datawriter, int current_count )
{
	Publisher *p_publisher;
	DomainParticipant *p_domainParticipant;

	p_publisher = p_datawriter->p_publisher;
	p_domainParticipant = p_datawriter->p_publisher->p_domain_participant;
	
	p_datawriter->status.publication_matched.current_count = current_count;
	p_datawriter->status.publication_matched.current_count_change++ ;

//	check by jun
//	p_datawriter->status.offered_incompatible_qos.policies 
	
	p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = true;
	p_datawriter->l_status_changes += PUBLICATION_MATCHED_STATUS;
	
	if ((p_datawriter->l_status_mask & PUBLICATION_MATCHED_STATUS )&&(p_datawriter->p_datawriter_listener != NULL)&&(p_datawriter->p_datawriter_listener->on_publication_matched != NULL)){

		p_datawriter->p_datawriter_listener->on_publication_matched(p_datawriter, &p_datawriter->status.publication_matched);
		p_datawriter->status.publication_matched.total_count_change = 0;
		p_datawriter->status.publication_matched.current_count_change = 0;
		p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = false;
		p_datawriter->l_status_changes -= PUBLICATION_MATCHED_STATUS;

	} else if ((p_publisher->l_status_mask & PUBLICATION_MATCHED_STATUS )&&(p_publisher->p_publisher_listener != NULL)&&(p_publisher->p_publisher_listener->on_publication_matched != NULL)){

		p_publisher->p_publisher_listener->on_publication_matched(p_datawriter, &p_datawriter->status.publication_matched);
		p_datawriter->status.publication_matched.total_count_change = 0;
		p_datawriter->status.publication_matched.current_count_change = 0;
		p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = false;
		p_datawriter->l_status_changes -= PUBLICATION_MATCHED_STATUS;
		
	} else if ((p_domainParticipant->l_status_mask & PUBLICATION_MATCHED_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_publication_matched != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_publication_matched(p_datawriter, &p_datawriter->status.publication_matched);
		p_datawriter->status.publication_matched.total_count_change = 0;
		p_datawriter->status.publication_matched.current_count_change = 0;
		p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = false;
		p_datawriter->l_status_changes -= PUBLICATION_MATCHED_STATUS;
	}

	status_condition_plain_status_set( (Entity *) p_datawriter, PUBLICATION_MATCHED_STATUS );
	return RETCODE_OK;
}

void init_datawriter_status_change_flag(DataWriter* p_datawriter)
{
	p_datawriter->status_changed_flag.b_liveliness_lost_status_changed_flag = false;
	p_datawriter->status_changed_flag.b_offered_deadline_missed_status_changed_flag = false;
	p_datawriter->status_changed_flag.b_offered_incompatible_qos_status_changed_flag = false;
	p_datawriter->status_changed_flag.b_publication_matched_status_changed_flag = false;

}

void init_datawriter_status(DataWriter * p_datawriter)
{
	init_OfferedDeadlineMissed(&p_datawriter->status.offered_deadline_missed);
	init_OfferedIncompatibleQoS(&p_datawriter->status.offered_incompatible_qos);
	init_LivelinessLost(&p_datawriter->status.liveliness_lost);
	init_PublicationMathced(&p_datawriter->status.publication_matched);
	init_datawriter_status_change_flag(p_datawriter);
}

void destroy_datawriter_status(DataWriter * p_datawriter)
{
	free(p_datawriter->status.offered_incompatible_qos.policies);

}


///// datareader status ////////

void init_RequestedDeadlineMissed(RequestedDeadlineMissedStatus* status)
{
	status->total_count = 0;
	status->total_count_change = 0;
	status->last_instance_handle = 0;
}

ReturnCode_t change_RequestedDeadlineMissed_status(DataReader * p_datareader,InstanceHandle_t handle)
{
	Subscriber *p_subscriber;
	DomainParticipant *p_domainParticipant;

	p_subscriber = p_datareader->p_subscriber;
	p_domainParticipant = p_datareader->p_subscriber->p_domain_participant;
	
	p_datareader->status.requested_deadline_missed.total_count++;
	p_datareader->status.requested_deadline_missed.total_count_change++;
	p_datareader->status.requested_deadline_missed.last_instance_handle = handle;
	
	p_datareader->status_changed_flag.b_requested_deadline_missed_status_changed_flag = true;
	p_datareader->l_status_changes += REQUESTED_DEADLINE_MISSED_STATUS;
	
	if ((p_datareader->l_status_mask & REQUESTED_DEADLINE_MISSED_STATUS )&&(p_datareader->p_datareader_listener != NULL)&&(p_datareader->p_datareader_listener->on_requested_deadline_missed != NULL)) {

		p_datareader->p_datareader_listener->on_requested_deadline_missed(p_datareader, &p_datareader->status.requested_deadline_missed);
		p_datareader->status.requested_deadline_missed.total_count_change = 0;
		p_datareader->status_changed_flag.b_requested_deadline_missed_status_changed_flag = false;
		p_datareader->l_status_changes -= REQUESTED_DEADLINE_MISSED_STATUS;
		
	}else if((p_subscriber->l_status_mask & REQUESTED_DEADLINE_MISSED_STATUS )&&(p_subscriber->p_subscriber_listener != NULL)&&(p_subscriber->p_subscriber_listener->on_requested_deadline_missed != NULL)){

		p_subscriber->p_subscriber_listener->on_requested_deadline_missed(p_datareader, &p_datareader->status.requested_deadline_missed);
		p_datareader->status.requested_deadline_missed.total_count_change = 0;
		p_datareader->status_changed_flag.b_requested_deadline_missed_status_changed_flag = false;
		p_datareader->l_status_changes -= REQUESTED_DEADLINE_MISSED_STATUS;
		
	}else if((p_domainParticipant->l_status_mask & REQUESTED_DEADLINE_MISSED_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_requested_deadline_missed != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_requested_deadline_missed(p_datareader, &p_datareader->status.requested_deadline_missed);
		p_datareader->status.requested_deadline_missed.total_count_change = 0;
		p_datareader->status_changed_flag.b_requested_deadline_missed_status_changed_flag = false;
		p_datareader->l_status_changes -= REQUESTED_DEADLINE_MISSED_STATUS;
		
	}

	status_condition_plain_status_set( (Entity *) p_datareader, REQUESTED_DEADLINE_MISSED_STATUS );
	return RETCODE_OK;
}

void init_RequestedIncompatibleQoS(RequestedIncompatibleQosStatus* status)
{
	int i=0;
	status->total_count = 0;
	status->total_count_change = 0;
	status->last_policy_id = 0;
//check by jun   check all the QoS
	status->policies = (QosPolicyCount *)malloc(sizeof(QosPolicyCount)*22);
	memset(status->policies, '\0', sizeof(QosPolicyCount) * 22);

	for(i=0; i<22; i++){
		status->policies[i].policy_id = i; //QoS Policy 값 
		status->policies[i].count = 0;	
	}
}

ReturnCode_t change_RequestedIncompatibleQoS_status(DataReader * p_datareader,QosPolicyId_t id)
{
	int i;

	Subscriber *p_subscriber;
	DomainParticipant *p_domainParticipant;

	p_subscriber = p_datareader->p_subscriber;
	p_domainParticipant = p_datareader->p_subscriber->p_domain_participant;
	
	p_datareader->status.requested_incompatible_qos.total_count++;
	p_datareader->status.requested_incompatible_qos.total_count_change++;
	p_datareader->status.requested_incompatible_qos.last_policy_id = id;

	for(i=0; i<22 ; i++){
		if(p_datareader->status.requested_incompatible_qos.policies[i].policy_id == id){
			p_datareader->status.requested_incompatible_qos.policies[i].count++;
			break;
		}		
	}
	//check by jun
	//	p_datareader->status.requested_incompatible_qos.policies;
	
	p_datareader->status_changed_flag.b_requested_incompatible_qos_status_changed_flag = true;
	p_datareader->l_status_changes += REQUESTED_INCOMPATIBLE_QOS_STATUS;

	if ((p_datareader->l_status_mask & REQUESTED_INCOMPATIBLE_QOS_STATUS )&&(p_datareader->p_datareader_listener != NULL)&&(p_datareader->p_datareader_listener->on_requested_incompatible_qos != NULL)){

		p_datareader->p_datareader_listener->on_requested_incompatible_qos(p_datareader, &p_datareader->status.requested_incompatible_qos);
		p_datareader->status.requested_incompatible_qos.total_count_change = 0;
		p_datareader->status_changed_flag.b_requested_incompatible_qos_status_changed_flag = false;
		p_datareader->l_status_changes -= REQUESTED_INCOMPATIBLE_QOS_STATUS;
		
	} else if ((p_subscriber->l_status_mask & REQUESTED_INCOMPATIBLE_QOS_STATUS )&&(p_subscriber->p_subscriber_listener != NULL)&&(p_subscriber->p_subscriber_listener->on_requested_incompatible_qos != NULL)){

		p_subscriber->p_subscriber_listener->on_requested_incompatible_qos(p_datareader, &p_datareader->status.requested_incompatible_qos);
		p_datareader->status.requested_incompatible_qos.total_count_change = 0;
		p_datareader->status_changed_flag.b_requested_incompatible_qos_status_changed_flag = false;
		p_datareader->l_status_changes -= REQUESTED_INCOMPATIBLE_QOS_STATUS;
		
	} else if ((p_domainParticipant->l_status_mask & REQUESTED_INCOMPATIBLE_QOS_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_requested_incompatible_qos != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_requested_incompatible_qos(p_datareader, &p_datareader->status.requested_incompatible_qos);
		p_datareader->status.requested_incompatible_qos.total_count_change = 0;
		p_datareader->status_changed_flag.b_requested_incompatible_qos_status_changed_flag = false;
		p_datareader->l_status_changes -= REQUESTED_INCOMPATIBLE_QOS_STATUS;
	}

	status_condition_plain_status_set( (Entity *) p_datareader, REQUESTED_INCOMPATIBLE_QOS_STATUS );
	
	return RETCODE_OK;
}


void init_SampleRejected(SampleRejectedStatus* status)
{
	status->total_count = 0;
	status->total_count_change = 0;
	status->last_reason = 0;
	status->last_instance_handle = 0;
}

ReturnCode_t change_SampleRejected_status(DataReader * p_datareader,SampleRejectedStatusKind reason, InstanceHandle_t handle)
{

	Subscriber *p_subscriber;
	DomainParticipant *p_domainParticipant;

	p_subscriber = p_datareader->p_subscriber;
	p_domainParticipant = p_datareader->p_subscriber->p_domain_participant;
	
	p_datareader->status.sample_rejected.total_count++;
	p_datareader->status.sample_rejected.total_count_change++;
	p_datareader->status.sample_rejected.last_reason = reason;
	p_datareader->status.sample_rejected.last_instance_handle = handle;
	
	p_datareader->status_changed_flag.b_sample_rejected_status_changed_flag = true;
	p_datareader->l_status_changes += SAMPLE_REJECTED_STATUS;

	if ((p_datareader->l_status_mask & SAMPLE_REJECTED_STATUS )&&(p_datareader->p_datareader_listener != NULL)&&(p_datareader->p_datareader_listener->on_sample_rejected != NULL)){

		p_datareader->p_datareader_listener->on_sample_rejected(p_datareader, &p_datareader->status.sample_rejected);
		p_datareader->status.sample_rejected.total_count_change = 0;
		p_datareader->status_changed_flag.b_sample_rejected_status_changed_flag = false;
		p_datareader->l_status_changes -= SAMPLE_REJECTED_STATUS;
		
	} else if ((p_subscriber->l_status_mask & SAMPLE_REJECTED_STATUS )&&(p_subscriber->p_subscriber_listener != NULL)&&(p_subscriber->p_subscriber_listener->on_sample_rejected != NULL)){

		p_subscriber->p_subscriber_listener->on_sample_rejected(p_datareader, &p_datareader->status.sample_rejected);
		p_datareader->status.sample_rejected.total_count_change = 0;
		p_datareader->status_changed_flag.b_sample_rejected_status_changed_flag = false;
		p_datareader->l_status_changes -= SAMPLE_REJECTED_STATUS;
		
	} else if ((p_domainParticipant->l_status_mask & SAMPLE_REJECTED_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_sample_rejected != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_sample_rejected(p_datareader, &p_datareader->status.sample_rejected);
		p_datareader->status.sample_rejected.total_count_change = 0;
		p_datareader->status_changed_flag.b_sample_rejected_status_changed_flag = false;
		p_datareader->l_status_changes -= SAMPLE_REJECTED_STATUS;
	}

//		status_condition_plain_status_set( (Entity *) p_datareader, SAMPLE_REJECTED_STATUS );
	return RETCODE_OK;
}


void init_LivelinessChanged(LivelinessChangedStatus* status)
{
	status->alive_count = 0;
	status->not_alive_count = 0;
	status->alive_count_change = 0;
	status->not_alive_count_change = 0;
	status->last_publication_handle = 0;	
}

ReturnCode_t change_LivelinessChanged_status(DataReader * p_datareader, InstanceHandle_t handle, const bool b_is_alive)
{
	Subscriber *p_subscriber;
	DomainParticipant *p_domainParticipant;

	p_subscriber = p_datareader->p_subscriber;
	p_domainParticipant = p_datareader->p_subscriber->p_domain_participant;

	if (b_is_alive == true)
	{
		p_datareader->status.liveliness_changed.alive_count++;
		p_datareader->status.liveliness_changed.not_alive_count--;
		p_datareader->status.liveliness_changed.alive_count_change++;
		p_datareader->status.liveliness_changed.not_alive_count_change--;
	}
	else
	{
		p_datareader->status.liveliness_changed.alive_count--;
		p_datareader->status.liveliness_changed.not_alive_count++;
		p_datareader->status.liveliness_changed.alive_count_change--;
		p_datareader->status.liveliness_changed.not_alive_count_change++;
	}
	p_datareader->status.liveliness_changed.last_publication_handle = handle;
	
	p_datareader->status_changed_flag.b_liveliness_changed_status_changed_flag = true;
	p_datareader->l_status_changes += LIVELINESS_CHANGED_STATUS;
		
	if ((p_datareader->l_status_mask & LIVELINESS_CHANGED_STATUS)&&(p_datareader->p_datareader_listener != NULL)&&(p_datareader->p_datareader_listener->on_liveliness_changed != NULL))
	{
		p_datareader->p_datareader_listener->on_liveliness_changed(p_datareader, &p_datareader->status.liveliness_changed);
		p_datareader->status.liveliness_changed.alive_count_change = 0;
		p_datareader->status.liveliness_changed.not_alive_count_change = 0;
		p_datareader->status_changed_flag.b_liveliness_changed_status_changed_flag = false;
		p_datareader->l_status_changes -= LIVELINESS_CHANGED_STATUS;
	}
	else if ((p_subscriber->l_status_mask & LIVELINESS_CHANGED_STATUS)&&(p_subscriber->p_subscriber_listener != NULL)&&(p_subscriber->p_subscriber_listener->on_liveliness_changed != NULL))
	{
		p_subscriber->p_subscriber_listener->on_liveliness_changed(p_datareader, &p_datareader->status.liveliness_changed);
		p_datareader->status.liveliness_changed.alive_count_change = 0;
		p_datareader->status.liveliness_changed.not_alive_count_change = 0;
		p_datareader->status_changed_flag.b_liveliness_changed_status_changed_flag = false;
		p_datareader->l_status_changes -= LIVELINESS_CHANGED_STATUS;
	}
	else if ((p_domainParticipant->l_status_mask & LIVELINESS_CHANGED_STATUS)&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_liveliness_changed != NULL))
	{
		p_domainParticipant->p_domain_participant_listener->on_liveliness_changed(p_datareader, &p_datareader->status.liveliness_changed);
		p_datareader->status.liveliness_changed.alive_count_change = 0;
		p_datareader->status.liveliness_changed.not_alive_count_change = 0;
		p_datareader->status_changed_flag.b_liveliness_changed_status_changed_flag = false;
		p_datareader->l_status_changes -= LIVELINESS_CHANGED_STATUS;
	}
//		status_condition_plain_status_set( (Entity *) p_datareader, SAMPLE_REJECTED_STATUS );
	return RETCODE_OK;
}

void init_SubscriberMathed(SubscriptionMatchedStatus* status)
{
	status->total_count = 0;
	status->total_count_change = 0;
	status->current_count = 0;
	status->current_count_change = 0;
	status->last_publication_handle = 0;
}

ReturnCode_t change_SubscriberMathed_status(DataReader * p_datareader, int current_count, InstanceHandle_t handle)
{
	Subscriber *p_subscriber;
	DomainParticipant *p_domainParticipant;

	p_subscriber = p_datareader->p_subscriber;
	p_domainParticipant = p_datareader->p_subscriber->p_domain_participant;

	p_datareader->status.subscriber_matched.total_count++;
	p_datareader->status.subscriber_matched.total_count_change++;
	p_datareader->status.subscriber_matched.current_count = current_count;
	p_datareader->status.subscriber_matched.current_count_change++;
	p_datareader->status.subscriber_matched.last_publication_handle = handle;
	
	p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = true;
	p_datareader->l_status_changes += SUBSCRIPTION_MATCHED_STATUS;

	if ((p_datareader->l_status_mask & SUBSCRIPTION_MATCHED_STATUS )&&(p_datareader->p_datareader_listener != NULL)&&(p_datareader->p_datareader_listener->on_subscription_matched != NULL)){

		p_datareader->p_datareader_listener->on_subscription_matched(p_datareader, &p_datareader->status.subscriber_matched);
		p_datareader->status.subscriber_matched.total_count_change = 0;
		p_datareader->status.subscriber_matched.current_count_change = 0;
		p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = false;
		p_datareader->l_status_changes -= SUBSCRIPTION_MATCHED_STATUS;
	
	} else if((p_subscriber->l_status_mask & SUBSCRIPTION_MATCHED_STATUS )&&(p_subscriber->p_subscriber_listener != NULL)&&(p_subscriber->p_subscriber_listener->on_subscription_matched != NULL)){

		p_subscriber->p_subscriber_listener->on_subscription_matched(p_datareader, &p_datareader->status.subscriber_matched);
		p_datareader->status.subscriber_matched.total_count_change = 0;
		p_datareader->status.subscriber_matched.current_count_change = 0;
		p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = false;
		p_datareader->l_status_changes -= SUBSCRIPTION_MATCHED_STATUS;
		
	} else if((p_domainParticipant->l_status_mask & SUBSCRIPTION_MATCHED_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_subscription_matched != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_subscription_matched(p_datareader, &p_datareader->status.subscriber_matched);
		p_datareader->status.subscriber_matched.total_count_change = 0;
		p_datareader->status.subscriber_matched.current_count_change = 0;
		p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = false;
		p_datareader->l_status_changes -= SUBSCRIPTION_MATCHED_STATUS;
		
	}

	status_condition_plain_status_set( (Entity *) p_datareader, SUBSCRIPTION_MATCHED_STATUS );
	
	return RETCODE_OK;
}


ReturnCode_t change_SubscriberMathed_CurrentCount_status(DataReader * p_datareader, int current_count)
{
	Subscriber *p_subscriber;
	DomainParticipant *p_domainParticipant;

	p_subscriber = p_datareader->p_subscriber;
	p_domainParticipant = p_datareader->p_subscriber->p_domain_participant;


	p_datareader->status.subscriber_matched.current_count = current_count;
	p_datareader->status.subscriber_matched.current_count_change++;
	
	p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = true;
	p_datareader->l_status_changes += SUBSCRIPTION_MATCHED_STATUS;

	if ((p_datareader->l_status_mask & SUBSCRIPTION_MATCHED_STATUS )&&(p_datareader->p_datareader_listener != NULL)&&(p_datareader->p_datareader_listener->on_subscription_matched != NULL)){

		p_datareader->p_datareader_listener->on_subscription_matched(p_datareader, &p_datareader->status.subscriber_matched);
		p_datareader->status.subscriber_matched.total_count_change = 0;
		p_datareader->status.subscriber_matched.current_count_change = 0;
		p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = false;
		p_datareader->l_status_changes -= SUBSCRIPTION_MATCHED_STATUS;

	} else if ((p_subscriber->l_status_mask & SUBSCRIPTION_MATCHED_STATUS )&&(p_subscriber->p_subscriber_listener != NULL)&&(p_subscriber->p_subscriber_listener->on_subscription_matched != NULL)){

		p_subscriber->p_subscriber_listener->on_subscription_matched(p_datareader, &p_datareader->status.subscriber_matched);
		p_datareader->status.subscriber_matched.total_count_change = 0;
		p_datareader->status.subscriber_matched.current_count_change = 0;
		p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = false;
		p_datareader->l_status_changes -= SUBSCRIPTION_MATCHED_STATUS;
		
	} else if ((p_domainParticipant->l_status_mask & SUBSCRIPTION_MATCHED_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_subscription_matched != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_subscription_matched(p_datareader, &p_datareader->status.subscriber_matched);
		p_datareader->status.subscriber_matched.total_count_change = 0;
		p_datareader->status.subscriber_matched.current_count_change = 0;
		p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = false;
		p_datareader->l_status_changes -= SUBSCRIPTION_MATCHED_STATUS;
	}

	status_condition_plain_status_set( (Entity *) p_datareader, SUBSCRIPTION_MATCHED_STATUS );
	
	return RETCODE_OK;
}


void init_SampleLost(SampleLostStatus* status)
{
	status->total_count = 0;
	status->total_count_change = 0;
}

ReturnCode_t change_sample_lost_status( DataReader* p_datareader )
{
	Subscriber *p_subscriber;
	DomainParticipant *p_domainParticipant;

	p_subscriber = p_datareader->p_subscriber;
	p_domainParticipant = p_datareader->p_subscriber->p_domain_participant;
	
	p_datareader->status.sample_lost.total_count++;
	p_datareader->status.sample_lost.total_count_change++;
	
	p_datareader->status_changed_flag.b_sample_lost_status_changed_flag = true;
	p_datareader->l_status_changes += SAMPLE_LOST_STATUS;

	if ((p_datareader->l_status_mask & SAMPLE_LOST_STATUS )&&(p_datareader->p_datareader_listener != NULL)&&(p_datareader->p_datareader_listener->on_sample_lost != NULL)){

		p_datareader->p_datareader_listener->on_sample_lost(p_datareader, &p_datareader->status.sample_lost);
		p_datareader->status.sample_lost.total_count_change = 0;
		p_datareader->status_changed_flag.b_sample_lost_status_changed_flag = false;
		p_datareader->l_status_changes -= SAMPLE_LOST_STATUS;
		
	} else if ((p_subscriber->l_status_mask & SAMPLE_LOST_STATUS )&&(p_subscriber->p_subscriber_listener != NULL)&&(p_subscriber->p_subscriber_listener->on_sample_lost != NULL)){

		p_subscriber->p_subscriber_listener->on_sample_lost(p_datareader, &p_datareader->status.sample_lost);
		p_datareader->status.sample_lost.total_count_change = 0;
		p_datareader->status_changed_flag.b_sample_lost_status_changed_flag = false;
		p_datareader->l_status_changes -= SAMPLE_LOST_STATUS;
		
	} else if ((p_domainParticipant->l_status_mask & SAMPLE_LOST_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_sample_lost != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_sample_lost(p_datareader, &p_datareader->status.sample_lost);
		p_datareader->status.sample_lost.total_count_change = 0;
		p_datareader->status_changed_flag.b_sample_lost_status_changed_flag = false;
		p_datareader->l_status_changes -= SAMPLE_LOST_STATUS;
		
	}

	status_condition_plain_status_set( (Entity *) p_datareader, SAMPLE_LOST_STATUS );
	
	return RETCODE_OK;
}

ReturnCode_t change_DataOnReaders_DataAvailable_status(DataReader * p_datareader)
{

	Subscriber *p_subscriber;
	DomainParticipant *p_domainParticipant;

	p_subscriber = p_datareader->p_subscriber;
	p_domainParticipant = p_datareader->p_subscriber->p_domain_participant;

	p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = true;	
	p_subscriber->l_status_changes += DATA_ON_READERS_STATUS;
	
	p_datareader->status_changed_flag.b_data_available_status_changed_flag = true;
	p_datareader->l_status_changes += DATA_AVAILABLE_STATUS;

//	check by jun





//	publisher status	
	if((p_subscriber->l_status_mask & DATA_ON_READERS_STATUS )&&(p_subscriber->p_subscriber_listener != NULL)&&(p_subscriber->p_subscriber_listener->on_data_on_readers != NULL)){

//	if((p_subscriber->l_status_mask & DATA_ON_READERS_STATUS )){
			
//		if((p_subscriber->p_subscriber_listener != NULL)){
			
//			if((p_subscriber->p_subscriber_listener->on_data_on_readers != NULL)){
			
				p_subscriber->p_subscriber_listener->on_data_on_readers(p_subscriber);
				p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = false;
				p_subscriber->l_status_changes -= DATA_ON_READERS_STATUS;
//			}
//		}
	} else if ((p_domainParticipant->l_status_mask & DATA_ON_READERS_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_data_on_readers != NULL)){

		p_domainParticipant->p_domain_participant_listener->on_data_on_readers(p_subscriber);
		p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = false;
		p_subscriber->l_status_changes -= DATA_ON_READERS_STATUS;

	} else if ((p_datareader->l_status_mask & DATA_AVAILABLE_STATUS )&&(p_datareader->p_datareader_listener != NULL)&&(p_datareader->p_datareader_listener->on_data_available != NULL)){

		

		p_datareader->p_datareader_listener->on_data_available(p_datareader);

		p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = false;
		p_subscriber->l_status_changes -= DATA_ON_READERS_STATUS;
		
		p_datareader->status_changed_flag.b_data_available_status_changed_flag = false;
		p_datareader->l_status_changes -= DATA_AVAILABLE_STATUS;


		
	} else if ((p_subscriber->l_status_mask & DATA_AVAILABLE_STATUS )&&(p_subscriber->p_subscriber_listener != NULL)&&(p_subscriber->p_subscriber_listener->on_data_available != NULL)){
		p_subscriber->p_subscriber_listener->on_data_available(p_datareader);
		
		p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = false;
		p_subscriber->l_status_changes -= DATA_ON_READERS_STATUS;
		
		p_datareader->status_changed_flag.b_data_available_status_changed_flag = false;
		p_datareader->l_status_changes -= DATA_AVAILABLE_STATUS;
		
	} else if ((p_domainParticipant->l_status_mask & DATA_AVAILABLE_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_data_available != NULL)){
		p_domainParticipant->p_domain_participant_listener->on_data_available(p_datareader);

		p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = false;
		p_subscriber->l_status_changes -= DATA_ON_READERS_STATUS;
		
		p_datareader->status_changed_flag.b_data_available_status_changed_flag = false;
		p_datareader->l_status_changes -= DATA_AVAILABLE_STATUS;
		
	}
	
	
	data_on_readers_statuscondition((Entity *)p_datareader->p_subscriber);



	data_available_statuscondition((Entity *)p_datareader);	



	return RETCODE_OK;
}


void init_datareader_status_change_flag(DataReader * p_datareader)
{
	p_datareader->status_changed_flag.b_sample_rejected_status_changed_flag = false;
	p_datareader->status_changed_flag.b_liveliness_changed_status_changed_flag = false;
	p_datareader->status_changed_flag.b_requested_deadline_missed_status_changed_flag = false;
	p_datareader->status_changed_flag.b_requested_incompatible_qos_status_changed_flag = false;
//추가
	p_datareader->status_changed_flag.b_data_available_status_changed_flag = false;
	p_datareader->status_changed_flag.b_sample_lost_status_changed_flag = false;
	p_datareader->status_changed_flag.b_subscription_matched_status_changed_flag = false;
}


void init_datareader_status(DataReader * p_datareader)
{
	init_RequestedDeadlineMissed(&p_datareader->status.requested_deadline_missed);
	init_RequestedIncompatibleQoS(&p_datareader->status.requested_incompatible_qos);
	init_SampleRejected(&p_datareader->status.sample_rejected);
	init_LivelinessChanged(&p_datareader->status.liveliness_changed);
	init_SubscriberMathed(&p_datareader->status.subscriber_matched);
	init_SampleLost(&p_datareader->status.sample_lost);

	init_datareader_status_change_flag(p_datareader);
}

void destroy_datareader_status(DataReader *p_datareader)
{
	free(p_datareader->status.requested_incompatible_qos.policies);

}


void init_InconsistentTopic(InconsistentTopicStatus* status)
{
	status->total_count = 0;
	status->total_count_change = 0;
}

void init_topic_status(Topic * p_topic)
{
	init_InconsistentTopic(&p_topic->status.inconsistent_topic);
	p_topic->status_changed_flag.b_inconsistent_topic_status_changed_flag = false;

}

ReturnCode_t change_topic_status(Topic * p_topic)
{
	DomainParticipant *p_domainParticipant;
	p_domainParticipant = p_topic->p_domain_participant; 
	
	p_topic->status.inconsistent_topic.total_count++;
	p_topic->status.inconsistent_topic.total_count_change++;
	p_topic->status_changed_flag.b_inconsistent_topic_status_changed_flag = true;
	p_topic->l_status_changes += INCONSISTENT_TOPIC_STATUS;

	if((p_topic->l_status_mask & INCONSISTENT_TOPIC_STATUS )&&(p_topic->p_topic_listener != NULL)&&(p_topic->p_topic_listener->on_inconsistent_topic != NULL)){

		p_topic->p_topic_listener->on_inconsistent_topic(p_topic, &p_topic->status.inconsistent_topic);
		p_topic->status.inconsistent_topic.total_count_change = 0;
		p_topic->status_changed_flag.b_inconsistent_topic_status_changed_flag = false;
		p_topic->l_status_changes -= INCONSISTENT_TOPIC_STATUS;
		
	} else if ((p_domainParticipant->l_status_mask & INCONSISTENT_TOPIC_STATUS )&&(p_domainParticipant->p_domain_participant_listener != NULL)&&(p_domainParticipant->p_domain_participant_listener->on_inconsistent_topic != NULL)){
		p_domainParticipant->p_domain_participant_listener->on_inconsistent_topic(p_topic, &p_topic->status.inconsistent_topic);
		p_topic->status.inconsistent_topic.total_count_change = 0;
		p_topic->status_changed_flag.b_inconsistent_topic_status_changed_flag = false;
		p_topic->l_status_changes -= INCONSISTENT_TOPIC_STATUS;
	}

//	status_condition_plain_status_set( (Entity *) p_topic, INCONSISTENT_TOPIC_STATUS );

	return RETCODE_OK;
}

void init_subscriber_status(Subscriber* p_subscriber)
{
//	추가
	p_subscriber->status_changed_flag.b_data_on_readers_status_changed_flag = false;
}
