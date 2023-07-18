/***************************************************************
 * @file       include\dmcControl.h
 * @brief      Dmc operations woth token management and control
 **************************************************************/

#ifndef DMC_CONTROL_H
#define DMC_CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dmcToken.h"

// channel number defined: mqtt=0,zigbee=1,speaker=2

#define dmcTokenLength 32
typedef char typeDmcToken[dmcTokenLength + 1];

// @brief     operation request handle funtion
int opReqHandle(typeDmcToken token, short channel, short right, short param);

// @brief     token management request handle funtion
int tokenReqHandle(typeDmcToken token, short channel, short right, char param);

#endif 
