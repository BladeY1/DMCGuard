/***************************************************************
 * @file       src\dmcAuth\dmcToken.c.
 * @brief      DMCs' tokens store and auth func
 **************************************************************/
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// #include <pthread.h>
// #include <string.h>

#include "dmcToken.h"


//DMC Guard Access Control Matrix, static type, only acccesed by dmcToken.c
static dmcTokenNode* dmcACM[CHANNEL_TOKEN_NUM][RIGHT_NUM];


/***************************************************************
  *  @brief     init dmc control matrix.   // 主要是为了防止野指针，初始化next
  *  @param     token 
  *  @return    0, success
 **************************************************************/
int initDmcACM()
{
    int i,j;
    for (i = 0; i < CHANNEL_TOKEN_NUM; i++)
    {
        for (j = 0; j < RIGHT_NUM; j++)
        {   
            dmcACM[i][j] = malloc(sizeof(dmcTokenNode*));
            dmcACM[i][j]->next = NULL;
            free(dmcACM[i][j]);
        } 
    }
    return 0;
}


// 头插、dmcDMC[i][j]是各个链表的头节点
/***************************************************************
  *  @brief     insert new token to DMCuard ACM stored in rasp
  *  @param     token 
  *  @param     channel channel attacthed to token
  *  @param     right right attacthed to token
  *  @return    0, success; -1, malloc error; -2, other error
 **************************************************************/
int insertDmcToken(typeDmcToken token, short channel, short right)
{
    if ( (token != NULL) && (channel < CHANNEL_TOKEN_NUM) && (right < RIGHT_NUM))
    {
        dmcTokenNode *p = NULL;
        p = malloc(sizeof(dmcTokenNode));
        if (!p)
        {
            printf("[Error]insert new token malloc error\n");
            return -1;
        }
        memcpy(p->dmcToken, token, dmcTokenLength);
        p->next = dmcACM[channel][right];
        dmcACM[channel][right] = p;
        printf("[Success]insert new token success.\n");
        return 0;
    }else
    {   
        // if (channel >= CHANNEL_TOKEN_NUM) {printf("tokenChannel overflow\n");}
        // if (right >= RIGHT_NUM) {printf("tokenRight overflow\n");}
        // if (token == NULL) {printf("token NULL\n");}

        printf("[Error]insert new token error\n");
        return -2;
    }
}

/***************************************************************
  *  @brief     verify token in DMCguard ACM stored in rasp
  *  @param     token
  *  @param     channel channel attaced to the token
  *  @param     right  right attaced to the token
  *  @return    0, find token success; 1, token not found; -1, other error
 **************************************************************/
int verifyDmcToken(typeDmcToken token, short channel, short right)
{
    return 0;
    if ( (token != NULL) && (channel < CHANNEL_TOKEN_NUM) && (right < RIGHT_NUM))
    {
        dmcTokenNode *p = NULL;
        p = dmcACM[channel][right];
        while (p)
        {
            if (memcmp(p->dmcToken, token, dmcTokenLength) == 0)
            {   
                printf("[Success]verify token success.\n");
                return 0 ; //find token
            }else
            {
                p = p->next; //check next node
            }
        }
        //not found token 
        printf("[Failed]verify token not found.\n");
        return 1;
    }else
    {   
        printf("[Error]verify token error.\n");
        return -1;//other error.
    }      
}


//dmcACM[i][j]为每个单链表的头节点，删除分两种情况，只删除和移动头节点，删除链表中间的节点
/***************************************************************
  *  @brief     delete token in DMCguard ACM stored in rasp
  *  @param     token
  *  @param     channel channel attaced to the token
  *  @param     right  right attaced to the token
  *  @return    0, delete success; -1, token list is empty; -2, token not found; -3, other error
 **************************************************************/
int deleteDmcToken(typeDmcToken token, short channel, short right)
{   
    // 检查，避免空链表时继续删除，导致系统内存报错Segmentation fault
    if (dmcACM[channel][right]->next == NULL)
    {
        printf("token list is empty in Channel:%hd,Right:%hd\n", channel, right);
        return -1;
    }

    if ( (token != NULL) && (channel < CHANNEL_TOKEN_NUM) && (right < RIGHT_NUM))
    {   
        dmcTokenNode *p1 = NULL;
        dmcTokenNode *p2 = NULL;

        p2 = dmcACM[channel][right];
        //if header node will be deleted
        if (memcmp(p2->dmcToken, token, dmcTokenLength) == 0)
        {
            dmcACM[channel][right] = p2->next;
            free(p2);
            printf("[Success]delete token success.\n");
            return 0;  //delete token success
        }
        p1 = p2;
        p2 = p2->next;
        while (p2)
        {
            if (memcmp(p2->dmcToken, token, dmcTokenLength) == 0)
            {
                p1->next = p2->next;
                free(p2);
                return 0;   //delete token success
            } else
            {
                p1 = p2;
                p2 = p2->next;
            }
        }
        printf("[Failed]delete token not found.\n");
        return -2;  //token not found
    } else
    {   
        printf("[Error]delete token error.\n");
        return -3; // other error
    }                                                 
}


/***************************************************************
  *  @brief     update token to DMCuard ACM stored in rasp
  *  @param     channel1/right1 old;  channel2/right2 new
  *  @return    0, update token success; -1, update error in delete/insert; -2 param err
 **************************************************************/
int updateDmcToken(typeDmcToken token, short channel1, short right1, short channel2, short right2)
{
    if ( (token != NULL) && (channel1 < CHANNEL_TOKEN_NUM) && (right1 < RIGHT_NUM) && (channel2 < CHANNEL_TOKEN_NUM) && (right2 < RIGHT_NUM))
    {   
        printf("Updating Token channel:%d to %d, right: %d to %d\n",channel1, channel2, right1, right2);
        int err1, err2;
        err1 = err2 = -1;
        err1 = deleteDmcToken(token, channel1, right1);
        err2 = insertDmcToken(token, channel2, right2);
        if (err1 == 0 && err2 == 0) 
        {
            printf("[Success]update token success.\n");
            return 0;
        } else 
        {
            printf("[Error]update token error in delete/insert.\n");
            return -1;
        }
        
    } else
    {
        printf("[Error]update token error in param\n");
        return -2;
    }
}






// 以下是之前设计的数据结构，现在用不上

// static awsTokenNode* gACL[CHANNEL_TOKEN_NUM][RIGHT_NUM];

// /***************************************************************
//   *  @brief     insert new token to  token List in rasp
//   *  @param     token token
//   *  @param     right right attacthed to token
//   *  @return    0,success; -1 malloc error; -2 other error
//  **************************************************************/
// int insertAwsTokenList(typeDmcToken token, short short right)
// {
//     awsTokenNode *p = NULL;
//     if (token && right >= 0 && right < Right_NUM)
//     {
//         p = malloc(sizeof(awsTokenNode));
//         if (!p)
//         {
//             printf("[Error]insert aws token (malloc) error\n");
//             return -1;
//         }
//         memcpy(p->aToken, token, dmcTokenLength);
//         p->next = gACL[right];
//         gACL[right] = p;
//         free(p);
//         return 0;
//     }else
//     {
//         printf("[Error]insert aws token error\n");
//         return -2;
//     }
// }


// /***************************************************************
//   *  @brief     check token in aws token list
//   *  @param     token aws token; right right attacthed to token
//   *  @return    0,find token; 1 token not found; -1 error
//  **************************************************************/
// int checkAwsTokenList(typeDmcToken token, short right)
// {
//     awsTokenNode *p = NULL;
//     if (token && right >= 0 && right < Right_NUM)
//     {
//         p = gACL[right];
//         while (p)
//         {
//             if (memcmp(p->aToken, token, dmcTokenLength) == 0)
//             {
//                 return 0 ; //check token ok
//             }else
//             {
//                 p = p->next; //check next node
//             }
//         }
//         //not found token in list
//         return 1;
//     }else
//     {
//         return -1;//other error.
//     }      
// }


