/* Don't forget to save all files when changing components names or it won't flash */

#include <stdio.h>
#include "myWifi_test.h"
#include "nvs_flash.h" //nvs_flash initialization is required to initialize Wifi properly. It should be because ESP-IDF has made it mandatory to store wifi config, and doing so in nvs is their DEFAULT BEHAVIOR for wifi applications, to ease development.
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"

static const char *TAG1 = "myWifi";

/* Error Handling Function for NVS Flash : No Free Page Error. 
Erase NVS flash and Retries Initialisation until success (No time out, this might be needed in the future to avoid infinite loop issue in certain cases)*/
void nvs_nofreepage_handler(esp_err_t err)
{
    while (err == ESP_ERR_NVS_NO_FREE_PAGES)
    {
            ESP_LOGW(TAG1,"There is no freepage. Erasing NVS Flash before retrying...");
            nvs_flash_erase();

            ESP_LOGW(TAG1,"Retrying NVS Flash initialisation...");
            err = nvs_flash_init();

            if (err == ESP_OK)
            {
                ESP_LOGI(TAG1,"Success!");
            }
    }  
}

/* initialize_nvs_flash() function that implements some error recovering code */
void initialize_nvs_flash(void)
{
    ESP_LOGI(TAG1,"Initializing NVS Flash for Wifi configuration storage...");
    esp_err_t nvs_flash_init_result = nvs_flash_init(); //Try to Initialise Non Volatile Storage Flash and check if it has been successfully initialized

    switch(nvs_flash_init_result){
        case ESP_OK:
        ESP_LOGI(TAG1,"Success!");
        break;

        case ESP_ERR_NVS_NO_FREE_PAGES:
        nvs_nofreepage_handler(nvs_flash_init_result);
        break;

        case ESP_ERR_NOT_FOUND:
        ESP_LOGI(TAG1,"There is no NVS Partition label... No work around yet");
        break;

        case ESP_ERR_NO_MEM:
        ESP_LOGI(TAG1,"Memory could not be allocated for internal structures. No work around yet");
        break;
        
    }
}

/* Netif initialisation function with error handling */
void initialize_netif(void)
{
    ESP_LOGI(TAG1,"Initializing Net Interface for TCP/IP stack...");
    esp_err_t netif_init_result = esp_netif_init();

    switch (netif_init_result){
        case ESP_OK :
        ESP_LOGI(TAG1, "Success!");
        break;

        case ESP_FAIL:
        ESP_LOGW(TAG1, "Failed to initialize Net Interface. Retrying...");
        while (netif_init_result != ESP_OK)
        {
            netif_init_result = esp_netif_init();

            if (netif_init_result == ESP_OK)
            {
                ESP_LOGI(TAG1, "Success!");
            }
        }
        break;
    }
}

/*Create default event_loop and check result (no error handling done)*/
void create_default_event_loop(void)
{
    ESP_LOGI(TAG1,"Creating Default Event Loop...");
    esp_err_t create_default_event_loop_result = esp_event_loop_create_default();

    switch (create_default_event_loop_result){
        case ESP_OK:
        ESP_LOGI(TAG1,"Success!");
        break;

        case ESP_ERR_NO_MEM:
        ESP_LOGE(TAG1,"Cannot allocate memory for event loop list");
        break;

        case ESP_ERR_INVALID_STATE:
        ESP_LOGW(TAG1,"Default event loop has already been created");
        break;

        case ESP_FAIL:
        ESP_LOGE(TAG1,"Failed to create task loop");
        break;
    }
}

/*Event handler for wifi AP related events*/
void myWifi_event_handler(void* handler_arg, esp_event_base_t base, int32_t event_id, void* event_data)
{
    switch (event_id){
        case WIFI_EVENT_AP_START:
        ESP_LOGI(TAG1, "Wifi Event Confirming that AP Mode has started");
        break;

        case WIFI_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG1, "Station Connected");
        break;

        case WIFI_EVENT_AP_STADISCONNECTED:
        ESP_LOGW(TAG1, "Station Disconnected");
        break;

        case IP_EVENT_STA_GOT_IP:
        ESP_LOGW(TAG1, "Station Got IP");
        break;

        case IP_EVENT_GOT_IP6:
        ESP_LOGW(TAG1, "Got IP6");
        break;

        case IP_EVENT_STA_LOST_IP:
        ESP_LOGW(TAG1, "Station Lost IP");
        break;

        default:
        ESP_LOGW(TAG1, "Other Wifi Event");

    }

}

/* Handle error and success when setting wifi config*/
void error_check_wifi_config(esp_err_t set_config)
{
if (set_config != ESP_OK)
    {
        ESP_LOGE(TAG1,"Failed to set wifi configuration");
        if (set_config == ESP_ERR_WIFI_NOT_INIT)
        {
            ESP_LOGE(TAG1,"WiFi is not initialized by esp_wifi_init");
        }
        else if (set_config == ESP_ERR_INVALID_ARG)
        {
            ESP_LOGE(TAG1,"Invalid argument in function. esp_wifi_set_config(wifi_interface_t interface, wifi_config_t *conf)");
        }
        else if (set_config == ESP_ERR_WIFI_IF)
        {
            ESP_LOGE(TAG1,"Invalid Interface. Select one of these: WIFI_IF_STA / WIFI_IF_AP / WIFI_IF_NAN / WIFI_IF_MAX");
        }
        else if (set_config == ESP_ERR_WIFI_MODE)
        {
            ESP_LOGE(TAG1,"Invalid Mode. Select one of these: WIFI_MODE_NULL /WIFI_MODE_AP / WIFI_MODE_STA / WIFI_MODE_APSTA / WIFI_MODE_NAN / WIFI_MODE_MAX");
        }
        else if (set_config == ESP_ERR_WIFI_PASSWORD)
        {
            ESP_LOGE(TAG1,"Invalid Password");
        }
        else if (set_config == ESP_ERR_WIFI_NVS)
        {
            ESP_LOGE(TAG1,"WiFi Internal NVS error");
        }
        else
        {
            ESP_LOGE(TAG1,"refer to the error code in esp_err.h");
        }
    }
    else
    {
        ESP_LOGI(TAG1,"Wifi configuration properly set");
    }
}

/*Load default wifi configuration into wifi driver*/
void load_default_wifi_config(void)
{
    wifi_init_config_t my_wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();    //Set Wifi default configuration into my_wifi_cfg structure

    ESP_LOGI(TAG1, "Loading default Wifi Configuration Data...");
    esp_err_t wifi_init_result = esp_wifi_init(&my_wifi_cfg);

    switch (wifi_init_result){

        case ESP_OK:
        ESP_LOGI(TAG1, "Wifi default configuration load is a Success!");
        break;

        case ESP_ERR_NO_MEM:
        ESP_LOGE(TAG1, "Out of Memory. Please Check Memory Allocation");
        break;
        
        default:
        ESP_LOGE(TAG1, "esp_wifi_init() result is different from ESP_OK or ESP_ERR_NO_MEM");

    }
}

/*Set wifi interface*/
void set_wifi_interface(int wifi_mode, wifi_interface_t wifi_if)// Replace wifi_mode by MY_WIFI_MODE
{
    //esp_err_t wifi_set_config_result;
    //wifi_interface_t my_wifi_interface;

    switch (wifi_mode){
        case WIFI_MODE_NULL:
        printf("Setting Wifi Mode: NULL Mode\n");
        break;

        case WIFI_MODE_AP: //Evaluate if WIFI_MODE is set to AP and prepare the configuration to match AP mode
        printf("Preparing for Wifi Mode: AP Mode\n");
        //my_wifi_interface = WIFI_IF_AP;
        wifi_config_t my_config = {
            .ap = {
                .ssid = MY_WIFI_SSID,
                .password = MY_WIFI_PSWD,
                .channel = 0,
                .authmode = WIFI_AUTH_WPA_PSK,
                .ssid_hidden = 0, //SSID will be broadcasted, not hidden
                .max_connection = 4,
            }        
        };
        printf("Setting Wifi Configuration.\n");
        error_check_wifi_config(esp_wifi_set_config(wifi_if, &my_config)); //Set AP Mode configuration 
        break;

        case WIFI_MODE_STA:
        printf("Setting Wifi Mode: STA Mode\n");
        //my_wifi_interface = WIFI_IF_STA;
        break;

        case WIFI_MODE_APSTA:
        printf("Setting Wifi Mode: APSTA Mode\n");
        break;

        case WIFI_MODE_NAN:
        printf("Setting Wifi Mode: NAN Mode\n");
        break;

        case WIFI_MODE_MAX:
        printf("Setting Wifi Mode: MAX Mode\n");
        //my_wifi_interface = WIFI_IF_MAX;
        break;
    }
}

/* Handler for errors that come with esp_wifi_set_mode */
void error_check_wifi_mode(esp_err_t set_mode)
{
    switch (set_mode){

        case ESP_OK:
        ESP_LOGI(TAG1, "Wifi Mode Successfully set");
        break;

        case ESP_ERR_WIFI_NOT_INIT:
        ESP_LOGE(TAG1,"Wifi is not initialized by esp_wifi_init");
        break;

        case ESP_ERR_INVALID_ARG:
        ESP_LOGE(TAG1,"Invalid argument in function. esp_wifi_set_mode(wifi_mode_t mode)");
        break;

        default:
        ESP_LOGE(TAG1," Failed to set Wifi Mode, refer to error code in esp_err.h");

    }
}

void error_check_wifi_start(esp_err_t start_wifi)
{
    switch (start_wifi){

        case ESP_OK:
        ESP_LOGI(TAG1,"Wifi Control Block created, and wifi started");
        break;

        case ESP_ERR_WIFI_NOT_INIT:
        ESP_LOGE(TAG1,"Wifi is not initialized by esp_wifi_init");
        break;

        case ESP_ERR_INVALID_ARG:
        ESP_LOGE(TAG1,"Invalid argument in function. esp_wifi_set_mode(wifi_mode_t mode)");
        break;
        
        case ESP_ERR_NO_MEM:
        ESP_LOGE(TAG1,"Out of Memory");
        break;
        
        case ESP_ERR_WIFI_CONN:
        ESP_LOGE(TAG1,"WiFi internal error, station or soft-AP control block wrong");
        break;

        case ESP_FAIL:
        ESP_LOGE(TAG1,"Other Wifi internal errors");
        break;

    }
}

/* initialize all Wifi requirements */
void initialize_wifi(void)
{

    initialize_nvs_flash();
    initialize_netif();
    create_default_event_loop();
    
    esp_netif_create_default_wifi_ap();

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &myWifi_event_handler, NULL, NULL);

    load_default_wifi_config();
    set_wifi_interface(MY_WIFI_MODE, WIFI_IF_AP);
    error_check_wifi_mode(esp_wifi_set_mode(MY_WIFI_MODE)); //Setting wifi mode into selected "wifi_mode"
    
    error_check_wifi_start(esp_wifi_start()); //Creates selected "MY_WIFI_MODE" control blocks and Starts wifi in selected "MY_WIFI_MODE" and 

}

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

/* Mock Function that prints show if component was properly linked to the main folder */
void WifiComponentTest(void)
{
    printf("myWifi Component was successfully linked to main.c\n");

}
