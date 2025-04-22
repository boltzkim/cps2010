/*
	RTSP UTIL class
	작성자 : 
	이력
	2010-10-07 : 처음 시작
*/

#include "rtps.h"



void set_DefaultMulticastLocator(Locator_t** pp_locator) {}

ParameterWithValue* rtps_make_parameter(ParameterId_t pid, int i_size, void* p_value)
{
	ParameterWithValue* p_parameter = malloc(sizeof(ParameterWithValue));

	memset(p_parameter, 0, sizeof(ParameterWithValue));

	p_parameter->parameter_id = pid;

	if (i_size > 0)
	{
		p_parameter->length = i_size;
		p_parameter->p_value = malloc(p_parameter->length);
		memset(p_parameter->p_value, 0, p_parameter->length);
		memcpy(p_parameter->p_value, p_value, p_parameter->length);
	}
	else
	{
		p_parameter->length = i_size;
		p_parameter->p_value = NULL;
	}
	p_parameter->a_tom.p_extra = NULL;
	p_parameter->a_tom.p_next = NULL;
	p_parameter->a_tom.p_prev = NULL;
	return p_parameter;
}


void set_unicast_and_multicast_spdp( module_object_t* p_this, rtps_endpoint_t* p_endpoint )
{
	Locator_t* p_unicatlocator = NULL, *p_multicatlocator = NULL;

	p_unicatlocator = malloc(sizeof(Locator_t));
	p_multicatlocator = malloc(sizeof(Locator_t));

	memset(p_unicatlocator, 0, sizeof(Locator_t));
	memset(p_multicatlocator, 0, sizeof(Locator_t));

	rtps_get_spdp_multicast_locator((service_t*)p_this, p_multicatlocator);
	rtps_get_spdp_unicast_locator((service_t*)p_this, p_unicatlocator);

	INSERT_ELEM(p_endpoint->pp_unicast_locator_list, p_endpoint->i_unicast_locator, p_endpoint->i_unicast_locator,
				p_unicatlocator);

	INSERT_ELEM(p_endpoint->pp_multicast_locator_list, p_endpoint->i_multicast_locator,
				p_endpoint->i_multicast_locator, p_multicatlocator);
}


void set_unicast_and_multicast_sedp( module_object_t* p_this, rtps_endpoint_t* p_endpoint )
{
	Locator_t* p_unicatlocator = NULL, *p_multicatlocator = NULL;

	p_unicatlocator = malloc(sizeof(Locator_t));
	p_multicatlocator = malloc(sizeof(Locator_t));

	memset(p_unicatlocator, 0, sizeof(Locator_t));
	memset(p_multicatlocator, 0, sizeof(Locator_t));

	rtps_get_sedp_multicast_locator((service_t*)p_this, p_multicatlocator);
	rtps_get_sedp_unicast_locator((service_t*)p_this, p_unicatlocator);

	INSERT_ELEM(p_endpoint->pp_unicast_locator_list, p_endpoint->i_unicast_locator, p_endpoint->i_unicast_locator,
				p_unicatlocator);

	INSERT_ELEM(p_endpoint->pp_multicast_locator_list, p_endpoint->i_multicast_locator,
				p_endpoint->i_multicast_locator, p_multicatlocator);
}


void set_unicast_and_multicast_default( module_object_t* p_this, rtps_endpoint_t* p_endpoint )
{
	Locator_t* p_unicatlocator = NULL, *p_multicatlocator = NULL;

	p_unicatlocator = malloc(sizeof(Locator_t));
	p_multicatlocator = malloc(sizeof(Locator_t));

	memset(p_unicatlocator, 0, sizeof(Locator_t));
	memset(p_multicatlocator, 0, sizeof(Locator_t));

	rtps_get_default_multicast_locator((service_t*)p_this, p_multicatlocator);
	rtps_get_default_unicast_locator((service_t*)p_this, p_unicatlocator);

	INSERT_ELEM(p_endpoint->pp_unicast_locator_list, p_endpoint->i_unicast_locator, p_endpoint->i_unicast_locator,
				p_unicatlocator);

	INSERT_ELEM(p_endpoint->pp_multicast_locator_list, p_endpoint->i_multicast_locator,
				p_endpoint->i_multicast_locator, p_multicatlocator);
}

/************************************************************************/
/* SequenceNumber를 하나 증가된 값을 리턴한다.                          */
/************************************************************************/
SequenceNumber_t sequnce_number_inc( SequenceNumber_t a_sequnce_number )
{

	if (a_sequnce_number.low == 0xffffffff)
	{
		++a_sequnce_number.high;
		a_sequnce_number.low = 0;
	}
	else
	{
		++a_sequnce_number.low;
	}

	return a_sequnce_number;
}

/************************************************************************/
/* SequenceNumber를 하나 감소된 값을 리턴한다                           */
/************************************************************************/
SequenceNumber_t sequnce_number_dec( SequenceNumber_t a_sequnce_number )
{

	if (a_sequnce_number.low == 0)
	{
		if (a_sequnce_number.high == 0)
		{
			a_sequnce_number.low = 0;
		}
		else
		{
			--a_sequnce_number.high;
			//			a_SequnceNumber.low = 0x7fffffff;
			a_sequnce_number.low = 0xffffffff;
		}
	}
	else
	{
		--a_sequnce_number.low;
	}

	return a_sequnce_number;
}

int32_t sequnce_number_difference( SequenceNumber_t a_last_sequnce_number, SequenceNumber_t a_first_sequnce_number )
{
	int32_t i_difference = 0;

	if (a_first_sequnce_number.high - a_first_sequnce_number.high == 0)
	{
		i_difference = (a_last_sequnce_number.low - a_first_sequnce_number.low);
	}
	else
	{
		i_difference = ((uint64_t)(a_first_sequnce_number.high - a_first_sequnce_number.high)) *
			0xffffffff + (a_last_sequnce_number.low - a_first_sequnce_number.low);
	}

	if (i_difference > 255)
		i_difference = 255;

	return i_difference;
}

int32_t sequnce_number_difference2( SequenceNumber_t a_last_sequnce_number, SequenceNumber_t a_first_sequnce_number )
{
	int32_t i_difference = 0;

	if (a_first_sequnce_number.high - a_first_sequnce_number.high == 0)
	{
		i_difference = (a_last_sequnce_number.low - a_first_sequnce_number.low);
	}
	else
	{
		i_difference = ((uint64_t)(a_first_sequnce_number.high - a_first_sequnce_number.high)) *
			0xffffffff + (a_last_sequnce_number.low - a_first_sequnce_number.low);
	}

	//if (i_difference > 254)
	//	i_difference = 254;

	return i_difference;
}

bool sequnce_number_left_bigger_and_equal( SequenceNumber_t a_left_sequnce_number, SequenceNumber_t a_right_sequnce_number )
{
	if (a_left_sequnce_number.high > a_right_sequnce_number.high)
	{
		return true;
	}
	else if (a_left_sequnce_number.high == a_right_sequnce_number.high)
	{
		if (a_left_sequnce_number.low >= a_right_sequnce_number.low)
		{
			return true;
		}
	}

	return false;
}

bool sequnce_number_left_bigger( SequenceNumber_t a_left_sequnce_number, SequenceNumber_t a_right_sequnce_number )
{
	if (a_left_sequnce_number.high > a_right_sequnce_number.high)
	{
		return true;
	}
	else if (a_left_sequnce_number.high == a_right_sequnce_number.high)
	{
		if (a_left_sequnce_number.low > a_right_sequnce_number.low)
		{
			return true;
		}
	}

	return false;
}


bool sequnce_number_equal( SequenceNumber_t a_left_sequnce_number, SequenceNumber_t a_right_sequnce_number )
{

	if ((a_left_sequnce_number.high == a_right_sequnce_number.high) &&
		(a_left_sequnce_number.low == a_right_sequnce_number.low))
	{
		return true;
	}

	return false;
}


bool is_same_guid(const GUID_t* const p_guid1, const GUID_t* const p_guid2)
{
	return (memcmp(p_guid1, p_guid2, sizeof(GUID_t)) == 0);
}
