/***************************************************************
 * @file       Speaker.h
 * @brief      include\Speaker.h \\ control with Speaker channel 
 **************************************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "deviceControl.h"
#include "dmcControl.h"
#include "dmcToken.h"

#define MAXLINE 40
#define SER_PORT 6666

// your speaker hub ip
#define SPEAKER_HUB_IP "192.168.31.54"
#define SPEAKER_HUB_PORT 1234

// @brief      run Speaker channel
void runSpeaker(void);

