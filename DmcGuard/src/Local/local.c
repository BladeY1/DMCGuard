/***************************************************************
 * @file       Local.h
 * @brief      include\Local.h \\ control with Local channel
 **************************************************************/
#include "Local.h"

void runLocal(void)
{
    time_t now = time(NULL);
    printf("get into Local channel thread\n");

    int client_fd;
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    int addrlen = sizeof(server_addr);

   // 初始化套接字地址结构
    memset((void*)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SER_PORT);

    size_t i = 0;
    // 遍历通道列表
    for (i = 0; i < CHANNEL_LIST_LENGTH; i++) {
        if (strncmp(CHANNEL_LIST[i].channel_name, "Local", strlen("Local")) == 0) {
            // 创建一个TCP套接字
            if ((CHANNEL_LIST[i].channel_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }
            printf("created socket, channel_fd: %d\n", CHANNEL_LIST[i].channel_fd);

            // 设置套接字选项
            if (setsockopt(CHANNEL_LIST[i].channel_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
                perror("setsockopt failed");
                exit(EXIT_FAILURE);
            }

            // 绑定套接字到指定的IP地址和端口号
            if (bind(CHANNEL_LIST[i].channel_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }

            // 监听客户端连接
            if (listen(CHANNEL_LIST[i].channel_fd, MAX_CLIENTS) < 0) {
                perror("listen failed");
                exit(EXIT_FAILURE);
            }

            printf("Server listening on port %d\n", SER_PORT);
            printf("waiting for connect ......\n");

            break;  // 找到本地通道后退出循环
        }
    }

    // 接受客户端连接并创建线程处理
    while (1) {
        if ((client_fd = accept(CHANNEL_LIST[i].channel_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        printf("New client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // 检查通道是否已启用
        if (checkChannel("Local") == 1) {
            pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, handle_client, (void *)&client_fd) < 0) {
                perror("pthread_create failed");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Local channel is disabled. Ignoring client connection.\n");
            close(client_fd);
        }
    }
    
}


void *handle_client(void *arg) {
    time_t now = time(NULL);
    short R, param;
    typeDmcToken token;
    char buffer[MAXLINE] = {0};
    char bufRetn[MAXLINE];
    char status_msg[10];

    int client_fd = *(int *)arg;
    int bytes_read;

    while ((bytes_read = read(client_fd, buffer, MAXLINE)) > 0) {
        printf("Received %d bytes: %s\n", bytes_read, buffer);
        //test time
        printTimeWithMilliseconds2();
        //memset(buffer, 0, MAXLINE);
        R = buffer[0] - '0';
        if (R == -48) {
            break;
        }

        
        for (int i = 0; i <= dmcTokenLength; i++)
            token[i] = '\0';

        param = buffer[1] - '0';
        int i = 0;
        for (i = 0; i < 32; i++) {
            token[i] = buffer[2 + i];
        }

        int status;

        printf("%s", buffer);
        // local channel 4
        // RIGHT_COLOR_READ: 4=red 5=yellow 6=green; 可以使用简称 "lrd"（Light ReD）  "lyl"（Light YeLlow） "lgn"（Light GreeN）
        status = opReqHandle(token, 4, R, param);
        //test time
        printTimeWithMilliseconds2();   
        printf("operate status: %d\n", status);
        R = 0;
        param = 0;

        for (int ii = 0; ii < dmcTokenLength; ii++)
            token[ii] = '\0';
        memset(bufRetn, 0, sizeof(bufRetn));
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
        case 4:
            strcpy(bufRetn, "lrd\n");
            break;
        case 5:
            strcpy(bufRetn, "lyl\n");
            break;
        case 6:
            strcpy(bufRetn, "lgn\n");
            break;
        case 10:
        case 30:
        case 50:
        case 70:
        case 90:
        case 110:
            sprintf(bufRetn, "%d\n", status - 10);
            break;
        default:
            strcpy(bufRetn, "0\n");
            bufRetn[0] = (char)status;
            break;
        }

        printf("Response: %s", bufRetn);
        //test time
        printTimeWithMilliseconds2();
        send(client_fd, bufRetn, strlen(bufRetn), 0);

        memset(bufRetn, 0, MAXLINE);
        //sleep(1);
    }

    if (bytes_read == 0) {
        printf("Client disconnected\n");
    } else {
        perror("read failed");
    }

    // 关闭客户端套接字
    close(client_fd);

    return NULL;
}

