/*
 * config.h
 *
 * Created: 30/05/2022 1:21:16 PM
 *  Author: sli43
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "mqtt_client.h"
#include "dht11.h"
#include "cJSON.h"
#include "esp_smartconfig.h"

static int retry = 0;
static const char *TAG_MQTT = "ESP Relay";
/*
Maximum retry for wifi
*/
#define GPIO_OUTPUT_IO_3     3
#define GPIO_OUTPUT_IO_4     4
#define MAXIMUM_RETRY 3
/*
MQTT config
*/

#define RELAY_CONTROLL "mqtt/bedroom/power_relay/control/2"
#define RELAY_STATUS "mqtt/bedroom/power_relay/2"
#endif /* CONFIG_H_ */