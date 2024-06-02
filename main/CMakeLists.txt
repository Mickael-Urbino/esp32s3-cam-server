idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")


littlefs_create_partition_image(myWebServer ../components/myServer_test/Front_End FLASH_IN_PROJECT)