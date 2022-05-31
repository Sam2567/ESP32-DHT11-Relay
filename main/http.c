#include "http.h"


inline size_t MIN(size_t a, size_t b) { return a > b ? b : a; }
inline size_t MAX(size_t a, size_t b) { return a > b ? a : b; }
/* Our URI handler function to be called during GET /uri request */
esp_err_t get_handler(httpd_req_t *req)
{
    /* Send a simple response */
    char content[150];
    strcpy(content,req->uri);
    const char ch[2] = "=";
    // Returns first token
    char* token = strtok(content, ch);
    token = strtok(NULL, ch);
    //null the first token/the url
    while (token != NULL) {
        //copy the mqtt address
        strcpy(mqtt_address, token);
        httpd_resp_send(req, mqtt_address, HTTPD_RESP_USE_STRLEN);
        token = strtok(NULL, ch);
       
    }
    free(token);
    return ESP_OK;
}

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri      = "/mqtt/get",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}
/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}