#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define GPIO_OUTPUT_IO_3     3
#define GPIO_OUTPUT_IO_4     4
#define MAXIMUM_RETRY 10
#define WIFI_FAIL_BIT      BIT1
#define CONNECTED_BIT      BIT0
#define ESPTOUCH_DONE_BIT  BIT1
#define MQTT_CONTROL "mqtt/bedroom/power_relay/control/1"
#define MQTT_STATUS "mqtt/bedroom/power_relay/1"


int retry = 0;

esp_mqtt_client_handle_t client_init;
TaskHandle_t taskhandle_get_mqtt_server;
static const char *TAG = "ESP Relay";
/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t s_wifi_event_group;


static void smartconfig_task(void * parm);

