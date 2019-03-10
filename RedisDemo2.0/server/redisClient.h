#ifndef __REDIS_CLIENT_H__
#define __REDIS_CLIENT_H__
#include <WINSOCK2.H>
#include <stdio.h>
#include "redisDb.h"
using namespace std;

typedef struct redisClient{
	SOCKET sClient;
	SOCKADDR_IN my_client;
	//记录客户端当前正在使用的数据库
	redisDb*db;
	//...
}redisClient;

//初始化客户端
redisClient* redisClientCreate(){
	redisClient*client=(redisClient*)malloc(sizeof(redisClient));
	return client;
}


//切换数据库的操作
bool redisClientChangeDB(redisServer*server,int dbId,redisClient*&client){
	//判断要切换的数据库Id是否合法
	if(dbId>=server->dbnum||dbId<0){
		printf("PIG Redis ERROR : DbId is Illegal.\n");
		return false;
	}
	client->db=server->db[dbId];
	return true;
}


//向当前使用的数据库中插入/更新
bool redisClientInsertDB(redisClient*client,char* key,char* val){
	return redisDbInsert(client->db,key,val);
}


//向当前使用的数据库中删除指定key对应的键值对
void redisClientDeleteDB(redisClient*client,char* key){
	redisDbDelete(client->db,key);
	return;
}


//向当前使用的数据库中获取指定key对应的value
char* redisClientFetchDB(redisClient*client,char* key){
	//printf("===%s\n",(char*)(redisDbFetchValue(client->db,key)));
	return redisDbFetchValue(client->db,key);
}

#endif

