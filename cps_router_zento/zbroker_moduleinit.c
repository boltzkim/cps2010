

#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>


#define BrokerS_ADDR_TEXT "Brokers destination address."
#define BrokerS_ADDR_LONGTEXT "Brokers destination address.[ex:192.168.1.2:7400,192.168.1.3:7400]"

void zento_moduleinit(moduleload_t* p_moduleload)
{

	/*static module_config_t config_init = {
		CONFIG_HINT_END,NULL,0,0,NULL,0,NULL,NULL,NULL,
		NULL,NULL,NULL,false
	};
	int i_config = -1;
	module_config_t *p_config = NULL;





	p_config = p_moduleload->p_config;
	i_config = p_moduleload->i_config_items;
	
	add_string("Brokers", "192.168.0.2:8301", NULL, BrokerS_ADDR_TEXT, BrokerS_ADDR_LONGTEXT, false);
		
	p_moduleload->i_config_items = i_config;
	p_moduleload->p_config[++i_config] = config_init;
	
	config_dup(p_moduleload, p_config);*/
}