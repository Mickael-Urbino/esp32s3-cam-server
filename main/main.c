#include "myWifi_test.h"
#include "myServer_test.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_server.h"
#include "lwip/err.h"
#include "lwip/sys.h"



/* Function that will be called during the GET request */
 esp_err_t http_get_handler(httpd_req_t* req)
 {
    const char message[] = "GET : Test Get handler";
    httpd_resp_send(req, message, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
 }

 /* Function that will be called during POST request */
esp_err_t http_post_handler(httpd_req_t* req)
{
    char content[100]; //Buffer used for HTTP POST request content.
    size_t recv_size = sizeof(content); //Used to truncate if content length is larger than the buffer
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT){
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    const char message[] = "Responding to the post request by sending this text";
    httpd_resp_send(req, message, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Structure for GET */
httpd_uri_t uri_get = {
    .uri        = "/server",
    .method     = HTTP_GET,
    .handler    = http_get_handler,
    .user_ctx   = NULL
};

/* Structure for POST */
httpd_uri_t uri_post = {
    .uri        = "/server",
    .method     = HTTP_POST,
    .handler    = http_post_handler,
    .user_ctx   = NULL
};

/* Function that will be used to start webserver*/
httpd_handle_t start_webserver(void)
{
    httpd_config_t server_cfg = HTTPD_DEFAULT_CONFIG();

    httpd_handle_t server = NULL;

    if (httpd_start(&server, &server_cfg) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }
    return server;
}

/* Dummy task printing something every 1s to see if Mutitasking is working */
void vTaskTestFreeRTOS(void *pvParameters)
{
    //const TickType_t delay1000ms = pdMS_TO_TICKS(1000);

    for (;;)
    {
        //printf("1s delay task is printing now...\n");
        //vTaskDelay(delay1000ms);
    }

    vTaskDelete(NULL);

}

/* Create a task to start webserver */
void vTaskStartWebServer(void *pvParameters)
{
    start_webserver();

    for (;;)
    {
        //printf("Server Started..\n");
        //vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL);
}


void app_main(void)
{    

    initialize_wifi();

    WifiComponentTest();
    ServerComponentTest();

    start_webserver();

    //xTaskCreatePinnedToCore(vTaskTestFreeRTOS, "Test Task", 4096, NULL, 0, NULL, 1);
    //xTaskCreatePinnedToCore(vTaskStartWebServer, "Start Webserver Task", 4096, NULL, 0, NULL, 1);

/*
    esp_netif_ip_info_t ipInfo; 
    	
    // IP address.
    esp_netif_get_ip_info(ESP_IF_WIFI_AP, &ipInfo);
    printf("My IP: " IPSTR "\n", IP2STR(&ipInfo.ip));

*/

    //vTaskStartScheduler(); //vTaskStartScheduler() is not needed and creates an issue here, because it is already started before task app_main is called (it is default esp-idf behavior)

    /*printf("Wifi will be disabled now...\n");
    esp_err_t wifi_stop_result = esp_wifi_stop();

    if (wifi_stop_result != ESP_OK)
    {
        printf("Failed to stop Wifi. \n");
        if (wifi_stop_result == ESP_ERR_WIFI_NOT_INIT)
        {
            printf("Wifi is not initialized by esp_wifi_init. \n");
        }   
    }
    else
    {
        printf("Wifi Stopping Successful. \n");
    }


    printf("Starting Wifi Deinitialization...\n");
    esp_err_t wifi_deinit_result = esp_wifi_deinit();
    
    if (wifi_deinit_result != ESP_OK)
    {
        printf("Failed to deinit Wifi. \n");
        if (wifi_deinit_result == ESP_ERR_WIFI_NOT_INIT)
        {
            printf("Wifi is not initialized by esp_wifi_init. \n");
        }   
    }
    else
    {
        printf("Wifi Deinit Successful. \n");
    }*/

    
}