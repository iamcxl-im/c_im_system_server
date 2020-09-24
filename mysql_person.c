//
// Created by daquan on 9/22/20.
//

#include "mysql_person.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>

/**
 * 用户注册
 * @param id 用户id
 * @param password 用户密码
 * @param type 用户类型 1为管理员 0为用户
 * @return 注册失败返回1 ， 注册成功返回0
 */
int register_per(char* id,char *password,int type){
    int res=1;
    MYSQL *con;
    con = mysql_init(NULL);
    if (!con){
        //连接失败
        fprintf(stderr,"mysql error\n");
        return EXIT_FAILURE;
    }
    //建立连接
    con = mysql_real_connect(con,"lilq.cn","root","quan","imSystem",3306,NULL,0);
    if (con){
        //字符串拼接
        char str[50]="insert into person values('";
        strcat(str,id);
        strcat(str,"','");
        strcat(str,password);
        if (type==0){
            strcat(str,"',0)");
        } else if (type==1){
            strcat(str,"',1)");
        } else{
            strcat(str,"',-1)");
        }
        // printf("%s\n",str);
        res = mysql_query(con,str);
        // printf("%d\n",res);
    } else{
        //连接失败
        printf("error\n");
    }
    //释放连接
    mysql_close(con);
    return res;
}

/**
 * 用户登陆
 * @param id 用户id
 * @param password 用户密码
 * @param type 用户类型 1为管理员 0为用户
 * @return 登陆成功返回0 登陆失败返回1
 */
int login_per(char* id,char *password,int type){
    int ress=1;//返回值

    MYSQL *con;
    con = mysql_init(NULL);

    if (!con){
        fprintf(stderr,"mysql error\n");
        return EXIT_FAILURE;
    }
    //建立连接
    con = mysql_real_connect(con,"lilq.cn","root","quan","imSystem",3306,NULL,0);
    if (con){
        //字符串拼接
        char str[50]="select * from person where id = '";
        strcat(str,id);
        strcat(str,"'");

        int res = mysql_query(con,str);
        if (res){
            printf("mysql_query error\n");
        } else{
            MYSQL_RES *mysqlRes = mysql_use_result(con);
            if (mysqlRes){
                MYSQL_ROW row;
                while ((row=mysql_fetch_row(mysqlRes))){
                    //printf("fetched data...\n");
                    // printf("%s\n",row[0]);//id
                    // printf("%s\n",row[1]);//password
                    // printf("%s\n",row[2]);//type
                    if (strcmp(row[0],id)==0 && strcmp(row[1],password)==0){
                        /**
                         * 管理员
                         */
                        if (type==1){
                            if (strcmp(row[2],"1")==0){
                                ress=0;
                            }
                        }
                        /**
                         * 用户
                         */
                        if (type==0){
                            if (strcmp(row[2],"0")==0){
                                ress=0;
                            }
                        }

                    }
                }
                if (mysql_errno(con)){
                    printf("error\n");
                }
            }
            mysql_free_result(mysqlRes);
        }
    } else{
        printf("con error\n");
    }
    //释放连接
    mysql_close(con);
    return ress;
}