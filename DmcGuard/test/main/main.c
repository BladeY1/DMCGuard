/***************************************************************
 * @file       main.c
 * @brief      /test/main/main.c main file for run
 * @date       2021-11-25
 **************************************************************/
#include <stdio.h>
#include <pthread.h>  
#include <stdlib.h>

#include "aws_iot_log.h"

#include "common.h"
#include "dmcMqtt.h"
#include "deviceControl.h"


bool gLightBulbStatus = false; //global,status of bulb switch ,on/off true/false  
int gLightBulbBrightness = 60; //global,brightness of bulb , 20-100
int gLightBulbColor = 4; //global,bulb color.  wiringPi Pin4= red, 5= yellow, 6= green  
//int  gLightBulbColor = 0; //vacant

pthread_mutex_t gMutexLightBulbStatus;          //mutex state on/off
pthread_mutex_t gMutexLightBulbBrightness;    //mutex brightness
pthread_mutex_t gMutexLightBulbColor;         //mutex color

// //***********amazon MQTT log function below****************
// //print mqtt log / print string
// void mqtt_log(char *l){
//     printf("%s\n", l);
// }

// //if error print err_mqtt_log
// void check_err(int err, int good, char* err_mqtt_log){
//     if(err != good){
//         printf("%s\n", err_mqtt_log);
//         exit(1);
//     }
//     return;
// }


// /***************************************************************
//   *  @brief     iot_subscribe_callback_handler,iot上传回调处理函数，接受MQTT传来的单字符然后调用操作函数
//   *  @param     pClient, topicName, topicNameLen,params,pData
//   *  params其中void *payload;		///< Pointer to MQTT message payload (bytes).
//   *  @return    void 
//  **************************************************************/
// void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
// 									IoT_Publish_Message_Params *params, void *pData) {
//     char cmd = 0;
//     int err;
//     //only take one char in payload string
//     cmd = ((char*) params->payload)[0];
//     //
//     printf("received command:>>%c", cmd);

//     switch(cmd) {
//         //bulb switch on
//         case 't':
// 			operateDevice(true,"amazonMQTT");
//             break;
//         //bulb switch off
//         case 'f':
// 			operateDevice(false,"amazonMQTT");
//             break;
//         //bulb brightness +20
//         case '+':
//             operateDeviceBrightness(20,"amazonMQTT");
//             break;
//         //bulb brightness -20
//         case '-':
//             operateDeviceBrightness(-20,"amazonMQTT");
//             break;
//         //query bulb on/off and brightness
//         case 'q':
//             //mutex_lock,如果放在readDeviceStatus()函数内部，担心周期read会与修改设备属性请求冲突，所以先不改
//             pthread_mutex_lock(&gMutexLightBulbStatus);
// 			readDeviceStatus();
//             pthread_mutex_unlock(&gMutexLightBulbStatus);

//             pthread_mutex_lock(&gMutexLightBulbBrightness);
//             readDeviceBrightness();
//             pthread_mutex_unlock(&gMutexLightBulbBrightness);
//             break;
//         // exit AWS channel
//         case 'e':
//             IOT_INFO("Exiting from AWSChannel.");
//             pthread_exit(NULL);
//             break;
//         // enable Homekit Chennel
//         case 'Z':
//             err = enableChannel("Zigbee");
//             check_err(err, 0, "Failed to enable Zigbee channel.");
//             mqtt_log("Zigbee channel enabled!");
//             break;
//         // diable Zigbee Chennel
//         case 'z':
//             disableChannel("Zigbee");
//             break;
//         // enable Gadget channel
//         case 'S':
//             err = enableChannel("Speaker");
//             check_err(err, 0, "Failed to enable Speaker channel.");
//             mqtt_log("Speaker channel enabled!");
//             break;
//         // diable Speaker Chennel
//         case 's':
//             disableChannel("Speaker");
//             break;
//         default:
//             printf("[Error:] Unsupported command.\n");
//             break;
// 		}
// }

// main fucntion. 1. init channel 2. init mqtt 3. 4 threads                                                                  enable
int main(void){
    //init channel
    char *channel_names[] = {"amazonMQTT", "Zigbee", "Speaker", "Local"};
    int err;
    //init Bulb mutex value
	pthread_mutex_init(&gMutexLightBulbStatus, NULL);
    pthread_mutex_init(&gMutexLightBulbBrightness, NULL);

    err = initChannelList(channel_names, 4);
    check_err(err, 0, "Failed to initialize.");
    mqtt_log("channel initialized!");

    // init DMC access control matrix
    err = initDmcACM();
    check_err(err, 0, "DMC ACM Failed to initialize.");
    mqtt_log("DMC ACM initialized!");
    
    //enable 4 channels
    err = enableChannel("amazonMQTT");
    check_err(err, 0, "Failed to enable amazonMQTT channel.");
    mqtt_log("amazonMQTT channel enabled!");

    // err = enableChannel("Homekit");
    // check_err(err, 0, "Failed to enable Homekit channel.");
    // mqtt_log("Homekit channel enabled!");

    err = enableChannel("Zigbee");
    check_err(err, 0, "Failed to enable Zigbee channel.");
    mqtt_log("Zigbee channel enabled!");

    err = enableChannel("Speaker");
    check_err(err, 0, "Failed to enable Speaker channel.");
    mqtt_log("Speaker channel enabled!");
    
    err = enableChannel("Local");
    check_err(err, 0, "Failed to enable Local channel.");
    mqtt_log("Local channel enabled!");

    //
    if(CHANNEL_LIST[0].channel_thread == 0 && CHANNEL_LIST[1].channel_thread == 0 && CHANNEL_LIST[2].channel_thread == 0 && CHANNEL_LIST[3].channel_thread == 0){
        printf("enable channels Process error.\n");
        exit(1);
    } else{
        pthread_join(CHANNEL_LIST[0].channel_thread, NULL);
        pthread_join(CHANNEL_LIST[1].channel_thread, NULL);
        pthread_join(CHANNEL_LIST[2].channel_thread, NULL);
        pthread_join(CHANNEL_LIST[3].channel_thread, NULL);
    }

    printf("End of main process.\n");

    return 0;
}
