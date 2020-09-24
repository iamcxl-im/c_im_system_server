# c语言聊天服务器

### 数据源
+ mysql
+ redis

### 所用框架
+ libmysqlclient
+ hiredis
+ cJSON

### 通信协议
使用 json
```c
typedef struct Mess
{
    char user[16];
    char sendObject[16];//发送对象
    char news[128];//消息
    char date[32];
    int function;//指令 1为消息 2为登陆 3为注册 4为退出 5服务器允许登陆 默认为0
} Mess;
```

### 使用方式
已经打包成docker镜像[地址](https://hub.docker.com/repository/docker/lilqcn/imsystem)
```shell
docker run -d -p 6666:6666 lilqcn/imsystem:v1
```