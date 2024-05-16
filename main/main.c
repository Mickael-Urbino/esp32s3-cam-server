#include <stdio.h>
#include "esp_wifi.h"
#include "nvs_flash.h" //nvs_flash initialization is required to initialize Wifi properly. It should be because ESP-IDF has made it mandatory to store wifi config, and doing so in nvs is their DEFAULT BEHAVIOR for wifi applications, to ease development.
#include "esp_spiffs.h"
#include "myserver_test.h"

#define MY_WIFI_SSID "ESP32S3-CAM-SERVER"   //Name of the wifi ACCESS POINT (Used in ESP-IDF's wifi_config_ap "structure" ??)
#define MY_WIFI_PSWD "DroneVTX"             //Password to connect to the ACCESS POINT (Used in ESP-IDF's wifi_config_ap "structure" ??)

#define MY_WIFI_MODE    WIFI_MODE_AP        //Select Default Wifi Mode. WIFI_MODE_NULL / WIFI_MODE_STA / WIFI_MODE_AP / WIFI_MODE_APSTA / WIFI_MODE_NAN / WIFI_MODE_MAX


void app_main(void)
{
    printf("Build Successful ! \n\n");

    printf("Initializing NVS Flash to allow Wifi configuration storage...\n");
    esp_err_t nvs_flash_init_result = nvs_flash_init(); //Try to Initialise Non Volatile Storage Flash and check if it has been successfully initialized

    if (nvs_flash_init_result != ESP_OK)
    {
        printf("Issue when initializing NVS Flash.\n");
        if (nvs_flash_init_result == ESP_ERR_NVS_NO_FREE_PAGES)
        {
            printf("There is no freepage. Erasing NVS Flash...\n");
            nvs_flash_erase();
        }
        if (nvs_flash_init_result == ESP_ERR_NOT_FOUND)
        {
            printf("There is no NVS Partition label...\n");
        }
         if (nvs_flash_init_result == ESP_ERR_NO_MEM)
        {
            printf("Memory could not be allocated for internal structures.\n");
        }
    }
    else
    {
        printf("Successful NVS Flash initialization. \n");
    }


    printf("Initializing Wifi...\n");
    wifi_init_config_t my_wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();    //Set Wifi default configuration into my_wifi_cfg structure
    esp_err_t wifi_init_result = esp_wifi_init(&my_wifi_cfg);                                    //

    if (wifi_init_result != ESP_OK)
    {
        printf("Issue with implementing Wifi Configuration. \n");
        if (wifi_init_result == ESP_ERR_NO_MEM)
        {
            printf("Out of Memory. Please Check Memory Allocation. \n");
        }
    }
    else 
    {
        printf("Wifi Configuration Successful \n");
    }

    esp_err_t wifi_set_config_result;
    wifi_interface_t my_wifi_interface;

    switch (MY_WIFI_MODE){
        case WIFI_MODE_NULL:
        printf("Setting Wifi Mode: NULL Mode\n");
        break;

        case WIFI_MODE_AP: //Evaluate if WIFI_MODE is set to AP and prepare the configuration to match AP mode
        printf("Preparing for Wifi Mode: AP Mode\n");
        my_wifi_interface = WIFI_IF_AP;
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
        wifi_set_config_result = esp_wifi_set_config(my_wifi_interface, &my_config); //Set AP Mode configuration 
        break;

        case WIFI_MODE_STA:
        printf("Setting Wifi Mode: STA Mode\n");
        my_wifi_interface = WIFI_IF_STA;
        break;

        case WIFI_MODE_APSTA:
        printf("Setting Wifi Mode: APSTA Mode\n");
        break;

        case WIFI_MODE_NAN:
        printf("Setting Wifi Mode: NAN Mode\n");
        break;

        case WIFI_MODE_MAX:
        printf("Setting Wifi Mode: MAX Mode\n");
        my_wifi_interface = WIFI_IF_MAX;
        break;
    }

    
    esp_err_t wifi_set_mode_result = esp_wifi_set_mode(MY_WIFI_MODE); //Setting wifi mode into selected "MY_WIFI_MODE"

    if (wifi_set_mode_result != ESP_OK)
    {
        printf("Failed to set Wifi Mode. \n");
        if (wifi_set_mode_result == ESP_ERR_WIFI_NOT_INIT)
        {
            printf("Wifi is not initialized by esp_wifi_init. \n");
        }
        else if (wifi_set_mode_result == ESP_ERR_INVALID_ARG)
        {
            printf("Invalid argument in function. esp_wifi_set_mode(wifi_mode_t mode)\n");
        }
        else
        {
            printf("Refer to error code in esp_err.h");
        }
           
    }
    else
    {
        printf("Wifi Mode Successfully set. \n");
    }

    if (wifi_set_config_result != ESP_OK)
    {
        printf("Failed to set wifi configuration.\n");
        if (wifi_set_config_result == ESP_ERR_WIFI_NOT_INIT)
        {
            printf("WiFi is not initialized by esp_wifi_init.\n");
        }
        else if (wifi_set_config_result == ESP_ERR_INVALID_ARG)
        {
            printf("Invalid argument in function. esp_wifi_set_config(wifi_interface_t interface, wifi_config_t *conf).\n");
        }
        else if (wifi_set_config_result == ESP_ERR_WIFI_IF)
        {
            printf("Invalid Interface. Select one of these: WIFI_IF_STA / WIFI_IF_AP / WIFI_IF_NAN / WIFI_IF_MAX.\n");
        }
        else if (wifi_set_config_result == ESP_ERR_WIFI_MODE)
        {
            printf("Invalid Mode. Select one of these: WIFI_MODE_NULL /WIFI_MODE_AP / WIFI_MODE_STA / WIFI_MODE_APSTA / WIFI_MODE_NAN / WIFI_MODE_MAX.\n");
        }
        else if (wifi_set_config_result == ESP_ERR_WIFI_PASSWORD)
        {
            printf("Invalid Password.\n");
        }
        else if (wifi_set_config_result == ESP_ERR_WIFI_NVS)
        {
            printf("WiFi Internal NVS error.\n");
        }
        else
        {
            printf("refer to the error code in esp_err.h.\n");
        }
    }
    else
    {
        printf("Wifi configuration properly set.");
    }
    

    esp_err_t wifi_start_result = esp_wifi_start(); //Creates selected "MY_WIFI_MODE" control blocks and Starts wifi in selected "MY_WIFI_MODE" and 

    if (wifi_start_result != ESP_OK)
    {
        printf("Failed to start Wifi. \n");
        if (wifi_start_result  == ESP_ERR_WIFI_NOT_INIT)
        {
            printf("Wifi is not initialized by esp_wifi_init. \n");
        }
        else if (wifi_start_result  == ESP_ERR_INVALID_ARG)
        {
            printf("Invalid argument in function. esp_wifi_set_mode(wifi_mode_t mode)\n");
        }
        else if (wifi_start_result  == ESP_ERR_NO_MEM)
        {
            printf("Out of Memory\n");
        }
        else if (wifi_start_result  == ESP_ERR_WIFI_CONN)
        {
            printf("WiFi internal error, station or soft-AP control block wrong\n");
        }
        else if (wifi_start_result  == ESP_FAIL)
        {
            printf("Other Wifi internal errors\n");
        }
           
    }
    else
    {
        printf("Wifi Control Block created, and wifi started. \n");
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
    }

    myserver_func_test();
}