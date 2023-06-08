/*
	Name:
	Copyright:劣等生
	Author:劣等生
	Date:02/04/23 14:36
	Description:多项式的运算-链表实现
*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define LEN 1000//能输入的多项式的最大项数

/* 多项式的基本结构体 */
struct poly{
	double coef;//系数
	double index;//指数
	struct poly *next;
};
struct poly *result[100];//存储结果
int size = 0;//记录当前存储的结果数量
	
//data为一个包含多项式各项的系数和指数的二维数组,它的一维数组有两项，索引0的为系数，索引1的为指数
//length表示多项式中的项数
//生成一个按幂次从高到低排序的多项式
struct poly* createPoly(double (*data)[2],int length)
{
	/*创建一个头结点*/
	struct poly *head = (struct poly *)malloc(sizeof(struct poly));
	struct poly *first = (struct poly *)malloc(sizeof(struct poly));//first指向首项
	/* 插入多项式的第一项*/
	head->next = first;
	first->coef = data[0][0];
	first->index = data[0][1];
	first->next = NULL;
	
	/* 将多项式中的其他项插入到链表中 */
	struct poly *p1,*p2,*pre;
	int i = 1;
	while(i < length)//第一项已经插入，所以i = 1
	{
		/* 生成结点*/
		p1 = (struct poly*)malloc(sizeof(struct poly));//p1指向要插入的结点
		p1->coef = data[i][0];
		p1->index = data[i][1];
		
		pre = head;//pre始终指向p2的前一个结点
 		p2 = first;//p2用于遍历
		//从多项式的首项开始遍历，找到合适的位置插入
		while(p2 && p2->index > p1->index){
			pre = pre->next;
			p2 = p2->next;
		}//直到p2的指数小于或等于要插入的结点的指数时，停止遍历
		
		if(p2 == NULL){//如果插入点在链表尾部
			p1->next = NULL;
			pre->next = p1;
		}
		else {
			//如果幂次相同直接合并同类项
			if(p2->index == p1->index)
			{
				p2->coef += p1->coef;
				free(p1);
			}
			else//否则插入到pre和p2之间
			{
				pre->next = p1;
				p1->next = p2;
				if(pre == head) first = p1;//如果新插入的结点成为了首项，更新first
			}
		}
		i++;
	}
	free(head);
	return first;//返回首项地址
}

//coef_digit指定系数的最大小数位数
//index_digit指定指数的最大小数位数
void showPoly(struct poly *P,int coef_digit,int index_digit){
	if(P){
		struct poly *node = P;
		while(node->next){
			if(node->coef != 0){
				printf("(%.*f)*[X^%.*f] + ",coef_digit,node->coef,index_digit,node->index);
			}
			node = node->next;
		}
		//最后一项后不需要输出+，所以单独输出
		printf("(%.*f)*[X^%.*f]",coef_digit,node->coef,index_digit,node->index);
	}
}

/* 计算多项式f1和f2的乘积 */
struct poly *multiply(struct poly *f1,struct poly *f2) {
	struct poly *head = (struct poly *)malloc(sizeof(struct poly));
	struct poly *res = NULL;//多项式的首项

	/* 计算多项式f1和f2的乘积 */
	struct poly *p1,*p2,*node,*pre,*pt;
	p1 = f1;//p1用于遍历f1多项式的各项
	while(p1){
		//printf("p1 %.0f[X^%.0f]\n",p1->coef,p1->index);//测试
		p2 = f2;//p2用于遍历f2多项式的各项
		while(p2){
			//printf("%.0f[X^%.0f]*%.0f[X^%.0f]=%.0f[X^%.0f]\n",p1->coef,p1->index,p2->coef,p2->index,node->coef,node->index);//测试
			/* 生成存储某两项相乘结果的结点 */
			node = (struct poly *)malloc(sizeof(struct poly));
			node->coef = p1->coef * p2->coef;//系数相乘
			node->index = p1->index + p2->index;//指数相加
			
			/* 插入到存储结果的链表中，结点的幂次按从高到低排序 */
			if(res == NULL) {//如果存储结果的链表中没有结点
				res = node;
				head->next = res;
				res->next = NULL;
				p2 = p2->next;
				continue;
			}
			pre = head;//pre始终指向pt的前一个结点
 			pt = res;//pt用于遍历存储结果的链表
			//从多项式的首项开始遍历，找到合适的位置插入
			while(pt && pt->index > node->index){
				pre = pre->next;
				pt = pt->next;
			}//直到pt的指数小于或等于要插入的结点的指数时，停止遍历
			if(pt == NULL){//如果插入点在链表尾部
				node->next = NULL;
				pre->next = node;
			}
			else {
				//如果幂次相同直接合并同类项
				if(pt->index == node->index)
				{
					pt->coef += node->coef;
					free(node);
				}
				else//否则插入到pre和p2之间
				{
					pre->next = node;
					node->next = pt;
					if(pre == head) res = node;//如果新插入的结点成为了首项，更新res
				}
			}
			p2 = p2->next;
		}
		p1 = p1->next;
	}
	free(head);
	return res;
}

/* 多项式的幂计算 */
struct poly *poly_pow(struct poly *f,int n){
	struct poly *res,*pre,*t;
	int i;
	res = f;
	pre = f;
	i = 1;
	while(i < n){
		res = multiply(pre,f);
		if(pre != f){
	 		while(pre){//释放计算过程中弃用的链表
				t = pre->next;
				free(pre);
				pre = t;
	 		}
		}
		pre = res;
	  	i++;
	}
	return res;
}

void clear(struct poly *P){
	struct poly* t;
    while(P){
		t = P->next;
		free(P);
		P = t;
	}
}

void menu(){
	printf("\t\t______________________________________________\n");
	printf("\t\t|                                            |\n");
	printf("\t\t|          ________________________          |\n");
	printf("\t\t|          |>>>>>多项式的运算<<<<<|          |\n");
	printf("\t\t|          菜单：                            |\n");
	printf("\t\t|          退出- - - - - - - - - - - 0       |\n");
	printf("\t\t|                                            |\n");
	printf("\t\t|          两个多项式的乘法运算- - - 1       |\n");
	printf("\t\t|                                            |\n");
	printf("\t\t|          多个多项式的乘法运算- - - 2       |\n");
	printf("\t\t|                                            |\n");
	printf("\t\t|          多项式的幂计算运算- - - - 3       |\n");
	printf("\t\t|                                            |\n");
	printf("\t\t|          查看保存结果- - - - - - - 4       |\n");
	printf("\t\t|____________________________________________|\n");
	printf("\n\t\t>>");
}

struct poly *input(){
	double data[LEN][2];
	struct poly *P;
	int i,len;
    printf("\t\t多项式项数：");
	scanf("%d",&len);//输入项数
	printf("\t\t      系数 指数\n");
	for(i = 0;i < len;i++){
		printf("\t\t第%d项：",i + 1);
		scanf("%lf%lf",&data[i][0],&data[i][1]);//输入各项系数和指数
	}
	P = createPoly(data,len);//创建多项式
	printf("\t\t输入结果：");
 	showPoly(P,0,0);
 	printf("\n\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	return P;
}

void depose_res(struct poly *res){
	int coef_digit,index_digit,flag;
	printf("\t\t系数的小数部分精确到的位数:");
	scanf("%d",&coef_digit);
	printf("\t\t指数的小数部分精确到的位数:");
	scanf("%d",&index_digit);
	printf("\t\t计算结果:");
   	showPoly(res,coef_digit,index_digit);
   	system("pause>NULL");
    printf("\n\t\t是否保存该结果？(是[1]/否[0])   >>");
    scanf("%d",&flag);
	if(flag == 1){
		result[size++] = res;
		printf("\t\t保存结果成功!\n");
	}
	else {//释放res链表的空间
		clear(res);
		printf("\t\t释放空间成功!\n");
	}
	printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
}

int main(){
	printf("\n\n");
	int option;
	while(1){
		menu();
		scanf("%d",&option);
		if(option == 1){
            struct poly *poly1,*poly2,*res;
            /* 输入两个多项式*/
            printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
            printf("\t\t>>>f1\n");
            poly1 = input();
            printf("\t\t>>>f2\n");
            poly2 = input();
			/* 计算poly1和poly2的乘积 */
			res = multiply(poly1,poly2);
			/* 处理结果 */
			depose_res(res);
		}
		else if(option == 2){
			int i,num;
			do{
            	printf("\t\t请输入多项式条数：");
            	scanf("%d",&num);
            	if(num < 2) printf("\t\t多项式条数必须大于1\n");
            }while(num < 2);
			struct poly *P[num],*res,*pre;
            for(i = 0;i < num;i++){
            	printf("\t\t>>>f%d\n",i + 1);
				P[i] = input();
			}
			/* 计算结果 */
			res = P[0];
			pre = res;
			i = 1;
			while(i < num){
				res = multiply(res,P[i]);
				clear(P[i-1]);//释放使用完的多项式P[i]
				/*
				clear(pre);//释放弃用的多项式链表
				pre = res;
				此处有bug，释放res的空间导致程序错误，需要改进
				由于暂时没有找到解决办法，所以此处的抛弃的res空间未释放
				*/
				i++;
			}
			clear(P[i-1]);//释放使用完的多项式P[i]
			/* 处理结果 */
   			depose_res(res);
		}
		else if(option == 3){
			int index;
			struct poly *P,*res;
            printf("\t\t请输入多项式的次幂：");
            scanf("%d",&index);
            P = input();//输入多项式
			res = poly_pow(P,index);//计算多项式的幂
			/* 处理结果 */
   			depose_res(res);
		}
		else if(option == 4) {
			int n,coef_digit,index_digit;
            printf("\t\t当前共存储%d条结果，如果要查看某条结果，请输入该项序号\n",size);
            printf("\t\t查看全部请结果输入0\n\t\t>>");
            scanf("%d",&n);
            if(n > 0 && n < size){
	 			printf("\t\t系数的小数部分精确到的位数:");
				scanf("%d",&coef_digit);
				printf("\t\t指数的小数部分精确到的位数:");
				scanf("%d",&index_digit);
				printf("\t\t计算结果:");
   				showPoly(result[n],coef_digit,index_digit);
			}
			else if(n == 0) {
				int i;
				printf("\t\t系数的小数部分精确到的位数:");
				scanf("%d",&coef_digit);
				printf("\t\t指数的小数部分精确到的位数:");
				scanf("%d",&index_digit);
				printf("\t\t存储结果：\n");
				i = 0;
				while(i < size){
					printf("\t\t%d  ",i);
	 				showPoly(result[i],coef_digit,index_digit);
					printf("\n");
					i++;
				}
			}
			else printf("\t\t输入错误！\n");
			system("pause>NULL");
			printf("\n\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
		}
		else if(option == 0){
			printf("\t\t感谢您的使用！");
			return 0;
		}
		else {
			printf("\t\t输入错误！");
			system("pause>NULL");
			system("cls");
		}
	}
	return 0;
}

