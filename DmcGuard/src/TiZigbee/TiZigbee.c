/***************************************************************
 * @file       zigbee.h
 * @brief      include\zigbee.h \\ control with Zigbee channel
 **************************************************************/

// #include <wiringSerial.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>

#include "TiZigbee.h"

int msgHandle(char *msg, int length, int *certify_flag)
{
    printf("[Zigbee] MSG RECE %s length: %d\n", msg, length);

    if (length < dmcTokenLength + 2)
    {
        if (length == 4)
        {

            for (int i = 0; i < 4; i++)
            {
                if (msg[i] != DEVICEKEY[i])
                {
                    *certify_flag = 0;
                    return -5;
                }
            }
            *certify_flag = 1;
            printf("[Zigbee] DEVICE KEY CORRECT\n");
            /*
            *certify_flag = memcmp(msg, DEVICEKEY, 4)? 1 : 0;

            if (*certify_flag)
                printf("[Zigbee] Certify success\n");
            */
            return -5;
        }
        else if (msg[0] == 'c')
        {
            *certify_flag = 0;
        }
        else if (msg[0] == 'n')
            return -5;
        return -5;
    }

    // get token
    // Distinguish tokens, R and O, passed to the opReqHandle function
    typeDmcToken token;
    memcpy(token, msg, dmcTokenLength);
    token[dmcTokenLength] = '\0';

    char R_, O_, P_ = 0;
    short R, O, param = 0;
    memcpy(&R_, msg + dmcTokenLength, 1);
    memcpy(&O_, msg + dmcTokenLength + 1, 1);
    memcpy(&P_, msg + dmcTokenLength + 2, 1);

    // #ifdef DEBUG_3

    R = (short)R_ - 48;
    O = (short)O_ - 48;
    param = (short)P_ - 48;

    printf("token: %s\n", token);
    printf("R: %hd\n", R);
    printf("O: %hd\n", O);
    printf("param: %hd\n", param);


    // #endif

    int status = opReqHandle(token, 1, R, param);
    *certify_flag = 1;
    return status;
}

void msgGet(char *buf, int *length, int fd)
{
    char buf_char;
    char record_flag = 0;
    *length = 0;

    while (1)
    {
        buf_char = serialGetchar(fd);
        if (buf_char == '\n')
        {
            buf[*length] = '\0';
            break;
        }
        else if (buf_char == '$')
        {
            record_flag = 1;
            continue;
        }
        else if (record_flag == 1)
        {
            buf[*length] = buf_char;
            (*length)++;
        }
    }
}

void runZigbee(void)
{
    int fd;
    int baud = 9600;
    const char *serialPort = "/dev/ttyUSB0";

    if ((fd = serialOpen(serialPort, baud)) < 0)
    {
        printf("[Zigbee] Unable to open serial device\n");
        return;
    }

    if (wiringPiSetup() == -1)
    {
        printf("[Zigbee] Unable to start wiringPi\n");
        return;
    }

    serialFlush(fd);
    // store data received from serial port
    // if size > 50, flush the buffer
    char buf[50];

    int buf_length = 0;
    int certify_flag = 0;
    int status = -5;

    while (1)
    {
        // add check channel
        // if returns 0 or 2, this channel will be stoped.
        if (checkChannel("Zigbee") != 1)
        {
            printf("[Zigbee] Zigbee channel closed!\n");
            serialClose(fd);
            return;
        }

        if (!certify_flag)
        {
            memset(buf, 0, sizeof(buf));
            memcpy(buf, "$", 2);
            printf("[Zigbee] CERTIFY FAILED\n");
        }
        else
        {
            memset(buf, 0, sizeof(buf));

            switch (status)
            {
            case -5:
                strcpy(buf, "err\n");
                break;
            case -1:
                strcpy(buf, "ntf\n");
                break;
            case -2:
                strcpy(buf, "err\n");
                break;
            case -3:
                strcpy(buf, "ntf\n");
                break;
            case -4:
                strcpy(buf, "err\n");
                break;
            case 0:
                strcpy(buf, "suc\n");
                break;
            case 1:
                strcpy(buf, "lon\n");
                break;
            case 2:
                strcpy(buf, "lof\n");
                break;
            case 4:
                strcpy(buf, "red\n");
                break;
            case 5:
                strcpy(buf, "yellow\n");
                break;
            case 6:
                strcpy(buf, "green\n");
                break;
            default:
                strcpy(buf, "0\n");
                buf[0] = (char)status;
                break;
            }
        }

        printf("[Zigbee] DATA SEND: %s\n", buf);
        //sleep(1);

        serialPuts(fd, buf);
        // serialPuts(fd, "\n");

        msgGet(buf, &buf_length, fd);

        printf("[Zigbee] Buf Content: %s\n", buf);

        status = msgHandle(buf, buf_length, &certify_flag);
    }

    serialClose(fd);
    return;
}
