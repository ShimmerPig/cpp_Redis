#ifndef MY_REDIS_H
#include <WINSOCK2.H>   
#include <iostream>
#include <string>
#include <map>
using namespace std;
#define MY_REDIS_H
//数据库的个数
#define dbNum 15

//定义在redis中需要使用到的数据结构
//使用STL中的map作为dict，后期可能会自己造轮子
typedef map<string,string> myDB;
class Server;
class Client;
//对命令的函数指针进行typedef
typedef void (*CommandFun) (Server*,Client*,string,string&);

//存储资源使用的数据库类
/*class MyDB{
public:
	//存放资源的字典
	dict Dict;
};*/

//服务端
class Server{
public:
	SOCKET sListen;
	SOCKADDR_IN local;
	//数据库数组 每个元素是一个MyDB类型的指针
	myDB* DB[dbNum];
	//端口号
	int port;
	//命令集合
	//用一个map表示 key为函数的名字，value为一个指向函数的指针，根据函数的名字指向对应的函数
	map<string,CommandFun> Commands;
};

//客户端
class Client{
public:
	//数据库的num
	//int dbNum;
	SOCKET sClient;
	SOCKADDR_IN my_client;
	//客户端对应的数据库
	myDB db;
};

#endif