/***************************************************************
 * @file       Speaker.h
 * @brief      include\Speaker.h \\ control with Speaker channel
 **************************************************************/
#include "Speaker.h"

void runSpeaker(void)
{
    // char buf[32];
    printf("get into Speaker channel thread\n");
    short R, param;
    typeDmcToken token;
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    char bufRetn[MAXLINE];
    char status_msg[10];

    int sockfd;
    printf("connecting socket \n");
    /*sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.31.54", &servaddr.sin_addr);
    servaddr.sin_port = htons(SER_PORT);*/

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&servaddr, '0', sizeof(servaddr));
    /*The IP of the hub of the Speaker*/
    servaddr.sin_addr.s_addr = inet_addr(SPEAKER_HUB_IP);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SPEAKER_HUB_PORT);


    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error");
    } 
    printf("socket connected\n");
    while (1)
    {
        for (int i = 0; i <= dmcTokenLength; i++)
            token[i] = '\0';
        // speakerSocketBuf(token, &R, &param);
        memset(buf, 0, MAXLINE);
        read(sockfd, buf, MAXLINE); 
        printf("\n");
        //************************
        // close(sockfd);
        R = buf[0] - '0';
        param = buf[1] - '0';
        int i = 0;
        for (i = 0; i < 32; i++)
        {
            token[i] = buf[2 + i];
        }

        int status;

        // if (*R != 0)
        if (R == -48)
        {
            break;
        }
        printf("%s",buf);
        status = opReqHandle(token, 2, R, param);
        printf("operate status:%d\n", status);
        R = 0;
        param = 0;

        for (int ii = 0; ii < dmcTokenLength; ii++)
            token[ii] = '\0';
        memset(bufRetn, 0, sizeof(buf));
        switch (status)
        {
        case -1:
            strcpy(bufRetn, "ntf\n");
            break;
        case -2:
            strcpy(bufRetn, "err\n");
            break;
        case -3:
            strcpy(bufRetn, "ntf\n");
            break;
        case -4:
            strcpy(bufRetn, "err\n");
            break;
        case 0:
            strcpy(bufRetn, "suc\n");
            break;
        case 1:
            strcpy(bufRetn, "lon\n");
            break;
        case 2:
            strcpy(bufRetn, "lof\n");
            break;
        default:
            strcpy(bufRetn, "0\n");
            bufRetn[0] = (char)status;
            break;
        }
        // memcpy(bufRetn, status_msg, 4);
        // sprintf(status_msg, "%d", status);
        printf("%s",bufRetn);
        send(sockfd, bufRetn, 4, 0);
        memset(bufRetn, 0, MAXLINE);
        // close(sockfd);
        sleep(5);
    }

    
}