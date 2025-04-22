/*
	RTSP Data class
	작성자 : 
	이력
	2010-08-10 : 처음 시작
*/

/*
Represents the data that may be associated with a change made to a data-object.
*/

#include "rtps.h"


DataFull *rtps_data_new(rtps_cachechange_t *p_rtps_cachechange)
{
	DataFull *p_data = malloc(sizeof(DataFull));
	memset(p_data, 0, sizeof(DataFull));

	p_data->writer_id.value = p_rtps_cachechange->writer_guid.entity_id;
	p_data->writer_sn.value = p_rtps_cachechange->sequence_number;
	p_data->p_serialized_data = p_rtps_cachechange->p_data_value;
	p_data->inline_qos.p_head_first = NULL;
	p_data->inline_qos.p_head_last = NULL;
	p_data->inline_qos.i_linked_size = 0;
	p_data->octets_to_inline_qos = 0;

	return p_data;
}


Data *get_participantinfo_type_data()
{
	
	return NULL;
}




void destory_datafull( DataFull* p_data )
{

	ParameterWithValue *p_atom_parameter;
	ParameterWithValue *p_tmp_atom_parameter;

	p_atom_parameter = get_real_parameter(p_data->inline_qos.p_head_first);

	while(p_atom_parameter)
	{
		p_tmp_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);
		remove_linked_list((linked_list_head_t *)&p_data->inline_qos, (linked_list_atom_t *)&p_atom_parameter->a_tom);
		FREE(p_atom_parameter->p_value);
		FREE(p_atom_parameter);

		p_atom_parameter = p_tmp_atom_parameter;
	}

	FREE(p_data);
}

void destory_datafull_all( DataFull* p_data )
{
	ParameterWithValue *p_atom_parameter;
	ParameterWithValue *p_tmp_atom_parameter;

	p_atom_parameter = get_real_parameter(p_data->inline_qos.p_head_first);

	while(p_atom_parameter)
	{
		p_tmp_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);

		remove_linked_list((linked_list_head_t *)&p_data->inline_qos, (linked_list_atom_t *)&p_atom_parameter->a_tom);

		FREE(p_atom_parameter->p_value);
		FREE(p_atom_parameter);

		p_atom_parameter = p_tmp_atom_parameter;
		
	}

	if(p_data->p_serialized_data)
	{
		FREE(p_data->p_serialized_data->p_value);
		FREE(p_data->p_serialized_data);
	}
	FREE(p_data);
}


void destory_datafull_except_serialized_data( DataFull* p_data )
{
	ParameterWithValue *p_atom_parameter;
	ParameterWithValue *p_tmp_atom_parameter;

	p_atom_parameter = get_real_parameter(p_data->inline_qos.p_head_first);

	while(p_atom_parameter)
	{
		p_tmp_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);

		remove_linked_list((linked_list_head_t *)&p_data->inline_qos, (linked_list_atom_t *)&p_atom_parameter->a_tom);

		FREE(p_atom_parameter->p_value);
		FREE(p_atom_parameter);

		p_atom_parameter = p_tmp_atom_parameter;
	}

	/*FREE(p_data->p_serialized_data->p_value);
	FREE(p_data->p_serialized_data);*/
	FREE(p_data);
}


void destory_data_frag_full( DataFragFull* p_datafrag )
{

	ParameterWithValue *p_atom_parameter;
	ParameterWithValue *p_tmp_atom_parameter;

	p_atom_parameter = get_real_parameter(p_datafrag->inline_qos.p_head_first);

	while(p_atom_parameter)
	{
		p_tmp_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);

		remove_linked_list((linked_list_head_t *)&p_datafrag->inline_qos, (linked_list_atom_t *)&p_atom_parameter->a_tom);

		FREE(p_atom_parameter->p_value);
		FREE(p_atom_parameter);

		p_atom_parameter = p_tmp_atom_parameter;
	}

	FREE(p_datafrag);
}

void destory_data_frag_full_all( DataFragFull* p_datafrag )
{
	ParameterWithValue *p_atom_parameter;
	ParameterWithValue *p_tmp_atom_parameter;

	p_atom_parameter = get_real_parameter(p_datafrag->inline_qos.p_head_first);

	while(p_atom_parameter)
	{
		p_tmp_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);

		remove_linked_list((linked_list_head_t *)&p_datafrag->inline_qos, (linked_list_atom_t *)&p_atom_parameter->a_tom);

		FREE(p_atom_parameter->p_value);
		FREE(p_atom_parameter);

		p_atom_parameter = p_tmp_atom_parameter;
	}

	FREE(p_datafrag->p_serialized_data->p_value);
	FREE(p_datafrag->p_serialized_data);
	FREE(p_datafrag);
}


void destory_data_frag_full_except_serialized_data( DataFragFull* p_datafrag )
{
	ParameterWithValue *p_atom_parameter;
	ParameterWithValue *p_tmp_atom_parameter;

	p_atom_parameter = get_real_parameter(p_datafrag->inline_qos.p_head_first);

	while(p_atom_parameter)
	{
		p_tmp_atom_parameter = get_real_parameter(p_atom_parameter->a_tom.p_next);

		remove_linked_list((linked_list_head_t *)&p_datafrag->inline_qos, (linked_list_atom_t *)&p_atom_parameter->a_tom);

		FREE(p_atom_parameter->p_value);
		FREE(p_atom_parameter);

		p_atom_parameter = p_tmp_atom_parameter;
	}

	/*FREE(p_data->p_serialized_data->p_value);
	FREE(p_data->p_serialized_data);*/
	FREE(p_datafrag);
}


void destory_data_frag_full_all_except_inline_qos( DataFragFull* p_datafrag )
{
	/*while(p_datafrag->inline_qos.i_parameters)
	{
		FREE(p_datafrag->inline_qos.pp_parameters[0]->value);
		FREE(p_datafrag->inline_qos.pp_parameters[0]);
		REMOVE_ELEM( p_datafrag->inline_qos.pp_parameters, p_datafrag->inline_qos.i_parameters, 0);
	}*/

	FREE(p_datafrag->p_serialized_data->p_value);
	FREE(p_datafrag->p_serialized_data);
	FREE(p_datafrag);
}