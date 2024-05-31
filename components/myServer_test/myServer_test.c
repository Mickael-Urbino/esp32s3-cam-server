#include <stdio.h>
#include "myServer_test.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "sys/param.h"



/* Function that will be called during the GET request */
 esp_err_t http_get_handler(httpd_req_t* req)
 {

    const char header[] = "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>esp32s3-cam-server</title></head><body style=background-color:#DBF9FC><style> h1 { color: #999; font-family: sans-serif} </style> <h1>Welcome to esp32s3-cam-server</h1>";
    const char body[] = "<form action=\"http://192.168.4.1/server\" method=\"post\"><label for=\"s1\">ESP32S3-CAM-SERVER :</label><input type =\"text\" id=\"s1\" name=\"s1\" value=\"Write something here\"/><br/><button style=background-color:SpringGreen>Send</button></form></body></html>";
    char message[512] = "";
    strcat(message, header);
    strcat(message, body);
    httpd_resp_send(req, message, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
 }

 /* Function that will be called during POST request */
esp_err_t http_post_handler(httpd_req_t* req)
{
    char content[100]; //Buffer used for HTTP POST request content.
    size_t recv_size = MIN(req->content_len, sizeof(content)); //Used to truncate if content length is larger than the buffer
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

/* Mock Function that prints show if component was properly linked to the main folder */
void ServerComponentTest(void)
{
    printf("myServer Component was successfully linked to main.c\n");
}
