#include<stdio.h>
#include<stdlib.h>
#include <sstream>
#include<memory.h>
#include <graphics.h>		// 引用图形库头文件
#include <conio.h>			// 控制台输入输出头文件
#include<tchar.h>		    // 宽字符头文件
#define ElementType int
#define ERROR 0x7fffffff //入队时队列为空返回的错误码

typedef struct Queue//循环队列类型
{
	ElementType* element; //初始化的动态分配存储空间
	int front; //队头指针， 若队列非空，指向队列头元素
	int rear; //队尾指针， 若队列非空，指向队尾元素的下一个位置
	int QueueSize; //当前分配的存储空间(以sizeof(ElementType)为单位）
}Queue;

/* 队列的参数设定 */
int QUEUE_INIT_SIZE = 10; //队列存储空间的初始分配量
double QUEUE_RECYCLE_PERCENT = 0.5; //队列闲置空间达到的回收比例
double QUEUE_EXPAND_PERCENT = 2; //队列扩容的比例
double QUEUE_SHINK_PERCENT = 0.70; //队列收缩的比例

/*
	如果deQueueTimes >= enQueueTimes * 2，说明队列具有收缩的趋势，需要进行回收空间
*/
int enQueueTimes = 0;//进队次数
int deQueueTimes = 0;//出队次数

wchar_t buffer[512] = { '0' };//存储输入的队列数据的缓冲区
int token[256];//记录输入的数据，最多一次输入256个数据
Queue* QUEUE = NULL;

COLORREF usedColor = RGB(160, 160, 240);//队列已用部分颜色，紫
COLORREF headColor = RGB(60, 200, 160);//队列头部填充颜色，绿
COLORREF tailColor = RGB(255, 150, 150);//队列尾部填充颜色，红
COLORREF borderColor = RGB(180, 180, 180);//边框的颜色，灰
COLORREF freeColor = RGB(240, 240, 240);//队列自由存储空间的填充颜色，灰
COLORREF indexColor = RGB(0, 0, 0);//索引字体颜色，黑
COLORREF dataColor = RGB(255, 255, 255);//元素的数据字体颜色，白

int animationSpeed = 100;//动画速度

void resizeAnimation();//队列收缩动画函数的声明
void visualQueue();//队列可视化函数的声明

/* 初始化队列 */
Queue* initQueue() {
	Queue* Q = (Queue*)malloc(sizeof(Queue));
	Q->element = (ElementType*)malloc(sizeof(ElementType) * QUEUE_INIT_SIZE);//根据队列初始分配量分配空间
	Q->front = 0;
	Q->rear = 0;
	Q->QueueSize = QUEUE_INIT_SIZE;//队列容量

	//将近期出队次数和入队次数设置为0
	enQueueTimes = 0;
	deQueueTimes = 0;
	return Q;
}

/* 判断队列是否为空 */
bool isEmpty(Queue &Q) {
	return Q.front == Q.rear;
}

/* 获取队列长度 */
int getLength(Queue &Q) {
	if (Q.front < Q.rear) return Q.rear - Q.front;
	else
		return Q.QueueSize - (Q.front - Q.rear);
}

/* 取队头元素 */
ElementType getHead(Queue &Q) {
	if (Q.front != Q.rear)
		return Q.element[Q.front];
	else
		return ERROR;//队列为空
}

/* 将队列收缩或扩张到原先的percent比例的状态 */
void resizeQueue(Queue& Q, double percent) {
	int length = getLength(Q);//队列已用的长度
	int newSize = Q.QueueSize * percent;//队列新的长度
	if (length >= newSize) {
		printf("队列收缩后的空间不足以存储所有数据，无法完成收缩操作！");
		return;
	}

	//按照变换比例 percent 分配收缩或扩张后的队列存储空间
	ElementType* newSpace = (ElementType* )malloc(sizeof(ElementType) * newSize);

	//将队列中的元素复制到新的存储空间，将数据从新空间的起始位置开始存储
	//内存对齐：进行元素复制时，内存拷贝函数 memcpy() 来实现，确保操作高效。
	if (Q.front < Q.rear) //队头在队尾前面
		memcpy(newSpace, &Q.element[Q.front], sizeof(ElementType) * getLength(Q));
	else {//队尾在队头前面的情况
		//第一部分，将队头到队列存储空间末尾位置的数据复制到新的存储空间中
		int len = Q.QueueSize - Q.front;
		memcpy(newSpace, &Q.element[Q.front], sizeof(ElementType) * len);

		//第二部分，从队列存储空间起始位置到队尾的数据复制到新的存储空间中
		memcpy(newSpace + len, Q.element, sizeof(ElementType) * Q.rear);
	}

	// 释放原队列空间
	free(Q.element);

	// 更新队列的存储空间和头部指针
	Q.element = newSpace;
	Q.QueueSize = newSize;
	Q.front = 0; // 将队头指针设置为0
	Q.rear = length; //队尾指针设置为队列长度
}

/* 进队 */
void enQueue(Queue& Q, ElementType& elemt) {
	Q.element[Q.rear] = elemt;
	Q.rear = (Q.rear + 1) % Q.QueueSize;
	enQueueTimes++;//进队次数加一

	//队满，进行扩容
	if (Q.rear == Q.front) {
		printf("  队列的扩张动画：\n");
		resizeAnimation();//队列扩张动画
		int index = Q.rear;//队头指针与队尾指针的相遇位置
		resizeQueue(Q, QUEUE_EXPAND_PERCENT);//按照队列扩容比例进行扩容
		enQueueTimes = 0;//将近期入队次数清零
		printf("  _______________________________________________________________________\n");
		printf("  |                                                                      |\n");
		printf("       front = rear = %d，队列进行一次扩容，队列容量扩大至 %d\n", index, Q.QueueSize);
		printf("  |______________________________________________________________________|\n\n");
		
	}
}


/* 出队 */
ElementType deQueue(Queue &Q) {
	if (isEmpty(Q))//队列为空
		return ERROR;//返回错误码
	ElementType elemt = Q.element[Q.front];
	Q.front = (Q.front + 1) % Q.QueueSize;
	deQueueTimes++;//出队次数加一

	/*
		队列空闲空间达到回收比例，且最近队列具有收缩的趋势,对队列的存储空间进行回收
		当最近出队次数deQueueTimes 不小于最近进队次数 enQueueTimes 的2倍时，判定队列具有收缩的趋势
		例如，队列空间为10，已用5，近期出队次数为10，入队次数为5，则将队列进行收缩
	*/
	if (getLength(Q) < Q.QueueSize * QUEUE_RECYCLE_PERCENT && deQueueTimes >= enQueueTimes * 2) {
		printf("  队列的收缩动画：\n");
		resizeAnimation();//队列收缩动画
		int front = Q.front, rear = Q.rear;
		double percent = (Q.QueueSize - getLength(Q)) * 100.0 / Q.QueueSize;
		printf("  ______________________________________________________________________\n");
		printf("  |                                                                     |\n");
		printf("       当前空闲空间占比 %3.2f%% >= 最大空闲占比 %3.2f%%\n", percent,QUEUE_RECYCLE_PERCENT * 100);
		printf("       队列近期进队次数 %d，队列近期出队次数 %d，队列具有收缩趋势\n", enQueueTimes, deQueueTimes);
		printf("       front = %d，rear = %d，队列容量收缩至 %.0f\n", front, rear, Q.QueueSize * QUEUE_SHINK_PERCENT);
		printf("  |_____________________________________________________________________|\n\n");

		resizeQueue(Q, QUEUE_SHINK_PERCENT);//按照队列收缩比例进行存储空间回收
		deQueueTimes = 0;//将近期出队次数清零
	}
	return elemt;
}

/* 队列的实际模拟 */
void simulate(int n) {
	int option, value;
	srand(time(NULL));
	for (int i = 0; i < n; i++) {
		option = rand() % 2;//如果option = 1表示进队，option = 0表示出队
		if (option) {
			value = rand() % 1000;//产生一个1 ~ 999的随机值
			printf("  进队：index = %d, element = %d\n", QUEUE->rear, value);
			enQueue(*QUEUE,value);
			visualQueue();//入队操作的可视化
		}
		else {
			if (getLength(*QUEUE) > 1) {//队列只是存在2个元素才进行出队
				printf("  出队：index = %d, element = %d\n", QUEUE->front, deQueue(*QUEUE));
				visualQueue();//出队操作的可视化
			}
			else {
				printf("  出队失败，队列仅剩一个元素，已跳过此次出队操作！\n");
			}
		}
	}
}

/* 遍历队列 */
void traverseQueue(Queue &Q) {
	if (Q.front == Q.rear) return;//队列为空
	int front = Q.front;
	while (front != Q.rear) {
		printf("  index = %d, element = %d\n",front, Q.element[front]);
		front = (front + 1) % Q.QueueSize;
	}
}

void freeQueue(Queue *Q) {
	free(Q->element);
	free(Q);
	Q = NULL;
	//将进去出队次数和进队次数设置为0
	deQueueTimes = 0;
	enQueueTimes = 0;
}

int input(TCHAR prompt[], TCHAR title[]) {
	TCHAR text[512];
	_stprintf(text, _T("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 \
21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 \
41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 \
61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 \
81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 \
101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 \
121 122 123 124 125 126 127 128\
"));

	//弹出输入框用于数据输入
	int flag = InputBox(buffer, 1024,prompt,title,text, 640, 400, false);

	if (!flag) {//输入取消
		printf("  输入取消！\n");
		return 0;
	}

	//将wchar_t的buffer转换成多字节字符串
	char data[1024] = { '0' };
	wcstombs(data, buffer, 1024);

	char* p = data + strlen(data) - 1;
	//删除队尾空格
	while (*p == ' ' || *p == '\n') 
		p--;
	*(p + 1) = '\0';

	//打印输入的数据
	printf("  输入数据：");
	printf("%s\n", data);

	//从字符串data中读取出所有整数并存储到全局整型数组token中
	p = data;
	int n = 0;//记录数据的数量
	while (sscanf(p, "%d", &token[n]) == 1) {
		n++;
		p = strchr(p, ' '); // 查找下一个空格
		if (!p) break; // 如果没有找到空格，退出循环
		p++; // 跳过空格
	}

	return n;//返回数据的数目
}

/* =================================== 队列可视化展示 =================================== */

/* 队列可视化 */
void visualQueue() {
	setbkcolor(WHITE);//设置背景颜色为白色
	cleardevice();// 用背景色清空屏幕
	setbkmode(TRANSPARENT);//设置输出的文字背景为透明

	LOGFONT f;
	memset(&f, 0, sizeof(LOGFONT));
	f.lfHeight = 12; // 字体高度
	f.lfWidth = 0; // 字体宽度
	f.lfEscapement = 0; // 字符方向
	f.lfOrientation = 0; // 字符方向
	f.lfWeight = FW_NORMAL; // 字体粗细
	f.lfItalic = FALSE; // 斜体
	f.lfUnderline = FALSE; // 下划线
	f.lfStrikeOut = FALSE; // 删除线
	f.lfCharSet = DEFAULT_CHARSET; // 字符集
	f.lfOutPrecision = OUT_DEFAULT_PRECIS; // 输出精度
	f.lfClipPrecision = CLIP_DEFAULT_PRECIS; // 剪辑精度
	f.lfQuality = PROOF_QUALITY; // 字体质量：正稿质量
	f.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; // 字体族和Pitch
	_tcscpy_s(f.lfFaceName, _T("MonaspaceArgonVarVF")); // 字体名称
	settextstyle(&f);//设置文字字体

	BeginBatchDraw();

	/* 顶部信息区 */
	settextcolor(BLACK);//设置信息区字体颜色
	setlinecolor(borderColor);//边框颜色
	rectangle(50,10,1150,90);
	setlinecolor(borderColor);//默认颜色
	int length = getLength(*QUEUE);
	TCHAR text[50];
	//队列容量
	RECT capacity_part = { 100, 15, 350, 35 };
	_stprintf(text, _T("队列容量：%d(每单位%d字节)"), QUEUE->QueueSize,sizeof(ElementType));
	drawtext(text, &capacity_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	//队列已用的存储空间
	RECT usedSpace_part = { 100, 40, 350, 60 };
	_stprintf(text, _T("队列已用的存储空间：%d(每单位%d字节)"), length,sizeof(ElementType));
	drawtext(text, &usedSpace_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);//左对齐、居中、一行显示

	//队列空闲的存储空间
	RECT freeSpace_part = { 100, 65, 350, 85 };
	_stprintf(text, _T("队列空闲的存储空间：%d(每单位%d字节)"), QUEUE->QueueSize - length,sizeof(ElementType));
	drawtext(text, &freeSpace_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	//队列近期出队次数
	RECT deQueue_part = { 400,15,650,35 };
	_stprintf(text, _T("队列近期出队次数：%d"), deQueueTimes);
	drawtext(text, &deQueue_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//队列近期进队次数
	RECT enQueue_part = { 400,40,650,60 };
	_stprintf(text, _T("队列近期入队次数：%d"), enQueueTimes);
	drawtext(text, &enQueue_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//空闲空间占比
	RECT freePercent_part = { 400,65,650,85 };
	_stprintf(text, _T("队列空闲空间占比：%3.1f%c"), (QUEUE->QueueSize - length) * 100.0 / QUEUE->QueueSize,_T('%'));//将浮点数保留一位小数后转换为TCHAR类型的字符串
	drawtext(text, &freePercent_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//队头指针
	RECT front_part = { 700,15,1000,35 };
	_stprintf(text, _T("队头指针front = %d，队头元素element = %d"), QUEUE->front,QUEUE->element[QUEUE->front]);
	drawtext(text, &front_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//队尾指针
	RECT rear_part = { 700,40,1000,60 };
	_stprintf(text, _T("队尾指针rear = %d，队尾元素element = %d"), QUEUE->rear,QUEUE->element[QUEUE->rear - 1]);
	drawtext(text, &rear_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//注释部分
	RECT comment_part = { 700,65,1200,85 };
	_stprintf(text, _T("注：在下方队列可视化部分中，黑色数据为索引，白色数据为队列元素"));
	drawtext(text, &comment_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//队列数据部分
	setlinecolor(borderColor);//边框颜色
	rectangle(20, 110, 1180, 780);

	int x = 60;//队列单位存储空间的中心横坐标
	int y = 150;//队列单位存储空间的中心纵坐标
	int width = 16;//一个数据的宽度
	int height = 20;//一个数据的高度

	if (QUEUE->front < QUEUE->rear) {
		/* 未使用的存储空间 */
		setfillcolor(freeColor);//设置未用存储空间的填充颜色
		for (int i = 0; i < QUEUE->front; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
		/* 绘制队头元素 */
		setfillcolor(headColor);//设置队头的填充颜色
		fillrectangle(x - width, y - height, x + width, y + height);
		_stprintf(text, _T("%d"), QUEUE->front);
		settextcolor(indexColor);//设置队头索引颜色
		outtextxy(x - width / 2, y - height + 5, text);
		setlinecolor(WHITE);
		line(x - width, y, x + width, y);//分隔线
		setlinecolor(borderColor);//将线的颜色调整会边框颜色
		_stprintf(text, _T("%d"), QUEUE->element[QUEUE->front]);
		settextcolor(dataColor);//设置元素的数据颜色
		outtextxy(x - width / 2, y + 5, text);
		x += width * 2 + 5;
		if (x > 1150 - width) {
			y += height * 2 + 5;
			x = 60;
		}
		/* 已使用的存储空间 */
		setfillcolor(usedColor);//设置填充颜色为紫色
		for (int i = QUEUE->front + 1; i < QUEUE->rear - 1; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			_stprintf(text, _T("%d"), i);
			settextcolor(indexColor);//设置队头索引颜色
			outtextxy(x - width / 2, y - height + 5, text);
			setlinecolor(WHITE);
			line(x - width, y, x + width, y);//分隔线
			setlinecolor(borderColor);//将线的颜色调整会边框颜色
			_stprintf(text, _T("%d"), QUEUE->element[i]);
			settextcolor(dataColor);//设置元素的数据颜色
			outtextxy(x - width / 2, y + 5, text);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
		/* 绘制队尾元素 */
		setfillcolor(tailColor);//设置队头的填充颜色
		fillrectangle(x - width, y - height, x + width, y + height);
		_stprintf(text, _T("%d"), QUEUE->rear - 1);
		settextcolor(indexColor);//设置队头索引颜色
		outtextxy(x - width / 2, y - height + 5, text);
		setlinecolor(WHITE);
		line(x - width, y, x + width, y);//分隔线
		setlinecolor(borderColor);//将线的颜色调整会边框颜色
		_stprintf(text, _T("%d"), QUEUE->element[QUEUE->rear - 1]);
		settextcolor(dataColor);//设置元素的数据颜色
		outtextxy(x - width / 2, y + 5, text);
		x += width * 2 + 5;
		if (x > 1150 - width) {
			y += height * 2 + 5;
			x = 60;
		}
		/* 未使用的存储空间 */
		setfillcolor(freeColor);//设置未用存储空间的填充颜色
		for (int i = QUEUE->rear; i < QUEUE->QueueSize; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
	}
	else if(QUEUE->front > QUEUE->rear){
		/* 已使用的存储空间 */
		setfillcolor(usedColor);//设置填充颜色为紫色
		for (int i = 0; i < QUEUE->rear; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			_stprintf(text, _T("%d"), i);
			settextcolor(indexColor);//设置队头索引颜色
			outtextxy(x - width / 2, y - height + 5, text);
			setlinecolor(WHITE);
			line(x - width, y, x + width, y);//分隔线
			setlinecolor(borderColor);//将线的颜色调整会边框颜色
			_stprintf(text, _T("%d"), QUEUE->element[i]);
			settextcolor(dataColor);//设置元素的数据颜色
			outtextxy(x - width / 2, y + 5, text);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
		/* 绘制队尾元素 */
		setfillcolor(tailColor);//设置队头的填充颜色
		fillrectangle(x - width, y - height, x + width, y + height);
		_stprintf(text, _T("%d"), QUEUE->rear);
		settextcolor(indexColor);//设置队头索引颜色
		outtextxy(x - width / 2, y - height + 5, text);
		setlinecolor(WHITE);
		line(x - width, y, x + width, y);//分隔线
		setlinecolor(borderColor);//将线的颜色调整会边框颜色
		_stprintf(text, _T("%d"), QUEUE->element[QUEUE->rear]);
		settextcolor(dataColor);//设置元素的数据颜色
		outtextxy(x - width / 2, y + 5, text);
		x += width * 2 + 5;
		if (x > 1150 - width) {
			y += height * 2 + 5;
			x = 60;
		}
		/* 未使用的存储空间 */
		setfillcolor(freeColor);//设置未用存储空间的填充颜色
		for (int i = QUEUE->rear; i < QUEUE->front; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
		/* 绘制队头元素 */
		setfillcolor(headColor);//设置队头的填充颜色
		fillrectangle(x - width, y - height, x + width, y + height);
		_stprintf(text, _T("%d"), QUEUE->front);
		settextcolor(indexColor);//设置队头索引颜色
		outtextxy(x - width / 2, y - height + 5, text);
		setlinecolor(WHITE);
		line(x - width, y, x + width, y);//分隔线
		setlinecolor(borderColor);//将线的颜色调整会边框颜色
		_stprintf(text, _T("%d"), QUEUE->element[QUEUE->front]);
		settextcolor(dataColor);//设置元素的数据颜色
		outtextxy(x - width / 2, y + 5, text);
		x += width * 2 + 5;
		if (x > 1150 - width) {
			y += height * 2 + 5;
			x = 60;
		}
		/* 已使用的存储空间 */
		setfillcolor(usedColor);//设置填充颜色为紫色
		for (int i = QUEUE->front + 1; i < QUEUE->QueueSize; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			_stprintf(text, _T("%d"), i);
			settextcolor(indexColor);//设置队头索引颜色
			outtextxy(x - width / 2, y - height + 5, text);
			setlinecolor(WHITE);
			line(x - width, y, x + width, y);//分隔线
			setlinecolor(borderColor);//将线的颜色调整会边框颜色
			_stprintf(text, _T("%d"), QUEUE->element[i]);
			settextcolor(dataColor);//设置元素的数据颜色
			outtextxy(x - width / 2, y + 5, text);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
	}
	else {
		/* 未使用的存储空间 */
		setfillcolor(freeColor);//设置未用存储空间的填充颜色
		for (int i = 0; i < QUEUE->QueueSize; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
	}
	

	EndBatchDraw();
}

void resizeAnimation() {
	setbkcolor(WHITE);//设置背景颜色为白色
	cleardevice();// 用背景色清空屏幕
	setbkmode(TRANSPARENT);//设置输出的文字背景为透明

	LOGFONT f;
	memset(&f, 0, sizeof(LOGFONT));
	f.lfHeight = 12; // 字体高度
	f.lfWidth = 0; // 字体宽度
	f.lfEscapement = 0; // 字符方向
	f.lfOrientation = 0; // 字符方向
	f.lfWeight = FW_NORMAL; // 字体粗细
	f.lfItalic = FALSE; // 斜体
	f.lfUnderline = FALSE; // 下划线
	f.lfStrikeOut = FALSE; // 删除线
	f.lfCharSet = DEFAULT_CHARSET; // 字符集
	f.lfOutPrecision = OUT_DEFAULT_PRECIS; // 输出精度
	f.lfClipPrecision = CLIP_DEFAULT_PRECIS; // 剪辑精度
	f.lfQuality = PROOF_QUALITY; // 字体质量：正稿质量
	f.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; // 字体族和Pitch
	_tcscpy_s(f.lfFaceName, _T("MonaspaceArgonVarVF")); // 字体名称
	settextstyle(&f);//设置文字字体

	BeginBatchDraw();

	/* 顶部信息区 */
	settextcolor(BLACK);//设置信息区字体颜色
	setlinecolor(borderColor);//边框颜色
	rectangle(50, 10, 1150, 90);
	setlinecolor(borderColor);//默认颜色
	int length = getLength(*QUEUE);
	TCHAR text[50];
	//队列容量
	RECT capacity_part = { 100, 15, 350, 35 };
	_stprintf(text, _T("队列容量：%d(每单位%d字节)"), QUEUE->QueueSize, sizeof(ElementType));
	drawtext(text, &capacity_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//队列已用的存储空间
	RECT usedSpace_part = { 100, 40, 350, 60 };
	_stprintf(text, _T("队列已用的存储空间：%d(每单位%d字节)"), length, sizeof(ElementType));
	drawtext(text, &usedSpace_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);//左对齐、居中、一行显示

	//队列空闲的存储空间
	RECT freeSpace_part = { 100, 65, 350, 85 };
	_stprintf(text, _T("队列空闲的存储空间：%d(每单位%d字节)"), QUEUE->QueueSize - length, sizeof(ElementType));
	drawtext(text, &freeSpace_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//队列近期出队次数
	RECT deQueue_part = { 400,15,650,35 };
	_stprintf(text, _T("队列近期出队次数：%d"), deQueueTimes);
	drawtext(text, &deQueue_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//队列近期进队次数
	RECT enQueue_part = { 400,40,650,60 };
	_stprintf(text, _T("队列近期入队次数：%d"), enQueueTimes);
	drawtext(text, &enQueue_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//空闲空间占比
	RECT freePercent_part = { 400,65,650,85 };
	_stprintf(text, _T("队列空闲空间占比：%3.1f%c"), (QUEUE->QueueSize - length) * 100.0 / QUEUE->QueueSize, _T('%'));//将浮点数保留一位小数后转换为TCHAR类型的字符串
	drawtext(text, &freePercent_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//队头指针
	RECT front_part = { 700,15,1000,35 };
	_stprintf(text, _T("队头指针front = %d，队头元素element = %d"), QUEUE->front, QUEUE->element[QUEUE->front]);
	drawtext(text, &front_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//队尾指针
	RECT rear_part = { 700,40,1000,60 };
	_stprintf(text, _T("队尾指针rear = %d，队尾元素element = %d"), QUEUE->rear, QUEUE->element[QUEUE->rear - 1]);
	drawtext(text, &rear_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//注释部分
	RECT comment_part = { 700,65,1200,85 };
	_stprintf(text, _T("注：在下方队列可视化部分中，黑色数据为索引，白色数据为队列元素"));
	drawtext(text, &comment_part, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//队列数据部分
	setlinecolor(borderColor);//边框颜色
	rectangle(20, 110, 1180, 780);

	int x = 60;//队列单位存储空间的中心横坐标
	int y = 150;//队列单位存储空间的中心纵坐标
	POINT front;//头元素与尾元素的中心坐标
	int width = 16;//一个数据的宽度
	int height = 20;//一个数据的高度
	if (QUEUE->front < QUEUE->rear) {
		/* 未使用的存储空间 */
		setfillcolor(freeColor);//设置未用存储空间的填充颜色
		for (int i = 0; i < QUEUE->front; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
		front.x = x, front.y = y;//获取队头元素的位置
		/* 已使用的存储空间 */
		setfillcolor(usedColor);//设置填充颜色为紫色
		for (int i = QUEUE->front; i < QUEUE->rear; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			_stprintf(text, _T("%d"), i);
			settextcolor(indexColor);//设置队头索引颜色
			outtextxy(x - width / 2, y - height + 5, text);
			setlinecolor(WHITE);
			line(x - width, y, x + width, y);//分隔线
			setlinecolor(borderColor);//将线的颜色调整会边框颜色
			_stprintf(text, _T("%d"), QUEUE->element[i]);
			settextcolor(dataColor);//设置元素的数据颜色
			outtextxy(x - width / 2, y + 5, text);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
		/* 未使用的存储空间 */
		setfillcolor(freeColor);//设置未用存储空间的填充颜色
		for (int i = QUEUE->rear; i < QUEUE->QueueSize; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}

		EndBatchDraw();

		int length = getLength(*QUEUE);
		x = 60, y = 150;
		//memcpy的复制动画
		for (int i = 0; i < length; i++) {
			BeginBatchDraw();

			printf("  新的存储空间坐标：(%d,%d)\n", x, y);
			//将移动的元素的新存储位置设置为已用部分
			setfillcolor(usedColor);//将填充颜色设置为已用空间的颜色
			fillrectangle(x - width, y - height, x + width, y + height);
			_stprintf(text, _T("%d"), i);
			settextcolor(indexColor);//设置队头索引颜色
			outtextxy(x - width / 2, y - height + 5, text);
			setlinecolor(WHITE);
			line(x - width, y, x + width, y);//分隔线
			_stprintf(text, _T("%d"), QUEUE->element[QUEUE->front + i]);
			settextcolor(dataColor);//设置元素的数据颜色
			outtextxy(x - width / 2, y + 5, text);

			//将移动的元素变成未存储部分
			setlinecolor(borderColor);//将线的颜色调整会边框颜色
			setfillcolor(freeColor); //将填充颜色设置为未用存储空间的颜色
			fillrectangle(front.x - width, front.y - height, front.x + width, front.y + height);

			/* 坐标更新 */
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
			front.x += width * 2 + 5;
			if (front.x > 1150 - width) {
				front.y += height * 2 + 5;
				front.x = 60;
			}

			EndBatchDraw();
		}
	}
	else {
		/* 已使用的存储空间 */
		setfillcolor(usedColor);//设置填充颜色为紫色
		for (int i = 0; i < QUEUE->rear; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			_stprintf(text, _T("%d"), i);
			settextcolor(indexColor);//设置队头索引颜色
			outtextxy(x - width / 2, y - height + 5, text);
			setlinecolor(WHITE);
			line(x - width, y, x + width, y);//分隔线
			setlinecolor(borderColor);//将线的颜色调整会边框颜色
			_stprintf(text, _T("%d"), QUEUE->element[i]);
			settextcolor(dataColor);//设置元素的数据颜色
			outtextxy(x - width / 2, y + 5, text);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
		/* 绘制队尾元素 */
		setfillcolor(tailColor);//设置队头的填充颜色
		fillrectangle(x - width, y - height, x + width, y + height);
		_stprintf(text, _T("%d"), QUEUE->rear);
		settextcolor(indexColor);//设置队头索引颜色
		outtextxy(x - width / 2, y - height + 5, text);
		setlinecolor(WHITE);
		line(x - width, y, x + width, y);//分隔线
		setlinecolor(borderColor);//将线的颜色调整会边框颜色
		_stprintf(text, _T("%d"), QUEUE->element[QUEUE->rear]);
		settextcolor(dataColor);//设置元素的数据颜色
		outtextxy(x - width / 2, y + 5, text);
		x += width * 2 + 5;
		if (x > 1150 - width) {
			y += height * 2 + 5;
			x = 60;
		}
		/* 未使用的存储空间 */
		setfillcolor(freeColor);//设置未用存储空间的填充颜色
		for (int i = QUEUE->rear; i < QUEUE->front; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}
		front.x = x, front.y = y;//获取队头元素的位置
		/* 绘制队头元素 */
		setfillcolor(headColor);//设置队头的填充颜色
		fillrectangle(x - width, y - height, x + width, y + height);
		_stprintf(text, _T("%d"), QUEUE->front);
		settextcolor(indexColor);//设置队头索引颜色
		outtextxy(x - width / 2, y - height + 5, text);
		setlinecolor(WHITE);
		line(x - width, y, x + width, y);//分隔线
		setlinecolor(borderColor);//将线的颜色调整会边框颜色
		_stprintf(text, _T("%d"), QUEUE->element[QUEUE->front]);
		settextcolor(dataColor);//设置元素的数据颜色
		outtextxy(x - width / 2, y + 5, text);
		x += width * 2 + 5;
		if (x > 1150 - width) {
			y += height * 2 + 5;
			x = 60;
		}
		/* 已使用的存储空间 */
		setfillcolor(usedColor);//设置填充颜色为紫色
		for (int i = QUEUE->front + 1; i < QUEUE->QueueSize; i++) {
			fillrectangle(x - width, y - height, x + width, y + height);
			_stprintf(text, _T("%d"), i);
			settextcolor(indexColor);//设置队头索引颜色
			outtextxy(x - width / 2, y - height + 5, text);
			setlinecolor(WHITE);
			line(x - width, y, x + width, y);//分隔线
			setlinecolor(borderColor);//将线的颜色调整会边框颜色
			_stprintf(text, _T("%d"), QUEUE->element[i]);
			settextcolor(dataColor);//设置元素的数据颜色
			outtextxy(x - width / 2, y + 5, text);
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
		}

		EndBatchDraw();

		int length = getLength(*QUEUE);
		x = 60, y = 150;
		//memcpy的复制动画
		//第一部分，front~QueueSize
		int len = QUEUE->QueueSize - QUEUE->front;
		for (int i = 0; i < len; i++) {
			BeginBatchDraw();

			printf("  新的存储空间坐标：(%d,%d)\n", x, y);
			//将移动的元素的新存储位置设置为已用部分
			setfillcolor(usedColor);//将填充颜色设置为已用空间的颜色
			fillrectangle(x - width, y - height, x + width, y + height);
			_stprintf(text, _T("%d"), i);
			settextcolor(indexColor);//设置队头索引颜色
			outtextxy(x - width / 2, y - height + 5, text);
			setlinecolor(WHITE);
			line(x - width, y, x + width, y);//分隔线
			_stprintf(text, _T("%d"), QUEUE->element[QUEUE->front + i]);
			settextcolor(dataColor);//设置元素的数据颜色
			outtextxy(x - width / 2, y + 5, text);

			//将移动的元素变成未存储部分
			setlinecolor(borderColor);//将线的颜色调整会边框颜色
			setfillcolor(freeColor); //将填充颜色设置为未用存储空间的颜色
			fillrectangle(front.x - width, front.y - height, front.x + width, front.y + height);

			/* 坐标更新 */
			x += width * 2 + 5;
			if (x > 1150 - width) {
				y += height * 2 + 5;
				x = 60;
			}
			front.x += width * 2 + 5;
			if (front.x > 1150 - width) {
				front.y += height * 2 + 5;
				front.x = 60;
			}

			EndBatchDraw();
		}
		//第二部分，0~rear
		for (int i = 0; i < QUEUE->rear; i++) {
			BeginBatchDraw();

			printf("  新的存储空间坐标：(%d,%d)\n", x, y);
			//将移动的元素的新存储位置设置为已用部分
			setfillcolor(usedColor);//将填充颜色设置为已用空间的颜色
			fillrectangle(x - width, y - height, x + width, y + height);
			_stprintf(text, _T("%d"), i);
			settextcolor(indexColor);//设置队头索引颜色
			outtextxy(x - width / 2, y - height + 5, text);
			setlinecolor(WHITE);
			line(x - width, y, x + width, y);//分隔线
			_stprintf(text, _T("%d"), QUEUE->element[i]);
			settextcolor(dataColor);//设置元素的数据颜色
			outtextxy(x - width / 2, y + 5, text);

			//更新中心坐标
			front.x += width * 2 + 5;
			if (front.x > 1150 - width) {
				front.y += height * 2 + 5;
				front.x = 60;
			}

			EndBatchDraw();
		}
			
	}
}

/* =================================== 队列可视化展示 =================================== */

//控制台，用于演示
void console() {
	int option = 6;
	while (true) {
		printf("  __________________________________________\n");
		printf("  |                                         |\n");
		printf("  |          菜单：                         |\n");
		printf("  |          退出             >> 0          |\n");
		printf("  |          创建队列         >> 1          |\n");
		printf("  |          入队             >> 2          |\n");
		printf("  |          出队             >> 3          |\n");
		printf("  |          模拟             >> 4          |\n");
		printf("  |          遍历队列         >> 5          |\n");
		printf("  |          清空队列         >> 6          |\n");
		printf("  |          设置             >> 7          |\n ");
		printf(" |_________________________________________|\n");
		printf("  >> ");
		scanf("%d", &option);
		//处理错误的输入
		char ch;
		while ((ch = getchar()) != '\n')
			continue;
		//处理输入
		if (option == 0) {
			printf("  已退出控制台。\n");
			return;
		}
		if (option > 1 && option < 7) {
			if (QUEUE == NULL) {
				printf("  队列尚未创建，请先创建队列！\n");
				system("pause > NULL");
				continue;
			}
		}
		TCHAR prompt[150];//输入框提示词
		TCHAR title[10];//输入框标题
		switch (option) {
		case 1: {
			if (QUEUE != NULL)
				printf("  队列已经创建，无法重复创建队列！\n");
			else {
				QUEUE = initQueue();
				printf("  创建队列成功，队列初始存储空间：%d\n", QUEUE_INIT_SIZE);
				visualQueue();
			}
			break;
		}
		case 2: {
			_stprintf(prompt, _T("请输入入队的数据项，允许输入多项整数。\n请将数据项用单个空格或换行进行分隔，注意首字符不能出现空格。\n取消操作请点击取消按钮。"));
			_stprintf(title, _T("入队"));
			int n = input(prompt, title);//n指代数据项数
			if (n == 0)//输入取消
				break;
			printf("  进队：\n");
			for (int i = 0; i < n; i++) {
				printf("  index = %d, element = %d\n", QUEUE->rear, token[i]);
				enQueue(*QUEUE, token[i]);
				visualQueue();//队列可视化
			}
			putchar('\n');
			break;
		}
		case 3: {
			int length = getLength(*QUEUE);
			if (isEmpty(*QUEUE)) {
				printf("  队列为空！请先向队列中添加数据。\n");
				break;
			}
			_stprintf(prompt, _T("请输入出队次数，只能输入一个正整数。\n当前最大出队次数为%d，如果输入的出队次数超过 %d，则会将队列一直出队至队列只剩一个元素。\n取消当前操作请点击取消按钮。"), length, length);
			_stprintf(title, _T("出队"));
			int n = input(prompt, title);//输入的数据存储在token[0]，n指代数据项数
			if (n == 0)//输入取消
				break;
			int len = getLength(*QUEUE);
			if (token[0] > len) {//如果出队数量超过队列长度
				printf("  出队次数将超过队列已用长度，将保留队列中最后一个元素！\n");
				token[0] = len - 1;
			}
			printf("  出队：\n");
			for (int i = 0; i < token[0]; i++) {
				visualQueue();//队列可视化
				printf("  index = %d, element = %d\n", QUEUE->front, deQueue(*QUEUE));
			}
			putchar('\n');
			break;
		}
		case 4: {
			_stprintf(prompt, _T("请输入模拟队列操作的次数，只能输入一个正整数。\n如果当前无法完成操作，程序将自动跳过该次操作。"));
			_stprintf(title, _T("队列的实际使用模拟"));
			int n = input(prompt, title);//输入的数据存储在token[0]，n指代数据项
			if (n == 0)//输入取消
				break;
			simulate(token[0]);
			printf("  %d 次队列实际使用模拟结束！\n", token[0]);
			break;
		}
		case 5: {
			if (isEmpty(*QUEUE)) {
				printf("  当前队列为空！\n");
				break;
			}
			printf("  遍历队列：\n");
			traverseQueue(*QUEUE);
			printf("\n");
			break;
		}
		case 6: {
			freeQueue(QUEUE);
			QUEUE = NULL;
			printf("  清空队列成功！\n");
			break;
		}
		case 7: {
			_stprintf(prompt,_T("请依次输入队列的初始分配量、队列闲置空间达到回收的比例(默认值50)、队列的扩容的比例(默认值200)、队列收缩的比例(默认值70)、动画的停留时长(默认值100)。\n注意所有的数据都必须为正整数，如果是比例则输入大于0的正整数。动画的停留时长的单位为毫秒。"));
			_stprintf(title, _T("设置"));
			int n = input(prompt,title);
			if (n < 5) 
				printf("  参数输入不全，此次参数修改失效。\n");
			else {
				double value[5] = { QUEUE_INIT_SIZE,QUEUE_RECYCLE_PERCENT,QUEUE_EXPAND_PERCENT,QUEUE_SHINK_PERCENT,animationSpeed };
				QUEUE_INIT_SIZE = token[0];
				QUEUE_RECYCLE_PERCENT = token[1] * 1.0 / 100;
				QUEUE_EXPAND_PERCENT = token[2] * 1.0 / 100;
				QUEUE_SHINK_PERCENT = token[3] * 1.0 / 100;
				animationSpeed = token[4];
				printf("  队列的初始分配量由 %.0f 修改为 %.d\n", value[0], QUEUE_INIT_SIZE);
				printf("  队列的闲置空间达到回收的比例由 %.1f%% 修改为 %.1f%%\n", value[1] * 100, QUEUE_RECYCLE_PERCENT * 100.0);
				printf("  队列的扩容比例由 %.1f%% 修改为 %.1f%%\n", value[2] * 100, QUEUE_EXPAND_PERCENT * 100.0);
				printf("  队列的收缩比例由 %.1f%% 修改为 %.1f%%\n", value[3] * 100, QUEUE_SHINK_PERCENT * 100.0);
				printf("  动画的停留时长由 %.0fms 修改为 %dms\n", value[4], animationSpeed);
			}
			break;
		}
			default: printf("  输入错误，请重新输入！\n");
		}
		system("pause > NULL");
	}
}

int main() {
	system("color f0");//设置控制台的背景颜色和字体颜色
	system("title 循环队列存储空间的动态回收方法");//设置控制台的标题

	//创建画布
	initgraph(1200, 800, EX_SHOWCONSOLE | EX_NOCLOSE | EX_NOMINIMIZE);//显示控制台窗口并禁用关闭按钮
	IMAGE cover;
	loadimage(&cover, _T("img\\封面.png"),1200,800,false);//加载封面
	putimage(0, 0, &cover);//设置封面

	console();
	system("pause > NULL");
	printf("  即将关闭可视化窗口...\n");
	closegraph();
	return 0;
}

