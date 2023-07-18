/***************************************************************
 * @file       src\dmcMqtt\dmcMqtt.c
 * @brief      amazon aws mqtt function,publish message to aws
 **************************************************************/

// modify from aws c sdk subscribe_publish_library_sample.c
/**
 * @file subscribe_publish_library_sample.c
 * @brief simple MQTT publish and subscribe on the same topic using the SDK as a library
 *
 * This example takes the parameters from the aws_iot_config.h file and establishes a connection to the AWS IoT MQTT Platform.
 * It subscribes and publishes to the same topic - "sdkTest/sub"
 *
 * If all the certs are correct, you should see the messages received by the application in a loop.
 *
 * The application takes in the certificate path, host name , port and the number of times the publish should happen.
 *
 */

#include "dmcMqtt.h"

#define HOST_ADDRESS_SIZE 255
//Amazon mqtt parameter init**********************   注意一下几个topic不是都有在使用，主要只用了每一个sub接受命令，publish主要是用于调试
char *certDirectory = "../../test/main/cert";
char *subStatusTopicName = "Light_01-StatusCommand"; // subscribe light and dmc status // only used in Developer debugging, admin mode 
char *pubStatusTopicName = "Light_01-StatusReport"; // publish light and dmc status   // Developer debugging, admin mode

char *subDmcTopicName = "Light_01-DeviceDmcCommand"; // subscribe dmc status, user control dmc on/off.
char *pubDmcTopicName = "Light_01-DeviceDmcReport"; // publish dmc on/off status   //现用于三个dmc的状态上报

char *subCommandTopicName = "Light_01-OperateCommand"; // subscribe light command
char *pubCommandTopicName = "Light_01-OperateCommandReport"; // publish light command report , report mqtt command report

char *subTokenTopicName = "Light_01-TokenCommand"; // subscribe light token
char *pubTokenTopicName = "Light_01-TokenReport"; // publish light token report  // NOT USE NOW

QoS qos = QOS0;  // define in \aws_iot_mqtt_client.h,vlaue = 0/1

pApplicationHandler_t pApplicationHandler = &iot_subscribe_callback_handler; // 目前这个topic只用于开发模式测试

pApplicationHandler_t pApplicationDeviceHandler = &iot_dmc_control_subscribe_callback_handler;  // decice dmc control on/off
pApplicationHandler_t pApplicationCommandHandler = &iot_command_subscribe_callback_handler;
pApplicationHandler_t pApplicationTokenHandler = &iot_token_subscribe_callback_handler;

void *pApplicationHandlerData = NULL;

void *pApplicationDeviceHandlerData = NULL;
void *pApplicationCommandHandlerData = NULL;
void *pApplicationTokenHandlerData = NULL;
char *awsChannel = "amazonMQTT";

// client init
AWS_IoT_Client *client =  NULL;

//***********amazon MQTT log function below****************
//print mqtt log / print string
void mqtt_log(char *l){
    printf("%s\n", l);
}

//if error print err_mqtt_log
void check_err(int err, int good, char* err_mqtt_log){
    if(err != good){
        printf("%s\n", err_mqtt_log);
        exit(1);
    }
    return;
}


/***************************************************************
  *  @brief     aws mqtt disconnect with server Handler,atuo and manual reconnect.
  *  @param     pClient 
  *  @param     data 
  *  @return    void
 **************************************************************/
void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) {
    IOT_WARN("[MQTT] disconnect");
    IoT_Error_t rc = FAILURE; //error return code 
    if(NULL == pClient) {
        return;
    }
    IOT_UNUSED(data);/* Used to avoid warnings in case of unused parameters in function pointers */
    
    if(aws_iot_is_autoreconnect_enabled(pClient)) {
        IOT_INFO("[MQTT] Auto Reconnect is enabled, Reconnecting attempt will start now");
    } else {
        IOT_WARN("[MQTT] Auto Reconnect not enabled. Starting manual reconnect...");
        rc = aws_iot_mqtt_attempt_reconnect(pClient);
        if(NETWORK_RECONNECTED == rc) {
            IOT_WARN("[MQTT] Manual Reconnect Successful");
        } else {
            IOT_WARN("[MQTT] Manual Reconnect Failed - %d", rc);
        }
    }
}

/***************************************************************
  *  @brief     iot订阅处理回调，接受MQTT传来的单字符然后调用操作函数 对应的是sSubStatus   目前只用于开发模式测试
  *  @param     pClient, topicName, topicNameLen,params,pData
  *  params其中void *payload;		///< Pointer to MQTT message payload (bytes).
  *  @return    void 
 **************************************************************/
void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
									IoT_Publish_Message_Params *params, void *pData) {
    char cmd = 0;
    int err;
    //only take one char in payload string
    cmd = ((char*) params->payload)[0];
    //
    printf("[MQTT]StatusCommand received command:>>%c", cmd);

    switch(cmd) {
        //bulb switch on
        case 't':
			// operateDevice(true);
            changeLightStatus(true);
            operateDeviceLight();
            break;
        //bulb switch off
        case 'f':
			// operateDevice(false);
            changeLightStatus(false);
            operateDeviceLight();
            break;
        //bulb brightness +20
        case '+':
            // operateDeviceBrightness(20);
            changeLightBrightness(20);
            operateDeviceLight();
            break;
        //bulb brightness -20
        case '-':
            // operateDeviceBrightness(-20);
            changeLightBrightness(-20);
            operateDeviceLight();
            break;
        //bulb color=4 red
        case '4':
            changeLightColor(4);
            operateDeviceLight();
            break;
        //bulb color=5 yellow
        case '5':
            changeLightColor(5);
            operateDeviceLight();
            break;
        //bulb color=6 green
        case '6':
            changeLightColor(6);
            operateDeviceLight();
            break;
        //query bulb on/off and brightness
        case 'q':
            // pthread_mutex_lock(&gMutexLightBulbStatus);
			readDeviceStatus();
            // pthread_mutex_unlock(&gMutexLightBulbStatus);

            // pthread_mutex_lock(&gMutexLightBulbBrightness);
            readDeviceBrightness();
            // pthread_mutex_unlock(&gMutexLightBulbBrightness);
            readDeviceColor();
            break;
        // exit AWS channel
        case 'e':
            IOT_INFO("Exiting from AWSChannel.");
            pthread_exit(NULL);
            break;
        // enable Homekit Chennel
        case 'Z':
            err = enableChannel("Zigbee");
            check_err(err, 0, "Failed to enable Zigbee channel.");
            mqtt_log("Zigbee channel enabled!");
            break;
        // diable Zigbee Chennel
        case 'z':
            disableChannel("Zigbee");
            break;
        // enable Gadget channel
        case 'S':
            err = enableChannel("Speaker");
            check_err(err, 0, "Failed to enable Speaker channel.");
            mqtt_log("Speaker channel enabled!");
            break;
        // diable Speaker Chennel
        case 's':
            disableChannel("Speaker");
            break;
        // enable Local channel
        case 'L':
            err = enableChannel("Local");
            check_err(err, 0, "Failed to enable Local channel.");
            mqtt_log("Local channel enabled!");
            break;
        // diable Local Chennel
        case 'l':
            disableChannel("Local");
            break;
        default:
            printf("[Error:] Unsupported command.\n");
            break;
		}
}

/***************************************************************
  *  @brief     iot device and dmc control operation request callback 
  *  @param     pClient, topicName, topicNameLen,params,pData
  *  params其中void *payload;		///< Pointer to MQTT message payload (bytes).
  *  @return    void 
 **************************************************************/
void iot_dmc_control_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
									IoT_Publish_Message_Params *params, void *pData) {
    char cmd = 0;
    int err;
    //only take one char in payload string
    cmd = ((char*) params->payload)[0];
    //
    printf("[MQTT]Dmc Control Command received command:>>%c", cmd);

    switch(cmd) {
        // exit AWS MQTT channel
        case 'e':
            IOT_INFO("Exiting from AWSChannel.");
            pthread_exit(NULL);
            break;
        // enable Zigbee Chennel
        case 'Z':
            err = enableChannel("Zigbee");
            check_err(err, 0, "Failed to enable Zigbee channel.");
            mqtt_log("Zigbee channel enabled!");
            break;
        // disable Zigbee Chennel
        case 'z':
            disableChannel("Zigbee");
            mqtt_log("Zigbee channel disabled!");
            break;
        // enable Speaker channel
        case 'S':
            err = enableChannel("Speaker");
            check_err(err, 0, "Failed to enable Speaker channel.");
            mqtt_log("Speaker channel enabled!");
            break;
        // diable Speaker Chennel
        case 's':
            disableChannel("Speaker");
            mqtt_log("Speaker channel disabled!");
            break;
        // enable Local channel
        case 'L':
            err = enableChannel("Local");
            check_err(err, 0, "Failed to enable Local channel.");
            mqtt_log("Local channel enabled!");
            break;
        // diable Local Chennel
        case 'l':
            disableChannel("Local");
            break;
        default:
            printf("[Error:] Device Dmc control Unsupported command.\n");
            break;
		}
}

/***************************************************************
  *  @brief     iot commmand operation request callback
  *  @param     pClient, topicName, topicNameLen,params,pData
  *  params其中void *payload;		///< Pointer to MQTT message payload (bytes).
  *  @return    void 
 **************************************************************/
void iot_command_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
									IoT_Publish_Message_Params *params, void *pData) {
    /*****************************************
    operation request payload format:
    channel,right,param,token
    e.g.:   
    0,1,1,abcdefghijklmnopqrstuvwxyz123456
    means set light status to on
    **********************************************/
    // 因为是server传来的数据转发,固定格式之后就不用strtok(NULL, " ,{}");分隔字符了
    char payloadChannel = -1;
    char payloadRight = -1; 
    char payloadParam = -1;
    // int err;
    // int payloadLen = params->payloadLen;

    payloadChannel = ((char*) params->payload)[0];
    payloadRight = ((char*) params->payload)[2];
    payloadParam = ((char*) params->payload)[4];

    //typeDmcToken payloadToken = ""; // token in payload
    char payloadToken[dmcTokenLength + 1] = "";
    strncpy(payloadToken, ((char*) params->payload) + 6, dmcTokenLength);

    printf("[MQTT]OperationCommand received token:>>int channel: %c, right: %c, param: %c, token: %s\n", payloadChannel, payloadRight, payloadParam, payloadToken);

    short channel = (short)payloadChannel - 48; // char to short, should minus 48
    short right = (short)payloadRight - 48;
    short opParam = (short)payloadParam - 48;

    //OpRequest info flow : dmcMqtt iot_command_subscribe_callback_handler() -> dmcControl opReqHandle() -> deviceControl executeOperate()
    //execute device operation

/*********************opReqHandle return value******************************
 * @return    0 success(write opertion success return), -1 right in req not found(out right range),-2 = other error 
 * @return    RIGHT_STATUS_READ: 1=on 2=off ;RIGHT_STATUS_WRITE: 0=success;
 * @return    RIGHT_BRIGHTNESS_READ: Ret=readDeviceBrightness() + 10; RIGHT_BRIGHTNESS_WRITE: 0=success
 * @return    RIGHT_COLOR_READ: 4=red 5=yellow 6=green; RIGHT_COLOR_WRITE: 0=success
 * @return    -3 = token not found in gACM;  -4 = other error in opReqHandle
 * ***********************************************************************/
    channel = 0;  //aws channel = 0
    int mqttOpRet = opReqHandle(payloadToken, channel, right, opParam);
    char commandPubPayload[20];
    // char commandPubPayload[50];

    sprintf(commandPubPayload, "%d",mqttOpRet);

    // 以下这部分不在树莓派上二次处理了，返回mqttOpRet的值，交由服务器处理
    // if (mqttOpRet == 0)
    //     sprintf(commandPubPayload, "success");  // light on
    // else if (mqttOpRet == 1) {
    //     sprintf(commandPubPayload, "on");  // read req: light on
    // }
    // else if (mqttOpRet == 2) {
    //     sprintf(commandPubPayload, "off"); // read req: light off
    // }
    // else if (mqttOpRet >= 10 && mqttOpRet <= 110) {
    //     int brightnessPub = mqttOpRet - 10; // minus addtional 10
    //     sprintf(commandPubPayload, "bright%d", brightnessPub); //read req: brighness value
    // }
    // else {
    //     sprintf(commandPubPayload, "error in Channel MQTT's opReqHandle"); //read req: brighness value        
    // }

    // publish to the topic "Light_01-OperateCommandReport" when receive req
    IoT_Publish_Message_Params paramsCommandReqQOS0;
    paramsCommandReqQOS0.qos= QOS0;
    paramsCommandReqQOS0.payload = (void *) commandPubPayload;
    paramsCommandReqQOS0.payloadLen = strlen(commandPubPayload);
    paramsCommandReqQOS0.isRetained = 0;

    //IoT_Error_t rc = FAILURE;
    //rc = aws_iot_mqtt_publish(client, pubStatusTopicName, strlen(pubStatusTopicName), &paramsQOS0); 
    // sleep(1);
    printf("publish to Topic \"Light_01-OperateCommandReport\" for command report\n");
    aws_iot_mqtt_publish(client, pubCommandTopicName, strlen(pubCommandTopicName), &paramsCommandReqQOS0);
    // 因为是QOS0 所以其实也可以不用检测是否发送成功，至多一次发送    
    // if(SUCCESS != rc) {
    //     IOT_ERROR("[MQTT]Error subscribing : %d ", rc);
    //     return;
    // }   
}
// // publish device status when Operation request allowed
// void publishStatus(string payload) {
    
// }

// /***************************************************************
//   *  @brief     get token infomation from mqtt payload
//   *  @param     
//   *  @return    void 
//  **************************************************************/
// void getTokenInfo(char * payload, ){
// void getTokenInfo(typeDmcToken ad, typebase64AuthorizationData base64ad, typeRight * rightArray, char * payload, int numberofRights){
// 	int i = 0;
// 	char * rightSplit = NULL;

// 	rightSplit = strtok(payload + strlen("* ad: ") + dmcTokenLength + strlen(" rights: "), " ,{}");
// 	rightSplit = strtok(NULL, " ,{}");

// 	for ( i = 0; i < numberofRights; i ++) {
// 		rightArray[i] = atoi(rightSplit);
// 		rightSplit = strtok(NULL, " ,{}");
// 	}


// }


/***************************************************************
  *  @brief     iot订阅处理回调，处理token更新相关
  *  @param     pClient, topicName, topicNameLen,params,pData
  *  params其中void *payload;		///< Pointer to MQTT message payload (bytes).
  *  @return    void 
 **************************************************************/
void iot_token_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
									IoT_Publish_Message_Params *params, void *pData) {
    /*****************************************
    token request payload format:
    param,channel,right,token
    e.g.:   
    a,1,1,abcdefghijklmnopqrstuvwxyz123456
    means add a token in channel zigbee in right 1
    **********************************************/
    // 因为是server传来的数据转发,固定格式之后就不用strtok(NULL, " ,{}");分隔字符了
    char cmd = 0;
    char payloadChannel = -1;
    char payloadRight = -1; 
    int err;
    // int payloadLen = params->payloadLen;

    cmd = ((char*) params->payload)[0];
    payloadChannel = ((char*) params->payload)[2];
    payloadRight = ((char*) params->payload)[4];

    //typeDmcToken payloadToken = ""; // token in payload
    char payloadToken[dmcTokenLength + 1] = "";
    strncpy(payloadToken, ((char*) params->payload) + 6, dmcTokenLength);


    printf("[MQTT]TokenCommand received command:>>%c\n", cmd);
    printf("[MQTT]TokenCommand received token:>>int channel: %c, right: %c, token: %s\n", payloadChannel, payloadRight, payloadToken);

    short channel = (short)payloadChannel - 48; // char to short, should minus 48
    short right = (short)payloadRight - 48;

    // strncpy(token, payloadToken, dmcTokenLength);

    //
    printf("channel: %hd, right: %hd, token: %s\n", channel, right, payloadToken);
    switch(cmd) {
        case 'a':
            // add token 
            err = insertDmcToken(payloadToken, channel, right);
            if (err == 0)
            {
                printf("[OK]token add success\n");
            } else
            {
                printf("[Error]token add error. Error code: %d \n", err);
            }
            
            break;
        case 'd':
            // delete token

            err = deleteDmcToken(payloadToken, channel, right);
            if (err == 0)
            {
                printf("[OK]token delete success\n");
            } else
            {
                printf("[Error]token delete error\n");
            }
            break;

        default:
            printf("[Error:] Unsupported command.\n");
            break;
		}
}



/***************************************************************
  *  @brief     amazon mqtt dmc thread main function
  *  @param     void parameter in header file
  *  @return    void
 **************************************************************/
//主要包括以下几个操作1.检测运行前是否有误2.将证书、端口、mqtt参数等信息写入3.connect和reconnect的设置 4.订阅 
//5连接和订阅完成之后进入while循环，循环1s打印channel和bulb状态
void runAmazonMQTT(void){
    // exit if this channel is not avaliable
    if (checkChannel(awsChannel) == 0){
        printf("amazonMQTT Channel closed. \n");
        return;
    }
    //check mqtt connect parameter
    if (certDirectory == NULL || subStatusTopicName == NULL ||qos == -1 || pApplicationHandler == NULL || pApplicationDeviceHandler == NULL || pApplicationTokenHandler == NULL || pApplicationCommandHandler == NULL){
        printf("Parameters error.\n");
        if(certDirectory == NULL){
            printf("cerDirectory NULL\n");
        }
        if(subStatusTopicName == NULL){
            printf("subStatusTopicName NULL\n");
        }
        if(qos == -1){
            printf("qos -1\n");
        }
        if(pApplicationHandler == NULL || pApplicationDeviceHandler == NULL || pApplicationTokenHandler == NULL || pApplicationCommandHandler == NULL ){
            printf("pApplicationHandler NULL\n");
        }
        return;
    }
    //AWS_IOT_MQTT_HOST
    char HostAddress[HOST_ADDRESS_SIZE] = AWS_IOT_MQTT_HOST;

    //setup parameter include in AWSConfig.h
    uint32_t port = AWS_IOT_MQTT_PORT;

    //certs file path
    char rootCA[PATH_MAX + 1];
    char clientCRT[PATH_MAX + 1];
    char clientKey[PATH_MAX + 1];
    // char CurrentWD[PATH_MAX + 1]; //如果这也是PATH_MAX 有snprintf截断的风险,但一般也不会出现
    char CurrentWD[180];
    char cPayload[100];
    // char statusPubPayload[100];
    // char dmcPubPayload[100];

    IoT_Error_t rc = FAILURE;

    //AWS_IoT_Client *client = malloc(sizeof(AWS_IoT_Client));
    // AWS_IoT_Client *client =  NULL;
    client = ((AWS_IoT_Client *)malloc(sizeof(AWS_IoT_Client)));

    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

    // IoT_Publish_Message_Params paramsQOS0;
	// IoT_Publish_Message_Params paramsQOS1;

    // get certs path
    getcwd(CurrentWD, sizeof(CurrentWD));
    snprintf(rootCA, PATH_MAX + 1, "%s/%s/%s", CurrentWD, certDirectory, AWS_IOT_ROOT_CA_FILENAME);
    snprintf(clientCRT, PATH_MAX + 1, "%s/%s/%s", CurrentWD, certDirectory, AWS_IOT_CERTIFICATE_FILENAME);
    snprintf(clientKey, PATH_MAX + 1, "%s/%s/%s", CurrentWD, certDirectory, AWS_IOT_PRIVATE_KEY_FILENAME);

    mqttInitParams.enableAutoReconnect = false; // We enable this later below
    mqttInitParams.pHostURL = HostAddress;
    mqttInitParams.port = port;
    mqttInitParams.pRootCALocation = rootCA;
    mqttInitParams.pDeviceCertLocation = clientCRT;
    mqttInitParams.pDevicePrivateKeyLocation = clientKey;
    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = NULL;

    // aws mqtt init
    rc = aws_iot_mqtt_init(client, &mqttInitParams);

    if(SUCCESS != rc) {
        IOT_ERROR("aws_iot_mqtt_init returned error : %d ", rc);
        return;
    }

    connectParams.keepAliveIntervalInSec = 600;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = AWS_IOT_MQTT_CLIENT_ID;
    connectParams.clientIDLen = (uint16_t) strlen(AWS_IOT_MQTT_CLIENT_ID);
    connectParams.isWillMsgPresent = false;

    //********以下是mqtt的连接和重连的设置******对应subscribe_publish_library_sample.c
    IOT_INFO("Connecting...");
    //rc is error code defined by aws. rc <= -1 error.rc>= 0 success.
    // rc enum type
    rc = aws_iot_mqtt_connect(client, &connectParams);
    if(SUCCESS != rc) {
        IOT_ERROR("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
        return;
    }

    /*
     * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in LightBulb.h
     *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
     *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
     * 
     *  // Auto Reconnect specific config
        #define AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL 1000 ///< Minimum time before the First reconnect attempt is made as part of the exponential back-off algorithm
        #define AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL 128000 ///< Maximum time interval after which exponential back-off will stop attempting to reconnect.
     */

    //Auto reconnect
    rc = aws_iot_mqtt_autoreconnect_set_status(client, true);
    if(SUCCESS != rc) {
        IOT_ERROR("[MQTT]Unable to set Auto Reconnect to true - %d", rc);
        return;
    }
  
    //aws Subscribing to topic light Status. Only used in developer debugging mode
    IOT_INFO("[MQTT]Subscribing to topic: %s", subStatusTopicName);
    rc = aws_iot_mqtt_subscribe(client, subStatusTopicName, strlen(subStatusTopicName), qos, pApplicationHandler, pApplicationHandlerData);
    if(SUCCESS != rc) {
        IOT_ERROR("[MQTT]Error subscribing : %d ", rc);
        return;
    }
    
    //aws Subscribing to topic light device dmc status and control
    IOT_INFO("[MQTT]Subscribing to topic: %s", subDmcTopicName);
    rc = aws_iot_mqtt_subscribe(client, subDmcTopicName, strlen(subDmcTopicName), qos, pApplicationDeviceHandler, pApplicationDeviceHandlerData);
    if(SUCCESS != rc) {
        IOT_ERROR("[MQTT]Error subscribing : %d ", rc);
        return;
    }

    //aws Subscribing to topic light Operation Command (new)
    IOT_INFO("[MQTT]Subscribing to topic: %s", subCommandTopicName);
    rc = aws_iot_mqtt_subscribe(client, subCommandTopicName, strlen(subCommandTopicName), qos, pApplicationCommandHandler, pApplicationCommandHandlerData);
    if(SUCCESS != rc) {
        IOT_ERROR("[MQTT]Error subscribing : %d ", rc);
        return;
    }

    //aws Subscribing to topic light Token
    IOT_INFO("[MQTT]Subscribing to topic: %s", subTokenTopicName);
    rc = aws_iot_mqtt_subscribe(client, subTokenTopicName, strlen(subTokenTopicName), qos, pApplicationTokenHandler, pApplicationTokenHandlerData);
    if(SUCCESS != rc) {
        IOT_ERROR("[MQTT]Error subscribing : %d ", rc);
        return;
    }

    //连接和订阅完成之后进入while循环，循环2sPublishchannel和bulb状态
    //awa mqtt reconnect or succes,than keep loop
    while(NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc) {

        // printf("[MQTT]checkChannel returned value: %d\n", checkChannel(awsChannel));

        if (checkChannel(awsChannel) != 1){
            IOT_INFO("[MQTT]MQTT channel is closed.\n");
            break;
        }

        // loop to publish to the topic sub
        sprintf(cPayload, "[MQTT]Light Status(On/Off): %d , Light Brightness(20-100): %d , Light Color: %d.", gLightBulbStatus, gLightBulbBrightness, gLightBulbColor);
        IoT_Publish_Message_Params paramsQOS0;
	    paramsQOS0.qos= QOS0;
	    paramsQOS0.payload = (void *) cPayload;
        paramsQOS0.payloadLen = strlen(cPayload);
	    paramsQOS0.isRetained = 0;
        // device publish topic 
        printf("[MQTT]Light Status(On/Off): %d , Light Brightness(20-100): %d , Light Color: %d. Publish to %s.\n", gLightBulbStatus, gLightBulbBrightness, gLightBulbColor, pubStatusTopicName);
        rc = aws_iot_mqtt_publish(client, pubStatusTopicName, strlen(pubStatusTopicName), &paramsQOS0);


        //***************dmc status upload********************
        // loop to publish to the topic pubDmcTopicName
        char dmcPubPayload[30];
        //"amazonMQTT" "Zigbee" "Speaker" //checkChannel return 1 = enabel, 0 = diable, 2 = other error
        int amazonMQTTStatus = checkChannel("amazonMQTT");
        int ZigbeeStatus = checkChannel("Zigbee");
        int SpeakerStatus = checkChannel("Speaker");
        int LocalStatus = checkChannel("Local");

        sprintf(dmcPubPayload, "[MQTT]Dmc Status:%d%d%d%d.", amazonMQTTStatus, ZigbeeStatus, SpeakerStatus, LocalStatus);
        IoT_Publish_Message_Params paramsDmcPubQOS0;
	    paramsDmcPubQOS0.qos= QOS0;
	    paramsDmcPubQOS0.payload = (void *) dmcPubPayload;
        paramsDmcPubQOS0.payloadLen = strlen(dmcPubPayload);
	    paramsDmcPubQOS0.isRetained = 0;

        printf("[MQTT]Dmc Status:%d%d%d%d.\n", amazonMQTTStatus, ZigbeeStatus, SpeakerStatus);
        rc = aws_iot_mqtt_publish(client, pubDmcTopicName, strlen(pubDmcTopicName), &paramsDmcPubQOS0);
        
        //Max time the yield function will wait for read messages
        rc = aws_iot_mqtt_yield(client, 100);
        
        if(NETWORK_ATTEMPTING_RECONNECT == rc) {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
        }
        // 2 seconds loop
        sleep(2);

    }
    
    // Wait for all the messages to be received
    aws_iot_mqtt_yield(client, 100);

    if(SUCCESS != rc) {
        IOT_ERROR("An error occurred in the loop.\n");
    } else {
        IOT_INFO("LightBulb amazonMQTT channel closed.\n");
    }

    return;
}


