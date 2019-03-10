#ifndef __ADLIST_H__
#define __ADLIST_H__

//双端链表

//双端链表的结点
typedef struct listNode{
	struct listNode *prev;//指向点一个结点的指针
	struct listNode *next;//指向下一个结点的指针
	void *value;//结点存放的值
}listNode;

//链表
typedef struct list{
	listNode *head;//头结点
	listNode *tail;//尾结点
	int len;//链表的长度

	//用于实现多态链表所需的类型的特定函数
	//函数指针
	//用于复制链表结点所保存的值
	void *(*dup)(void *ptr);
	//用于释放链表结点所保存的值
	void (*free)(void *ptr);
	//用于对比
	int (*match)(void *ptr, void *key);
}list;

//定义对链表进行操作的宏
//获取链表长度
#define listLength(l) ((l)->len)
//获取链表的头结点
#define listFirst(l) ((l)->head)
//获取链表的尾结点
#define listLast(l) ((l)->tail)
//获取前一个结点
#define listPrevNode(n) ((n)->prev)
//获取下一个结点
#define listNextNode(n) ((n)->next)
//获取该结点的值
#define listNodeValue(n) ((n)->value)
//设置复制操作的函数指针
#define listSetDupMethod(l,m) ((l)->dup = (m))
//设置释放操作的函数指针
#define listSetFreeMethod(l,m) ((l)->free = (m))
//设置对比操作的函数指针
#define listSetMatchMethod(l,m) ((l)->match = (m))
//获取复制链表结点的函数指针
#define listGetDupMethod(l) ((l)->dup)
//获取释放链表结点的函数指针
#define listGetFree(l) ((l)->free)
//获取比较链表结点的函数指针
#define listGetMatchMethod(l) ((l)->match)

//创建一个不包含任何结点的新链表
list *listCreate(void);
//释放给定链表，以及链表中的所有结点
void listRelease(list *l);
//将一个包含给定值的新节点添加到给定链表的表头
list *listAddNodeHead(list *l, void *value);
//将一个包含给定值的新节点添加到给定链表的表尾
list *listAddNodeTail(list *l, void *value);
//将一个包含给定值的新节点添加到给定结点的之前或之后
list *listInsertNode(list *l, listNode *old_node, void *value, int after);
//从链表中删除给定结点
list *listDelNode(list *l, listNode *node);
//复制一个给定链表的副本
list *listDup(list *orig);
//查找并返回链表中包含给定值的结点
listNode *listSearchKey(list *l, void *key);
//返回链表在给定索引上的结点
listNode *listIndex(list *l, long index);
//将链表结点的表位结点弹出，然后将被弹出的结点插
//入到链表的表头，成为新的表头结点
void listRotate(list *l);

#endif