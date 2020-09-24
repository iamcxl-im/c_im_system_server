//
// Created by 全 on 2020/9/21.
//
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "message.h"
#include "mysql_person.h"
#include "redis_mq.h"

#define PORT 6666 //服务器端口
#define ARRAY 10  //队列数
#define BUFFLEN 128 //读取消息长度
#define ADDR "0.0.0.0"

typedef struct thread_arg
{
    int *client_fd;
    char *id;
} thread_arg;

/**
 * 单个线程子线程 专门用于写输出
 * @param argv
 */
static void handle_writer(void *argv){
    thread_arg threadArg = *((thread_arg*)argv);//客户端文件描述符
    char buffer[BUFFLEN]={0};//xie数据 数组
    int n=0;//xie取长度
    while (1){
        //写入长度不对 即为退出
        pthread_testcancel();
        memset(buffer,0,BUFFLEN);
        strcpy(buffer,get_mq(threadArg.id));
        if (strlen(buffer)==0){
            sleep(1);
            continue;
        }
        n = write(*threadArg.client_fd,buffer, sizeof(buffer));
        write(*threadArg.client_fd,"\n",strlen("\n"));
        printf("send mess\n");
        if (n==0||n==-1){
            break;
        }
    }
    printf("退出了!!!!\n");
}

/**
 * 单个线程
 * @param argv
 */
static void handle_request(void *argv){
    int client_fd = *((int*)argv);//客户端文件描述符
    signal(SIGPIPE,SIG_IGN);
    char buffer[BUFFLEN]={0};//接收数据 数组
    int n=0;//读取长度
    pthread_t pthread;//写线程
    /**
     * 1.登陆或则注册 密码错误直接退出
     * 2.读写线程 各一个
     * 3.写线程为子线程
     * 4.读线程为父线程
     * 5.读取推出指令时 退出
     */
    /*接收和发送数据*/
    memset(buffer,0,BUFFLEN);//归0
    recv(client_fd,buffer,BUFFLEN,0);//读取登陆数据
    printf("%s\n",buffer);
    Mess mess = Mess_getMess(buffer);
    Mess_print(mess);
    if (mess.function==2){
        //登陆
        //需要判断密码
        int m = login_per(mess.user,mess.news,0);
        if (m==1){
            //登陆失败
            close(client_fd);
            printf("登陆失败,结束连接\n");
            return;
        }
    } else if(mess.function==3){
        //注册
        int m = register_per(mess.user,mess.news,0);
        if (m==1){
            //注册失败
            close(client_fd);
            printf("注册失败,结束连接\n");
            return;
        }
    } else{
        //退出登陆
        close(client_fd);
        printf("结束连接\n");
        return;
    }
    /**
     * 允许登陆
     * 发送登陆指令
     */
    Mess mess1 = {.function=5};
    memset(buffer,0,BUFFLEN);//归0
    strcpy(buffer,Mess_toJSON(&mess1));
    write(client_fd,buffer,strlen(buffer));
    write(client_fd,"\n",strlen("\n"));
    /**
     * 创建写线程
     */
    thread_arg threadArg={.id=mess.user,.client_fd=&client_fd,};
    pthread_create(&pthread,NULL,(void*)handle_writer,&threadArg);
    while(1)
    {
        memset(buffer,0,BUFFLEN);
        n = recv(client_fd,buffer,BUFFLEN,0);
        if (n==0){
            printf("客户端断开连接错误退出\n");
            break;
        }
        mess = Mess_getMess(buffer);
        if (mess.function==0){
            printf("错误退出\n");
            break;
        }
        if (mess.function==4){
            printf("正确退出");
            break;
        }
        if (mess.function==1){
            //提交消息队列
            int m = insert_mq(mess.sendObject,Mess_toJSON(&mess));
            if (m==1){
                //提交失败
                printf("消息队列提交失败\n");
                break;
            }
        }
    }
    //jie shu zhi xian cheng
    pthread_cancel(pthread);
    close(client_fd);
    printf("结束连接\n");
}

/**
 * 处理客户端连接函数（线程创建）
 * @param server_fd 服务器套接字描述符
 */
static void handle_connect(int server_fd){
    int client_fd;//客户端套接字描述符
    struct sockaddr_in client_addr;//客户端地址
    socklen_t len = sizeof(client_addr);
    pthread_t pthread;//线程
    /**
     * 主处理过程
     */
     while (1){
         /*
          * 接收客户端连接
          */
         client_fd = accept(server_fd,(struct sockaddr *)&client_addr, &len);
         printf("连接进入\n");
         if (client_fd>0){
             /**
              * 创建线程处理连接
              */
              pthread_create(&pthread,NULL,(void*)handle_request,&client_fd);
         }
     }
}


int main(){
    printf("hello world\n");
    int server_fd;//服务器套节字文件描述符
    struct sockaddr_in server_addr;//服务器地址
    /*建立流式套接字*/
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    //初始化地址和端口
    memset(&server_addr,0, sizeof(server_addr));
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(ADDR);
//    /*设定端口利用，避免端口冷却*/
    //int on = 1;
    //setsockopt(server_fd,SOL_SOCKET,SO_RANDOMPORT,&on, sizeof(on));
    /*绑定地址结构到套接字描述符*/
    bind(server_fd,(struct sockaddr *)&server_addr, sizeof(server_addr));
    /*设置侦听*/
    listen(server_fd,ARRAY);
//    /**
//     * 处理客户端连接
//     */
    printf("服务器启动\n");
    handle_connect(server_fd);
    //关闭服务器套接字
    close(server_fd);
    return 0;
}