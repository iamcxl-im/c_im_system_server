//
// Created by daquan on 9/23/20.
//
#include "redis_mq.h"
#include "hiredis/hiredis.h"
#include <string.h>

/**
 * 增加队列
 * @param id 用户id
 * @param mq 该用户id的消息
 * @return 增加成功返回0 否则失败返回1
 */
int insert_mq(char* id,char* mq){
    redisContext *con= redisConnect("lilq.cn",6379);
    if (con->err){
        redisFree(con);
        printf("con error\n");
        return 1;
    }
    //printf("success\n");
    //执行语句
    redisReply *reply = (redisReply *)redisCommand(con, "rpush %s %s",id,mq);
    //printf("exect....\n");
    if (reply==NULL){
        printf("com execute error1\n");
        redisFree(con);
        return 1;
    }
    //执行失败
    if (REDIS_REPLY_ERROR==reply->type){
        printf("com execute error2\n");
        freeReplyObject(reply);
        redisFree(con);
        return 1;
    }
    //执行成功
    freeReplyObject(reply);
    redisFree(con);
    //printf("command execute success\n");
    return 0;
}

/**
 * 获得队列 获得消息
 * @param id 用户id
 * @return 有消息返回一条消息 没有消息或者查询失败返回""
 */
char* get_mq(char* id){
    redisContext *con= redisConnect("lilq.cn",6379);
    if (con->err){
        redisFree(con);
        printf("con error\n");
        return "";
    }
    //执行语句
    redisReply *reply = (redisReply *)redisCommand(con, "lpop %s",id);
    if (reply==NULL){
        printf("获得消息 com execute error1\n");
        redisFree(con);
        return "";
    }
    if (REDIS_REPLY_ERROR==reply->type || REDIS_REPLY_NIL==reply->type){
        printf("获得消息 com execute error2\n");
        freeReplyObject(reply);
        redisFree(con);
        return "";
    }
    printf("获得消息\n");
    return reply->str;
    //应该在返回前进行释放但是没有得到好的解决方案
}