/***************************************************************
 * @file       src\dmcControl\dmcControl.c
 * @brief      Dmc operations woth token management and control
 **************************************************************/
#include "dmcControl.h"

/***************************************************************
 * @brief     operation request handle funtion
 * @param     token   token in request
 * @param     channel channel in request
 * @param     right   right in request
 * @param     param   operation params 
 * 
 * @return    0 success(write opertion success return), -1 right in req not found(out right range),-2 = other error 
 * @return    RIGHT_STATUS_READ: 1=on 2=off ;RIGHT_STATUS_WRITE: 0=success;
 * @return    RIGHT_BRIGHTNESS_READ: Ret=readDeviceBrightness() + 10; RIGHT_BRIGHTNESS_WRITE: 0=success
 * @return    RIGHT_COLOR_READ: 4=red 5=yellow 6=green; RIGHT_COLOR_WRITE: 0=success
 * @return    -3 = token not found in gACM;  -4 = other error in opReqHandle
 **************************************************************/
    /*****************************************
    operation request payload format:
    channel,right,param,token
    e.g.:   
    0,1,1,abcdefghijklmnopqrstuvwxyz123456
    means set light status to on
    **********************************************/
int opReqHandle(typeDmcToken token, short channel, short right, short param)
{   
    int verifyRet;
    printf("[OpReq]receive operation request.from channel %hd,in right %hd,with param %hd, with token: %s\n",channel, right, param, token);
    if(channel == 4) // local channel
    {
        if(strcmp(token, localToken) == 0)
        {
            verifyRet = 0;
        }
        else
        {
            verifyRet = -1;
        }
    }
    else //other channel
    {
        verifyRet = verifyDmcToken(token, channel, right);
    }
    int opReqHandleRet = -3; //Func opReqHandle default Ret is -3
    //verify token success
    if (verifyRet == 0)
    {   
        printf("[OpReq]verify Success, execute device control in channel %hd,in right %hd,with param %hd\n",channel, right, param);
        //executeOperate(channel, right, param);
        opReqHandleRet = executeOperate(channel, right, param);
        return opReqHandleRet;
    //not found token
    }else if (verifyRet == 1)
    {   
        printf("[OpReq]operation request verify Not Found.\n");
        return -3;
    }else
    {   
        printf("[OpReq]operation request verify other Error.\n");
        return -4;
    }    
}




// 注意tokenReq只由aws通道实现,其他通道实现opReqHandle即可
// aws通道的的功能写到了 dmcMqtt的iot_token_subscribe_callback_handler里,可能需要转移出来,因为更新token都是主通道进行的，所以其实也可以留在dmcMQTT里
/***************************************************************
 * @brief     token management request handle funtion
 * @param     token   token in request
 * @param     channel channel in request
 * @param     right   right in request
 * @param     param   token params , char ,'i' and 'd'
 * @return    0, token add success; 1, token delete success; -1 add error;-2 delete erroe;-3,wrong param;
 **************************************************************/
    /*****************************************
     * 注意tokenReq只由aws通道实现,其他通道实现opReqHandle即可
    token request payload format:
    param,channel,right,token
    e.g.:   
    a,1,1,abcdefghijklmnopqrstuvwxyz123456
    means add a token in channel zigbee in right 1
    **********************************************/
int tokenReqHandle(typeDmcToken token, short channel, short right, char param)
{
    printf("[TokenReq]receive token management request.from channel %d,in right %d,with param %c\n",channel, right, param);
    int ret = 0;
    if (param == 'a')
    {
        ret = insertDmcToken(token, channel, right);
        if (ret == 0) 
        {
            printf("[TokenReq]token request add Success.\n");
            return 0;
        } else
        {   
            printf("[TokenReq]token request add Failed.\n");
            return -1;
        }
    }else if (param == 'd')
    {
        ret = deleteDmcToken(token, channel, right);
        if (ret == 0) 
        {   
            printf("[TokenReq]token request delete Success.\n");
            return 1;
        } else
        {   
            printf("[TokenReq]token request delete Failed.\n");
            return -2;
        }
    }else
    {   
        printf("[TokenReq]token request, wrong param.\n");
        return -3;
    }  
}

