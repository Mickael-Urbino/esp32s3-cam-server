#ifndef MYSERVER_H_ /*Don't forget to change the include guard when copy pasting, it makes one of the header not included in the code and create implicite function declaration in main.c */
#define MYSERVER_H_

#include "esp_http_server.h"

/* Function that will be used to start webserver*/
httpd_handle_t start_webserver(void);

/* Initialize lifflefs */
void init_littlefs(void);

#endif