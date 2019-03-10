#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "redisDb.h"

/*//哈希表的大小
#define HASHSIZE 10

//定义对哈希表进行相关操作的函数集
//计算哈希值的函数
unsigned int myHashFunction(void *key,int size){
	char* charkey=(char*)key;
	unsigned int hash=0;
	for(;*charkey;++charkey){
		hash=hash*33+*charkey;
	}
	return hash%size;
}
//复制键的函数
void *myKeyDup(void *key){
	return key;
}
//复制值的函数
void *myValDup(void *obj){
	return obj;
}
//对比键的函数
int myKeyCompare(void *key1, void *key2){
	char*charkey1=(char*)key1;
	char*charkey2=(char*)key2;
	return strcmp(charkey1,charkey2);
}
//销毁键的函数
void myKeyDestructor(void *key){
	//free(key);
}
//销毁值的函数
void myValDestructor(void *obj){
	//free(obj);
}


*/
//初始化一个数据库
redisDb* redisDbCreate(dictType*type,int hashSize,int id){
	redisDb*db=(redisDb*)malloc(sizeof(redisDb));
	db->dict=dictCreate(type,hashSize);
	db->id=id;
	return db;
}

//初始化一个数据库
redisDb* redisDbCreate(dictType*type,int hashSize){
	redisDb*db=(redisDb*)malloc(sizeof(redisDb));
	db->dict=dictCreate(type,hashSize);
	db->id=NULL;
	printf("PIG Redis WARNING : redisDb's Id is NULL.\n");
	return db;
}

//向数据库中添加/更新新建
bool redisDbInsert(redisDb*db,char* key,char* val){
/*	if(db==NULL){
		printf("NULL\n");
		return false;
	}else if(db->dict==NULL){
		printf("NULL\n");
		return false;
	}*/
	int flag=dictInsert(db->dict,key,val);
	if(flag){
		printf("PIG Redis : redisDb insert/refresh success.\n");
		return true;
	}else{
		printf("PIG Redis : redisDb insert/refresh fail.\n");
		return false;
	}
}

//从数据库中删除键
void redisDbDelete(redisDb*db,char* key){
	dictDelete(db->dict,key);
	printf("PIG Redis : redisDb delete OK.\n");
	return ;
}

//从数据库中获取给定key对应的value
char* redisDbFetchValue(redisDb*db,char* key){
	char* result=dictFetchValue(db->dict,key);
	if(result==""){
		printf("PIG Redis WARNING: redisDb fetch NULL.\n");
	}else{
		printf("PIG Redis : redisDb fetch success.\n");
	}
	return result;
}

//释放数据库
void redisDbRelease(redisDb*db){
	if(db==NULL)return ;
	dictRelease(db->dict);
	free(db);
}


/*int main(){
	dictType*type=(dictType*)malloc(sizeof(dictType));
	type->hashFunction=myHashFunction;
	type->keyDup=myKeyDup;
	type->valDup=myValDup;
	type->keyCompare=myKeyCompare;
	type->keyDestructor=myKeyDestructor;
	type->valDestructor=myValDestructor;

	redisDb*db=redisDbCreate(type,HASHSIZE);
	
	char*k1="aaa";
	char*v1="121";
	redisDbInsert(db,k1,v1);
	char*v2=(char*)redisDbFetchValue(db,k1);
	redisDbDelete(db,k1);
	
	printf("%s\n",v2);

	redisDbRelease(db);
	system("pause");
	return 0;
}*/