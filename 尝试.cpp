#include<graphics.h>
#include<iostream>
#include<ctime>
#include<cstdlib>
#include<vector>
#include<algorithm>
#include<cmath>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

// 音乐文件路径定义（使用多字节字符）
const char* MAIN_MENU_MUSIC = "D:/CloudMusic/主界面.mp3";
const char* LEVEL_SELECT_MUSIC = "D:/CloudMusic/关卡选择.mp3";
const char* GAME_MUSIC = "D:/CloudMusic/游戏界面.mp3";
const char* BULLET_SOUND = "C:/Users/lenovo/Downloads/发射子弹.wav";
const char* HIT_SOUND = "C:/Users/lenovo/Downloads/子弹击中树.wav";
const char* COLLISION_SOUND = "C:/Users/lenovo/Downloads/球碰撞.wav";
const char* JUMP_SOUND = "C:/Users/lenovo/Downloads/跳与蹲.wav";
const char* BUTTON_CLICK_SOUND = "C:/Users/lenovo/Downloads/按键.wav";

// 常量定义
#define WIDTH    300
#define LENGTH   300
#define WIDTH1   200
#define LENGTH1  200
#define WIDTH2   600
#define LENGTH2  600
#define WIDTH3   1400
#define LENGTH3  800
class bullet;//子弹//
class Obstacle;//障碍物//
// 全局变量
IMAGE ball1[13];        // 关卡一的小球
IMAGE imgMask1[10];     // 小球掩码图
IMAGE img;              // 森林背景
IMAGE imgmask2[4];      // 障碍物掩码
IMAGE img1[4];          // 障碍物前景图
IMAGE mainImage1;//主页面//
IMAGE mainImage2;//选择关卡的画面//
IMAGE boom[2];//碰撞效果//
IMAGE gameOverBackground;//关卡一界面
bool isDialogShowing = false;    // 标记对话框是否显示
bool dialogResult = false; // 对话框结果（true=确认退出，false=取消）
bool isDialogShowgameover = false;    // 标记游戏结束对话框是否显示
bool dialogResultgameover = false;    // 游戏结束对话框结果
bool gameOver = false;               // 游戏结束状态标记
IMAGE dialogBackground;          // 保存对话框背景
bool running = true;
bool fact1;
bool fact2;
bool con = true;
int score = 0;//分数//
int highscore = 0;//历史最高分数//
// 当前音乐别名
char currentMusicAlias[50] = "";

// 定义小球结构体
struct rou {
    int x;
    int y;
    int r;
};
rou circle1 = { 100, 650, 40 }; // 初始化小球结构体

// 定义爆炸效果结构体
struct bom {
    int x;
    int y;
    bool active;
    int frame;
};

// 背景音乐播放函数（使用多字节字符）
void PlayBackgroundMusic(const char* filePath) {
    // 停止并关闭当前音乐
    if (strlen(currentMusicAlias) > 0) {
        char stopCmd[100];
        sprintf_s(stopCmd, "stop %s", currentMusicAlias);
        mciSendString(stopCmd, NULL, 0, NULL);

        char closeCmd[100];
        sprintf_s(closeCmd, "close %s", currentMusicAlias);
        mciSendString(closeCmd, NULL, 0, NULL);
    }

    // 生成新别名
    static int musicCount = 0;
    sprintf_s(currentMusicAlias, "music_%d",  ++musicCount);

    // 打开并播放新音乐
    char openCmd[200];
    sprintf_s(openCmd, "open \"%s\" alias %s", filePath, currentMusicAlias);
    mciSendString(openCmd, NULL, 0, NULL);

    char playCmd[100];
    sprintf_s(playCmd, "play %s repeat", currentMusicAlias);
    mciSendString(playCmd, NULL, 0, NULL);
}

// 播放音效（使用多字节字符）
void PlaySoundEffect(const char* filePath) 
{
    PlaySound(filePath, NULL, SND_FILENAME | SND_ASYNC);
}

//子弹//
class bullet {
private:
    int bx;//横坐标//
    int by;//纵坐标//
    int bulletspeed;//子弹移动速度//
    int r;//子弹大小// 
public:
    bool active;
    bullet(int x1 = 170, int y1 = 690, int bulletspeed1 = 18, int r1 = 10) :active(true)
    {
        bx = x1;
        by = y1;
        bulletspeed = bulletspeed1;
        r = r1;
    }
    int getx()const { return bx; }
    int gety()const { return by; }
    int getbulletspeed()const { return bulletspeed; }
    int getr()const { return r; }
    bool getactive()const { return active; }
    void update() {
        bx += bulletspeed;
        if (bx - r > WIDTH3) {
            active = false;
        }

    }
    void draw() {
        if (!active) return;
        setfillcolor(RGB(120, 200, 255));
        solidcircle(bx, by, r);
    }
};
// 障碍物类（调整速度和高度）
class Obstacle {
public:
    int x;
    int y;
    int width;
    int height;
    int type; // 0:树, 1:猫, 2:飞叶, 3:钻石
    int speed;
    bool active;

    Obstacle(int type = 0) : type(type), active(true) {
        switch (type) {
        case 0: // 树
            x = WIDTH3;
            y = 525;
            width = 200;
            height = 210;
            speed = 6;
            break;
        case 1: // 猫
            x = WIDTH3;
            y = 678;
            width = 50;
            height = 60;
            speed = 10;
            break;
        case 2: // 飞叶
            x = WIDTH3;
            y = 555;
            width = 60;
            height = 120;
            speed = 14;     // 速度提高
            break;
        case 3: // 钻石
            x = WIDTH3;
            y = 630 - rand() % 60;
            width = 40;
            height = 50;
            speed = 6;
            break;
        }
    }
    void update() {
        x -= speed;
        if (x + width < 0) {
            active = false;
        }
        //将障碍物也看成圆，碰撞简化为两圆相交//
        double circleCenterX = circle1.x + 40; //球心
        double circleCenterY = circle1.y + 40; // 球心
        double obsCenterX = x + width / 2.0;   // 障碍物中心x
        double obsCenterY = y + height / 2.0;  // 障碍物中心y

        double dx = obsCenterX - circleCenterX;
        double dy = obsCenterY - circleCenterY;
        double dist = sqrt(dx * dx + dy * dy); // 使用实际距离

        double obsRadius;
        switch (type) {
        case 0: obsRadius = width / 4.0; break;  // 树：树干较窄
        case 1: obsRadius = width / 2.0; break;  // 猫：体型小
        case 2: obsRadius = height / 3.0; break; // 飞叶：细长
        case 3: obsRadius = width / 2.0; break;  // 钻石：小体型
        default: obsRadius = width / 2.0;
        }
        double sumRadius = obsRadius + circle1.r; // 小球半径（40）+ 障碍物半径

        // 碰撞检测 - 使用实际距离
        if (dist < sumRadius && type != 3) { // 钻石不会导致失败
            active = false;
            gameOver = true; // 设置游戏结束标志
            PlaySoundEffect(COLLISION_SOUND); // 播放碰撞音效
        }
        if (dist < sumRadius && type == 3) { // 钻石不会导致失败
            active = false;
            score++;
            if (score > highscore)
            {
                highscore = score;
            }
        }
    }
    void draw() {
        if (!active) return;
        switch (type) {
        case 0:
            putimage(x, y, imgmask2 + 2, SRCAND);
            putimage(x, y, img1 + 2, SRCPAINT);
            break;
        case 1:
            putimage(x, y, imgmask2 + 3, SRCAND);
            putimage(x, y, img1 + 3, SRCPAINT);
            break;
        case 2:
            putimage(x, y, imgmask2, SRCAND);
            putimage(x, y, img1, SRCPAINT);
            break;
        case 3:
            putimage(x, y, imgmask2 + 1, SRCAND);
            putimage(x, y, img1 + 1, SRCPAINT);
            break;
        }
    }
};
vector<Obstacle> obstacles;//障碍物//
vector<bullet>bul;//子弹//
vector<bom>exploration;//爆炸效果//
int lastSpawnTime = 0;
const int minSpawnInterval = 1500;
const int maxSpawnInterval = 3500;
int nextSpawnTime = minSpawnInterval;

// 函数声明
void init();
void getmouse();
void playgame1();
void levelselection();
void showout();
void loadresource1();
void yabian();
void moveup();
void generatebullet();//生成子弹//
void updateExplosions();//更新爆炸效果//
void drawExplosions();//绘画爆炸效果//
void handleReturnDialog();//关卡内返回键//
void showgameover();//失败界面//
void CleanupMusic(); // 清理音乐资源

int main() {
    initgraph(WIDTH, LENGTH);
    BeginBatchDraw();

    // 播放主菜单音乐
    

    while (running) {
        init();
        getmouse();
        FlushBatchDraw();
        Sleep(16);
    }

    // 清理音乐资源
    CleanupMusic();

    EndBatchDraw();
    closegraph();
    return 0;
}

// 清理音乐资源
void CleanupMusic() {
    mciSendString("close all", NULL, 0, NULL);
    strcpy_s(currentMusicAlias, "");
}

void init() {
    setbkcolor(RGB(230, 210, 170));
    cleardevice();
    setfillcolor(RGB(200, 160, 110));
    solidrectangle(75, 100, 225, 140);
    solidrectangle(75, 160, 225, 200);

    settextcolor(RGB(153, 102, 51));
    setbkmode(TRANSPARENT);
    settextstyle(50, 0, "楷体");
    outtextxy(30, 20, "滚动吧球球");
    settextstyle(25, 0, "楷体");
    outtextxy(102, 110, "开始游戏");
    outtextxy(102, 170, "退出游戏");
    getimage(&mainImage1, 0, 0, WIDTH, LENGTH);
    PlayBackgroundMusic(MAIN_MENU_MUSIC);
    FlushBatchDraw();
}

void getmouse() {
    while (1) {
        
        MOUSEMSG msg = GetMouseMsg();

        if (msg.x >= 75 && msg.x <= 225 && msg.y <= 140 && msg.y >= 100) {
            fact1 = true;
        }
        if (msg.x >= 75 && msg.x <= 225 && msg.y <= 200 && msg.y >= 160) {
            fact2 = true;
        }

        if (fact1) {
            setlinecolor(RED);
            rectangle(70, 95, 230, 145);
            fact1 = false;
            if (msg.uMsg == WM_LBUTTONDOWN) {
                PlaySoundEffect(BUTTON_CLICK_SOUND);
                levelselection();
            }
        }
        else if (fact2) {
            setlinecolor(RED);
            rectangle(70, 155, 230, 205);
            fact2 = false;
            if (msg.uMsg == WM_LBUTTONDOWN) {
                PlaySoundEffect(BUTTON_CLICK_SOUND);
                showout();
            }
        }
        else {
            setlinecolor(RGB(230, 210, 170));
            rectangle(70, 155, 230, 205);
            rectangle(70, 95, 230, 145);
        }
        
        FlushBatchDraw();
    }
}
void showout() {
    initgraph(WIDTH1, LENGTH1);
    setbkcolor(RGB(230, 210, 170));
    cleardevice();
    settextstyle(40, 0, "楷体");
    settextcolor(RGB(153, 102, 51));
    setbkmode(TRANSPARENT);
    outtextxy(30, 50, "是否退出");
    settextstyle(25, 0, "楷体");
    setfillcolor(RED);
    solidrectangle(45, 125, 95, 165);
    settextcolor(WHITE);
    outtextxy(46, 135, "确认");
    setfillcolor(BLUE);
    solidrectangle(115, 125, 165, 165);
    outtextxy(116, 135, "取消");

    while (1) {
        MOUSEMSG msg = GetMouseMsg();

        if (msg.x >= 45 && msg.x <= 95 && msg.y >= 125 && msg.y <= 165) {
            setfillcolor(RGB(139, 0, 0));
            solidrectangle(45, 125, 95, 165);
            outtextxy(46, 135, "确认");
            if (msg.uMsg == WM_LBUTTONDOWN) {
                PlaySoundEffect(BUTTON_CLICK_SOUND);
                running = false;
                closegraph();
                return;
            }
        }
        else {
            setfillcolor(RED);
            solidrectangle(45, 125, 95, 165);
            outtextxy(46, 135, "确认");
        }

        if (msg.x >= 115 && msg.x <= 165 && msg.y >= 125 && msg.y <= 165) {
            setfillcolor(RGB(0, 0, 139));
            solidrectangle(115, 125, 165, 165);
            outtextxy(116, 135, "取消");
            if (msg.uMsg == WM_LBUTTONDOWN) {
                PlaySoundEffect(BUTTON_CLICK_SOUND);
                initgraph(WIDTH, LENGTH);
                putimage(0, 0, &mainImage1);
                return;
            }
        }
        else {
            setfillcolor(BLUE);
            solidrectangle(115, 125, 165, 165);
            outtextxy(116, 135, "取消");
        }
        FlushBatchDraw();
    }
}

void levelselection() {
    // 播放关卡选择音乐
    PlayBackgroundMusic(LEVEL_SELECT_MUSIC);

    initgraph(WIDTH2, LENGTH2);
    setbkcolor(RGB(230, 210, 170));
    cleardevice();
    settextcolor(RGB(153, 102, 51));
    setbkmode(TRANSPARENT);
    settextstyle(60, 0, "楷体");
    outtextxy(190, 150, "关卡选择");
    setfillcolor(RGB(200, 160, 110));
    solidrectangle(410, 250, 510, 350);
    solidrectangle(260, 250, 360, 350);
    solidrectangle(110, 250, 210, 350);
    solidrectangle(10, 10, 90, 50);
    solidrectangle(0, 550, 600, 600);
    settextstyle(40, 0, "楷体");
    outtextxy(150, 260, "1");
    outtextxy(300, 260, "2");
    outtextxy(450, 260, "3");
    settextstyle(40, 0, "楷体");
    outtextxy(120, 305, "森林");
    outtextxy(270, 305, "雪山");
    outtextxy(420, 305, "熔岩");
    settextstyle(30, 0, "楷体");
    outtextxy(20, 15, "返回");
    getimage(&mainImage2, 0, 0, WIDTH2, LENGTH2);
    FlushBatchDraw();
    while (1) {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.x >= 110 && msg.x <= 210 && msg.y >= 250 && msg.y <= 350) {
            if (msg.uMsg == WM_LBUTTONDOWN) {
                PlaySoundEffect(BUTTON_CLICK_SOUND);
                playgame1();
            }
        }
        if (msg.x >= 10 && msg.x <= 90 && msg.y >= 10 && msg.y <= 50) {
            if (msg.uMsg == WM_LBUTTONDOWN) {
                // 返回主菜单时播放主菜单音乐
                PlaySoundEffect(BUTTON_CLICK_SOUND);
                PlayBackgroundMusic(MAIN_MENU_MUSIC);

                initgraph(WIDTH, LENGTH);
                putimage(0, 0, &mainImage1);
                return;
            }
        }
        FlushBatchDraw();
    }
    closegraph();
}

void loadresource1() {
    loadimage(ball1, "D:/picture-game/球1.png", 80, 80, true);
    loadimage(ball1 + 1, "D:/picture-game/球2.png", 80, 80, true);
    loadimage(ball1 + 2, "D:/picture-game/球3.png", 80, 80, true);
    loadimage(ball1 + 3, "D:/picture-game/球4.png", 80, 80, true);
    loadimage(ball1 + 4, "D:/picture-game/球5.png", 80, 80, true);
    loadimage(ball1 + 5, "D:/picture-game/球6.png", 80, 80, true);
    loadimage(ball1 + 6, "D:/picture-game/球7.png", 80, 80, true);
    loadimage(ball1 + 7, "D:/picture-game/球8.png", 80, 80, true);
    loadimage(ball1 + 8, "D:/picture-game/球9.png", 80, 80, true);
    loadimage(imgMask1, "D:/picture-game/球1掩码图.jpg", 80, 80, true);
    loadimage(ball1 + 9, "D:/picture-game/压扁.png", 80, 70, true);
    loadimage(imgMask1 + 1, "D:/picture-game/球2掩码图.jpg", 80, 70, true);
    loadimage(ball1 + 10, "D:/picture-game/压扁1.png", 80, 60, true);
    loadimage(imgMask1 + 2, "D:/picture-game/球3掩码图.jpg", 80, 60, true);
    loadimage(ball1 + 11, "D:/picture-game/压扁2.png", 80, 50, true);
    loadimage(imgMask1 + 3, "D:/picture-game/球4掩码图.jpg", 80, 50, true);
    loadimage(ball1 + 12, "D:/picture-game/压扁3.png", 80, 40, true);
    loadimage(imgMask1 + 4, "D:/picture-game/球5掩码图.jpg", 80, 40, true);

    loadimage(img1, "D:/picture-game/飞叶前景图.png", 60, 120, true);
    loadimage(imgmask2, "D:/picture-game/飞叶掩码图.jpg", 60, 120, true);
    loadimage(imgmask2 + 1, "D:/picture-game/钻石掩码图.jpg", 40, 50, true);
    loadimage(img1 + 1, "D:/picture-game/钻石前景图.png", 40, 50, true);
    loadimage(imgmask2 + 2, "D:/picture-game/树掩码图.jpg", 200, 210, true);
    loadimage(img1 + 2, "D:/picture-game/树前景图.png", 200, 210, true);
    loadimage(imgmask2 + 3, "D:/picture-game/猫掩码图.jpg", 50, 60, true);
    loadimage(img1 + 3, "D:/picture-game/猫前景图.png", 50, 60, true);
    loadimage(boom, "D:/picture-game/爆炸前景图.png", 30, 30, true);
    loadimage(boom + 1, "D:/picture-game/爆炸.jpg", 30, 30, true);

}

// 下蹲动画（增加下蹲幅度，确保穿过矮障碍物）
void yabian() {
    PlaySoundEffect(JUMP_SOUND);
    int i = 1;
    int j = 4;
    while (i <= 4) {
        // 修复重影：清除屏幕
        cleardevice();

        putimage(0, 0, &img);
        setfillcolor(RGB(101, 67, 33));
        solidrectangle(0, 730, WIDTH3, LENGTH3);

        // 更新绘制障碍物
        for (auto& obstacle : obstacles) {
            obstacle.update();
            obstacle.draw();
        }
        for (auto& bu : bul) {
            bu.update();
            bu.draw();
        }
        setfillcolor(RGB(101, 67, 33));
        solidrectangle(10, 10, 90, 50);
        solidrectangle(WIDTH3 - 110, 10, WIDTH3 - 10, 50);
        settextcolor(RGB(255, 220, 10));
        setbkmode(TRANSPARENT);
        settextstyle(30, 0, "楷体");
        outtextxy(20, 15, "返回");
        solidrectangle(570, 10, 695, 60);
        solidrectangle(710, 10, 860, 60);
        char scoreStr[50];
        sprintf_s(scoreStr, "分数: %d", score);
        outtextxy(580, 20, scoreStr);
        char highScoreStr[50];
        sprintf_s(highScoreStr, "最高分: %d", highscore);
        outtextxy(720, 20, highScoreStr);
        outtextxy(WIDTH3 - 105, 15, "暂停/P");
        obstacles.erase(
            remove_if(obstacles.begin(), obstacles.end(),
                [](const Obstacle& o) { return !o.active; }),
            obstacles.end()
        );
        bul.erase(
            remove_if(bul.begin(), bul.end(),
                [](const bullet& o) { return !o.getactive(); }),
            bul.end()
        );
        circle1.y += 10;
        putimage(circle1.x, circle1.y, imgMask1 + i, SRCAND);
        putimage(circle1.x, circle1.y, ball1 + i + 8, SRCPAINT);
        FlushBatchDraw(); // 确保刷新屏幕
        i++;
        Sleep(50);
    }
    //起//
    while (j >= 1) {
        // 修复重影：清除屏幕
        cleardevice();

        putimage(0, 0, &img);
        setfillcolor(RGB(101, 67, 33));
        solidrectangle(0, 730, WIDTH3, LENGTH3);

        // 更新绘制障碍物
        for (auto& obstacle : obstacles) {
            obstacle.update();
            obstacle.draw();
        }
        for (auto& bu : bul) {
            bu.update();
            bu.draw();
        }
        setfillcolor(RGB(101, 67, 33));
        solidrectangle(10, 10, 90, 50);
        solidrectangle(WIDTH3 - 110, 10, WIDTH3 - 10, 50);
        settextcolor(RGB(255, 220, 10));
        setbkmode(TRANSPARENT);
        settextstyle(30, 0, "楷体");
        outtextxy(20, 15, "返回");
        solidrectangle(570, 10, 695, 60);
        solidrectangle(710, 10, 860, 60);
        char scoreStr[50];
        sprintf_s(scoreStr, "分数: %d", score);
        outtextxy(580, 20, scoreStr);
        char highScoreStr[50];
        sprintf_s(highScoreStr, "最高分: %d", highscore);
        outtextxy(720, 20, highScoreStr);
        outtextxy(WIDTH3 - 105, 15, "暂停/P");
        obstacles.erase(
            remove_if(obstacles.begin(), obstacles.end(),
                [](const Obstacle& o) { return !o.active; }),
            obstacles.end()
        );
        bul.erase(
            remove_if(bul.begin(), bul.end(),
                [](const bullet& o) { return !o.getactive(); }),
            bul.end()
        );
        putimage(circle1.x, circle1.y, imgMask1 + j, SRCAND);
        putimage(circle1.x, circle1.y, ball1 + j + 8, SRCPAINT);
        circle1.y -= 10;
        FlushBatchDraw(); // 确保刷新屏幕
        j--;
        Sleep(50);
    }
}

// 跳跃动画（增加跳跃高度，确保越过高障碍物）
void moveup() {
    // 播放跳跃音效
    PlaySoundEffect(JUMP_SOUND);

    int i;
    // 上升阶段（跳得更高）
    for (i = 1; i <= 8; i++) {
        // 修复重影：清除屏幕
        cleardevice();

        putimage(0, 0, &img);
        setfillcolor(RGB(101, 67, 33));
        solidrectangle(0, 730, WIDTH3, LENGTH3);

        // 更新绘制障碍物
        for (auto& obstacle : obstacles) {
            obstacle.update();
            obstacle.draw();
        }
        for (auto& bu : bul) {
            bu.update();
            bu.draw();
        }
        setfillcolor(RGB(101, 67, 33));
        solidrectangle(10, 10, 90, 50);
        solidrectangle(WIDTH3 - 110, 10, WIDTH3 - 10, 50);
        settextcolor(RGB(255, 220, 10));
        setbkmode(TRANSPARENT);
        settextstyle(30, 0, "楷体");
        outtextxy(20, 15, "返回");
        solidrectangle(570, 10, 695, 60);
        solidrectangle(710, 10, 860, 60);
        char scoreStr[50];
        sprintf_s(scoreStr, "分数: %d", score);
        outtextxy(580, 20, scoreStr);
        char highScoreStr[50];
        sprintf_s(highScoreStr, "最高分: %d", highscore);
        outtextxy(720, 20, highScoreStr);
        outtextxy(WIDTH3 - 105, 15, "暂停/P");
        obstacles.erase(
            remove_if(obstacles.begin(), obstacles.end(),
                [](const Obstacle& o) { return !o.active; }),
            obstacles.end()
        );
        bul.erase(
            remove_if(bul.begin(), bul.end(),
                [](const bullet& o) { return !o.getactive(); }),
            bul.end()
        );
        // 上升高度增加（y坐标减少更多）
        circle1.y -= 25;
        putimage(circle1.x, circle1.y, imgMask1, SRCAND);
        putimage(circle1.x, circle1.y, ball1 + i, SRCPAINT);
        FlushBatchDraw(); // 确保刷新屏幕
        Sleep(35);
    }
    // 下降阶段
    for (i = 1; i <= 8; i++) {
        // 修复重影：清除屏幕
        cleardevice();

        putimage(0, 0, &img);
        setfillcolor(RGB(101, 67, 33));
        solidrectangle(0, 730, WIDTH3, LENGTH3);
        circle1.y += 25;
        // 更新绘制障碍物
        for (auto& obstacle : obstacles) {
            obstacle.update();
            obstacle.draw();
        }
        for (auto& bu : bul) {
            bu.update();
            bu.draw();
        }
        setfillcolor(RGB(101, 67, 33));
        solidrectangle(10, 10, 90, 50);
        solidrectangle(WIDTH3 - 110, 10, WIDTH3 - 10, 50);
        settextcolor(RGB(255, 220, 10));
        setbkmode(TRANSPARENT);
        settextstyle(30, 0, "楷体");
        outtextxy(20, 15, "返回");
        solidrectangle(570, 10, 695, 60);
        solidrectangle(710, 10, 860, 60);
        char scoreStr[50];
        sprintf_s(scoreStr, "分数: %d", score);
        outtextxy(580, 20, scoreStr);
        char highScoreStr[50];
        sprintf_s(highScoreStr, "最高分: %d", highscore);
        outtextxy(720, 20, highScoreStr);
        outtextxy(WIDTH3 - 105, 15, "暂停/P");
        obstacles.erase(
            remove_if(obstacles.begin(), obstacles.end(),
                [](const Obstacle& o) { return !o.active; }),
            obstacles.end()
        );
        bul.erase(
            remove_if(bul.begin(), bul.end(),
                [](const bullet& o) { return !o.getactive(); }),
            bul.end()
        );

        putimage(circle1.x, circle1.y, imgMask1, SRCAND);
        putimage(circle1.x, circle1.y, ball1 + i, SRCPAINT);
        FlushBatchDraw(); // 确保刷新屏幕
        Sleep(35);
    }
}

// 游戏主逻辑
void playgame1() {
    // 播放游戏音乐
    PlayBackgroundMusic(GAME_MUSIC);
    cleardevice();
    putimage(0, 0, &img);
    isDialogShowing = false;
    dialogResult = false;
    isDialogShowgameover = false;
    dialogResultgameover = false;
    gameOver = false;
    con = true;
    score = 0;
    // 清空游戏对象
    obstacles.clear();
    bul.clear();
    exploration.clear();

    // 重置小球位置
    circle1.x = 100;
    circle1.y = 650;
    circle1.r = 40;
    loadresource1();
    initgraph(WIDTH3, LENGTH3);
    srand(time(0));
    BeginBatchDraw();

    loadimage(&img, "D:/picture-game/森林背景.jpg", 1400, 800);
    lastSpawnTime = GetTickCount();
    nextSpawnTime = minSpawnInterval + rand() % (maxSpawnInterval - minSpawnInterval);

    int frameCounter = 0;
    static int ballFrame = 0;
    bool firstShowGameOver = true;

    while (true) {
        cleardevice();
        putimage(0, 0, &img);
        setfillcolor(RGB(101, 67, 33));
        solidrectangle(0, 730, WIDTH3, LENGTH3);
        solidrectangle(10, 10, 90, 50);
        solidrectangle(570, 10, 695, 60);
        solidrectangle(710, 10, 860, 60);
        solidrectangle(WIDTH3 - 110, 10, WIDTH3 - 10, 50);
        settextcolor(RGB(255, 220, 10));
        setbkmode(TRANSPARENT);
        settextstyle(30, 0, "楷体");
        outtextxy(20, 15, "返回");
        char scoreStr[50];
        sprintf_s(scoreStr, "分数: %d", score);
        outtextxy(580, 20, scoreStr);
        char highScoreStr[50];
        sprintf_s(highScoreStr, "最高分: %d", highscore);
        outtextxy(720, 20, highScoreStr);
        outtextxy(WIDTH3 - 105, 15, "暂停/P");
        int speedBoost = score / 5; // 每5分速度加1
        if (speedBoost > 10) speedBoost = 10; // 最大速度加成
        // 显示游戏结束对话框
        if (isDialogShowgameover) {
            showgameover();
        }

        // 只在这里绘制障碍物一次
        for (auto& obstacle : obstacles) {
            obstacle.draw();
        }

        // 绘制子弹
        for (auto& bu : bul) {
            bu.draw();
        }

        // 绘制爆炸效果
        drawExplosions();

        // 绘制小球
        putimage(circle1.x, circle1.y, imgMask1, SRCAND);
        putimage(circle1.x, circle1.y, ball1 + ballFrame, SRCPAINT);

        // 如果暂停，显示"暂停"文字
        if (!con && !gameOver) {
            settextcolor(RED);
            settextstyle(50, 0, "楷体");
            outtextxy(WIDTH3 / 2 - 50, LENGTH3 / 2 - 25, "暂停");
        }
        // 正常游戏逻辑
        if (con && !gameOver) {
            // 绘制小球（控制转速）
            frameCounter++;
            if (frameCounter >= 2) {
                ballFrame = (ballFrame + 1) % 9;
                frameCounter = 0;
            }
            putimage(circle1.x, circle1.y, imgMask1, SRCAND);
            putimage(circle1.x, circle1.y, ball1 + ballFrame, SRCPAINT);

            // 生成障碍物
            DWORD currentTime = GetTickCount();
            if (currentTime - lastSpawnTime > nextSpawnTime) {
                int obstacleType = rand() % 4;
                Obstacle newObstacle(obstacleType);
                newObstacle.speed += speedBoost; // 应用速度加成
                obstacles.push_back(newObstacle);
                lastSpawnTime = currentTime;
                nextSpawnTime = minSpawnInterval + rand() % (maxSpawnInterval - minSpawnInterval);
            }
            //检查子弹与树碰撞//
            for (size_t i = 0; i < bul.size(); ++i) {
                for (size_t j = 0; j < obstacles.size(); ++j) {
                    bullet& bulItem = bul[i];
                    Obstacle& obsItem = obstacles[j];
                    //检查active和树type//
                    if (!bulItem.getactive() || !obsItem.active || obsItem.type != 0) {
                        continue;
                    }
                    bool collide = bulItem.getx() + bulItem.getr() >= obsItem.x + 90;
                    if (collide) {
                        bulItem.active = false; // 子弹消失
                        obsItem.active = false; // 树消失
                        int explorationx = obsItem.x + obsItem.width / 2;
                        int explorationy = obsItem.y + obsItem.height / 2;
                        bom e = { explorationx, explorationy, true, 0 };
                        exploration.push_back(e);

                        // 播放击中音效
                        PlaySoundEffect(HIT_SOUND);
                    }
                }
            }
            //爆炸效果//
            drawExplosions();
            updateExplosions();

            // 更新绘制障碍物
            for (auto& obstacle : obstacles) {
                obstacle.update();
                obstacle.draw();
            }

            //检查球和钻石碰撞//
            for (size_t j = 0; j < obstacles.size(); ++j) {
                if (obstacles[j].type != 3) {
                    continue;
                }
                if (!obstacles[j].active)
                    score++;
            }

            // 移除不活跃障碍物
            obstacles.erase(
                remove_if(obstacles.begin(), obstacles.end(),
                    [](const Obstacle& o) { return !o.active; }),
                obstacles.end()
            );

            for (auto& bu : bul) {
                bu.update();
                bu.draw();
            }
            bul.erase(
                remove_if(bul.begin(), bul.end(),
                    [](const bullet& o) { return !o.getactive(); }),
                bul.end()
            );
        }
        ExMessage msg;
        while (peekmessage(&msg)) {
            if (msg.message == WM_KEYDOWN) {
                switch (msg.vkcode) {
                case 'w': case 'W': case VK_UP:
                    if (!gameOver) moveup();  break;
                case 's': case 'S': case VK_DOWN:
                    if (!gameOver) yabian();  break;
                case'p':case'P':
                    if (!gameOver) con = !con;
                    break;
                case' ':
                    if (!gameOver) generatebullet();
                    break;
                }
            }
            if (msg.message == WM_LBUTTONDOWN &&
                msg.x >= 10 && msg.x <= 90 && msg.y >= 10 && msg.y <= 50) {
                if (!gameOver) isDialogShowing = true; // 触发对话框显示
            }
        }
        if (isDialogShowing) {
            handleReturnDialog();
            con = !con;//暂停游戏，展开对话框
            // 检查对话框结果
            if (dialogResult) {
                // 点击了"确认"，返回关卡选择
                // 停止游戏音乐，播放关卡选择音乐
                PlayBackgroundMusic(LEVEL_SELECT_MUSIC);

                initgraph(WIDTH2, LENGTH2);
                putimage(0, 0, &mainImage2);
                return;
            }
            else
                con = !con;//点击取消//
        }
        // 处理游戏结束状态
        if (gameOver) {
            // 保存当前屏幕作为背景
            getimage(&gameOverBackground, 0, 0, WIDTH3, LENGTH3);
            showgameover();  // 进入游戏结束处理循环

            // 如果showgameover返回，表示选择了重新开始或退出
            if (gameOver) {
                // 如果gameOver仍然为true，表示选择了退出
                // 停止游戏音乐，播放关卡选择音乐
                PlayBackgroundMusic(LEVEL_SELECT_MUSIC);

                putimage(0, 0, &mainImage2);
                return;
            }
            else {
                putimage(0, 0, &gameOverBackground);

            }
        }

        FlushBatchDraw();
        Sleep(35);
    }//while(true)
    EndBatchDraw();
}//playgame(1)

//生成子弹//
void generatebullet() {
    // 播放子弹音效
    PlaySoundEffect(BULLET_SOUND);

    bul.push_back(bullet());
}

//更新爆炸//
// 更新爆炸动画（放在主循环中）
void updateExplosions() {
    for (auto& exp : exploration) {
        if (!exp.active) continue;
        // 控制动画速度（每35毫秒更新一帧，主函数sleep35）
        exp.frame++;
        // 播放完所有帧后，标记为不活跃
        if (exp.frame >= 5) {
            exp.active = false;
        }
    }
    // 清理不活跃的爆炸效果
    exploration.erase(
        remove_if(exploration.begin(), exploration.end(),
            [](const bom& e) { return !e.active; }),
        exploration.end()
    );
}

//绘画爆炸效果//
void drawExplosions() {
    for (const auto& exp : exploration) {
        if (!exp.active) continue;
        // 绘制爆炸当前帧（位置调整为爆炸中心）
        putimage(exp.x, exp.y, boom + 1, SRCAND);
        putimage(exp.x, exp.y, boom, SRCPAINT);
    }
}

//识别关卡内返回的操作//
void handleReturnDialog() {
    if (!isDialogShowing) return;
    static IMAGE dialogBackground;
    static bool firstShow = true;
    if (firstShow) {
        // 保存当前背景
        getimage(&dialogBackground, 0, 0, WIDTH3, LENGTH3);
        firstShow = false;
    }

    // 绘制对话框背景
    putimage(0, 0, &dialogBackground);

    // 绘制确认对话框
    setfillcolor(RGB(230, 210, 170));
    solidrectangle(500, 300, 900, 500);
    settextcolor(RGB(153, 102, 51));
    settextstyle(40, 0, "楷体");
    outtextxy(570, 330, "确认退出关卡？");

    // 绘制按钮
    setfillcolor(RED);
    solidrectangle(550, 400, 650, 450);
    outtextxy(560, 410, "确认");

    setfillcolor(BLUE);
    solidrectangle(700, 400, 800, 450);
    outtextxy(710, 410, "取消");

    FlushBatchDraw(); // 确保对话框立即显示

    // 处理对话框交互
    bool dialogActive = true;
    while (dialogActive && isDialogShowing) {
        ExMessage msg;
        if (peekmessage(&msg, EX_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                // 确认按钮
                PlaySoundEffect(BUTTON_CLICK_SOUND);
                if (msg.x >= 550 && msg.x <= 650 &&
                    msg.y >= 400 && msg.y <= 450) {
                    dialogResult = true;
                    isDialogShowing = false;
                    dialogActive = false;
                }
                // 取消按钮
                else if (msg.x >= 700 && msg.x <= 800 &&
                    msg.y >= 400 && msg.y <= 450) {
                    dialogResult = false;
                    isDialogShowing = false;
                    dialogActive = false;
                }
            }
        }
        Sleep(10); // 避免CPU占用过高
    }

    // 重置首次显示标志
    firstShow = true;
}

void showgameover() {
    // 绘制对话框背景
    putimage(0, 0, &gameOverBackground);

    // 绘制确认对话框
    setfillcolor(RGB(230, 210, 170));
    solidrectangle(500, 300, 900, 500);
    settextcolor(RGB(153, 102, 51));
    settextstyle(60, 0, "楷体");
    outtextxy(620, 330, "失败");

    // 绘制按钮
    settextstyle(40, 0, "楷体");
    setfillcolor(RED);
    solidrectangle(510, 400, 680, 450);
    outtextxy(520, 410, "退出关卡");

    setfillcolor(BLUE);
    solidrectangle(700, 400, 870, 450);
    outtextxy(710, 410, "重新开始");

    FlushBatchDraw();

    // 处理按钮点击
    while (true) {
        ExMessage msg;
        if (peekmessage(&msg, EX_MOUSE) && msg.message == WM_LBUTTONDOWN) {
            // 退出关卡按钮
            PlaySoundEffect(BUTTON_CLICK_SOUND);
            if (msg.x >= 510 && msg.x <= 680 &&
                msg.y >= 400 && msg.y <= 450) {
                // 返回关卡选择
                initgraph(WIDTH2, LENGTH2);
                putimage(0, 0, &mainImage2);
                return;
            }
            // 重新开始按钮
            else if (msg.x >= 700 && msg.x <= 870 &&
                msg.y >= 400 && msg.y <= 450) {
                // 重置游戏状态
                gameOver = false;
                obstacles.clear();
                bul.clear();
                exploration.clear();
                circle1.x = 100;
                circle1.y = 650;
                circle1.r = 40;
                lastSpawnTime = GetTickCount();
                con = true;
                score = 0;
                return;
            }
        }
        Sleep(10); // 避免CPU占用过高
    }
}