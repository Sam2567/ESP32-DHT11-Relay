#ifndef MQTT_H_
#define MQTT_H_

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

#define WIFI_FAIL_BIT      BIT1
#define CONNECTED_BIT      BIT0
#define ESPTOUCH_DONE_BIT  BIT1

int retry = 0;


esp_mqtt_client_handle_t client_init;
static EventGroupHandle_t s_wifi_event_group;
/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */

static const char *TAG = "ESP Relay";
/* FreeRTOS event group to signal when we are connected & ready to make a request */


static void smartconfig_task(void * parm);
static void initialise_wifi(void);
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
static void log_error_if_nonzero(const char * message, int error_code);
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void mqtt_app_start(void);

#endif