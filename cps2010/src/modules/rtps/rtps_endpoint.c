/*
	RTSP Endpoint class
	작성자 : 
	이력
	2010-08-10 : 처음 시작
*/

/*
Specialization of RTPS Entity representing the objects that can be communication
endpoints. That is, the objects that can be the sources or destinations of RTPS messages.
*/

#include "rtps.h"


void init_rtps_endpoint( rtps_endpoint_t* p_endpoint )
{
	init_rtps_entity((rtps_entity_t *)p_endpoint);
	p_endpoint->pp_multicast_locator_list = NULL;
	p_endpoint->reliability_level = BEST_EFFORT;
	p_endpoint->topic_kind = NO_KEY;
	p_endpoint->pp_unicast_locator_list = NULL;

	p_endpoint->i_unicast_locator = 0;
	p_endpoint->i_multicast_locator = 0;
	p_endpoint->p_rtps_participant = NULL;
}

void destroy_endpoint( rtps_endpoint_t* p_endpoint )
{

	while (p_endpoint->i_unicast_locator)
	{
		FREE(p_endpoint->pp_unicast_locator_list[0]);
		REMOVE_ELEM(p_endpoint->pp_unicast_locator_list, p_endpoint->i_unicast_locator, 0);
	}

	FREE(p_endpoint->pp_unicast_locator_list);

	while (p_endpoint->i_multicast_locator)
	{
		FREE(p_endpoint->pp_multicast_locator_list[0]);
		REMOVE_ELEM( p_endpoint->pp_multicast_locator_list, p_endpoint->i_multicast_locator, 0);
	}

	FREE(p_endpoint->pp_multicast_locator_list);


	//FREE(p_endPoint);
}



