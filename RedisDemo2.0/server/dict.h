#ifndef __DICT_H
#define __DICT_H
#include <string>
using namespace std;
//哈希表的结点使用dictEntry结构来表示
//每个dictEntry结构都保存着一个key-value
typedef struct dictEntry{
	//键
	char* key;
	//值
	char* value;
	//指向下个哈希表结点，形成链表——避免键冲突
	struct dictEntry *next;
}dictEntry;

//保存一组用于操作特定类型键值对的函数
typedef struct dictType {
	//计算哈希值的函数
    unsigned int (*hashFunction)(char* key,int ssize);
    //复制键的函数
	char* (*keyDup)(char* key);
    //复制值的函数
	char* (*valDup)(char* obj);
    //对比键的函数
	int (*keyCompare)(char* key1, char* key2);
    //销毁键的函数
	void (*keyDestructor)(char* key);
    //销毁值的函数
	void (*valDestructor)(char* obj);
} dictType;

//哈希表
typedef struct dictht {
	//哈希表数组
    dictEntry **table;
	//哈希表大小
    int size;
	//该哈希表已有结点的数量
    int used;
} dictht;

//字典
//其实字典就是对普通的哈希表再做一层封装
//增加了一些属性
typedef struct dict {
	//类型特定函数
    dictType *type;
	//哈希表
    dictht *ht;
} dict;

//创建一个新的字典
//需要传入哈希表的大小
dict *dictCreate(dictType *type,int hashSize);
//根据key寻找其在hashTable中对应的结点
dictEntry* lookup(dict *d,char* key);
//将给定的键值对添加到字典里面
//将给定的键值对添加到字典里面，如果键已经存在于字典，
//那么用新值取代原有的值
bool dictInsert(dict *d, char* key, char* val);
//返回给定的键的值
char* dictFetchValue(dict *d, char* key);
//从字典中删除给定键所对应的键值对
void dictDelete(dict *d, char* key);
//释放给定字典，以及字典中包含的所有键值对
void dictRelease(dict *d);

#endif