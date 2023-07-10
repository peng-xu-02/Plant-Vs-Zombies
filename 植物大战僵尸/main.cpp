/*1.创建项目，导入素材
2.创建场景*/
#include<stdio.h>
#include<graphics.h>
#include<easyx.h>
#include"tools.h"
#include<time.h>
#include<mmsystem.h>
#pragma comment(lib,"Winmm.lib")

#define win_wide 1000
#define win_height 600
#define maxsize 2 //植物数量；


int card_x;  //拖动过程的植物的当前位置；
int card_y;

int cardzhiwu;   //选择的植物；
int sunshinegrade; //定义收集的阳光值；

IMAGE bg;    //草坪；
IMAGE lan;    //植物栏；
IMAGE imagecards[maxsize];  //植物卡片类型一样；

IMAGE* zhiwu[maxsize][20]; //定义拖动的植物；

IMAGE sunball[30];     //阳光图片；

struct zhiwu {
	int type;   //植物类型，如;0就是没有。1是豌豆，2是向日葵；
	int firm;    //植物动作到了那一帧；
};

struct zhiwu map[5][10];  //定义地图，每一个都有一个植物类型；

struct sunshineball { //定义阳光结构体；
	int x, y;            //阳光生产地；
	int shineframe;  //阳光的动画显示帧序号，图片的序号；
	int destion_x, destion_y;        //阳光目的地；
	bool used;   //判断阳光池中的阳光是否被使用;
	int timer;

};

sunshineball sunshine[20]; //定义一个阳光池，后续阳光产生时直接从阳光池李取出来；

struct zm {   //僵尸;
	int x, y;
	int lifegrade;//生命值；
	int destion_x, destion_y;
	int zmframe;
	int speed;
	bool used;
};

struct zm zmchi[50]; //僵尸池；
IMAGE zm_stand[50]; //僵尸图片；

void creatsunball()
{
	static int count = 0;     
	static int fre = 100;     
	count++;
	if (count >= fre) {       //每fre帧就执行一次生产阳光；
		count = 0;
		fre = 100 + rand() % 100;
		int sunballMax = sizeof(sunshine) / sizeof(sunshine[0]);
		int i;
		for (i = 0; i < sunballMax && sunshine[i].used; i++);   //空循环，满足i<sunballMax并且找到第i个没有使用的阳光，即sunshine[i].used=0;
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
			sunshine[i].shineframe = (sunshine[i].shineframe + 1) % 29;    //阳光变化的帧数逐加并取余，方便取完之后再从头开始；
			if (sunshine[i].timer == 0) {
				sunshine[i].y += 4;
			}
			if (sunshine[i].y >= sunshine[i].destion_y) { //停留一段时间；
				sunshine[i].timer++;
				if (sunshine[i].timer > 60) {
					sunshine[i].used = false;
				}
				             
			}
		}
	}
}


void getsunshine(ExMessage* msg) {     //收取阳光；
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


bool fileexisit(const char*a) {  //判断植物的文件名是否存在；
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



void gameinit() //游戏初始化;
{
	initgraph(win_wide, win_height,1);
	loadimage(&bg, "res/bg1.jpg");
	loadimage(&lan, "res/bar.png");
	//初始化数组;
	memset(zhiwu, 0, sizeof(zhiwu));     
	memset(map, 0, sizeof(map));
	memset(sunshine, 0, sizeof(sunshine));
	memset(zmchi, 0, sizeof(zmchi));

	char name[65];
	for (int i = 0; i < maxsize; i++) {
		sprintf_s(name,sizeof(name), "res/Cards/card_%d.png",i+1);   //给name数组写入字符串，用来表示卡片的地址；
		loadimage(&imagecards[i], name);
		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			if (fileexisit(name)) {
				zhiwu[i][j] = new IMAGE;         //一定不要忘了开辟内存；
				loadimage(zhiwu[i][j], name);

			}
			else {
				break;
			}
		}
	}

	for (int i = 0; i < 29; i++) {    //加载阳光球；
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&sunball[i], name);
	}
	for (int i = 0; i < 47; i++) {   //加载僵尸；
		sprintf_s(name, sizeof(name), "res/Zombie/Zombie-%d.png", i + 1);
		loadimage(&zm_stand[i], name);
	}

	srand(time(NULL));



	cardzhiwu = 0;
	sunshinegrade = 50;

	LOGFONT f; //定义字体；
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");  //字体显示类型，如雅黑宋体；
	f.lfQuality = ANTIALIASED_QUALITY; //抗锯齿效果；
	settextstyle(&f);
	setbkmode(TRANSPARENT);    //字体背景透明;
	settextcolor(BLACK);

	
	

}

void startUI()        //开始菜单；
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
		ExMessage msg1;  //鼠标；
		if (peekmessage(&msg1)) {
			if (msg1.message == WM_LBUTTONDOWN) { //点击菜单才结束循环，进入下一个阶段；
				if (msg1.x > 470 && msg1.x < 810 && msg1.y>60 && msg1.y < 200) {
					break;

				}
			}
			else if (msg1.message == WM_MOUSEMOVE) {    //鼠标放上去变颜色；
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
void updateinit()              //更新游戏界面； 渲染；
{
	BeginBatchDraw();//双缓冲去除一直刷新闪烁；
	putimage(0, 0, &bg);
	putimage(250, 0, &lan);
	for (int i = 0; i < maxsize; i++) {
		int x = 334 + i * 58;           //植物卡片宽；
		int y = 4;
		putimage(x,y,&imagecards[i]);

	}
	//放置植物
	for(int i=0;i<5;i++)
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				int x = 252 + j * 80;   
				int y = 82 + i * 100;
				int zhiwutype = map[i][j].type - 1;        //因为文件命名是0，1；
				int index = map[i][j].firm;
				putimagePNG(x, y,zhiwu[zhiwutype][index]);
			}
		}
	//拖动植物
	if (cardzhiwu > 0) {
		putimagePNG(card_x - 30, card_y - 30, zhiwu[cardzhiwu - 1][0]); //拖动植物，减去是为了光标在中间；
	}
	//sunshine();
	int ballmax = sizeof(sunshine) / sizeof(sunshine[0]);
	for (int i = 0; i < ballmax; i++) {
		if (sunshine[i].used) {
			putimagePNG(sunshine[i].x, sunshine[i].y, &sunball[sunshine[i].shineframe]);
		}

		
	}

	char sunshinetext[8];   //输出阳光值；
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

void userbehave()     //用户点击；
{
	ExMessage msg;  //鼠标；
	
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
					int col = (msg.x - 252) / 80;       //列；
					int row = (msg.y - 78) / 100;      //行；
					if (map[row][col].type == 0) {      //该格没有植物；
						map[row][col].type = cardzhiwu;  //cardzhiwu是植物的类型，0是没有，1是豌豆；
						map[row][col].firm = 0;           //植物摇摆的图片序号；
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
			if (map[i][j].type > 0) { //有植物；
				map[i][j].firm++;           //植物摇摆图片加加；
				int zhiwutype = map[i][j].type - 1;
				int index = map[i][j].firm;
				if (zhiwu[zhiwutype][index] == NULL)
				{
					map[i][j].firm = 0;
				}


			}
		}
	}

	creatsunball();  //生产阳光；
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
		timer += getDelay();    //来计算两次用户操作的时间，如果操作时间间隔太短，就不刷新页面；因为刷新页面很耗时，而且这过程用户点击会延时；
		if (timer > 30) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			updateinit();
			updategame();

		}                 //更新植物摇摆；
		
		}
	

	
	system("pause");
	return 0;
}
