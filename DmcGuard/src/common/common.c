/***************************************************************
 * @file       src\common\common.c
 * @brief      channel threads run and manage funnction
 **************************************************************/

#include "common.h"



// To determine the accessibility status of each channel
struct type_channel_list *CHANNEL_LIST = NULL;
// The length of the channel list should be defined by the developer. 
int CHANNEL_LIST_LENGTH = 4; //amazonMQTT Zigbee Speaker Local

/***************************************************************
 * @brief     Initiate the channel list
 * @param     channel_names channel to be initiated
 * @param     channel_num the length of the channel list
 * @return    0, success; -1, error 
 **************************************************************/
int initChannelList(char **channel_names, size_t channel_num){
    CHANNEL_LIST_LENGTH = channel_num;
    
    CHANNEL_LIST = (struct type_channel_list *)malloc(CHANNEL_LIST_LENGTH * sizeof(struct type_channel_list));
    if (CHANNEL_LIST == NULL){
        return -1;
    }
    
    //copy name to CHANNEL_LIST[i].channel_name,set enabled to 1,thread to 0
    for (int i = 0; i < CHANNEL_LIST_LENGTH; i++){
        int length = strlen(channel_names[i]);
        // setup channels' names
        CHANNEL_LIST[i].channel_name = malloc(length + 1);
        if (CHANNEL_LIST[i].channel_name == NULL){
            return -1;
        }
        strncpy(CHANNEL_LIST[i].channel_name, channel_names[i], length);
        // set initial status to True.
        CHANNEL_LIST[i].enabled = 1;
        CHANNEL_LIST[i].channel_thread = 0;
    }
    
    return 0; //init channel success
}

// Check the channel's availability. 
// Return Value。 1 = enabel, 0 = diable, 2 = other error

int checkChannel(char *channel_name){
    for (size_t i = 0; i < CHANNEL_LIST_LENGTH; i++){
        // printf("-%s-%s-", CHANNEL_LIST[i].channel_name, channel_name);
        if(strncmp(CHANNEL_LIST[i].channel_name, channel_name, strlen(channel_name)) == 0){
            if(CHANNEL_LIST[i].enabled == 1){
                // return 1 if this channel is avaliable.
                // printf("enable\n");
                return 1;
            } else if (CHANNEL_LIST[i].enabled == 0){
                // return 0 if this channel is not avaliable.
                // printf("disable\n");
                return 0;
            } else{
                // raise an error.
                // printf("error\n");
                return 2;
            }
        }
    }
    return 2;
}

// Enable a channel. This function will only change the status of CHANNEL_LIST.
// Return -1 if there's no such channel name. Otherwise return 0.
int enableChannel(char *channel_name){
    for (size_t i = 0; i < CHANNEL_LIST_LENGTH; i++){
        if(strncmp(CHANNEL_LIST[i].channel_name, channel_name, strlen(channel_name)) == 0){
            // check if this channel is running
            if (CHANNEL_LIST[i].enabled == 1 && CHANNEL_LIST[i].channel_thread != 0){
                printf("This channel is already enabled and running.\n");
                return 0;
            }
            
            // set the channel status to True.
            CHANNEL_LIST[i].enabled = 1;

            // start the channel's process.
            pthread_t *channel_thread_id = (pthread_t *)malloc(sizeof(pthread_t));
            int ThreadSuccess = 0;//thread create flag,0=success

            // compare the channel's name with every known channel's name. To launch the channel process.
            if(strcmp(channel_name, "amazonMQTT") == 0){
                ThreadSuccess = pthread_create(channel_thread_id, NULL, (void *)runAmazonMQTT, NULL); 
                if(0 != ThreadSuccess){
                    printf("Create AWSmqtt pthread error\n");
                    exit(1);
                }
                CHANNEL_LIST[i].channel_thread = *channel_thread_id; //get pthread  id
            }
             // if channel_name is zigbee call runZigbee to start zigbee control
            else if(strcmp(channel_name, "Zigbee") == 0){
                ThreadSuccess = pthread_create(channel_thread_id, NULL, (void *)runZigbee, NULL); 
                if(0 != ThreadSuccess){
                    printf("Create Zigbee pthread error\n");
                    exit(1);
                }
                CHANNEL_LIST[i].channel_thread = *channel_thread_id;
            }
            
             // if channel_name is Gadget call Speaker to start Gadget control
            else if(strcmp(channel_name, "Speaker") == 0){
                ThreadSuccess = pthread_create(channel_thread_id, NULL, (void *)runSpeaker, NULL); 
                if(0 != ThreadSuccess){
                    printf("Create Gadget pthread error\n");
                    exit(1);
                }
                CHANNEL_LIST[i].channel_thread = *channel_thread_id;
            }  
            
            // if channel_name is Local call runLocal to start Local control
            else if(strcmp(channel_name, "Local") == 0){
                ThreadSuccess = pthread_create(channel_thread_id, NULL, (void *)runLocal, NULL); 
                if(0 != ThreadSuccess){
                    printf("Create Local pthread error\n");
                    exit(1);
                }
                CHANNEL_LIST[i].channel_thread = *channel_thread_id;
            }            
            
            return 0;
        }
    }
    return -1;
}

//Close other Channel.This function used when one channel used.(first run=1)
//success return 1, other subordinate channel not found return -1
int closeOtherChannel(char *channel_name)
{   

    for(size_t i = 0; i < CHANNEL_LIST_LENGTH; i++){
        if(strncmp(CHANNEL_LIST[i].channel_name, channel_name, strlen(channel_name)) != 0){
            // main channel/amazonMQTT should keep run
            if (strcmp(CHANNEL_LIST[i].channel_name, "amazonMQTT") != 0) {
                CHANNEL_LIST[i].enabled = 0;
                CHANNEL_LIST[i].channel_thread = 0; //close
            }
        }
        else {
            printf("other subordinate channel(s) not found.\n");
            return -1; // other subordinate channel not found
        }
    }
    printf("All other subordinate channel(s) channels have been closed.\n");
    return 1;
}

// Disable a channel. This function will only change the status of CHANNEL_LIST.
// Return -1 if there's no such channel name. Otherwise return 0.
int disableChannel(char *channel_name){
    for (size_t i = 0; i < CHANNEL_LIST_LENGTH; i++){
        if(strncmp(CHANNEL_LIST[i].channel_name, channel_name, strlen(channel_name)) == 0){
            // set the channel status to False.
            CHANNEL_LIST[i].enabled = 0;
            // remove the thread address
            CHANNEL_LIST[i].channel_thread = 0;
            if (strcmp(CHANNEL_LIST[i].channel_name, "local") == 0) {
                // Close the socket.
                close(CHANNEL_LIST[i].channel_fd);
                printf("Closed local channel socket.\n");
                // Perform any additional cleanup or handling specific to the "local" channel.
                // ...
            }

            return 0;
        }
    }   
    printf("disableChannel error/n");
    return -1; 
}
