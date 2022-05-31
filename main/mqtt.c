#include "mqtt.h"
#include "http.c"
/*Start a web server for mqtt server address update in case of mqtt server address change*/
void mqtt_backup_server(void * pvParameters){
    start_webserver();
    vTaskDelete(taskhandle_get_mqtt_server);  
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        if (retry < MAXIMUM_RETRY) {
            esp_wifi_connect();
            retry++;
            xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
            ESP_LOGI(TAG, "Retry to connect to the WIFI");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            ESP_LOGI(TAG,"SC_task start");
        }
        ESP_LOGI(TAG,"Connect to the WIFI fail");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (retry < MAXIMUM_RETRY) {
            esp_wifi_connect();
            retry++;
            xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
            ESP_LOGI(TAG, "Retry to connect to the WIFI");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            ESP_LOGI(TAG,"Connect to the WIFI fail");
           
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    } else {

    }
}

static void initialise_wifi(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}




static void log_error_if_nonzero(const char * message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

            msg_id = esp_mqtt_client_subscribe(client, MQTT_CONTROL, 0);
            
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            xTaskCreatePinnedToCore (mqtt_backup_server,"server_task",3000, NULL,6,taskhandle_get_mqtt_server,1);
            msg_id = esp_mqtt_client_subscribe(client, MQTT_CONTROL, 0);
            
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            xTaskCreatePinnedToCore (mqtt_backup_server,"server_task",3000, NULL,6,taskhandle_get_mqtt_server,1);
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            int switc = gpio_get_level(GPIO_OUTPUT_IO_4);
            if( switc == 1){
                gpio_set_level(GPIO_OUTPUT_IO_4, 0);
                msg_id = esp_mqtt_client_publish(client, MQTT_STATUS, "OFF", 0, 0, false);
            } else {
                msg_id = esp_mqtt_client_publish(client, MQTT_STATUS, "ON", 0, 0, false);
                gpio_set_level(GPIO_OUTPUT_IO_4, 1);
            }
            break;
        case MQTT_EVENT_ERROR:
            xTaskCreatePinnedToCore (mqtt_backup_server,"server_task",3000, NULL,6,taskhandle_get_mqtt_server,1);
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}



static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://192.168.3.56",
        .username = "mqtt",
        .password = "mqtt",
        .event_handle = mqtt_event_handler,
    };
    client_init = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client_init, ESP_EVENT_ANY_ID, mqtt_event_handler, client_init);
    esp_mqtt_client_start(client_init);
}
