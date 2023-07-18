#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


#include "dmcMqtt.h"
#include "TiZigbee.h"
#include "Speaker.h"
#include "Local.h"

// branch token2 已经删除homekit部分
// To determine the accessibility status of each channel
struct type_channel_list{
    char *channel_name;       
    pthread_t channel_thread;
    int enabled;
	int channel_fd; // Add channel_fd field that can only be used in the local channel
};
extern struct type_channel_list *CHANNEL_LIST;
// The length of the channel list should be defined by the developer. 
extern int CHANNEL_LIST_LENGTH;

// Initiate the channel list. First argument is the channels' names. Second argument
// is the length of the channel list (How many channels should be enabled).
// Initially, every channel is enabled.
// Return -1 if error occurs. Otherwise return 0.
int initChannelList(char **channel_names, size_t channel_num);

// Check the channel's availability. Return 1 if the channel is avaliable. Otherwise return 0.
// If there's no such channel or the status is broken(not 1 or 0), return -1.
int checkChannel(char *channel_name);

// Enable a channel. This function will only change the status of CHANNEL_LIST.
// Return -1 if there's no such channel name. Otherwise return 0.
int enableChannel(char *channel_name);

// Disable a channel. This function will only change the status of CHANNEL_LIST.
// Return -1 if there's no such channel name. Otherwise return 0.
int disableChannel(char *channel_name);

//Close other Channel.This function used when one channel used.(first run=1)
//success return 1, other subordinate channel not found return -1
int closeOtherChannel(char *channel_name);


// //run amazon MQTT pthread
// void runAmazonMQTT(void);

// //run Zigbee pthread
// void runZigbee(void);

// //run Gadget pthread
// void runGadget(void);


#endif
