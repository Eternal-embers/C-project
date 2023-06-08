#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define dict_addr "D:\\C-project\\英语词典排版系统\\dictionary.txt"
#define dict_sort "D:\\C-project\\英语词典排版系统\\dictionary_sort.txt"
#define dict_backup "D:\\C-project\\英语词典排版系统\\dict_backup.txt"
struct word{
	char Eng[15];
	char meaning[20];
	struct word *next;
}dict;

void menu(void){
	printf("\n\n");
	printf("\t\t______________________________________________\n");
	printf("\t\t|                                             |\n");
	printf("\t\t|    ______________________________________   |\n");
	printf("\t\t|    |>>>>>>>>>>英语词典排版系统<<<<<<<<<<|   |\n");
	printf("\t\t|                                             |\n");
	printf("\t\t|        菜单:                                |\n");
	printf("\t\t|        录入- - - - - - - - - -N             |\n");
	printf("\t\t|                                             |\n");
	printf("\t\t|        删除- - - - - - - - - -D             |\n");
	printf("\t\t|                                             |\n");
	printf("\t\t|        浏览- - - - - - - - - -B             |\n");
	printf("\t\t|                                             |\n");
	printf("\t\t|        排序- - - - - - - - - -S             |\n");
	printf("\t\t|                                             |\n");
	printf("\t\t|        A-Z - - - - - - - - - -P             |\n");
	printf("\t\t|                                             |\n");
	printf("\t\t|        备份- - - - - - - - - -R             |\n");
	printf("\t\t|                                             |\n");
	printf("\t\t|        复习- - - - - - - - - -V             |\n");
	printf("\t\t|                                             |\n");
	printf("\t\t|        打字- - - - - - - - - -T             |\n");
	printf("\t\t|                                             |\n");
	printf("\t\t|        退出- - - - - - - - - -Q             |\n");
	printf("\t\t|_____________________________________________|\n");
	printf("\t\t >>");
}

int input(){
 	int n;
	FILE *fp;
	if((fp = fopen(dict_addr,"a"))==NULL){
		printf("\t\t----打开dictionary.txt失败!----\n");
		return 0;
	}
 	printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	while(1){
		printf("\t\t单词：");
		scanf("%s",dict.Eng);
		if(dict.Eng[0]=='#') break;
		printf("\t\t释义：");
		scanf("%s",dict.meaning); 
		fprintf(fp,"%15s%20s\n",dict.Eng,dict.meaning);
  		printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	}
	while(getchar()!='\n')
		continue;
	fclose(fp);
	return 1;
}

void del(void){
	FILE *fp;
	int n = 0,i;
	struct word *p1,*p2,*head,*pre,*temp;
	char s1[15],s2[15];
	if((fp = fopen(dict_addr,"r"))==NULL){
	printf("\t\t----打开dictionary.txt失败!----\n");
	return;
	}
	p1 = (struct word *)malloc(sizeof(struct word));
	fscanf(fp,"%20s%15s",p1->Eng,p1->meaning);
	while(!feof(fp)){
		n++;
		if(n==1) head = p1;
		else p2->next = p1;
		p2 = p1;
		p1 = (struct word *)malloc(sizeof(struct word));
		fscanf(fp,"%15s%20s\n",p1->Eng,p1->meaning);
	}
	p2->next = NULL;
	fclose(fp);
	p1 = head;//从链表头部逐个进行检查 
 	printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	while(p1->next!=NULL){
		pre = p1;
		p2 = p1->next;
		while(p2!=NULL){ 
			if(strcmp(p1->Eng,p2->Eng)==0)//如果发现重复的单词 
			{
				printf("\t\t删除 \"%s  %s\" 成功\n",p2->Eng,p2->meaning);
				temp = p2;
				pre->next = p2->next;//删除结点 
				p2 = p2->next;
				free(temp);//释放空间 
				temp = NULL;
				continue; 
			}
			p2 = p2->next;
			pre = pre->next;
		}
		p1 = p1->next;
	}
	//将处理后的数据重新写入 
	if((fp = fopen(dict_addr,"w"))==NULL){
	printf("\t\t----打开dictionary.txt失败!----\n");
	return;
	}
	p1 = head;
	printf("\t\t写入：\n");
	while(p1!=NULL){
		printf("\t\t>>>%15s%20s\n",p1->Eng,p1->meaning);
		fprintf(fp,"%15s%20s\n",p1->Eng,p1->meaning);
		p1 = p1->next;
	}
	fclose(fp);
	printf("\t\t--------删除重复单词成功！--------\n\t\t");
	while(head){
		p1 = head;
		head = head->next;
		free(head);
	}
	system("pause>NULL");//测试 
}

void scan(void){
	FILE *fp;
	int count = 0;
	if((fp = fopen(dict_addr,"r"))==NULL){
	printf("\t\t--------打开dictionary.txt失败!--------\n");
	return;
	}
	printf("\n\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	printf("\t\t单词                释义\n");
	while(1){
		fscanf(fp,"%15s%20s",dict.Eng,dict.meaning);
		if(feof(fp)) break;
		printf("\t\t%-8d%-20s%-s\n\n",count,dict.Eng,dict.meaning);
		count++;
	}
	fclose(fp);
	printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	printf("\t\t总共存储%d条单词信息",count);
	system("pause>NULL");
}

void sort(){
	FILE *fp;
	int n = 0;
	struct word *p1,*p2,*head,*pt,*pre,*_pre,*after;
	//_pre指向p1前一个结点，pre指向pt前一个结点 
	//after指向p1的后一个结点 
	if((fp = fopen(dict_addr,"r"))==NULL){
	printf("\t\t----打开dictionary.txt失败!----\n");
	return;
	}
	p1 = (struct word *)malloc(sizeof(struct word));
	fscanf(fp,"%15s%20s",p1->Eng,p1->meaning);
	while(!feof(fp)){
		n++;
		if(n==1) head = p1; 
		else p2->next = p1;
		p2 = p1;
		p1 = (struct word *)malloc(sizeof(struct word));
		fscanf(fp,"%15s%20s",p1->Eng,p1->meaning);
	}
	p2->next = NULL;
	fclose(fp);
	//创建一个新的头结点 
	pre = (struct word *)malloc(sizeof(struct word));
	strcpy(pre->Eng,"aaa");
	strcpy(pre->meaning,"头");
	pre->next = head;
	head = pre; 
	printf("\t\t    - - - - - - - -按A-Z进行排序- - - - - - - -\n");//测试
	//选择排序
	_pre = head;
	p1 = head->next;//从头结点下一个开始 
	while(p1->next != NULL){
		p2 = p1->next; 
		pt = p1;
		//向后遍历找出单词的字典序最小的那个结点
		while(p2!=NULL){
			if(strcmp(pt->Eng,p2->Eng)>0) pt = p2;//如果单词字典序小于pt->Eng,结点地址赋给pt 
			p2 = p2->next; 
		}
		//找到单词为最小字典序的结点的前一个结点
		if(strcmp(pt->Eng,p1->next->Eng)==0)//如果单词字典序最小的结点在p1后一个结点 
		{
			strcpy(dict.Eng,pt->Eng);
			strcpy(dict.meaning,pt->meaning);
			strcpy(pt->Eng,p1->Eng);
			strcpy(pt->meaning,p1->meaning);
			strcpy(p1->Eng,dict.Eng);
			strcpy(p1->meaning,dict.meaning);
			_pre = _pre->next;
			p1 = pt;
			continue;
		}
		else if(strcmp(pt->Eng,p1->Eng)!=0){
			p2 = p1;
			//找出pt的前一个结点 
			while(p2!=NULL){
				if(strcmp(p2->next->Eng,pt->Eng)==0){
					pre = p2;
					break;
				}
			p2 = p2->next;
			}
			//交换结点位置
			if(p1->next==NULL) after = NULL;
				else after = &(*(*p1).next);
			//将p1移动到pt的位置 
			p1->next = pt->next;
			pre->next = p1;
			//将pt移动到p1的位置
			pt->next = &(*after);
			_pre->next = pt; 
		}
		_pre = _pre->next;//记录p1前一个结点 
		p1 = pt->next;
	}
 	printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	//将排序好的数据存储到文件中
	if((fp = fopen(dict_sort,"w"))==NULL){
	printf("----打开dictionary_sort.txt失败!----\n");
	return;
	}
	p1 = head->next;
	printf("\t\t>>>写入\n");
	while(p1!=NULL){
		printf("\t\t>>>%15s %20s\n",p1->Eng,p1->meaning);//测试
		fprintf(fp,"%15s%20s\n",p1->Eng,p1->meaning); 
		p1 = p1->next;
	}
	fclose(fp);
	printf("\t\t-------按A-Z存储字典成功!-------\n\t\t");
	system("pause>NULL");
}

void print_sort(void){
	FILE *fp;
	if((fp = fopen(dict_sort,"r"))==NULL){
	printf("\t\t----打开dictionary_sort.txt失败!----\n");
	return;
	}
	printf("\t\t单词                释义\n");
	while(1){
		fscanf(fp,"%15s%20s",dict.Eng,dict.meaning);
		if(feof(fp)) break;
		printf("\t\t%15s%20s\n\n",dict.Eng,dict.meaning);
	}
	fclose(fp);
	printf("\t\t   --------打印成功!--------\n");
	system("pause>NULL");
} 

void reserve(void){
	FILE *fp,*f;
	int option;
	printf("\n\t\t请选择备份文件来源(原始数据0/已排序1)：\n");
	printf("\t\t如果选项输入错误，默认备份排序文件。\n\t\t>>");
	scanf("%d",&option);
	while(getchar()!='\n');
	if(option==0){
		if((f = fopen(dict_addr,"r"))==NULL){
			printf("\t\t----打开dictionary_sort.txt失败!----\n");
			return;
		}
	}
	else {
		if((f = fopen(dict_sort,"r"))==NULL){
			printf("\t\t----打开dictionary_sort.txt失败!----\n");
			return;
		}
	}
	if((fp = fopen(dict_backup,"w"))==NULL){
	printf("\t\t----打开back_up.txt失败!----\n");
	return;
	}
	printf("\t\t>>>写入:\n");
	while(!feof(f)){
		fscanf(f,"%15s%20s",dict.Eng,dict.meaning);
		printf("\t\t>>>%15s %15s\n",dict.Eng,dict.meaning);
		fprintf(fp,"%15s%20s\n",dict.Eng,dict.meaning);
	}
	fclose(fp);
	fclose(f);
	printf("\t\t   --------备份成功--------\n\t\t");
}

void review(void){
	FILE *fp;
	char option[6];
	int i,count,add,index;
	system("cls");
	if((fp = fopen(dict_addr,"r"))==NULL){
	printf("\t\t-------打开dictionary.txt失败!-------\n");
	return;
	}
	printf("\n\n\n\t\t跳转到(输入浏览时对应单词条左边的序号)：");
	scanf("%d",&index);
	i = 0;
	while(i <= index){
		fscanf(fp,"%15s%20s",dict.Eng,dict.meaning);
		i++;
	}
	printf("\t\t>>>此时跳转到%s\n",dict.Eng);
	printf("\t\t请选择您要复习的单词个数：\n\t\t>>");
	scanf("%d",&count);
	printf("\n\n\n\t\t>>>已复习：0\n");
	printf("\t\t单词：%s\n\t\t",dict.Eng);
	system("pause>NULL");
	printf("释义:%s",dict.meaning);
	system("pause>NULL");
	system("cls");
	for(i = 1;i<count;i++){
		printf("\n\n\n\t\t>>>已复习：%d\n",i);
		fscanf(fp,"%15s%20s",dict.Eng,dict.meaning);
		if(feof(fp)) rewind(fp);//如果读取到结尾自动回到文件开头
		printf("\t\t单词：%s\n\t\t",dict.Eng);
		system("pause>NULL");
		printf("释义:%s",dict.meaning);
		system("pause>NULL");
		system("cls");
	}
	printf("\n\n\t\t您已经复习了%d个单词\n",count);
	while(1){
		printf("\n\n\t\t您是否还要继续复习接下来的单词呢？(Y/N)\n\t\t如果您要重新复习前面的单词，请跟我说\"好的\"\n\t\t>>");
		scanf("%s",option);
		while(getchar()!='\n')
			continue;
		if(option[0]=='Y') {
			printf("\n\n\t\t请选择您要复习的单词个数：\n\t\t>>");
			scanf("%d",&add);
			for(i = 0;i<add;i++){
				printf("\n\n\n\t\t>>>已复习：%d\n",i);
				fscanf(fp,"%15s%20s",dict.Eng,dict.meaning);
				if(feof(fp)) rewind(fp);//如果读取到结尾自动回到文件开头
				printf("\t\t单词：%s\n\t\t",dict.Eng);
				system("pause>NULL");
				printf("释义:%s",dict.meaning);
				system("pause>NULL");
				system("cls");
			}
			count+=add;
			printf("\n\n\t\t您复习了%d个单词了\n",count);
		}
		else if(strcmp(option,"好的")==0){
			rewind(fp);
			printf("\n\n\n\t\t请选择您要复习的单词个数：\n\t\t>>");
			scanf("%d",&count);
			for(i = 0;i<count;i++){
				printf("\n\n\t\t>>>已复习：%d\n",i);
				fscanf(fp,"%15s%20s",dict.Eng,dict.meaning);
				if(feof(fp)) rewind(fp);//如果读取到结尾自动回到文件开头
				printf("\t\t单词：%s\n\t\t",dict.Eng);
				system("pause>NULL");
				printf("释义:%s",dict.meaning);
				system("pause>NULL");
				system("cls");
			}
			printf("\n\n\n\t\t您已经复习了%d个单词\n",count);
		} 
		else {
			printf("\n\n\t\t感谢您的使用！\n\t\t");
			return;
		}
	}
	fclose(fp);
}

void typing(void){
	FILE *fp;
	char option[3],prac[20],_prac[20];
	int i,num;
	float t = 0.0;
	system("cls");
	if((fp = fopen(dict_addr,"r"))==NULL){
	printf("\t\t-------打开dictionary.txt失败!-------\n");
	return;
	}
	printf("\n\n\n");
	printf("\t\t亲，欢迎您使用打字系统，请选择模式(英文/中文/中英):\n\n");
	printf("\t\t英文- - - - - - - - - - - - - -En\n\n");
	printf("\t\t中文- - - - - - - - - - - - - -Ch\n\n");
	printf("\t\t中英- - - - - - - - - - - - - -EC\n\n\t\t>>");
	scanf("%s",option);
	while(getchar()!='\n')
		continue;
	if(strcmp(option,"En")==0){
		printf("\t\t亲，请输入您需要练习的单词数:\n\t\t>>");
		scanf("%d",&num);
		for(i = 0;i<num;i++){
			fscanf(fp,"%15s%20s",dict.Eng,dict.meaning);
			if(feof(fp)) rewind(fp);//如果读取到结尾自动回到文件开头
			printf("\t\t%-15s\t",dict.Eng);
			scanf("%s",prac);
			while(getchar()!='\n');
			printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
			if(strcmp(prac,dict.Eng)==0) t++;
		}
		printf("\t\t亲，您的正确率是%.2f%%,正确拼写%.0f个，错误拼写%d个\n",(100*t)/num,t,num-t);
	}
	else if(strcmp(option,"Ch")==0){
		printf("\t\t亲，请输入您需要练习的单词数:\n\t\t>>");
		scanf("%d",&num);
		for(i = 0;i<num;i++){
			fscanf(fp,"%15s%20s",dict.Eng,dict.meaning);
			if(feof(fp)) rewind(fp);//如果读取到结尾自动回到文件开头
			printf("\t\t%-20s\t",dict.meaning);
			scanf("%s",prac);
			while(getchar()!='\n');
			printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
			if(strcmp(prac,dict.meaning)==0) t++;
		}
		printf("\t\t亲，您的正确率是%.2f%%,正确拼写%.0f个，错误拼写%d个\n",(100*t)/num,t,num-t);
	}
	else{
		printf("\t\t亲，请输入您需要练习的单词数:\n\t\t>>");
		scanf("%d",&num);
		for(i = 0;i<num;i++){
			fscanf(fp,"%15s%20s",dict.Eng,dict.meaning);
			if(feof(fp)) rewind(fp);//如果读取到结尾自动回到文件开头
			printf("\t\t%-15s %-20s\n\t\t",dict.Eng,dict.meaning);
			scanf("%s%s",prac,_prac);
			while(getchar()!='\n');
			printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
			if(strcmp(prac,dict.Eng)==0 && strcmp(_prac,dict.meaning)==0) t++;
		}
		printf("\t\t亲，您的正确率是%.2f%%,正确拼写%.0f个，错误拼写%d个\n",(100*t/num),t,num-t);
	}
	printf("\t\t亲,打字练习结束！欢迎您再次光临\n\t\t");
	system("pause>NULL");
	fclose(fp);
}

int main(){
	char option;
	while(1){
		menu();
		scanf("%c",&option);
		getchar();
		if(option >= 'a' && option <= 'z') option -= 32;
		switch(option){
		case 'N':
		if(input()==1)printf("\t\t--------录入成功!--------\n\t\t");
			else printf("\t\t--------录入失败!--------\n\t\t");
		break;
		case 'D':del();break;
		case 'B':scan();break;
		case 'S':sort();break;
		case 'P':print_sort();break;
		case 'R':reserve();break;
		case 'V':review();break;
		case 'T':typing();break;
		case 'Q':printf("\n\t\t---------感谢您的使用!---------\n");return 0;
		default:
		printf("\t\t----选项输入错误，请重新输入!----\n\t\t");
		while(getchar()!='\n')
			continue;
		}
	}
}
