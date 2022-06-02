/*
 * config.h
 *
 * Created: 30/05/2022 1:21:16 PM
 *  Author: sli43
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

/*
Maximum retry for wifi
*/
#define GPIO_OUTPUT_IO_3     3
#define GPIO_OUTPUT_IO_4     4
#define MAXIMUM_RETRY 3
/*
MQTT config
*/

#define RELAY_CONTROLL "mqtt/bedroom/power_relay/control/1"
#define RELAY_STATUS "mqtt/bedroom/power_relay/1"
#endif /* CONFIG_H_ */