#define _CRT_SECURE_NO_WARNINGS
#undef UNICODE
#undef _UNICODE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>
#include <time.h>
//#include <conio.h>
#include <mmsystem.h>
#include <math.h>

#define INF 0xffff

const int maxNum = 21;//地图横纵顶点数
const int width = 1000;//窗口宽
const int height = 660;//窗口高
const int length = 30;//小格边长
const int upLength = 30;//上边缘长
const int leftLength = 30;//左边缘长
const int radius = 8;//顶点半径
const COLORREF colorMAP = RGB(206, 231, 255);//地图窗口，淡蓝色
const COLORREF colorWND = RGB(250, 250, 250);//整体窗口，淡灰色
const COLORREF colorWALK = RGB(255, 255, 128);//步行路段，淡黄色
const COLORREF colorBUS = RGB(255, 100, 100);//公交路段，淡红色
const COLORREF colorSUBWAY = RGB(0, 255, 128);//地铁路段，淡绿色
const COLORREF colorPOINT1 = RGB(0, 220, 0);//景点原颜色，绿色
const COLORREF colorPOINT2 = RGB(255, 0, 0);//展示的景点颜色，大红色
const COLORREF colorBUTTON = RGB(192, 192, 192);//按钮，灰色
const COLORREF colorOUTPUT = RGB(255, 255, 255);//输出窗口，白色
//按钮相关坐标
const int buttonX[6] = { 680, 840, 680, 840, 680, 840 };//按钮横像素坐标值
const int buttonY[6] = { 40, 40, 100, 100, 160, 160 };//按钮竖像素坐标值
const int buttonW = 120;//按钮宽度
const int buttonH = 40;//按钮高度
int buttonFLAG = 0;//按钮标签值，0：主界面，1：按钮1界面，以此类推
int edgeTraffic[maxNum][maxNum][2];//路径交通情况，0不通、1步行、2公交、3地铁，每个点的右边和下边//0右边，1下边
const int times[3] = { 10, 3, 1 };//时间，依次步行、公交、地铁（分钟）
const int money[3] = { 0, 3, 5 };//价格，依次步行、公交、地铁（元）
const int dx[4] = { 1, -1, 0, 0 };//右、左、下、上
const int dy[4] = { 0, 0, 1, -1 };
const int ex[4] = { 0, -1, 0, 0 };
const int ey[4] = { 0, 0, 0, -1 };
const int ei[4] = { 0, 0, 1, 1 };

const int scnt = 14;//景点数
struct//景点结构体
{
    int x;//x值
    int y;//y值
    int ticket;//票价
    char number[3];//编号
    char name[10];//名称
    char introduce[100];//简介
}scenery[scnt];

struct point//顶点结构体
{
    int x;
    int y;
    struct point* pre;//前趋结点
}pointS[maxNum][maxNum];//21*21

struct pointDis//任意两点间最优路径
{
    int data;
    point onePoint[maxNum * maxNum];//两点间最优路径
}pointDis_[scnt][scnt];
//用于最优路径算法存储值
int cnt1[scnt];
int cnt2[scnt];
int mindata;
int idata;

void initScenery();//初始化景点
void drawPoint();//画点
void initLine();//初始化线
void drawLine();//画线
void drawMap();//重新绘制地图区域
void drawOutput(int flag);//画提示区和输出区
void outText(const char* text, int flag);//在提示区和输出区输出信息
void button(int bx, int by, int w, int h, const char* str);//按钮
void drawButton();//画按钮界面
void clickScenecy(ExMessage msg);//景点查询,点击查询
void outScenery(int i);//在第二行输出景点信息
void setRED(int i);//将目标景点设置为红色
void jumpButton(ExMessage msg);//按钮界面跳转
void button1_1();//实现按钮1_1，景点查询，编号查询
void button1_2();//实现按钮1_2，景点查询，名称查询
void button2(int flag);//实现按钮2_1，问路查询，最短路径
int preColor(point cur);//寻找前一个路段的颜色
void drawLines(int targetX, int targetY, int startX, int startY);//画带箭头的轨迹
int Dijkstra(int n1, int n2, int flag);//迪杰斯特拉算法求两点之间最短路径
void button3(int flag);//实现按钮3_1，观光查询，最短路径
void preBfs(int* cnt, int n, int flag);//求出并存储任意两点间最优路径
void backtrack(int cur, int n, int flag);//回溯法遍历，求出n个景点的最优路径，n>10时速度明显变慢
void insertway(int n);//插入快速不精确查找
//void button4(int flag);//实现按钮5_1，其他，播放音乐

int main()
{
    initgraph(width, height);//画窗口
    BeginBatchDraw();
    setbkcolor(colorWND);//设置窗口背景颜色
    settextcolor(BLACK);//设置字体颜色
    cleardevice();//清屏
    initScenery();//初始化景点
    initLine();//初始化路段
    drawMap();//画地图
    drawOutput(0);//画提示区
    drawOutput(1);//画输出区
    drawButton();//画按钮
    EndBatchDraw();
    for (int i = 0; i < maxNum; i++)
    {
        for (int j = 0; j < maxNum; j++)
        {
            pointS[i][j].x = i;
            pointS[i][j].y = j;
            pointS[i][j].pre = NULL;
        }
    }
    while (1)
    {
        ExMessage msg;
        msg = getmessage(EM_MOUSE);//获取鼠标信息
        if (msg.message == WM_LBUTTONDOWN)//如果鼠标左键按下
        {
            if (msg.x >= 20 && msg.x <= 640 && msg.y >= 20 && msg.y <= 640) clickScenecy(msg);//如果点击地图区域，实现点击查询
            else if (msg.x >= 680 && msg.x <= 960 && msg.y >= 40 && msg.y <= 200) jumpButton(msg);//如果点击按钮区域
        }
        //BeginBatchDraw();
        //FlushBatchDraw();
        //EndBatchDraw();
    }
}

void initScenery()//初始化景点
{
    //景点
    scenery[0].x = 10;
    scenery[0].y = 10;
    scenery[0].ticket = 68;
    strcpy(scenery[0].number, "01");
    strcpy(scenery[0].name, "黄鹤楼");
    strcpy(scenery[0].introduce, "地处蛇山之巅，濒临万里长江");

    scenery[1].x = 20;
    scenery[1].y = 2;
    scenery[1].ticket = 200;
    strcpy(scenery[1].number, "02");
    strcpy(scenery[1].name, "欢乐谷");
    strcpy(scenery[1].introduce, "时尚、动感、激情的繁华都市\n开心地");//一行最多13个字

    scenery[2].x = 8;
    scenery[2].y = 18;
    scenery[2].ticket = 50;
    strcpy(scenery[2].number, "03");
    strcpy(scenery[2].name, "两江游览");
    strcpy(scenery[2].introduce, "于江面之上，揽两岸风光，夜\n景更佳");

    scenery[3].x = 1;
    scenery[3].y = 1;
    scenery[3].ticket = 100;
    strcpy(scenery[3].number, "04");
    strcpy(scenery[3].name, "知音号");
    strcpy(scenery[3].introduce, "20世纪30年代风格的大型主题\n演艺轮船");

    scenery[4].x = 14;
    scenery[4].y = 7;
    scenery[4].ticket = 150;
    strcpy(scenery[4].number, "05");
    strcpy(scenery[4].name, "汉秀剧场");
    strcpy(scenery[4].introduce, "顶级科技与演艺的完美呈现，\n世界级水舞秀");

    scenery[5].x = 3;
    scenery[5].y = 7;
    scenery[5].ticket = 75;
    strcpy(scenery[5].number, "06");
    strcpy(scenery[5].name, "木兰草原");
    strcpy(scenery[5].introduce, "华中地区少有的草原风情旅游\n区");

    scenery[6].x = 16;
    scenery[6].y = 10;
    scenery[6].ticket = 350;
    strcpy(scenery[6].number, "07");
    strcpy(scenery[6].name, "东湖");
    strcpy(scenery[6].introduce, "听涛休闲、磨山赏花、落雁观\n鸟、吹笛游山");

    scenery[7].x = 5;
    scenery[7].y = 3;
    scenery[7].ticket = 30;
    strcpy(scenery[7].number, "08");
    strcpy(scenery[7].name, "长江大桥");
    strcpy(scenery[7].introduce, "横跨于武昌蛇山和汉阳龟山之\n间，武汉地标性建筑");

    scenery[8].x = 17;
    scenery[8].y = 15;
    scenery[8].ticket = 120;
    strcpy(scenery[8].number, "09");
    strcpy(scenery[8].name, "蜡像馆");
    strcpy(scenery[8].introduce, "坐落于楚河汉街，聚集了众多\n名人蜡像");

    scenery[9].x = 9;
    scenery[9].y = 5;
    scenery[9].ticket = 20;
    strcpy(scenery[9].number, "10");
    strcpy(scenery[9].name, "汉口江滩");
    strcpy(scenery[9].introduce, "见证了“大武汉”兴盛和衰落\n的标志性景点");

    scenery[10].x = 6;
    scenery[10].y = 15;
    scenery[10].ticket = 60;
    strcpy(scenery[10].number, "11");
    strcpy(scenery[10].name, "晴川阁");
    strcpy(scenery[10].introduce, "与长江对面的黄鹤楼夹江相望\n，与黄鹤楼、古琴台并称“三\n楚圣境”");

    scenery[11].x = 1;
    scenery[11].y = 11;
    scenery[11].ticket = 40;
    strcpy(scenery[11].number, "12");
    strcpy(scenery[11].name, "归元禅寺");
    strcpy(scenery[11].introduce, "来归元寺祈福，拜平安，求幸\n福");

    scenery[12].x = 13;
    scenery[12].y = 4;
    scenery[12].ticket = 80;
    strcpy(scenery[12].number, "13");
    strcpy(scenery[12].name, "动物园");
    strcpy(scenery[12].introduce, "位于武汉市汉阳区墨水湖畔");

    scenery[13].x = 12;
    scenery[13].y = 20;
    scenery[13].ticket = 15;
    strcpy(scenery[13].number, "14");
    strcpy(scenery[13].name, "南湖");
    strcpy(scenery[13].introduce, "位于武昌东南部，是武汉市仅\n次于东湖和汤逊湖的第三大城\n中湖");
}

void drawPoint()//画点
{
    for (int i = 0; i < scnt; i++)
    {
        int sx = scenery[i].x;
        int sy = scenery[i].y;
        setfillcolor(colorPOINT1);//设置景点颜色
        solidcircle(leftLength + sx * length, upLength + sy * length, radius);
    }
}

void initLine()//初始化线
{
    //srand((unsigned int)time(NULL));
    //srand()给随机函数种子
    //随机函数rand()
    for (int i = 0; i < maxNum; i++)
        for (int j = 0; j < maxNum; j++)
            for (int k = 0; k < 2; k++)
            {
                //rand() + i + j + k //默认种子为1
                srand(rand() + i + j + k);//随机数种子
                int odd = rand() % 4;//0 - 6wan -> 0 - 3
                edgeTraffic[i][j][k] = odd;//四种路段等概率
            }
    for (int i = 0; i < maxNum; i++)//边缘路段去掉
    {
        edgeTraffic[i][maxNum - 1][1] = 0;//下边
        edgeTraffic[maxNum - 1][i][0] = 0;//右边
    }
}

void drawLine()//画线
{
    for (int i = 0; i < maxNum; i++)
        for (int j = 0; j < maxNum; j++)
            for (int k = 0; k < 2; k++)
            {
                if (edgeTraffic[i][j][k] == 0) continue;
                switch (edgeTraffic[i][j][k])
                {
                case 1:
                    setlinecolor(colorWALK);//人行
                    break;
                case 2:
                    setlinecolor(colorBUS);//公交
                    break;
                case 3:
                    setlinecolor(colorSUBWAY);//地铁
                    break;
                }
                int x1, y1, x2, y2;
                //起点
                x1 = leftLength + i * length;
                y1 = upLength + j * length;
                //终点
                x2 = x1 + (1 - k) * length;
                y2 = y1 + k * length;
                line(x1, y1, x2, y2);//画线
            }
}

void drawMap()//重新绘制地图区域
{
    BeginBatchDraw();
    setbkcolor(colorMAP);
    clearrectangle(20, 20, 640, 640);
    drawLine();
    drawPoint();
    EndBatchDraw();
}

void button(int bx, int by, int w, int h, const char* str)//画一个按钮
{
    //bx,by按钮左上角像素值，w宽度，h高度
    setfillcolor(colorBUTTON);
    solidroundrect(bx, by, bx + w, by + h, 5, 5);
    setbkcolor(colorBUTTON);//设置文字背景颜色
    settextcolor(BLACK);//设置文字颜色
    settextstyle(22, 0, "楷体");//设置字体风格
    int tx, ty;//设置文字居中
    tx = bx + (w - textwidth(str)) / 2;
    ty = by + (h - textheight(str)) / 2;
    outtextxy(tx, ty, str);
}

void drawButton()//画按钮界面
{
    BeginBatchDraw();
    setbkcolor(colorWND);
    clearrectangle(680, 40, 960, 200);//清空原区域
    char str[7][50];
    int strNum[5] = { 7, 4, 6, 6, 4 };
    switch (buttonFLAG)
    {
    case 0://主界面
    {
        char str0[7][50] = { "欢迎使用城市导游小程序！", "景点查询", "问路查询", "观光查询", "其他", "使用说明", "清屏" };
        for (int i = 0; i < strNum[buttonFLAG]; i++)
            strcpy(str[i], str0[i]);
        break;
    }
    case 1:
    {
        char str0[4][50] = { "欢迎使用景点查询！", "编号查询", "名称查询", "返回" };
        for (int i = 0; i < strNum[buttonFLAG]; i++)
            strcpy(str[i], str0[i]);
        break;
    }
    case 2:
    {
        char str0[6][50] = { "欢迎使用问路查询！", "最短路径", "最短时间", "最低花费", "步行最少", "返回" };
        for (int i = 0; i < strNum[buttonFLAG]; i++)
            strcpy(str[i], str0[i]);
        break;
    }
    case 3:
    {
        char str0[6][50] = { "欢迎使用观光查询！", "最短路径", "最短时间", "最低花费", "步行最少", "返回" };
        for (int i = 0; i < strNum[buttonFLAG]; i++)
            strcpy(str[i], str0[i]);
        break;
    }
    case 4:
    {
        char str0[4][50] = { "欢迎使用其他功能！", "播放音乐", "关闭音乐", "返回" };
        for (int i = 0; i < strNum[buttonFLAG]; i++)
            strcpy(str[i], str0[i]);
        break;
    }
    }
    outText(str[0], 0);//提示区输出欢迎使用信息
    for (int i = 1; i < strNum[buttonFLAG]; i++)
        button(buttonX[i - 1], buttonY[i - 1], buttonW, buttonH, str[i]);
    EndBatchDraw();
}

void drawOutput(int flag)//画提示区和输出区
{
    //设置颜色
    setfillcolor(colorOUTPUT);
    setlinecolor(BLACK);
    //画矩形
    if (flag == 0) fillrectangle(660, 240, 980, 280);
    else if (flag == 1) fillrectangle(660, 280, 980, 640);
}

void outText(const char* text, int flag)//在提示区和输出区输出信息
{
    //outtextxy
    BeginBatchDraw();
    drawOutput(flag);//重新绘制矩形
    RECT r;
    if (flag == 0) r = { 670, 250, 970, 270 };
    else if (flag == 1) r = { 670, 290, 970, 630 };
    settextcolor(BLACK);
    setbkcolor(colorOUTPUT);
    settextstyle(22, 0, "楷体");
    //\n换行符号
    drawtext(text, &r, DT_LEFT | DT_TOP);
    EndBatchDraw();
}

void clickScenecy(ExMessage msg)//景点查询,点击查询
{
    int i = (msg.x - 15) / 30;
    int j = (msg.y - 15) / 30;
    //int i = (msg.x + 30) / 120;
    //int j = (msg.y + 30) / 120;
    for (int k = 0; k < scnt; k++)
    {
        if (i == scenery[k].x && j == scenery[k].y)
        {
            outText("欢迎使用点击查询！", 0);
            outScenery(k);//输出景点信息
            drawPoint();//重新绘制所有点
            setRED(k);//把目标点设为红色
            break;
        }
    }
}

void outScenery(int i)//在第二行输出景点信息
{
    drawOutput(1);
    RECT r = { 670, 290, 970, 630 };
    setbkmode(colorOUTPUT);
    settextstyle(22, 0, "楷体");
    //strlen求字符串长度
    int sLength = strlen(scenery[i].number) + strlen(scenery[i].name) + strlen(scenery[i].introduce) + 50;
    char* result = (char*)malloc(sizeof(char) * sLength);
    //strcpy复制，strcat拼接
    strcpy(result, scenery[i].number);
    strcat(result, "  ");
    strcat(result, scenery[i].name);
    strcat(result, "\n");
    strcat(result, "票价：");
    char q[50];
    //sprintf，%d整型
    sprintf(q, "%d元\n", scenery[i].ticket);
    strcat(result, q);
    strcat(result, scenery[i].introduce);
    drawtext(result, &r, DT_LEFT | DT_TOP);
}

void setRED(int i)//将目标景点设置为红色
{
    setfillcolor(colorPOINT2);//设置红色
    int dx = leftLength + scenery[i].x * length;
    int dy = upLength + scenery[i].y * length;
    solidcircle(dx, dy, radius);
}

void jumpButton(ExMessage msg)//按钮界面跳转
{
    const int buttonNum[5] = { 6, 3, 5, 5, 3 };
    for (int k = 0; k < buttonNum[buttonFLAG]; k++)
    {
        if (msg.x >= buttonX[k] && msg.x <= buttonX[k] + buttonW && msg.y >= buttonY[k] && msg.y <= buttonY[k] + buttonH)
        {
            switch (buttonFLAG)
            {
            case 0://主界面
            {
                if (k == 4)//使用说明
                {
                    drawOutput(1);
                    outText("欢迎观看使用说明！", 0);
                    outText("图中绿点表示景点，不同颜色\n\
线段表示该路段所使用的交通\n\
工具不同。黄色表示步行路段，\n\
红色表示公交路段，绿色表示\n\
地铁路段，每单位长度为1公\n\
里。出行时间设定为，步行：\n\
10min/km，公交3min/km，地\n\
铁1min/km。价格设定为，步\n\
行免费，公交：3元/次，地铁\n\
5元/次。\n\
此外，问路查询给出两个景点\n\
间的最优路径，观光查询给出\n\
多个景点间的最优路径。在\"其\n\
他\"里可以选择播放/关闭音乐。", 1);
                }
                else if (k == 5)//清屏
                {
                    drawMap();
                    drawOutput(0);
                    drawOutput(1);
                    outText("欢迎使用城市导游小程序！", 0);
                }
                else
                {
                    buttonFLAG = k + 1;
                    drawButton();
                }
                break;
            }
            case 1://景点查询界面
            {
                if (k == 0) button1_1();//实现编号查询
                else if (k == 1) button1_2();//实现名称查询
                else if (k == 2)//返回
                {
                    buttonFLAG = 0;
                    drawButton();
                }
                break;
            }
            case 2://问路
            {
                if (k == 4)
                {
                    buttonFLAG = 0;
                    drawButton();
                }
                else button2(k + 1);//实现问路查询
                break;
            }
            case 3://观光
            {
                if (k == 4)
                {
                    buttonFLAG = 0;
                    drawButton();
                }
                else button3(k + 1);//实现观光查询
                break;
            }
            case 4://其他
            {
                if (k == 2)
                {
                    buttonFLAG = 0;
                    drawButton();
                }
                //else button4(k + 1);//实现播放/关闭音乐
                break;
            }
            }
            break;
        }
    }
}

void button1_1()//实现按钮1_1，景点查询，编号查询
{
    char s[3];
    outText("欢迎使用编号查询！", 0);
    InputBox(s, 3, "请输入想要查询的编号");//跳出弹窗
    for (int i = 0; i < scnt; i++)
    {
        if (s[0] == scenery[i].number[0] && s[1] == scenery[i].number[1])
        {
            outScenery(i);//输出第i个景点信息
            drawPoint();
            setRED(i);
            return;
        }
    }
    drawOutput(1);
    outText("无此景点！", 1);
}

void button1_2()//实现按钮1_2，景点查询，名称查询
{
    char s[9];
    outText("欢迎使用名称查询！", 0);
    InputBox(s, 9, "请输入想要查询的名称");
    int Slen = strlen(s);
    bool flag = false;
    int cnt;
    for (int i = 0; i < scnt; i++)
    {
        flag = true;
        cnt = i;
        for (int k = 0; k < Slen; k++)
        {
            if (s[k] != scenery[i].name[k])
            {
                flag = false;
                break;
            }
        }
        if (flag) break;
    }
    if (Slen == 0)
    {
        flag = false;
    }
    if (flag)//如果存在
    {
        outScenery(cnt);
        drawPoint();
        setRED(cnt);
    }
    else
    {
        drawOutput(1);
        outText("无此景点！", 1);
    }
}

void button2(int flag)//实现按钮2_1，问路查询，最短路径
{
    BeginBatchDraw();
    drawMap();
    char tip[4][50] = { "欢迎查询最短路径！", "欢迎查询最短时间！", "欢迎查询最低花费！", "欢迎查询步行最少！" };
    outText(tip[flag - 1], 0);
    char s[2][3];//定义两个编号数组
    int nums[2];//存入编号对应的第几个景点
    char q[2][50] = { "请输入您当前所在景点编号！", "请输入计划前往景点编号！" };
    for (int k = 0; k < 2; k++)
    {
        InputBox(s[k], 3, q[k]);
        bool flag = false;
        for (int i = 0; i < scnt; i++)
        {
            if (s[k][0] == scenery[i].number[0] && s[k][1] == scenery[i].number[1])
            {
                nums[k] = i;
                setRED(i);
                flag = true;
                break;
            }
        }
        if (!flag)
        {
            drawOutput(0);
            drawOutput(1);
            outText("欢迎使用问路查询！", 0);
            outText("无此景点！", 1);
            return;
        }
    }
    if (nums[0] == nums[1])
    {
        drawOutput(0);
        drawOutput(1);
        outText("欢迎使用问路查询！", 0);
        outText("您已在目标景点！", 1);
        return;
    }
    outText("计算中", 1);
    int a;//存储最优路径值
    int n1 = nums[0];
    int n2 = nums[1];
    a = Dijkstra(n1, n2, flag);//迪杰斯特拉算法
    drawOutput(1);
    char s1[50] = "路线如图所示：\n";

    char str1[4][50] = { "最短路径为", "最短时间为", "交通最低花费为", "交通最低花费为" };
    strcat(s1, str1[flag - 1]);

    char s2[10];
    //sprintf(s2, "%d", pointS[a].data);
    sprintf(s2, "%d", a);
    strcat(s1, s2);

    char str2[4][50] = { "公里", "分钟", "元", "公里" };
    strcat(s1, str2[flag - 1]);

    outText(s1, 1);

    point cur = pointS[scenery[n2].x][scenery[n2].y];
    while (cur.pre)
    {
        int sx = cur.x;
        int sy = cur.y;
        int tx = cur.pre->x;
        int ty = cur.pre->y;
        drawLines(sx, sy, tx, ty);
        cur = *(cur.pre);
    }
    EndBatchDraw();
}

int Dijkstra(int n1, int n2, int flag)//迪杰斯特拉算法求两点之间最短路径
{
    int pdis[maxNum * maxNum];
    bool ED[maxNum * maxNum];
    int num1 = scenery[n1].x + scenery[n1].y * maxNum;
    int num2 = scenery[n2].x + scenery[n2].y * maxNum;
    pointS[scenery[n1].x][scenery[n1].y].pre = NULL;
    for (int i = 0; i < maxNum * maxNum; i++)
    {
        pdis[i] = INF;
        ED[i] = false;
    }
    pdis[num1] = 0;
    bool allED = false;
    int num;
    while (1)
    {
        for (int i = 0; i < maxNum * maxNum; i++)
        {
            if (ED[i] == false)
            {
                num = i;
                break;
            }
            allED = true;
        }
        if (allED == true) break;
        for (int i = num + 1; i < maxNum * maxNum; i++)
        {
            if (pdis[i] < pdis[num] && ED[i] == false) num = i;
        }
        ED[num] = true;
        int i = num % maxNum;
        int j = num / maxNum;
        for (int k = 0; k < 4; k++)//右左下上
        {
            int nx = i + dx[k];
            int ny = j + dy[k];
            int mx = i + ex[k];
            int my = j + ey[k];
            int mi = ei[k];
            if (nx >= 0 && nx <= 21 && ny >= 0 && ny < 21 && edgeTraffic[mx][my][mi] > 0)
            {
                int temp;//存储权值
                switch (flag)
                {
                case 1:
                {
                    temp = 1;
                    break;
                }
                case 2:
                {
                    temp = times[edgeTraffic[mx][my][mi] - 1];
                    break;
                }
                case 3:
                {
                    int oldcolor;
                    oldcolor = preColor(pointS[i][j]);
                    if (edgeTraffic[mx][my][mi] > 1 && edgeTraffic[mx][my][mi] == oldcolor)
                        temp = 0;
                    else if (edgeTraffic[mx][my][mi] == 1)
                        temp = money[0];
                    else if (edgeTraffic[mx][my][mi] == 2)
                        temp = money[1];
                    else if (edgeTraffic[mx][my][mi] == 3)
                        temp = money[2];
                    break;
                }
                case 4:
                {
                    if (edgeTraffic[mx][my][mi] == 1) temp = 1;
                    else temp = 0;
                    break;
                }
                }
                if (pdis[nx + ny * maxNum] > pdis[i + j * maxNum] + temp)
                {
                    pdis[nx + ny * maxNum] = pdis[i + j * maxNum] + temp;
                    pointS[nx][ny].pre = &(pointS[i][j]);
                }
            }
        }
    }
    return pdis[num2];
}

int preColor(point cur)//寻找前一个路段的颜色
{
    if (cur.pre == NULL)
        return -1;
    int dx_ = cur.x - cur.pre->x;
    int dy_ = cur.y - cur.pre->y;
    for (int k = 0; k < 4; k++)
    {
        if (dx_ == dx[k] && dy_ == dy[k])
        {
            int nx = cur.pre->x + ex[k];
            int ny = cur.pre->y + ey[k];
            return edgeTraffic[nx][ny][ei[k]];
        }
    }
    return -1;
}

void drawLines(int targetX, int targetY, int startX, int startY)//画带箭头的轨迹
{
    //画轨迹
    int sx = leftLength + startX * length;
    int sy = upLength + startY * length;
    int tx = leftLength + targetX * length;
    int ty = upLength + targetY * length;
    int a1 = (tx - sx) / 10;
    int a2 = (tx - sx) / 2;
    int b1 = (ty - sy) / 10;
    int b2 = (ty - sy) / 2;
    setlinecolor(BLACK);
    line(sx, sy, sx + a1, sy + b1);
    line(sx + a2, sy + b2, sx + a1 + a2, sy + b1 + b2);

    //画箭头
    int tmpx = (sx - tx) / 15;
    int tmpy = (sy - ty) / 15;
    int stax = (tx + sx) / 2 - tmpx * 2;
    int stay = (ty + sy) / 2 - tmpy * 2;
    if (sy == ty)
    {
        line(stax, stay, stax + tmpx, stay + 2);
        line(stax, stay, stax + tmpx, stay - 2);
    }
    else
    {
        line(stax, stay, stax + 2, stay + tmpy);
        line(stax, stay, stax - 2, stay + tmpy);
    }
}

void button3(int flag)//实现按钮3_1，观光查询，最短路径
{
    BeginBatchDraw();
    drawMap();
    char tip[4][50];
    outText(tip[flag - 1], 0);
    char t[3];;
    InputBox(t, 3, "请输入您想游览的景点个数！");
    int n;
    if (t[1] < '0' || t[1] > '9')
    {
        if (t[0] < '2' || t[0] > '9')
        {
            outText("欢迎使用观光查询！", 0);
            outText("输入不合法！", 1);
            return;
        }
        n = t[0] - 48;
    }
    else
    {
        if (t[0] < '0' || t[0] > '9')
        {
            outText("欢迎使用观光查询！", 0);
            outText("输入不合法！", 1);
            return;
        }
        n = (t[0] - 48) * 10 + (t[1] - 48);
    }
    if (n == 1)
    {
        outText("欢迎使用观光查询！", 0);
        outText("输入不合法！", 1);
        return;
    }
    if (n > scnt)
    {
        outText("欢迎使用观光查询！", 0);
        outText("超出已有景点数！", 1);
        return;
    }
    /*char s[n][3];
    int cnt[n];*/
    char** s = new char * [n];
    for (int i = 0; i < n; ++i)
    {
        s[i] = new char[3];
    }
    int* cnt = new int[n];
    for (int i = 0; i < n; i++)
    {
        InputBox(s[i], 3, "请输入景点编号！");
        bool flag_ = false;
        for (int j = 0; j < scnt; j++)
        {
            if (s[i][0] == scenery[j].number[0] && s[i][1] == scenery[j].number[1])
            {
                for (int k = 0; k < i; k++)
                {
                    if (j == cnt[k])
                    {
                        outText("欢迎使用观光查询！", 0);
                        outText("请勿输入重复景点！", 1);
                        return;
                    }
                }
                cnt[i] = j;
                setRED(j);
                flag_ = true;
            }
        }
        if (!flag_)
        {
            outText("欢迎使用观光查询！", 0);
            outText("无此景点！", 1);
            return;
        }
    }
    preBfs(cnt, n, flag);//求出每两个景点之间的最优路径
    outText("计算中...", 1);
    char s1[1000] = "路线如图所示：\n";
    char s2[10];
    if (n <= 10)
    {
        mindata = 10000;
        backtrack(0, n, 0);
        sprintf(s2, "%d", mindata);
    }
    else
    {
        insertway(n);
        sprintf(s2, "%d", idata);
    }
    char str1[4][50] = { "最短路径为", "最短时间为", "交通最低花费为", "最少需步行" };
    strcat(s1, str1[flag - 1]);
    strcat(s1, s2);
    char str2[4][50] = { "公里", "分钟", "元", "公里" };
    strcat(s1, str2[flag - 1]);
    int allmoney = scenery[cnt2[0]].ticket;
    for (int i = 1; i < n; i++)
    {
        int ni = cnt2[i - 1];//起始点编号
        int nj = cnt2[i];//目标点编号
        allmoney += scenery[nj].ticket;
        point cur = pointDis_[ni][nj].onePoint[0];
        while (cur.pre)
        {
            int sx = cur.x;
            int sy = cur.y;
            int tx = cur.pre->x;
            int ty = cur.pre->y;
            drawLines(sx, sy, tx, ty);
            cur = *(cur.pre);
        }
    }
    char q[100];
    sprintf(q, "\n此外各景点门票共需%d元", allmoney);
    strcat(s1, q);
    outText(s1, 1);
    /*char q[10];
    sprintf(q, "为：%d", pointDis_[0][1].onePoint[0].pre->x);
    clearOutput2();
    outMessage(q);*/
    EndBatchDraw();
}

void preBfs(int* cnt, int n, int flag)//求出并存储任意两点间最优路径
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j) continue;
            int a = Dijkstra(cnt[i], cnt[j], flag);
            pointDis_[i][j].data = a;
            point cur = pointS[scenery[cnt[j]].x][scenery[cnt[j]].y];
            int k = 0;
            pointDis_[i][j].onePoint[k++] = cur;
            while (cur.pre)
            {
                cur = *(cur.pre);
                pointDis_[i][j].onePoint[k] = cur;
                pointDis_[i][j].onePoint[k - 1].pre = &(pointDis_[i][j].onePoint[k]);
                k++;
            }
            pointDis_[i][j].onePoint[k].pre = NULL;
        }
    }
}

void backtrack(int cur, int n, int flag)//回溯法遍历，求出n个景点的最优路径，n>10时速度明显变慢
{
    for (int i = 0; i < n; i++)
    {
        if (cur == n)
        {
            int temp = 0;
            for (int j = 1; j < n; j++)
            {
                temp += pointDis_[cnt1[j - 1]][cnt1[j]].data;
            }
            if (temp < mindata)
            {
                mindata = temp;
                for (int j = 0; j < n; j++)
                    cnt2[j] = cnt1[j];
            }
            return;
        }
        if (((flag >> i) & 0b1) == 1)
            continue;
        cnt1[cur] = i;
        backtrack(cur + 1, n, flag | (0b1 << i));
    }
}

void insertway(int n)//插入快速不精确查找
{
    cnt2[0] = 0;
    cnt2[1] = 1;
    idata = pointDis_[0][1].data;
    for (int i = 2; i < n; i++)
    {
        int* temp = new int[i + 1];
        for (int j = 0; j <= i; j++)
        {
            if (j == 0)
            {
                temp[j] = idata + pointDis_[i][cnt2[0]].data;
            }
            else if (j == i)
            {
                temp[j] = idata + pointDis_[cnt2[i - 1]][i].data;
            }
            else
            {
                temp[j] = idata - pointDis_[cnt2[j - 1]][cnt2[j]].data;
                temp[j] += pointDis_[cnt2[j - 1]][i].data;
                temp[j] += pointDis_[i][cnt2[j]].data;
            }
        }
        int flag = 0;
        for (int j = 1; j <= i; j++)
        {
            if (temp[j] < temp[flag])
            {
                flag = j;
            }
        }
        idata = temp[flag];
        for (int j = i; j > flag; j--)
        {
            cnt2[j] = cnt2[j - 1];
        }
        cnt2[flag] = i;
        delete[] temp;
    }
}

//void button4(int flag)//实现按钮5_1，其他，播放音乐
//{
//    switch (flag)
//    {
//    case 1:
//        mciSendString("play 1.mp3 repeat", 0, 0, 0);
//        outText("播放成功！", 0);
//        break;
//    case 2:
//        mciSendString("close 1.mp3", 0, 0, 0);
//        outText("关闭成功！", 0);
//        break;
//    }
//}
