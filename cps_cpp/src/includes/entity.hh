#if !defined(__ENTITY_HH__)
#define __ENTITY_HH__

namespace cps
{

class Entity2
{
public:
	Entity2(Entity *entity)
	{
		p_entity = entity;
	}

	Entity *getEntity()
	{
		return p_entity;
	}

	////////////////

	ReturnCode_t enable();
	StatusCondition2 get_statuscondition();
	StatusMask get_status_changes();
	InstanceHandle_t get_instance_handle();

	////////////////
private:
	Entity *p_entity;
};


}

#endif




	