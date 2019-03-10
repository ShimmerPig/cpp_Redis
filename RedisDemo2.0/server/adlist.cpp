#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adlist.h"

//创建一个不包含任何结点的新链表
list *listCreate(void){
	list* l=(list*)malloc(sizeof(list));
	//没有结点
	l->head=NULL;
	l->tail=NULL;
	l->len=0;
	l->dup=NULL;
	l->free=NULL;
	l->match=NULL;
	return l;
}
//释放给定链表，以及链表中的所有结点
void listRelease(list *l){
	if(l==NULL){
		return ;
	}
	//没有head(没有结点)
	if(l->head==NULL){
		return ;
	}
	//保证了链表是有结点存在的
	//用来移动的指针，指向第一个结点
	listNode*temp=l->head;
	while(temp->next!=NULL){
		temp=temp->next;
		//使用链表对应释放value的free来释放value的值
		if(l->free!=NULL){
			l->free(temp->value);
		}else{
			printf("PIG Redis WARNING : List->free is not define.\n");
		}
		free(temp->prev);
	}
	free(temp);
	l->head=NULL;
	l->tail=NULL;
	free(l);
	l=NULL;
	return;
}


//将一个包含给定值的新节点添加到给定链表的表头
list *listAddNodeHead(list *l, void *value){
	if(l==NULL){
		printf("PIG Redis ERROR : List NULL.\n");
		return NULL;
	}
	//链表中没有结点
	if(l->head==NULL){
		l->head=(listNode*)malloc(sizeof(listNode));
		l->head->next=NULL;
		l->head->prev=NULL;
		//使用链表对应复制value的dup来复制value的值
		if(l->dup!=NULL){
			l->head->value=l->dup(value);
		}else{
			printf("PIG Redis WARNING : List->dup is not define.\n");
			l->head->value=value;
		}
/*		int *c=(int*)(l->head->value);
		printf("%d====\n",*c);*/
		l->len=1;
		//头尾指针都指向新的结点
		l->tail=l->head;
		return l;
	}else{
		listNode*newone=(listNode*)malloc(sizeof(listNode));
		//newone->value=value;
		//使用链表对应复制value的dup来复制value的值
		if(l->dup!=NULL){
			newone->value=l->dup(value);
		}else{
			printf("PIG Redis WARNING : List->dup is not define.\n");
			newone->value=value;
		}
/*		int *cc=(int*)(newone->value);
		printf("%d====\n",*cc);*/

		newone->next=l->head;
		l->head->prev=newone;
		//新节点设为头结点
		l->head=newone;
		newone->prev=NULL;
		l->len++;
		return l;
	}
}

//将一个包含给定值的新节点添加到给定链表的表尾
list *listAddNodeTail(list *l, void *value){
	if(l==NULL){
		printf("PIG Redis ERROR : List NULL.\n");
		return NULL;
	}
	//链表中没有结点
	if(l->head==NULL){
		l->head=(listNode*)malloc(sizeof(listNode));
		//l->head->value=value;
		//使用链表对应复制value的dup来复制value的值
		if(l->dup!=NULL){
			l->head->value=l->dup(value);
		}else{
			printf("PIG Redis WARNING : List->dup is not define.\n");
			l->head->value=value;
		}		
		l->head->next=NULL;
		l->head->prev=NULL;
		l->tail=l->head;
		l->len=1;
		return l;
	}else{
		listNode*temp=(listNode*)malloc(sizeof(listNode));
		//temp->value=value;
		//使用链表对应复制value的dup来复制value的值
		if(l->dup!=NULL){
			temp->value=l->dup(value);
		}else{
			printf("PIG Redis WARNING : List->dup is not define.\n");
			temp->value=value;
		}
		temp->next=NULL;
		temp->prev=l->tail;
		l->tail->next=temp;
		l->tail=temp;
		l->len++;
		return l;
	}
}

//将一个包含给定值的新节点添加到给定结点的之前或之后
//after为1表示之后，after为0表示之前
list *listInsertNode(list *l, listNode *old_node, void *value, int after){
	listNode *newone=(listNode*)malloc(sizeof(listNode));
	//newone->value=value;
	//使用链表对应复制value的dup来复制value的值
	if(l->dup!=NULL){
		newone->value=l->dup(value);
	}else{
		printf("PIG Redis WARNING : List->dup is not define.\n");
		newone->value=value;
	}
	l->len++;
	if(after){
		newone->next=old_node->next;
		newone->prev=old_node;
		old_node->next->prev=newone;
		old_node->next=newone;
		//检查原来的temp是否为tail
		if(l->tail==old_node){
			l->tail=newone;
		}
		return l;
	}else{
		newone->next=old_node;
		newone->prev=old_node->prev;
		old_node->prev->next=newone;
		old_node->prev=newone;
		//检查原来的temp是否为头结点
		if(l->head==old_node){
			l->head=newone;
		}
		return l;		
	}
}	

//从链表中删除给定结点
list *listDelNode(list *l, listNode *node){
	l->len--;
	//使用链表对应释放value的free来释放value的值
	if(l->free!=NULL){
		l->free(node->value);
	}else{
		printf("PIG Redis WARNING : List->free is not define.\n");
	}
	//要删除的是最后一个结点
	if(l->head==node&&l->tail==node){
		free(node);
		l->head=NULL;
		l->tail=NULL;
		return l;
	}else if(l->head==node){
		printf("head\n");
		l->head=node->next;
		l->head->prev=NULL;
		free(node);
		return l;
	}else if(l->tail==node){
		l->tail=node->prev;
		l->tail->next=NULL;
		free(node);
		return l;
	}
	node->prev->next=node->next;
	node->next->prev=node->prev;
	free(node);
	return l;
}

//复制一个给定链表的副本
list *listDup(list *orig){
	if(orig==NULL){
		return NULL;
	}
	//该链表没有结点
	if(orig->head==NULL){
		list*l=listCreate();
		return l;
	}else{
		list*l=listCreate();
		listNode*temp=orig->head;
		while(temp!=NULL){
			//向表尾插入
			l=listAddNodeTail(l,temp->value);
			temp=temp->next;
		}
		return l;
	}
}

//查找并返回链表中包含给定值的结点
listNode *listSearchKey(list *l, void *key){
	if(l==NULL){
		printf("PIG Redis ERROR : List NULL.\n");
		return NULL;
	//链表中没有结点
	}else if(l->head==NULL){
		printf("PIG Redis ERROR : List does't have nodes.\n");
		return NULL;
	}else{
		listNode*temp=l->head;
		//检查是否定义了比较value的函数
		if(l->match==NULL){
			printf("PIG Redis ERROR : List->match is not define.\n");
			return NULL;
		}
		//match函数当两者相等时返回1
		while(temp!=NULL&&!(l->match(key,temp->value))){
			temp=temp->next;
		}
		if(temp==NULL){
			printf("PIG Redis ERROR : List doesn't have this node.\n");
			return NULL;
		}else{
			return temp;
		}
	}
}	

//返回链表在给定索引上的结点,index从0开始
listNode *listIndex(list *l, long index){
	if(l==NULL){
		printf("PIG Redis ERROR : List NULL.\n");
		return NULL;
	}else if(l->head==NULL){
		printf("PIG Redis ERROR : List doesn't have node.\n");
		return NULL;
	}
	listNode*temp=l->head;
	for(int i=0;i<index&&temp!=NULL;i++){
		temp=temp->next;
	}
	if(temp==NULL){
		printf("PIG Redis ERROR : Subscript out of range.\n");
		return NULL;	
	}
	return temp;
}

//将链表结点的表尾结点弹出，然后将被弹出的结点插
//入到链表的表头，成为新的表头结点
void listRotate(list *l){
	if(l==NULL){
		printf("PIG Redis ERROR : List NULL.\n");
		return ;
	}else if(l->head==NULL){
		printf("PIG Redis ERROR : List doesn't have node.\n");
		return ;
	}else if(l->head==l->tail){
		printf("PIG Redis ERROR : List only have one node.\n");
		return ;
	}	

	listNode*node=l->tail->prev;
	l->tail->prev->next=NULL;
	l->tail->next=l->head;
	l->head->prev=l->tail;
	l->head=l->tail;
	l->tail=node;
	l->head->prev=NULL;
}



/*
int intMatch(void *ptr, void *key){
	int *a=(int *)ptr;
	int *b=(int *)key;
	return (*a==*b)?1:0;
}

void *intDup(void *ptr){
	return ptr;
}
int main(){
	printf("listCreate()\n");
	list*l=listCreate();
	printf("%d\n",l->len);
	listSetDupMethod(l,&intDup);

	int b=111;
	int *a=&b;
	l=listAddNodeHead(l,a);
	printf("%d\n",l->len);
	//使用void*指针的时候需要强制转换
	int *c=(int *)(l->head->value);
	printf("%d\n",*c);

	int bb=12;
	int *aa=&bb;
	l=listAddNodeHead(l,aa);
	//listInsertNode(l,l->head,a,1);
	//l=listAddNodeTail(l,aa);
	//printf("%d\n",l->len);
	//l=listDelNode(l,l->head);
	//l=listDelNode(l,l->tail);
	//printf("%d\n",l->len);
	listRotate(l);
	//使用void*指针的时候需要强制转换
	int *cc=NULL;
	listNode*temp=l->tail;
	while(temp){
		cc=(int *)(temp->value);
		printf("%d\n",*cc);
		temp=temp->prev;
	}
	
	list*l2=listDup(l);
	temp=l2->tail;
	while(temp){
		cc=(int *)(temp->value);
		printf("%d\n",*cc);
		temp=temp->prev;
	}

	//listSetMatchMethod(l,&intMatch);
	listNode*node=listIndex(l,1);
	int *zhu=(int*)node->value;
	printf("*zhu:%d\n",*zhu);

	listRelease(l);
	//listRelease(l2);

	system("pause");
	return 0;
}*/