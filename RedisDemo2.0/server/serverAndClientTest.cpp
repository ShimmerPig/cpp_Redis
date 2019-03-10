#pragma warning(disable: 4786)
#pragma comment(lib, "ws2_32.lib")  
#include <WINSOCK2.H>   
#include <stdio.h>     
#include <string>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <process.h>
#include <vector>
#include "redisServer.h"
#include "redisClient.h"
using namespace std;

//服务端数据库数量——单机版中，16个数据库都是给同一个用户使用的
#define DBNUM 16
//端口号                    
#define PORT 5155    
//发送消息的最大字节长度
#define MSGSIZE 1024 
#define MAXCLIENTNUM 10

//哈希表的大小
#define HASHSIZE 10

//服务端
redisServer*server;

//分割字符串的方法
vector<string> split(string str,string ch){
	vector<string>vec;
	if(""==str){
		return vec;
	}
	string strs=str+ch;

	size_t pos=strs.find(ch);
	size_t size=strs.size();
	
	//npos为结束位置
	while(pos!=std::string::npos){
		string x=strs.substr(0,pos);
		vec.push_back(x);
		strs=strs.substr(pos+1,size);
		pos=strs.find(ch);
	}
	return vec;
}

//定义对哈希表进行相关操作的函数集
//计算哈希值的函数
unsigned int myHashFunction(char* key,int size){
	//char* charkey=(char*)key.data();
	unsigned int hash=0;
	for(;*key;++key){
		hash=hash*33+*key;
	}
	return hash%size;
}
//复制键的函数
char* myKeyDup(char* key){
	//strcpy
	return key;
}
//复制值的函数
char* myValDup(char* obj){
	return obj;
}
//对比键的函数
int myKeyCompare(char* key1, char* key2){
	//char*charkey1=(char*)key1.data();
	//char*charkey2=(char*)key2.data();
	
	return strcmp(key1,key2);
}
//销毁键的函数
void myKeyDestructor(char* key){
	//key=NULL;
	//free(key);
}
//销毁值的函数
void myValDestructor(char* obj){
	//obj=NULL;
	//free(obj);
}

//工作线程
DWORD WINAPI WorkerThread(LPVOID lpParam);

int main(){
	dictType*type=(dictType*)malloc(sizeof(dictType));
	type->hashFunction=myHashFunction;
	type->keyDup=myKeyDup;
	type->valDup=myValDup;
	type->keyCompare=myKeyCompare;
	type->keyDestructor=myKeyDestructor;
	type->valDestructor=myValDestructor;

	WSADATA wsaData;  
	int iaddrSize = sizeof(SOCKADDR_IN); 
	DWORD dwThreadId;
 
	//初始化Windows套接字库
    WSAStartup(0x0202, &wsaData);
	
	//初始化服务端
	//server=redisServerCreate(PORT,DBNUM,type,HASHSIZE);
	server=(redisServer*)malloc(sizeof(redisServer));

	//创建客户端的套接字
	server->sListen=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//绑定
	(server->local).sin_family=AF_INET;  
	(server->local).sin_port=htons(PORT); 
	(server->local).sin_addr.s_addr = htonl(INADDR_ANY); 
	bind(server->sListen, (struct sockaddr *) &(server->local), sizeof(SOCKADDR_IN));

	server->dbnum=DBNUM;

	//初始化数据库数组
	server->db=(redisDb**)malloc(sizeof(redisDb*)*server->dbnum);
	for(int k=0;k<server->dbnum;k++){
		server->db[k]=redisDbCreate(type,HASHSIZE,k);
	}

	//监听
	listen(server->sListen, 1);   

    //获得线程的句柄
    HANDLE hThread[MAXCLIENTNUM];
	
	for(int i=0;i<MAXCLIENTNUM;i++){
		//等待客户端连接
		redisClient*client=redisClientCreate();
		client->sClient=accept(server->sListen, (struct sockaddr *) &(client->my_client), &iaddrSize);  
		//将客户端对服务端中对应的数据库连接——默认从第0个数据库开始使用
		client->db=server->db[0];
		printf("Accepted client:%s:%d\n", inet_ntoa((client->my_client).sin_addr),      
            ntohs((client->my_client).sin_port));

		//创建工作线程
		hThread[i]=CreateThread(NULL, 0, WorkerThread, client, 0, NULL);
	}


	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
    //关闭套接字  
    closesocket(server->sListen);
	printf("PIG Redis : Threads are fully executed\n");

	redisServerRelease(server);

	system("pause");
	return 0;
}

//工作线程
DWORD WINAPI WorkerThread(LPVOID lpParam){
	char szMessage[MSGSIZE];      
	int ret;
	redisClient*client=(redisClient*)lpParam;
	client->db->dict;
	while(true){
		//ret是发送消息的字节长度
		//szMessage是发送消息的内容
		ret = recv(client->sClient, szMessage, MSGSIZE, 0); 
		szMessage[ret] = '\0'; 
		printf("PIG Redis : Received [%d bytes]: '%s'\n", ret, szMessage); 
		
		//类型转换
		string message(szMessage);

		//对用户发送的请求进行判断
		//用户请求版本信息
		if(message=="-v"||message=="--version"){
			char *str="Redis by Shimmer Pig v=0.0.2000 <00000000/0> 64 bit";
			//对用户的请求进行响应
			send(client->sClient,str,strlen(str)+sizeof(char),NULL);
			printf("PIG Redis : Send version information success.\n");
		//用户请求帮助文档
		}else if(message=="-h"||message=="--help"){
			char *str="help document not yet perfect";
			send(client->sClient,str,strlen(str)+sizeof(char),NULL);
			printf("PIG Redis : Send help document success.\n");	
		}else{
			//对发送的消息进行分割，检查是否符合其他命令格式
			//分隔符——空格
			string ch=" ";
			vector<string>vec=split(message,ch);
			
			//对命令格式进行判断
			if(vec[0]=="set"){
				//检查命令长度
				if(vec.size()==3){
					//获得键值
					vector<string>::iterator t;
					t=vec.begin();
					t++;
					string key=*t;
					t++;
					string value=*t;
					char*charkey=(char*)key.data();
					char*charvalue=(char*)value.data();
					//cout<<"DEBUG:"<<key<<" "<<value<<endl;
					//执行set命令
					bool flag=redisClientInsertDB(client,charkey,charvalue);
					//执行是否成功
					if(flag){
						char*str1="set ok";
						send(client->sClient,str1,strlen(str1)+sizeof(char),NULL);
						printf("PIG Redis : Send set ok\n");
					}else{
						char*str2="set error";
						send(client->sClient,str2,strlen(str2)+sizeof(char),NULL);
						printf("PIG Redis : Send set error\n");
					}

				//长度不匹配
				}else{
					char *str="PIG Redis ERROR : Wrong number of arguments for 'set' command";
					send(client->sClient,str,strlen(str)+sizeof(char),NULL);
					printf("PIG Redis : Send set error.\n");
				}
			}else if(vec[0]=="get"){
				//检查命令长度
				if(vec.size()==2){
					//获得键值
					vector<string>::iterator t;
					t=vec.begin();
					t++;
					string key=*t;
					char*charkey=(char*)key.data();
					char* getValue=redisClientFetchDB(client,charkey);
					//找不到这个键值对
					if(getValue==""){
						char *str="PIG Redis ERROR : Can't find such key-value pairs";
						send(client->sClient,str,strlen(str)+sizeof(char),NULL);
						printf("PIG Redis : Send get error\n");
					//找到了键值对，向客户端发送get到的值
					}else{
						send(client->sClient,getValue,strlen(getValue)+sizeof(char),NULL);
						printf("PIG Redis : Send get ok\n");
					}

				//命令长度不匹配
				}else{
					char *str="PIG Redis ERROR : Wrong number of arguments for 'get' command";
					send(client->sClient,str,strlen(str)+sizeof(char),NULL);
					printf("PIG Redis : Send get error.\n");
				}
			//当前命令库中还没有这个命令
			}else{
				char *str="PIG Redis ERROR : No such command";
				send(client->sClient,str,strlen(str)+sizeof(char),NULL);
				printf("PIG Redis : Send command error.\n");
			}

		}

	}
	return 0;
}