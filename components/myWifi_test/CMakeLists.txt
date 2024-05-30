idf_component_register(SRCS "myWifi_test.c"
                    INCLUDE_DIRS "include"
                    REQUIRES    "esp_wifi"
                                "esp_netif"
                                "nvs_flash"
                                "esp_event")
