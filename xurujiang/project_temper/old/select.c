#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <errno.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <fcntl.h>  
  
#define SERVER_HOST "127.0.0.1"  
#define SERVER_PORT 8080  
#define BUFFER_SIZE 1024  
#define RETRY_INTERVAL 5  // 重连间隔（秒）  
  
void error(const char *msg) {  
    perror(msg);  
    exit(1);  
}  
  
int main() {  
    int sockfd, newsockfd;  
    struct sockaddr_in serv_addr;  
    char buffer[BUFFER_SIZE] = {0};  
    fd_set readfds;  
  
    // 创建socket  
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)   
        error("ERROR opening socket");  
  
    // 设置服务器地址信息  
    memset((char *) &serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_HOST);  
    serv_addr.sin_port = htons(SERVER_PORT);  
  
    // 连接到服务器  
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)   
        error("ERROR connecting");  
  
    printf("Connected to server...\n");  
  
    // 主循环，持续尝试读取数据或重连  
    while (1) {  
        FD_ZERO(&readfds);  
        FD_SET(sockfd, &readfds);  
  
        // 使用select检测socket是否可读（即是否连接）  
        if (select(sockfd + 1, &readfds, NULL, NULL, NULL) == -1) {  
            perror("select()");  
            continue;  
        }  
  
        // 如果socket不可读，说明连接已断开，尝试重连  
        if (!FD_ISSET(sockfd, &readfds)) {  
            printf("Connection lost, retrying...\n");  
            close(sockfd);  
            sleep(RETRY_INTERVAL);  
  
            // 尝试重新连接  
            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
                error("ERROR opening socket");  
  
            if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {  
                perror("ERROR connecting");  
                continue;  
            }  
  
            printf("Reconnected to server...\n");  
            continue;  
        }  
  
       //  如果socket可读，读取数据（此处省略发送和接收数据的代码）  
         newsockfd = accept(sockfd, (struct sockaddr *) NULL, NULL);  
         read(newsockfd, buffer, BUFFER_SIZE);  
         write(newsockfd, "I got your message", 18);  
        close(newsockfd);  
  
        // 在实际应用中，你可能需要处理接收到的数据  
        // 这里只是演示如何检测连接状态  
    }  
  
    // 关闭socket  
    close(sockfd);  
    return 0;  
}
