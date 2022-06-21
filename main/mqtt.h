#ifndef MQTT_H_
#define MQTT_H_

#include "config.h"

#define WIFI_FAIL_BIT      BIT1
#define CONNECTED_BIT      BIT0
#define ESPTOUCH_DONE_BIT  BIT1

esp_mqtt_client_handle_t client_init;
EventGroupHandle_t s_wifi_event_group;
void smartconfig_task(void * parm);
void initialise_wifi(void);
void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
void log_error_if_nonzero(const char * message, int error_code);
esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void mqtt_app_start(void);

#endif