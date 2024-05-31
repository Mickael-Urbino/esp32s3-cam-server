idf_component_register(SRCS "myServer_test.c"
                    INCLUDE_DIRS "include"
                    REQUIRES    "esp_http_server"
                                "esp_littlefs")

littlefs_create_partition_image(myWebServer Front_End)