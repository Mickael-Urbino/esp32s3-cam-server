idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

spiffs_create_partition_image(myWebserver ../components/myServer_test/Front_End FLASH_IN_PROJECT)