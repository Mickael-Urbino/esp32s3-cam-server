# _ESP32S3-CAM-SERVER_

(Ongoing...)

This is a simple http server based on an ESP32-S3 working as an access point. HTML files, CSS files and Pictures are uploaded from a Littlefs partition. The example uses esp-idf.

## How to use example
1. Change Wifi Credentials to your preferences in myWifi.h

Example:
#define MY_WIFI_SSID "ESP32S3-CAM-SERVER" 
#define MY_WIFI_PSWD "DroneVTX" 

2. Flash and monitor, to see the ESP32's Access Point IP address. (i.e 192.168.4.1 by default)

3. Connect to the ESP32 AP using your credential.

4. Open your web browser and navigate to the Access Point's IP address.

5. The Website is displayed.

