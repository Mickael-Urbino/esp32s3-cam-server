#include "myWifi_test.h"
#include "myServer_test.h"
#include "myCamera_test.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"      //For real time operation libraries
#include "freertos/task.h"          //For Multitasking libraries


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

/* Create a task to start camera capture */
void vTaskStartCamera(void *pvParameters)
{
    CameraComponentTest();

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
    init_littlefs();        //Init LittleFS for Webserver files storage
    
    xTaskCreatePinnedToCore(vTaskStartWebServer, "Start Webserver Task", 4096, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(vTaskStartCamera, "Start Camera Task", 4096, NULL, 0, NULL, 1);

    //CameraComponentTest();
    //vTaskStartScheduler(); //vTaskStartScheduler() is not needed and creates an issue here, because it is already started before task app_main is called (it is default esp-idf behavior)
    
}