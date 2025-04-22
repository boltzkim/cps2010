

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


#define ROUTERS_ADDR_TEXT "Routers destination address."
#define ROUTERS_ADDR_LONGTEXT "Routers destination address.[ex:192.168.1.2:7400,192.168.1.3:7400]"

void cps_moduleinit(moduleload_t* p_moduleload)
{

	/*static module_config_t config_init = {
		CONFIG_HINT_END,NULL,0,0,NULL,0,NULL,NULL,NULL,
		NULL,NULL,NULL,false
	};
	int i_config = -1;
	module_config_t *p_config = NULL;





	p_config = p_moduleload->p_config;
	i_config = p_moduleload->i_config_items;
	
	add_string("routers", "192.168.0.2:8301", NULL, ROUTERS_ADDR_TEXT, ROUTERS_ADDR_LONGTEXT, false);
		
	p_moduleload->i_config_items = i_config;
	p_moduleload->p_config[++i_config] = config_init;
	
	config_dup(p_moduleload, p_config);*/
}