#ifndef __REDIS_SERVER_H__
#define __REDIS_SERVER_H__
#include <WINSOCK2.H>
#include "redisDb.h"

typedef struct redisServer{
	SOCKET sListen;
	SOCKADDR_IN local;
	//端口号
	int port;
	//一个数组，保存着服务器里面的所有数据库
	redisDb**db;
	//服务器的数据库数量
	int dbnum;
	//...
}redisServer;

//释放服务端
void redisServerRelease(redisServer*SERVER){
	for(int i=0;i<SERVER->dbnum;i++){
		//if(SERVER->db[i]->dict->ht->table[0]==NULL)
		//printf("%d\n",i);
		
		redisDbRelease(SERVER->db[i]);
	}
	//free(server->db);
	//free(server);
}

#endif
