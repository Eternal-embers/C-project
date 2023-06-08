/*
	Name:
	Copyright:劣等生
	Author:劣等生
	Date:2023/06/23 11:47
	Description:
*/
#include<stdio.h>
#include<stdlib.h>
/* 用链表记录课程信息 */
struct course{
	char *name;//存储课程的名称
	struct course* next;//next指针
};
struct course *courses = NULL;

int N = 0;//记录课程数量
char buffer[100];//存储临时课程信息

struct student{
	char *name;//姓名
	int *score;//成绩
	int total;//总成绩
	struct student *next;
};
struct student *head;

/* 创建一门课程 */
struct course *createCourse(char *name){
	int len = getLength(name);//获取课程名称长度
	int i;
	struct course *course = (struct course *)malloc(sizeof(struct course));//分配课程结构体的空间
	course->name = (char *)malloc(sizeof(char) * len + 1);//分配课程名称的空间
	course->next == NULL;
	/* 复制课程名称 */
	for(i = 0;i < len;i++){
		course->name[i] = name[i];
	}
	course->name[len] = '\0';
	return course;
};

/* 获取字符串长度 */
int getLength(char *str){
	if(str == NULL) return -1;
	else {
		int i;
		for(i = 0;str[i] != '\0';i++)
			continue;
		return i;
	}
}

void addCourse(){
	//输入添加课程数量
	int n = 0;
	printf("\t\t请输入添加课程数量：");
	scanf("%d",&n);
	if(n <= 0) return;//如果n小于等于0，停止添加课程
	else N += n;
	printf("\t\t请输入%d门课程名称：\n",n);
	struct course *p1 = NULL;
	struct course *p2 = NULL;
	int i = 0;
	if(courses == NULL){
       /* 输入第一门课程信息 */
       printf("\t\t1：");
	   scanf("%s",buffer);
	   courses = createCourse(buffer);//创建第一门课程作为链表头节点
	   courses->next = NULL;
	   if(n == 1) return;
	   for(i = 1;i < n;i++){
	   		if(i == 1) p1 = courses;
			else p2->next = p1;
			p2 = p1;
			printf("\t\t%d：",i + 1);
			scanf("%s",buffer);
			p1 = createCourse(buffer);//创建课程
	   }
	}
	else {
		for(i = 0;i < n;i++){
	   		if(i == 0) p1 = courses;
			else p2->next = p1;
			p2 = p1;
			printf("\t\t%d:",i + 1);
	   		gets(buffer);
			p1 = createCourse(buffer);//创建课程
	   }
	}
	p2->next = p1;
	p1->next = NULL;
}

void printCourse(){
	if(courses == NULL){
		printf("\t\t课程为空！\n");
		return;
	}
	struct course *p = courses;
	printf("\t\t");
	while(p){
		printf("%s ",p->name);
		p = p->next;
	}
	printf("\n");
}

void createHead(){
	head = (struct student *)malloc(sizeof(struct student));
	head->name = NULL;
	head->score = NULL;
	head->next = NULL;
}

/* 登记成绩 */
void signIn(){
	int i;
    printf("\t\t请输入姓名：");
	scanf("%s",buffer);
	struct student *stu = (struct student *)malloc(sizeof(struct student));
	stu->score = (int *)malloc(sizeof(int) * N);//开辟N个int空间存储N门课程的成绩
	stu->total = 0;//总成绩初始为0
	int len = getLength(buffer);
	stu->name = (char *)malloc(sizeof(char) * len);//开辟存储姓名的空间
	/* 复制姓名 */
	for(i = 0;i < len;i++)
		stu->name[i] = buffer[i];
	stu->name[len] = '\0';
	printf("\t\t%s,请输入对应成绩：\n",stu->name);
	struct course *p = courses;
	for(i = 0;i < N;i++){
		printf("\t\t%s：",p->name);
		scanf("%d",&(stu->score[i]));
		stu->total += stu->score[i];//加入总成绩中
		p = p->next;
	}
	//头插法插入到链表中
	stu->next = head->next;
	head->next = stu;
}

void search(){
	printf("\t\t请输入姓名：");
	scanf("%s",buffer);
	struct student *p1 = head->next;
	struct course *p2 = courses;
	while(p2){
		if(strcmp(p1->name,buffer) == 0){
			int i;
			for(i = 0;i < N;i++){
				printf("\t\t%s：%d\n",p2->name,p1->score[i]);
				p2 = p2->next;
			}
			printf("\t\t总成绩：%d\n",p1->total);
		}
		p1 = p1->next;
	}
}

void sort(){
	struct student *t = head->next;
	int i,j,n = 0;
    while(t){
		n++;
		t = t->next;
	}//统计有多少个学生
	t = head->next;
	struct student *p[n];//指针数组
	for(i = 0;i < n;i++) {
		p[i] = t;
		t = t->next;
	}
	/* 插入排序 */
	for(i = 1;i < n;i++) {
		t = p[i];
		for(j = i - 1;j >= 0;j--)
			if(t->total > p[j]->total) p[j + 1] = p[j];
			else break;
		p[j + 1] = t;
	}
	/* 输出排序结果 */
		printf("\t\t        排行榜\n ");
	for(i = 0;i < n;i++)
		printf("\t\t第%d名   %-5s   %d\n",i + 1,p[i]->name,p[i]->total);
}

void menu() {
	printf("\n");
	printf("\t\t           菜单\n");
	printf("\t\t退出- - - - - - - - - - 0\n");
    printf("\t\t录入课程信息- - - - - - 1\n");
    printf("\t\t查看课程- - - - - - - - 2\n");
    printf("\t\t登记成绩- - - - - - - - 3\n");
    printf("\t\t查询成绩- - - - - - - - 4\n");
    printf("\t\t成绩排行- - - - - - - - 5\n");;
    printf("\t\t>>");
}

int main(){
	createHead();//初始化
	int option;
	while(1){
		menu();
		scanf("%d",&option);
		printf("\n");
		switch(option){
			case 0: printf("\t\t感谢您的使用！"); return 0;
			case 1: addCourse();    break;
			case 2: printCourse();  break;
			case 3: signIn();       break;
			case 4: search();       break;
			case 5: sort();         break;
			default: printf("\t\t输入错误！");
		}
		system("pause>NULL");
		printf("\t\t- - - - - - - - - - - - - - - - - - - - - - -\n");
	}
}

