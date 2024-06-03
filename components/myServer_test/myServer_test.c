#include <stdio.h>
#include "myServer_test.h"
#include "esp_err.h"                //For esp_err_t objects
#include "esp_log.h"                //For ESP_LOGx functions (instead of printf)
#include "esp_http_server.h"        //For http structures, types, and functions
#include "sys/param.h"              //For MIN() function
#include "esp_littlefs.h"           //For File System
#include "sys/stat.h"               //For File size calculation

#define INDEX_PAGE_FILENAME             "/myWebserver/index.html"
#define CSS_FILENAME                    "/myWebserver/css/styles.css"
#define ICON_FILENAME                   "/myWebserver/icone.jpg"

static const char* TAG = "myServer";

/* Function that will be called during the GET request */
/* Function to read from file directly get handler */
esp_err_t http_get_handler(httpd_req_t* req)
 {
    FILE* myHTML_file = fopen(INDEX_PAGE_FILENAME, "r");
    ESP_LOGI(TAG,"Opening %s\n", INDEX_PAGE_FILENAME);

    if (myHTML_file == NULL) //Check if file exists
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return(1);
    }

    ESP_LOGI(TAG, "File Successfully opened");

    
    /*Get the file size*/
    struct stat file_stat;
    off_t file_size;

    if (stat(INDEX_PAGE_FILENAME, &file_stat) == 0)
    {
        ESP_LOGI(TAG, "File Size is: %ld Bytes", file_stat.st_size);
        file_size = file_stat.st_size;
    }
    else
    {
        ESP_LOGE(TAG, "Couldn't get file properties. Default to 200 Bytes");
        file_size = 200; //(default size)
    }

    /*Allocate memory to page_content to match size of the file*/
    char *page_content = (char*) malloc(sizeof(char) * (file_size + 1));
    page_content[0] = '\0';
    
    if (page_content == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for Page Buffer");
        fclose(myHTML_file);
        return(1);
    }

    char page_content_buff[1024]="";
    
    while (fgets(page_content_buff, sizeof(page_content), myHTML_file) != NULL) //Read through each line of the file
    {
        strcat(page_content, page_content_buff); //append content of buffer into page_content
        //Remove /n from the end of the string (replace with 0 byte) ? Might not be needed because it seems to work
    }

    fclose(myHTML_file);

    httpd_resp_send(req, page_content, HTTPD_RESP_USE_STRLEN);
    free(page_content);
    return ESP_OK;
 }

/* Function that will be called during the GET request */
/* Function to read from file directly get handler 2 */
esp_err_t http_get_handler2(httpd_req_t* req)
 {
    FILE* myCSS_file = fopen(CSS_FILENAME, "r");
    ESP_LOGI(TAG,"Opening %s\n", CSS_FILENAME);

    if (myCSS_file == NULL) //Check if file exists
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return(1);
    }

    ESP_LOGI(TAG, "File Successfully opened");

    
    /*Get the file size*/
    struct stat file_stat;
    off_t file_size;

    if (stat(CSS_FILENAME, &file_stat) == 0)
    {
        ESP_LOGI(TAG, "File Size is: %ld Bytes", file_stat.st_size);
        file_size = file_stat.st_size;
    }
    else
    {
        ESP_LOGE(TAG, "Couldn't get file properties. Default to 200 Bytes");
        file_size = 200; //(default size)
    }

    /*Allocate memory to page_content to match size of the file*/
    char *page_content = (char*) malloc(sizeof(char) * (file_size + 1));
    page_content[0] = '\0';
    
    if (page_content == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for Page Buffer");
        fclose(myCSS_file);
        return(1);
    }

    char page_content_buff[1024]="";
    
    while (fgets(page_content_buff, sizeof(page_content), myCSS_file) != NULL) //Read through each line of the file
    {
        strcat(page_content, page_content_buff); //append content of buffer into page_content
        //Remove /n from the end of the string (replace with 0 byte) ? Might not be needed because it seems to work
    }

    fclose(myCSS_file);

    httpd_resp_set_type(req, "text/css");

    httpd_resp_send(req, page_content, HTTPD_RESP_USE_STRLEN);
    free(page_content);
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

/* Structure for GET /server*/
httpd_uri_t uri_get_index = {
    .uri        = "/server",
    .method     = HTTP_GET,
    .handler    = http_get_handler,
    .user_ctx   = NULL
};

/* Structure for GET style.css*/
httpd_uri_t uri_get_css = {
    .uri        = "/css/styles.css",
    .method     = HTTP_GET,
    .handler    = http_get_handler2,
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
        httpd_register_uri_handler(server, &uri_get_index);
        httpd_register_uri_handler(server, &uri_get_css);
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
        .base_path = "/myWebserver",
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

void *read_file(char* filename)
{

    FILE* myHTML_file = fopen(filename, "r");
    ESP_LOGI(TAG,"Opening %s\n", filename);

    if (myHTML_file == NULL) //Check if file exists
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return NULL;
    }

    ESP_LOGI(TAG, "File Successfully opened");

    
    /*Get the file size*/
    struct stat file_stat;
    off_t file_size;

    if (stat(filename, &file_stat) == 0)
    {
        ESP_LOGI(TAG, "File Size is: %ld Bytes", file_stat.st_size);
        file_size = file_stat.st_size;
    }
    else
    {
        ESP_LOGE(TAG, "Couldn't get file properties. Default to 200 Bytes");
        file_size = 200; //(default size)
    }

    /*Allocate memory to page_content to match size of the file*/
    char *page_content = (char*) malloc(sizeof(char) * (file_size + 1));
    page_content[0] = '\0';
    
    if (page_content == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for Page Buffer");
        fclose(myHTML_file);
        return NULL;
    }

    char page_content_buff[1024]="";
    
    while (fgets(page_content_buff, sizeof(page_content), myHTML_file) != NULL) //Read through each line of the file
    {
        strcat(page_content, page_content_buff); //append content of buffer into page_content
        //Remove /n from the end of the string (replace with 0 byte)
    }

    fclose(myHTML_file);
    printf("%s", page_content);
    return page_content;
}

/* Mock Function that prints show if component was properly linked to the main folder */
void ServerComponentTest(void)
{
    init_littlefs();

    printf("myServer Component was successfully linked to main.c\n");
    char *page_content = (char*) read_file(INDEX_PAGE_FILENAME);
    if (page_content == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocated memory for Page_Content");
        return;
    }

    
    //free(page_content);

}
