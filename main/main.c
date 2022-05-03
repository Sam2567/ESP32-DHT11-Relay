
#include "driver/timer.h"
#include "mqtt_client.h"
#include "dht11.h"
#include "cJSON.h"
#include "mqtt.c"




#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_18) | (1ULL<<GPIO_OUTPUT_IO_19))
#define GPIO_OUTPUT_IO_18    18
#define GPIO_OUTPUT_IO_19    19
#define TIMER_DIVIDER         (16)  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
TaskHandle_t TaskHandle_dht;

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;
esp_mqtt_client_handle_t client_init;

static const char *TAG = "ESP Relay";




void dht11(){
    if(DHT11_read().status != 0){
        ESP_LOGI(TAG, "DHT_ERROR");
        ESP_ERROR_CHECK(esp_mqtt_client_publish(client_init, "mqtt/dinner/power_relay/dht11/1", "N/A", 0, 0, true));
    } else {
        int temperature = DHT11_read().temperature;
        int humidity = DHT11_read().humidity;
        if(temperature > 0 && humidity > 0 && temperature < 100 && humidity < 100) {
            printf("Temperature is %d \n", temperature);
            printf("Humidity is %d\n", humidity);
            cJSON *root;
	        root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root, "temperature", temperature);
            cJSON_AddNumberToObject(root, "humidity", humidity);
            char *send_data = cJSON_Print(root);
            ESP_ERROR_CHECK(esp_mqtt_client_publish(client_init, "mqtt/dinner/power_relay/dht11/1", send_data, 0, 0, true));
            cJSON_Delete(root);
        }
    }
    vTaskDelete(TaskHandle_dht);  
}

void conf_gpio(){
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT ;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}


static bool IRAM_ATTR timer_group_isr_callback(void *args)
{
    BaseType_t high_task_awoken = pdFALSE;
    xTaskCreatePinnedToCore (dht11,"dht_task",3000, NULL,1,TaskHandle_dht,0);
    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}

static void tg_timer_init(int group, int timer, bool auto_reload, int timer_interval_sec)
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config =  {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = auto_reload,
    }; // default clock source is APB
    timer_init(group, timer, &config);
    timer_set_counter_value(group, timer, 0);
    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(group, timer, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(group, timer);
    timer_isr_callback_add(group, timer, timer_group_isr_callback, NULL, 0);
    timer_start(group, timer);
}

void app_main(void)
{
    
    conf_gpio();
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    DHT11_init(GPIO_OUTPUT_IO_19);
    wifi_init_sta();
    mqtt_app_start();
    tg_timer_init(TIMER_GROUP_0, TIMER_0, true, 5);
   
    
}
    
    
