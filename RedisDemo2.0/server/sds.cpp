#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sds.h"

//创建一个包含给定c字符串的sds
sds sdsnew(char *init){
	sds sh=(sds)malloc(sizeof(struct sdshdr));
	sh->len=strlen(init);
	sh->free=0;
	sh->buf=(char*)malloc(sizeof(char)*(sh->len+1));
	//将字符串内容进行复制
	int i;
	for(i=0;i<sh->len;i++){
		(sh->buf)[i]=init[i];
	}
	(sh->buf)[i]='\0';
	return sh;
}

//为sds(也就是buf数组)分配指定空间/len
sds sdsnewlen(sds sh,int len){
	int i;
	sh->free=len-1-sh->len;
	//保存之前的buf内容
	char *str=(char *)malloc(sizeof(char)*(sh->len+1));
	for(i=0; i<(sh->len); i++){
		str[i]=sh->buf[i];	
	}
	str[i]='\0';
	//sh->buf=(char*)realloc(sh->buf,len);
	sh->buf=(char*)malloc(sizeof(char)*len);
	for(i=0; i<(sh->len); i++){
		sh->buf[i]=str[i];
	}
	sh->buf[i]='\0';
	free(str);
	return sh;
}


//创建一个不包含任何内容的空字符串
sds sdsempty(void){
	sds sh=(sds)malloc(sizeof(struct sdshdr));
	sh->len=0;
	sh->free=0;
	sh->buf=(char*)malloc(sizeof(char));
	sh->buf[0]='\0';
	return sh;
}

//释放给定的sds
void sdsfree(sds *sh){
	(*sh)->free=0;
	(*sh)->len=0;
	free((*sh)->buf);
	free(*sh);
}

//创建一个给定sds的副本
sds sdsdup(sds sh01){
	sds sh02=(sds)malloc(sizeof(struct sdshdr));
	sh02->free=sh01->free;
	sh02->len=sh01->len;
	sh02->buf=(char*)malloc(sizeof(char)*(sh02->free+sh02->len+1));
	int i;
	for(i=0;i<sh01->len;i++){
		sh02->buf[i]=sh01->buf[i];
	}
	sh02->buf[i]='\0';
	return sh02;
}

//清空sds保存的字符串内容
sds sdsclear(sds sh){
	int total=sh->len+sh->free+1;
	sh->len=0;
	sh->free=total-1;
	sh->buf[0]='\0';
	return sh;
}

//将给定c字符串拼接到另一个sds字符串的末尾
//先检查sds的空间是否满足修改所需的要求，如
//果不满足则自动将sds空间扩展至执行修改所需
//要的大小，然后在执行实际的修改操作——防止
//缓冲区溢出
//扩展空间的原则：拼接后的字符串是n个字节，则
//再给其分配n个字节的未使用空间，buf数组的实际长度为n+n+1
//当n超过1MB的时候，则为其分配1MB的未使用空间
//两个字符串cat，中间使用空格隔开
sds sdscat(sds sh,char *str){
	int newlen=strlen(str);
	int newfree;
	//剩余的空间不够cat操作
	if(sh->free<=newlen){
		//超出部分的空间
		newfree=newlen-sh->free;
		if(newfree<1024){
			newfree=newfree+newfree+1+sh->len+sh->free;
			sh=sdsnewlen(sh,newfree);
		}else{
			newfree=newfree+1024+1+sh->len+sh->free;
			sh=sdsnewlen(sh,newfree);
		}
	}
	int i;
	//执行cat操作
	sh->buf[sh->len]=' ';
	for(i=0;i<newlen;i++){
		sh->buf[sh->len+i+1]=str[i];
	}
	sh->buf[sh->len+i+1]='\0';
	sh->len+=(newlen+1);
	sh->free-=newlen;
	return sh;
}

//将给定sds字符串拼接到另一个sds字符串的末尾
sds sdscatsds(sds sh,sds str){
	int newlen=str->len;
	int newfree;
	//剩余的空间不够cat操作
	if(sh->free<=newlen){
		//超出部分的空间
		newfree=newlen-sh->free;
		if(newfree<1024){
			newfree=newfree+newfree+1+sh->len+sh->free;
			sh=sdsnewlen(sh,newfree);
		}else{
			newfree=newfree+1024+1+sh->len+sh->free;
			sh=sdsnewlen(sh,newfree);
		}
	}
	int i;
	//执行cat操作
	sh->buf[sh->len]=' ';
	for(i=0;i<newlen;i++){
		sh->buf[sh->len+i+1]=str->buf[i];
	}
	sh->buf[sh->len+i+1]='\0';
	sh->len+=(newlen+1);
	sh->free-=newlen;
	return sh;
}

//将给定的c字符串复制到sds里面，覆盖原有的字符串
//需要先检查
sds sdscpy(sds sh,char *str){
	//新来的长度
	int len=strlen(str);
	//需要使用到的新空间长度
	int newlen=len-sh->len;
	int total;
	//剩余的空间不够了需要重新分配，在copy
	if(newlen>=sh->free){
		//新空间长度大于1M,就只多分配newlen+1M+1
		//总的空间是len+newlen+1M+1
		if(newlen>=1024){
			total=len+newlen+1024+1;
			//copy后使用到的len,就是新字符串的长度
			sh->len=len;
			//空闲的空间长度
			//sh->free=total-len-1;
			//sh->buf=(char*)realloc(sh->buf,total);
			sh=sdsnewlen(sh,total);
		//分配newlen+newlen+1
		}else{
			total=len+newlen+newlen+1;
			sh->len=len;
			//sh->free=total-len-1;
			//sh->buf=(char*)realloc(sh->buf,total);
			sh=sdsnewlen(sh,total);
		}
		if(sh->buf==NULL){
			printf("PIG Redis ERROR : Realloc failed.\n");
		}
	}else{
		//剩余的空间够,不需要分配
		//原来拥有的总空间
		total=sh->len+sh->free;
		sh->len=len;
		sh->free=total-sh->len;
	}
	//开始copy
	int i;
	for(i=0;i<len;i++){
		(sh->buf)[i]=str[i];
	}
	sh->buf[i]='\0';
	return sh;
}

//保留sds给定区间内的数据，不在区间内的数据会被覆盖或清除
//s = sdsnew("Hello World");
//sdsrange(s,1,-1); => "ello World"
sds sdsrange(sds sh,int start,int end){
	int newlen=end-start+1;
	char *str=(char*)malloc(sizeof(char)*(sh->len+1));
	//sh1->free=sh->len-sh1->len;
	int i,j;
	for(i=start,j=0;i<=end;i++,j++){
		str[j]=sh->buf[i];
	}
	str[j]='\0';
	sh->buf=(char*)malloc(sizeof(char)*(sh->len+1));
	sh->free=sh->len-newlen;
	sh->len=newlen;
	for(i=0;i<strlen(str);i++){
		sh->buf[i]=str[i];
	}
	sh->buf[i]='\0';
	free(str);
	return sh;
}

//接受一个sds和一个c字符串作为参数，从sds中移除所有在c字符串中出现过的字符
//s = sdsnew("AA...AA.a.aa.aHelloWorld     :::");
//s = sdstrim(s,"A. :");
//printf("%s\n", s);
//Output will be just "Hello World".
//截断操作需要通过内存重分配来释放字符串中不再使用的空间，否则会造成内存泄漏
//大小写不敏感
//使用惰性空间释放优化字符串的缩短操作,执行缩短操作的时候，不立即使用内存重分
//配来回收缩短后多出来的字节，而是使用free属性记录这些字节，等待将来使用
sds sdstrim(sds s,const char *chstr);

//对比两个sds字符串是否相同
bool sdscmp(sds sh1,sds sh2){
	if(sh1->len!=sh2->len){
		return false;
	}
	for(int i=0;i<sh1->len;i++){
		if(sh1->buf[i]!=sh2->buf[i]){
			return false;
		}
	}
	return true;
}

/*int main(){
	printf("sdsnew('sss')\n");
	sds sh=sdsnew("sss");
	printf("%s\n",sh->buf);
	printf("%d\n",sh->len);
	printf("%d\n",sh->free);

	printf("sdscat(sh,'www')\n");
	sh=sdscat(sh,"www");
	printf("%s\n",sh->buf);
	for(int i=0;i<sh->len;i++){
		printf("%c",sh->buf[i]);
	}
	printf("%d\n",sh->len);
	printf("%d\n",sh->free);

	sds sh1=sdsnew("qqqq");
	sh=sdscatsds(sh,sh1);
	printf("%s\n",sh->buf);
	printf("%d\n",sh->len);
	printf("%d\n",sh->free);

	sh=sdsrange(sh,1,5);
	printf("%s\n",sh->buf);
	printf("%d\n",sh->len);
	printf("%d\n",sh->free);

	sds sh3=sdsnew("qqqq");
	sds sh4=sdsnew("qqqq");
	
	if(sdscmp(sh3,sh4)){
		printf("same\n");
	}else{
		printf("no same\n");
	}

	printf("sdscpy(sh,'wwww')\n");
	sh=sdscpy(sh,"wwww");
	printf("%s\n",sh->buf);
	printf("%d\n",sh->len);
	printf("%d\n",sh->free);

	printf("sdsnewlen(sh,12)\n");
	sh=sdsnewlen(sh,12);
	printf("%s\n",sh->buf);
	printf("%d\n",sh->len);
	printf("%d\n",sh->free);
	
	printf("sdsdup(sh)\n");
	sds sh1=sdsdup(sh);
	printf("%s\n",sh1->buf);
	printf("%d\n",sh1->len);
	printf("%d\n",sh1->free);

	printf("sdsclear(sh1)\n");
	sh1=sdsclear(sh1);
	printf("%s\n",sh1->buf);
	printf("%d\n",sh1->len);
	printf("%d\n",sh1->free);

	sdsfree(&sh);
	sdsfree(&sh1);
	//sdsfree(&sh2);
	sdsfree(&sh3);
	sdsfree(&sh4);
	system("pause");
	return 0;
}*/