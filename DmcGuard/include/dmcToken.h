/***************************************************************
 * @file       dmcToken.h
 * @brief      include\dmcToken.h \\ DMCs' tokens store and auth func 
 **************************************************************/

#ifndef DMC_TOKEN_H
#define DMC_TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dmcControl.h"
#include "deviceControl.h"


// move to deviceControl.h
// //status , brightness; write , read
// #define RIGHT_NUM 4
// //aws zigbee speaker
// #define CHANNEL_TOKEN_NUM 3


//DMC Token format
#define dmcTokenLength 32
//The fixed token of the local channel
const static localToken[32] = "LocalModeChannel_2023_03_17_0000"
//base64 token
//#define base64DmcTokenLength ( (dmcTokenLength %3) ? (dmcTokenLength / 3 + 1) * 4 : (dmcTokenLength / 3 * 4) )

//For convenience, char[32] is preset to '0'
typedef char typeDmcToken[dmcTokenLength + 1];
//typedef char typeBase64DmcToken[dmcTokenLength];

// dmc token node list
typedef struct dmcTokenNode
{
    typeDmcToken dmcToken;
    struct dmcTokenNode *next;
}dmcTokenNode;



// @brief     init dmc control matrix.   
int initDmcACM();

// @brief     insert new token to DMCuard ACM stored in rasp
int insertDmcToken(typeDmcToken token, short channel, short right);
// @brief     verify token in DMCguard ACM stored in rasp
int verifyDmcToken(typeDmcToken token, short channel, short right);
// @brief     delete token in DMCguard ACM stored in rasp
int deleteDmcToken(typeDmcToken token, short channel, short right);

// @brief     update token to DMCuard ACM stored in rasp
int updateDmcToken(typeDmcToken token, short channel1, short right1, short channel2, short right2);


// 下面几个链表后续应该用不上了

// // aws channel token list node
// //aws channel token list node
// typedef struct awsTokenNode
// {
//     typeDmcToken aToken;
//     struct awsTokenNode *next;
// }awsTokenNode;

// //zigbee channel token list node
// typedef struct zigbeeTokenNode
// {
//     typeDmcToken zToken;
//     struct zigbeeTokenNode *next;
// };zigbeeTokenNode

// //smart speaker channel token list node
// typedef struct speakerTokenNode
// {
//     typeDmcToken zToken;
//     struct speakerTokenNode *next;
// };speakerTokenNode



#endif 
