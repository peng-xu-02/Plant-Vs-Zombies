/*1.������Ŀ�������ز�
2.��������*/
#include<stdio.h>
#include<graphics.h>
#include<easyx.h>
#include"tools.h"
#include<time.h>
#include<mmsystem.h>
#pragma comment(lib,"Winmm.lib")

#define win_wide 1000
#define win_height 600
#define maxsize 2 //ֲ��������


int card_x;  //�϶����̵�ֲ��ĵ�ǰλ�ã�
int card_y;

int cardzhiwu;   //ѡ���ֲ�
int sunshinegrade; //�����ռ�������ֵ��

IMAGE bg;    //��ƺ��
IMAGE lan;    //ֲ������
IMAGE imagecards[maxsize];  //ֲ�￨Ƭ����һ����

IMAGE* zhiwu[maxsize][20]; //�����϶���ֲ�

IMAGE sunball[30];     //����ͼƬ��

struct zhiwu {
	int type;   //ֲ�����ͣ���;0����û�С�1���㶹��2�����տ���
	int firm;    //ֲ�ﶯ��������һ֡��
};

struct zhiwu map[5][10];  //�����ͼ��ÿһ������һ��ֲ�����ͣ�

struct sunshineball { //��������ṹ�壻
	int x, y;            //���������أ�
	int shineframe;  //����Ķ�����ʾ֡��ţ�ͼƬ����ţ�
	int destion_x, destion_y;        //����Ŀ�ĵأ�
	bool used;   //�ж�������е������Ƿ�ʹ��;
	int timer;

};

sunshineball sunshine[20]; //����һ������أ������������ʱֱ�Ӵ��������ȡ������

struct zm {   //��ʬ;
	int x, y;
	int lifegrade;//����ֵ��
	int destion_x, destion_y;
	int zmframe;
	int speed;
	bool used;
};

struct zm zmchi[50]; //��ʬ�أ�
IMAGE zm_stand[50]; //��ʬͼƬ��

void creatsunball()
{
	static int count = 0;     
	static int fre = 100;     
	count++;
	if (count >= fre) {       //ÿfre֡��ִ��һ���������⣻
		count = 0;
		fre = 100 + rand() % 100;
		int sunballMax = sizeof(sunshine) / sizeof(sunshine[0]);
		int i;
		for (i = 0; i < sunballMax && sunshine[i].used; i++);   //��ѭ��������i<sunballMax�����ҵ���i��û��ʹ�õ����⣬��sunshine[i].used=0;
		if (i > sunballMax)   return;
		sunshine[i].used = true;
		sunshine[i].shineframe = 0;
		sunshine[i].x = 260 + rand() % 710;
		sunshine[i].y = 20;
		sunshine[i].destion_x = sunshine[i].x;
		sunshine[i].destion_y = 180 + rand() % 300;
		sunshine[i].timer = 0;
	}
	

}

void updatesunshine()
{
	int ballMax = sizeof(sunshine) / sizeof(sunshine[0]);
	for (int i = 0; i < ballMax; i++) {
		if (sunshine[i].used == true) {
			sunshine[i].shineframe = (sunshine[i].shineframe + 1) % 29;    //����仯��֡����Ӳ�ȡ�࣬����ȡ��֮���ٴ�ͷ��ʼ��
			if (sunshine[i].timer == 0) {
				sunshine[i].y += 4;
			}
			if (sunshine[i].y >= sunshine[i].destion_y) { //ͣ��һ��ʱ�䣻
				sunshine[i].timer++;
				if (sunshine[i].timer > 60) {
					sunshine[i].used = false;
				}
				             
			}
		}
	}
}


void getsunshine(ExMessage* msg) {     //��ȡ���⣻
	int ballMax = sizeof(sunshine) / sizeof(sunshine[0]);
	for (int i = 0; i < ballMax; i++) {
		if (sunshine[i].used) {
			if (msg->x > sunshine[i].x && msg->x<sunshine[i].x + 80 && msg->y>sunshine[i].y && msg->y < sunshine[i].y + 80) {
				sunshine[i].used = false;
				sunshinegrade += 25;
				//mciSendString("play res/sunshine.mp3", 0, 0, 0);
			}
		}
	}
}

void creatzm() {
	int zmmax = sizeof(zmchi) / sizeof(zmchi[0]);
	static int frame = 100;
	static int count = 0;
	count++;
	if (count >= frame) {
		count = 0;
		frame = 100 + rand() % 100;
		int i = 0;
		for (i = 0; i < zmmax && zmchi[i].used; i++);
		if (i < zmmax) {
			zmchi[i].used = true;
			zmchi[i].x = win_wide;
			zmchi[i].y = 120 + (  rand() % 5) * 100;
			zmchi[i].speed = 1;
			zmchi[i].zmframe = 0;
		}
	}
	
}

void updatezm()
{
	int zmmax = sizeof(zmchi) / sizeof(zmchi[0]);
	for (int i = 0; i < zmmax; i++) {
		if (zmchi[i].used) {
			zmchi[i].x -= zmchi[i].speed;
			if (zmchi[i].x <= 100) {
				zmchi[i].used = false;
			}
		}
	}
	for (int i = 0; i< zmmax; i++) {
		if (zmchi[i].used) {
			zmchi[i].zmframe = (zmchi[i].zmframe + 1) % 47;
		}
	}
}


bool fileexisit(const char*a) {  //�ж�ֲ����ļ����Ƿ���ڣ�
	FILE *fp;
	fp = fopen(a,"r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}



void gameinit() //��Ϸ��ʼ��;
{
	initgraph(win_wide, win_height,1);
	loadimage(&bg, "res/bg1.jpg");
	loadimage(&lan, "res/bar.png");
	//��ʼ������;
	memset(zhiwu, 0, sizeof(zhiwu));     
	memset(map, 0, sizeof(map));
	memset(sunshine, 0, sizeof(sunshine));
	memset(zmchi, 0, sizeof(zmchi));

	char name[65];
	for (int i = 0; i < maxsize; i++) {
		sprintf_s(name,sizeof(name), "res/Cards/card_%d.png",i+1);   //��name����д���ַ�����������ʾ��Ƭ�ĵ�ַ��
		loadimage(&imagecards[i], name);
		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			if (fileexisit(name)) {
				zhiwu[i][j] = new IMAGE;         //һ����Ҫ���˿����ڴ棻
				loadimage(zhiwu[i][j], name);

			}
			else {
				break;
			}
		}
	}

	for (int i = 0; i < 29; i++) {    //����������
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&sunball[i], name);
	}
	for (int i = 0; i < 47; i++) {   //���ؽ�ʬ��
		sprintf_s(name, sizeof(name), "res/Zombie/Zombie-%d.png", i + 1);
		loadimage(&zm_stand[i], name);
	}

	srand(time(NULL));



	cardzhiwu = 0;
	sunshinegrade = 50;

	LOGFONT f; //�������壻
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");  //������ʾ���ͣ����ź����壻
	f.lfQuality = ANTIALIASED_QUALITY; //�����Ч����
	settextstyle(&f);
	setbkmode(TRANSPARENT);    //���屳��͸��;
	settextcolor(BLACK);

	
	

}

void startUI()        //��ʼ�˵���
{
	//initgraph(win_wide, win_height);
	IMAGE menu;
	IMAGE menu1;
	IMAGE menu2;
	BeginBatchDraw();
	loadimage(&menu, "res/menu.png");
	loadimage(&menu1, "res/menu1.png");
	loadimage(&menu2, "res/menu2.png");
	putimage(0, 0, &menu);
	putimagePNG(470, 60, &menu1);
	EndBatchDraw();
	while (1) {
		ExMessage msg1;  //��ꣻ
		if (peekmessage(&msg1)) {
			if (msg1.message == WM_LBUTTONDOWN) { //����˵��Ž���ѭ����������һ���׶Σ�
				if (msg1.x > 470 && msg1.x < 810 && msg1.y>60 && msg1.y < 200) {
					break;

				}
			}
			else if (msg1.message == WM_MOUSEMOVE) {    //������ȥ����ɫ��
				if (msg1.x > 470 && msg1.x < 810 && msg1.y>60 && msg1.y < 200) {
					putimagePNG(470, 60, &menu2);
				}
				else {
					putimagePNG(470, 60, &menu1);

				}
			}

		}
	}
}
void updateinit()              //������Ϸ���棻 ��Ⱦ��
{
	BeginBatchDraw();//˫����ȥ��һֱˢ����˸��
	putimage(0, 0, &bg);
	putimage(250, 0, &lan);
	for (int i = 0; i < maxsize; i++) {
		int x = 334 + i * 58;           //ֲ�￨Ƭ��
		int y = 4;
		putimage(x,y,&imagecards[i]);

	}
	//����ֲ��
	for(int i=0;i<5;i++)
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				int x = 252 + j * 80;   
				int y = 82 + i * 100;
				int zhiwutype = map[i][j].type - 1;        //��Ϊ�ļ�������0��1��
				int index = map[i][j].firm;
				putimagePNG(x, y,zhiwu[zhiwutype][index]);
			}
		}
	//�϶�ֲ��
	if (cardzhiwu > 0) {
		putimagePNG(card_x - 30, card_y - 30, zhiwu[cardzhiwu - 1][0]); //�϶�ֲ���ȥ��Ϊ�˹�����м䣻
	}
	//sunshine();
	int ballmax = sizeof(sunshine) / sizeof(sunshine[0]);
	for (int i = 0; i < ballmax; i++) {
		if (sunshine[i].used) {
			putimagePNG(sunshine[i].x, sunshine[i].y, &sunball[sunshine[i].shineframe]);
		}

		
	}

	char sunshinetext[8];   //�������ֵ��
	sprintf_s(sunshinetext, sizeof(sunshinetext), "%d", sunshinegrade);
	outtextxy(270, 60, sunshinetext);

	int zmmax = sizeof(zmchi) / sizeof(zmchi[0]);
	for (int i = 0; i < zmmax; i++) {
		if (zmchi[i].used) {
			putimagePNG(zmchi[i].x, zmchi[i].y, &zm_stand[zmchi[i].zmframe]);
		}
	}



	EndBatchDraw();
	
	
}

void userbehave()     //�û������
{
	ExMessage msg;  //��ꣻ
	
	 static int stack = 0;
	if (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN) {
				if (msg.x > 334 && msg.x < 334 + maxsize * 58 && msg.y < 96) {
					int index = (msg.x - 334) / 58;
					stack = 1;
					cardzhiwu = index + 1;
				

				}
				else {
					getsunshine(&msg);
					
				}
			}
			else if (msg.message == WM_MOUSEMOVE && stack == 1) {
				card_x = msg.x;
				card_y = msg.y;

			}
			else if (msg.message == WM_LBUTTONUP) {
				if (msg.y < 580 && msg.x>252 && msg.y >76) {
					int col = (msg.x - 252) / 80;       //�У�
					int row = (msg.y - 78) / 100;      //�У�
					if (map[row][col].type == 0) {      //�ø�û��ֲ�
						map[row][col].type = cardzhiwu;  //cardzhiwu��ֲ������ͣ�0��û�У�1���㶹��
						map[row][col].firm = 0;           //ֲ��ҡ�ڵ�ͼƬ��ţ�
					}


				}
				cardzhiwu = 0;
				stack = 0;

			}
	}
}
void updategame() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) { //��ֲ�
				map[i][j].firm++;           //ֲ��ҡ��ͼƬ�Ӽӣ�
				int zhiwutype = map[i][j].type - 1;
				int index = map[i][j].firm;
				if (zhiwu[zhiwutype][index] == NULL)
				{
					map[i][j].firm = 0;
				}


			}
		}
	}

	creatsunball();  //�������⣻
	updatesunshine();
	creatzm();
	updatezm();
}



int main()
{
	gameinit();
	startUI();
	int timer=0;
	bool flag = true;
	while (true) {
		userbehave();
		timer += getDelay();    //�����������û�������ʱ�䣬�������ʱ����̫�̣��Ͳ�ˢ��ҳ�棻��Ϊˢ��ҳ��ܺ�ʱ������������û��������ʱ��
		if (timer > 30) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			updateinit();
			updategame();

		}                 //����ֲ��ҡ�ڣ�
		
		}
	

	
	system("pause");
	return 0;
}
