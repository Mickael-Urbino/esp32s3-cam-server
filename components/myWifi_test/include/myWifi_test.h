#ifndef MYWIFI_H_
#define MYWIFI_H_

#define MY_WIFI_SSID "ESP32S3-CAM-SERVER"   //Name of the wifi ACCESS POINT (Used in ESP-IDF's wifi_config_ap "structure" ??)
#define MY_WIFI_PSWD "DroneVTX"             //Password to connect to the ACCESS POINT (Used in ESP-IDF's wifi_config_ap "structure" ??)

#define MY_WIFI_MODE    WIFI_MODE_AP        //Select Default Wifi Mode. WIFI_MODE_NULL / WIFI_MODE_STA / WIFI_MODE_AP / WIFI_MODE_APSTA / WIFI_MODE_NAN / WIFI_MODE_MAX

/* initialize_nvs_flash() function that implements some error recovering code */
void initialize_nvs_flash(void);

/* initialize all Wifi requirements */
void initialize_wifi(void);

/* Mock Function that prints show if component was properly linked to the main folder */
void WifiComponentTest(void);

#endif
