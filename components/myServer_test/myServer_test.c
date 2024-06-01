#include <stdio.h>
#include "myServer_test.h"
#include "esp_err.h"                //For esp_err_t objects
#include "esp_log.h"                //For ESP_LOGx functions (instead of printf)
#include "esp_http_server.h"        //For http structures, types, and functions
#include "sys/param.h"              //For MIN() function
#include "esp_littlefs.h"           //For File System
#include "sys/stat.h"               //For File size calculation

#define INDEX_PAGE_FILENAME             "/Front_End/index.html"

static const char* TAG = "myServer";

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

/*Handler for errors that come with littlefs refister function*/
void error_check_littlefs_init(esp_err_t init_littlefs)
{
    switch (init_littlefs){
        case ESP_OK:
        ESP_LOGI(TAG, "Filesystem Successfully Mounted");
        break;

        case ESP_ERR_NO_MEM:
        ESP_LOGE(TAG, "No memory for Filesystem");
        break;

        case ESP_ERR_INVALID_STATE:
        ESP_LOGE(TAG, "Filesystem Image already mounted or partitioin is encrypted");
        break;

        case ESP_ERR_NOT_FOUND:
        ESP_LOGE(TAG, "Partition for LittleFS was not found");
        break;

        case ESP_FAIL:
        ESP_LOGE(TAG, "Failed to mount/format Filesystem image");
        break;
    }
}

/* Initialize lifflefs */
void init_littlefs(void)
{
    ESP_LOGI(TAG, "Initializing LittleFs");

    esp_vfs_littlefs_conf_t littlefs_config = {
        .base_path = "/Front_End",
        .partition_label = "myWebserver",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };

    error_check_littlefs_init(esp_vfs_littlefs_register(&littlefs_config));

    size_t total_bytes = 0;
    size_t used_bytes = 0;

    esp_err_t get_littlefs_info_result = esp_littlefs_info(littlefs_config.partition_label, &total_bytes, &used_bytes);

    if (get_littlefs_info_result != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get LittleFS partition information");
    }
    else
    {
        ESP_LOGI(TAG, "LittleFS Partition size: total: %d,  used: %d", total_bytes, used_bytes);
    }

}

/* Function to read the HTML file and store its content inside a buffer */

void read_file(char* filename)
{

    /* Get the file size*/
    struct stat file_stat;
    off_t file_size;

    if (stat(filename, &file_stat) == 0)
    {
        ESP_LOGI(TAG, "File Size is: %ld Bytes", file_stat.st_size);
        file_size = file_stat.st_size;
    }
    else
    {
        ESP_LOGE(TAG, "Couldn't get file properties. Default to 1024 Bytes");
        file_size = 1024; //(default size)
    }

    /* Allocate memory to page_content to match size of the file*/
    char *page_content = (char*) malloc(sizeof(char) * (file_size + 1));
    if (page_content == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for Page Buffer");
        exit(1);
    }

    FILE* myHTML_file = fopen(filename, "r");

    if (myHTML_file == NULL) //Check if file exists
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        exit(1);
    }
    /*
    while (fgets(page_content, sizeof(page_content), myHTML_file) != NULL) //Read through each line of the file
    {

    }*/

    free(page_content);

    fclose(myHTML_file);
}

/* Mock Function that prints show if component was properly linked to the main folder */
void ServerComponentTest(void)
{
    printf("myServer Component was successfully linked to main.c\n");
}
