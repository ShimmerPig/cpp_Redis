//服务端 https://www.cnblogs.com/maowang1991/p/3567788.html
#include <WINSOCK2.H>   
#include <stdio.h>     

//端口号                    
#define PORT           5150    
//发送消息的最大字节长度
#define MSGSIZE        1024    
                   
#pragma comment(lib, "ws2_32.lib")      
        
//服务器需要一个存放资源的db数组，每个客户端一连接，
//就创建一个Client，并且让它与对应的资源db[i]绑定
//db中是一个字典构成的数组，让对应的Client存放key-value
//客户端输入命令，服务器对命令进行拆分，然后在命令
//表中查询命令是否合法后，执行set命令，将key-value
//存储在db的字典数组中，最后返回响应给客户端。
           
int main()      
{      
	//初始化操作
    WSADATA wsaData;      
    SOCKET sListen;      
    SOCKET sClient;      
    SOCKADDR_IN local;      
    SOCKADDR_IN client; 
	//消息数组
    char szMessage[MSGSIZE];      
    int ret;      
    int iaddrSize = sizeof(SOCKADDR_IN); 
	//初始化Windows套接字库
    WSAStartup(0x0202, &wsaData);      
              
	//创建客户端的套接字
    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);      
                   
	//绑定
    local.sin_family = AF_INET;      
    local.sin_port = htons(PORT);      
    local.sin_addr.s_addr = htonl(INADDR_ANY);      
    bind(sListen, (struct sockaddr *) &local, sizeof(SOCKADDR_IN));      
       
	//监听
	//1表示等待连接队列的最大长度
    listen(sListen, 1);      
            
	//等待客户端连接
    sClient = accept(sListen, (struct sockaddr *) &client, &iaddrSize);      
    printf("Accepted client:%s:%d\n", inet_ntoa(client.sin_addr),      
            ntohs(client.sin_port));      
                   
    while (TRUE) {     
		//ret是发送消息的字节长度
		//szMessage是发送消息的内容
        ret = recv(sClient, szMessage, MSGSIZE, 0); 
        szMessage[ret] = '\0';      
        printf("Received [%d bytes]: '%s'\n", ret, szMessage);      
    }      
    return 0;      
  }