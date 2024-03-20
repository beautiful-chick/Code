/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  fork_temperature.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(08/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "08/03/24 19:03:49"
 *                 
 ********************************************************************************/

#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/select.h>  
#include <sys/wait.h>  
  
int main() {  
    int fd[2]; // 定义管道文件描述符数组  
    pid_t pid; // 定义进程ID变量  
    int status; // 用于接收waitpid的返回值  
    fd_set readfds; // 定义文件描述符集合  
    struct timeval timeout; // 定义超时时间  
    char buffer[256]; // 定义缓冲区  
    ssize_t bytesRead; // 用于记录read函数的返回值  
  
    // 创建管道  
    if (pipe(fd) == -1) {  
        perror("pipe");  
        exit(EXIT_FAILURE);  
    }  
  
    // 创建子进程  
    pid = fork();  
    if (pid == -1) {  
        perror("fork");  
        exit(EXIT_FAILURE);  
    }  
  
    // 子进程  
    if (pid == 0) {  
        close(fd[0]); // 关闭管道的读端  
        sleep(1); // 假设子进程需要一些时间准备数据  
        write(fd[1], "Hello from child process!", 21);  
        close(fd[1]); // 关闭写端  
        exit(EXIT_SUCCESS);  
    }  
  
    // 父进程  
    else {  
        close(fd[1]); // 关闭管道的写端  
  
        FD_ZERO(&readfds); // 初始化文件描述符集合  
        FD_SET(fd[0], &readfds); // 将管道读端加入集合  
  
        // 设置超时时间，这里设置为5秒  
        timeout.tv_sec = 5;  
        timeout.tv_usec = 0;  
  
        // 使用select监听文件描述符集合的变化  
        int result = select(fd[0] + 1, &readfds, NULL, NULL, &timeout);  
        if (result == -1) {  
            perror("select");  
            exit(EXIT_FAILURE);  
        } else if (result == 0) {  
            printf("Timeout occurred. No data received.\n");  
        } else {  
            // 从管道读取数据  
            bytesRead = read(fd[0], buffer, sizeof(buffer) - 1);  
            if (bytesRead > 0) {  
                buffer[bytesRead] = '\0'; // 确保字符串以null结尾  
                printf("Received from child: %s\n", buffer);  
            } else {  
                perror("read");  
            }  
        }  
  
        close(fd[0]); // 关闭读端  
  
        // 等待子进程结束  
        waitpid(pid, &status, 0);  
        if (WIFEXITED(status)) {  
            printf("Child exited with status %d\n", WEXITSTATUS(status));  
        }  
    }  
  
    return 0;  
}
