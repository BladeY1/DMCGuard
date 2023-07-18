/***************************************************************
 * @file       include\deviceControl.h
 * @brief      IoT device attribute manage and control  
 **************************************************************/

#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h> 
#include <string.h> 
#include <wiringPi.h>
#include <softPwm.h>

#include "dmcToken.h"
#include "dmcControl.h"
#include "deviceControl.h"
#include "common.h"



// status , brightness, color ; write , read
#define RIGHT_NUM 6
// aws zigbee speaker local
#define CHANNEL_TOKEN_NUM 4

// device attributes
#define RIGHT_STATUS_READ 0
#define RIGHT_STATUS_WRITE 1

#define RIGHT_BRIGHTNESS_READ 2
#define RIGHT_BRIGHTNESS_WRITE 3

#define RIGHT_COLOR_READ 4
#define RIGHT_COLOR_WRITE 5

// device attributes Operation Params define

#define RIGHT_STATUS_WRITE_PARAM_ON 0
#define RIGHT_STATUS_WRITE_PARAM_OFF 1

#define RIGHT_BRIGHTNESS_WRITE_PARAM_PLUS20 0
#define RIGHT_BRIGHTNESS_WRITE_PARAM_MINUS20 1

#define RIGHT_COLOR_WRITE_RED     4
#define RIGHT_COLOR_WRITE_YELLOW  5
#define RIGHT_COLOR_WRITE_GREEN   6


//bulb on/off and brightness,global variable
extern bool gLightBulbStatus;
extern int  gLightBulbBrightness;
extern int  gLightBulbColor;

extern pthread_mutex_t gMutexLightBulbStatus;
extern pthread_mutex_t gMutexLightBulbBrightness;
extern pthread_mutex_t gMutexLightBulbColor;


// @brief    execute verfied operations
int executeOperate(short channel, short right, short param);

// change light status (on/off)
void changeLightStatus(bool value);

// change light brightness
void changeLightBrightness(int value);

// change light color
void changeLightColor(int color);

// clear raspberry gpio to Output LOW
void clearGPIO(void);

// operte Device Light to the configured status/brighness/color
void operateDeviceLight(void);

// //operate device on/off 
// void operateDevice(bool value);

// //operate device brightness
// void operateDeviceBrightness(int value);

//just check bulb status, no output
bool CheckDeviceStatus(void);

//print bulb on/off status and return
bool readDeviceStatus(void);

//print bulb brightness and return
int readDeviceBrightness(void);

//print bulb color and return
int readDeviceColor(void);

#endif 
