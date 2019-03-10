//拉链法实现hashTable小练习
#include <iostream>
#include <string>
#include <malloc.h>

using namespace std;

//哈希表的大小
#define HASHSIZE 10
typedef unsigned int uint;

//定义哈希表结点
typedef struct Node{
	const char*key;
	const char*value;
	Node*next;
}Node;

class HashTable{
private:
	Node*node[HASHSIZE];
public:
	//构造方法
	HashTable();
	//对键进行hash的方法
	uint hash(const char*key);
	//返回改key对应的结点
	Node*lookup(const char*key);
	//向哈希表中插入一个结点的方法
	bool insert(const char*key,const char*value);
	//从哈希表中获取某个key的结点的value的方法
	const char*get(const char*key);
	//打印出哈希表的方法
	void display();
};


//构造方法
HashTable::HashTable() {
	for (int i = 0; i < HASHSIZE; i++) {
		//将HashTable中的每个结点初始化为NULL
		node[i] = NULL;
	}
}

//定义哈希表的hash算法,这里使用time33
//hash算法--将key转换成节点数组中对应的index
uint HashTable::hash(const char*key){
	uint hash=0;
	//对key中的每个字符进行操作
	for(;*key;++key){
		hash=hash*33+*key;
	}
	return hash%HASHSIZE;
}

//根据key寻找其对应的结点在数组中的位置的方法
//得到了结点，我们就能知道这个结点中的value
//实现了根据key寻找value的过程
//HashTable要实现的就是一个根据key寻找value的过程
//其核心是将key转换成数组范围内的index
//对于可能出现的多个key对应一个index的情况，我们使用拉链法来处理
//虽然是同一个index，但是这个node会指向下一个node，形成一条链表
//所以我们可以根据对链表的遍历，得到key相同的node，从而得到里面的value
Node*HashTable::lookup(const char*key){
	Node*np;
	uint index;
	//计算出这个key对应的index
	index=hash(key);
	for(np=node[index];np;np=np->next){
		//找到了链表中key相同的node
		if(!strcmp(key,np->key)){
			return np;
		}
	}
	return NULL;
}

//向hashTabel中国插入结点的方法
bool HashTable::insert(const char*key, const char*value) {
	uint index;
	Node*np;
	//这个key对应的结点（是唯一的）在表中不存在，
	//则新建一个node，插入对应的index的链表头
	//这里的链表数组的元素--结点指针一直在变
	if(!(np=lookup(key))){
		index=hash(key);
		np=(Node*)malloc(sizeof(Node));
		if(!np)return false;
		np->key=key;
		//插入表头
		np->next=node[index];
		node[index]=np;
	}
	//若存在，由于key与结点是一一对应的，所以这里
	//直接修改其对应的value值即可
	np->value=value;
	return true;
}

void HashTable::display() {
	Node*temp;
	for (int i = 0; i < HASHSIZE; i++) {
		if (!node[i]) {
			cout << "[]" << endl;
		}
		else {
			cout << "[";
			for (temp = node[i]; temp; temp = temp->next) {
				cout << "[" << temp->key << "]" << "[" << temp->value << "]";
			}
			cout << "]" << endl;
		}
	}
}

/*
int main() {
	
	HashTable *ht = new HashTable();
	const char*key[] = { "a","b" ,"c","asdhu","asd","asdsad"};
	const char*value[] = { "value1","value2" ,"zhu","asf","sfdsf","asfdsagfsg"};
	for (int i = 0; i < 6; i++) {
		ht->insert(key[i], value[i]);
	}
	ht->display();
	system("pause");
	return 0;
}
*/
