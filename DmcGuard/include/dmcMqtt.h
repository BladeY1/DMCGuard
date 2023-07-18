/***************************************************************
 * @file       include\dmcMqtt.h
 * @brief      amazon aws mqtt function
 **************************************************************/
#ifndef DMC_MQTT_H
#define DMC_MQTT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "aws_iot_mqtt_client.h"
#include "aws_iot_log.h"
#include "aws_iot_mqtt_client_interface.h"

// #include "AWSConfig.h"
#include "dmcToken.h"
#include "dmcControl.h"
#include "deviceControl.h"
#include "common.h"

//void runAmazonMQTT(void);
// runAmazonMQTT's arguments.
// Only pApplicationHandlerData can be NULL.

// extern char *certDirectory ;
// extern char *pTopicName ;
// extern QoS qos ;
// extern pApplicationHandler_t pApplicationHandler ;
// extern void *pApplicationHandlerData ;

// extern AWS_IoT_Client *client;

void mqtt_log(char *l);
void check_err(int err, int good, char* err_mqtt_log);

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data);
void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,IoT_Publish_Message_Params *params, void *pData);

void iot_dmc_control_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,IoT_Publish_Message_Params *params, void *pData);
void iot_command_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,IoT_Publish_Message_Params *params, void *pData);
void iot_token_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,IoT_Publish_Message_Params *params, void *pData);
void runAmazonMQTT(void);

#endif