/***************************************************************
 * @file       Local.h
 * @brief      include\Local.h \\ control with Local channel 
 **************************************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "deviceControl.h"
#include "dmcControl.h"
#include "dmcToken.h"

#define MAXLINE 40
#define SER_PORT 8000
#define MAX_CLIENTS 10

// @brief      handler of the receiving thread
void *handle_client(void *arg);

// @brief      run Local channel
void runLocal(void);