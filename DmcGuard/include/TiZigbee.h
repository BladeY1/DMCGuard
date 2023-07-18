/***************************************************************
 * @file       zigbee.h
 * @brief      include\zigbee.h \\ control with Zigbee channel 
 **************************************************************/

#include <wiringSerial.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "dmcToken.h"
#include "deviceControl.h"
#include "dmcControl.h"
#include "common.h"

#define DEVICEKEY "test"


// @brief      handle msg received by serial port
int zigbeeMsgHandle(char *msg, int length, int* certify_flag);
// @brief      get msg from serial port
void zigbeeMsgGet(char* buf, int *length, int fd);
// @brief      run Zigbee channel
void runZigbee(void);