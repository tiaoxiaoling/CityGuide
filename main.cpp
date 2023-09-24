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

const int maxNum = 21;//��ͼ���ݶ�����
const int width = 1000;//���ڿ�
const int height = 660;//���ڸ�
const int length = 30;//С��߳�
const int upLength = 30;//�ϱ�Ե��
const int leftLength = 30;//���Ե��
const int radius = 8;//����뾶
const COLORREF colorMAP = RGB(206, 231, 255);//��ͼ���ڣ�����ɫ
const COLORREF colorWND = RGB(250, 250, 250);//���崰�ڣ�����ɫ
const COLORREF colorWALK = RGB(255, 255, 128);//����·�Σ�����ɫ
const COLORREF colorBUS = RGB(255, 100, 100);//����·�Σ�����ɫ
const COLORREF colorSUBWAY = RGB(0, 255, 128);//����·�Σ�����ɫ
const COLORREF colorPOINT1 = RGB(0, 220, 0);//����ԭ��ɫ����ɫ
const COLORREF colorPOINT2 = RGB(255, 0, 0);//չʾ�ľ�����ɫ�����ɫ
const COLORREF colorBUTTON = RGB(192, 192, 192);//��ť����ɫ
const COLORREF colorOUTPUT = RGB(255, 255, 255);//������ڣ���ɫ
//��ť�������
const int buttonX[6] = { 680, 840, 680, 840, 680, 840 };//��ť����������ֵ
const int buttonY[6] = { 40, 40, 100, 100, 160, 160 };//��ť����������ֵ
const int buttonW = 120;//��ť���
const int buttonH = 40;//��ť�߶�
int buttonFLAG = 0;//��ť��ǩֵ��0�������棬1����ť1���棬�Դ�����
int edgeTraffic[maxNum][maxNum][2];//·����ͨ�����0��ͨ��1���С�2������3������ÿ������ұߺ��±�//0�ұߣ�1�±�
const int times[3] = { 10, 3, 1 };//ʱ�䣬���β��С����������������ӣ�
const int money[3] = { 0, 3, 5 };//�۸����β��С�������������Ԫ��
const int dx[4] = { 1, -1, 0, 0 };//�ҡ����¡���
const int dy[4] = { 0, 0, 1, -1 };
const int ex[4] = { 0, -1, 0, 0 };
const int ey[4] = { 0, 0, 0, -1 };
const int ei[4] = { 0, 0, 1, 1 };

const int scnt = 14;//������
struct//����ṹ��
{
    int x;//xֵ
    int y;//yֵ
    int ticket;//Ʊ��
    char number[3];//���
    char name[10];//����
    char introduce[100];//���
}scenery[scnt];

struct point//����ṹ��
{
    int x;
    int y;
    struct point* pre;//ǰ�����
}pointS[maxNum][maxNum];//21*21

struct pointDis//�������������·��
{
    int data;
    point onePoint[maxNum * maxNum];//���������·��
}pointDis_[scnt][scnt];
//��������·���㷨�洢ֵ
int cnt1[scnt];
int cnt2[scnt];
int mindata;
int idata;

void initScenery();//��ʼ������
void drawPoint();//����
void initLine();//��ʼ����
void drawLine();//����
void drawMap();//���»��Ƶ�ͼ����
void drawOutput(int flag);//����ʾ���������
void outText(const char* text, int flag);//����ʾ��������������Ϣ
void button(int bx, int by, int w, int h, const char* str);//��ť
void drawButton();//����ť����
void clickScenecy(ExMessage msg);//�����ѯ,�����ѯ
void outScenery(int i);//�ڵڶ������������Ϣ
void setRED(int i);//��Ŀ�꾰������Ϊ��ɫ
void jumpButton(ExMessage msg);//��ť������ת
void button1_1();//ʵ�ְ�ť1_1�������ѯ����Ų�ѯ
void button1_2();//ʵ�ְ�ť1_2�������ѯ�����Ʋ�ѯ
void button2(int flag);//ʵ�ְ�ť2_1����·��ѯ�����·��
int preColor(point cur);//Ѱ��ǰһ��·�ε���ɫ
void drawLines(int targetX, int targetY, int startX, int startY);//������ͷ�Ĺ켣
int Dijkstra(int n1, int n2, int flag);//�Ͻ�˹�����㷨������֮�����·��
void button3(int flag);//ʵ�ְ�ť3_1���۹��ѯ�����·��
void preBfs(int* cnt, int n, int flag);//������洢�������������·��
void backtrack(int cur, int n, int flag);//���ݷ����������n�����������·����n>10ʱ�ٶ����Ա���
void insertway(int n);//������ٲ���ȷ����
//void button4(int flag);//ʵ�ְ�ť5_1����������������

int main()
{
    initgraph(width, height);//������
    BeginBatchDraw();
    setbkcolor(colorWND);//���ô��ڱ�����ɫ
    settextcolor(BLACK);//����������ɫ
    cleardevice();//����
    initScenery();//��ʼ������
    initLine();//��ʼ��·��
    drawMap();//����ͼ
    drawOutput(0);//����ʾ��
    drawOutput(1);//�������
    drawButton();//����ť
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
        msg = getmessage(EM_MOUSE);//��ȡ�����Ϣ
        if (msg.message == WM_LBUTTONDOWN)//�������������
        {
            if (msg.x >= 20 && msg.x <= 640 && msg.y >= 20 && msg.y <= 640) clickScenecy(msg);//��������ͼ����ʵ�ֵ����ѯ
            else if (msg.x >= 680 && msg.x <= 960 && msg.y >= 40 && msg.y <= 200) jumpButton(msg);//��������ť����
        }
        //BeginBatchDraw();
        //FlushBatchDraw();
        //EndBatchDraw();
    }
}

void initScenery()//��ʼ������
{
    //����
    scenery[0].x = 10;
    scenery[0].y = 10;
    scenery[0].ticket = 68;
    strcpy(scenery[0].number, "01");
    strcpy(scenery[0].name, "�ƺ�¥");
    strcpy(scenery[0].introduce, "�ش���ɽ֮�ۣ��������ﳤ��");

    scenery[1].x = 20;
    scenery[1].y = 2;
    scenery[1].ticket = 200;
    strcpy(scenery[1].number, "02");
    strcpy(scenery[1].name, "���ֹ�");
    strcpy(scenery[1].introduce, "ʱ�С����С�����ķ�������\n���ĵ�");//һ�����13����

    scenery[2].x = 8;
    scenery[2].y = 18;
    scenery[2].ticket = 50;
    strcpy(scenery[2].number, "03");
    strcpy(scenery[2].name, "��������");
    strcpy(scenery[2].introduce, "�ڽ���֮�ϣ���������⣬ҹ\n������");

    scenery[3].x = 1;
    scenery[3].y = 1;
    scenery[3].ticket = 100;
    strcpy(scenery[3].number, "04");
    strcpy(scenery[3].name, "֪����");
    strcpy(scenery[3].introduce, "20����30������Ĵ�������\n�����ִ�");

    scenery[4].x = 14;
    scenery[4].y = 7;
    scenery[4].ticket = 150;
    strcpy(scenery[4].number, "05");
    strcpy(scenery[4].name, "����糡");
    strcpy(scenery[4].introduce, "�����Ƽ������յ��������֣�\n���缶ˮ����");

    scenery[5].x = 3;
    scenery[5].y = 7;
    scenery[5].ticket = 75;
    strcpy(scenery[5].number, "06");
    strcpy(scenery[5].name, "ľ����ԭ");
    strcpy(scenery[5].introduce, "���е������еĲ�ԭ��������\n��");

    scenery[6].x = 16;
    scenery[6].y = 10;
    scenery[6].ticket = 350;
    strcpy(scenery[6].number, "07");
    strcpy(scenery[6].name, "����");
    strcpy(scenery[6].introduce, "�������С�ĥɽ�ͻ��������\n�񡢴�����ɽ");

    scenery[7].x = 5;
    scenery[7].y = 3;
    scenery[7].ticket = 30;
    strcpy(scenery[7].number, "08");
    strcpy(scenery[7].name, "��������");
    strcpy(scenery[7].introduce, "����������ɽ�ͺ�����ɽ֮\n�䣬�人�ر��Խ���");

    scenery[8].x = 17;
    scenery[8].y = 15;
    scenery[8].ticket = 120;
    strcpy(scenery[8].number, "09");
    strcpy(scenery[8].name, "�����");
    strcpy(scenery[8].introduce, "�����ڳ��Ӻ��֣��ۼ����ڶ�\n��������");

    scenery[9].x = 9;
    scenery[9].y = 5;
    scenery[9].ticket = 20;
    strcpy(scenery[9].number, "10");
    strcpy(scenery[9].name, "���ڽ�̲");
    strcpy(scenery[9].introduce, "��֤�ˡ����人����ʢ��˥��\n�ı�־�Ծ���");

    scenery[10].x = 6;
    scenery[10].y = 15;
    scenery[10].ticket = 60;
    strcpy(scenery[10].number, "11");
    strcpy(scenery[10].name, "�紨��");
    strcpy(scenery[10].introduce, "�볤������Ļƺ�¥�н�����\n����ƺ�¥������̨���ơ���\n��ʥ����");

    scenery[11].x = 1;
    scenery[11].y = 11;
    scenery[11].ticket = 40;
    strcpy(scenery[11].number, "12");
    strcpy(scenery[11].name, "��Ԫ����");
    strcpy(scenery[11].introduce, "����Ԫ��������ƽ��������\n��");

    scenery[12].x = 13;
    scenery[12].y = 4;
    scenery[12].ticket = 80;
    strcpy(scenery[12].number, "13");
    strcpy(scenery[12].name, "����԰");
    strcpy(scenery[12].introduce, "λ���人�к�����īˮ����");

    scenery[13].x = 12;
    scenery[13].y = 20;
    scenery[13].ticket = 15;
    strcpy(scenery[13].number, "14");
    strcpy(scenery[13].name, "�Ϻ�");
    strcpy(scenery[13].introduce, "λ��������ϲ������人�н�\n���ڶ�������ѷ���ĵ������\n�к�");
}

void drawPoint()//����
{
    for (int i = 0; i < scnt; i++)
    {
        int sx = scenery[i].x;
        int sy = scenery[i].y;
        setfillcolor(colorPOINT1);//���þ�����ɫ
        solidcircle(leftLength + sx * length, upLength + sy * length, radius);
    }
}

void initLine()//��ʼ����
{
    //srand((unsigned int)time(NULL));
    //srand()�������������
    //�������rand()
    for (int i = 0; i < maxNum; i++)
        for (int j = 0; j < maxNum; j++)
            for (int k = 0; k < 2; k++)
            {
                //rand() + i + j + k //Ĭ������Ϊ1
                srand(rand() + i + j + k);//���������
                int odd = rand() % 4;//0 - 6wan -> 0 - 3
                edgeTraffic[i][j][k] = odd;//����·�εȸ���
            }
    for (int i = 0; i < maxNum; i++)//��Ե·��ȥ��
    {
        edgeTraffic[i][maxNum - 1][1] = 0;//�±�
        edgeTraffic[maxNum - 1][i][0] = 0;//�ұ�
    }
}

void drawLine()//����
{
    for (int i = 0; i < maxNum; i++)
        for (int j = 0; j < maxNum; j++)
            for (int k = 0; k < 2; k++)
            {
                if (edgeTraffic[i][j][k] == 0) continue;
                switch (edgeTraffic[i][j][k])
                {
                case 1:
                    setlinecolor(colorWALK);//����
                    break;
                case 2:
                    setlinecolor(colorBUS);//����
                    break;
                case 3:
                    setlinecolor(colorSUBWAY);//����
                    break;
                }
                int x1, y1, x2, y2;
                //���
                x1 = leftLength + i * length;
                y1 = upLength + j * length;
                //�յ�
                x2 = x1 + (1 - k) * length;
                y2 = y1 + k * length;
                line(x1, y1, x2, y2);//����
            }
}

void drawMap()//���»��Ƶ�ͼ����
{
    BeginBatchDraw();
    setbkcolor(colorMAP);
    clearrectangle(20, 20, 640, 640);
    drawLine();
    drawPoint();
    EndBatchDraw();
}

void button(int bx, int by, int w, int h, const char* str)//��һ����ť
{
    //bx,by��ť���Ͻ�����ֵ��w��ȣ�h�߶�
    setfillcolor(colorBUTTON);
    solidroundrect(bx, by, bx + w, by + h, 5, 5);
    setbkcolor(colorBUTTON);//�������ֱ�����ɫ
    settextcolor(BLACK);//����������ɫ
    settextstyle(22, 0, "����");//����������
    int tx, ty;//�������־���
    tx = bx + (w - textwidth(str)) / 2;
    ty = by + (h - textheight(str)) / 2;
    outtextxy(tx, ty, str);
}

void drawButton()//����ť����
{
    BeginBatchDraw();
    setbkcolor(colorWND);
    clearrectangle(680, 40, 960, 200);//���ԭ����
    char str[7][50];
    int strNum[5] = { 7, 4, 6, 6, 4 };
    switch (buttonFLAG)
    {
    case 0://������
    {
        char str0[7][50] = { "��ӭʹ�ó��е���С����", "�����ѯ", "��·��ѯ", "�۹��ѯ", "����", "ʹ��˵��", "����" };
        for (int i = 0; i < strNum[buttonFLAG]; i++)
            strcpy(str[i], str0[i]);
        break;
    }
    case 1:
    {
        char str0[4][50] = { "��ӭʹ�þ����ѯ��", "��Ų�ѯ", "���Ʋ�ѯ", "����" };
        for (int i = 0; i < strNum[buttonFLAG]; i++)
            strcpy(str[i], str0[i]);
        break;
    }
    case 2:
    {
        char str0[6][50] = { "��ӭʹ����·��ѯ��", "���·��", "���ʱ��", "��ͻ���", "��������", "����" };
        for (int i = 0; i < strNum[buttonFLAG]; i++)
            strcpy(str[i], str0[i]);
        break;
    }
    case 3:
    {
        char str0[6][50] = { "��ӭʹ�ù۹��ѯ��", "���·��", "���ʱ��", "��ͻ���", "��������", "����" };
        for (int i = 0; i < strNum[buttonFLAG]; i++)
            strcpy(str[i], str0[i]);
        break;
    }
    case 4:
    {
        char str0[4][50] = { "��ӭʹ���������ܣ�", "��������", "�ر�����", "����" };
        for (int i = 0; i < strNum[buttonFLAG]; i++)
            strcpy(str[i], str0[i]);
        break;
    }
    }
    outText(str[0], 0);//��ʾ�������ӭʹ����Ϣ
    for (int i = 1; i < strNum[buttonFLAG]; i++)
        button(buttonX[i - 1], buttonY[i - 1], buttonW, buttonH, str[i]);
    EndBatchDraw();
}

void drawOutput(int flag)//����ʾ���������
{
    //������ɫ
    setfillcolor(colorOUTPUT);
    setlinecolor(BLACK);
    //������
    if (flag == 0) fillrectangle(660, 240, 980, 280);
    else if (flag == 1) fillrectangle(660, 280, 980, 640);
}

void outText(const char* text, int flag)//����ʾ��������������Ϣ
{
    //outtextxy
    BeginBatchDraw();
    drawOutput(flag);//���»��ƾ���
    RECT r;
    if (flag == 0) r = { 670, 250, 970, 270 };
    else if (flag == 1) r = { 670, 290, 970, 630 };
    settextcolor(BLACK);
    setbkcolor(colorOUTPUT);
    settextstyle(22, 0, "����");
    //\n���з���
    drawtext(text, &r, DT_LEFT | DT_TOP);
    EndBatchDraw();
}

void clickScenecy(ExMessage msg)//�����ѯ,�����ѯ
{
    int i = (msg.x - 15) / 30;
    int j = (msg.y - 15) / 30;
    //int i = (msg.x + 30) / 120;
    //int j = (msg.y + 30) / 120;
    for (int k = 0; k < scnt; k++)
    {
        if (i == scenery[k].x && j == scenery[k].y)
        {
            outText("��ӭʹ�õ����ѯ��", 0);
            outScenery(k);//���������Ϣ
            drawPoint();//���»������е�
            setRED(k);//��Ŀ�����Ϊ��ɫ
            break;
        }
    }
}

void outScenery(int i)//�ڵڶ������������Ϣ
{
    drawOutput(1);
    RECT r = { 670, 290, 970, 630 };
    setbkmode(colorOUTPUT);
    settextstyle(22, 0, "����");
    //strlen���ַ�������
    int sLength = strlen(scenery[i].number) + strlen(scenery[i].name) + strlen(scenery[i].introduce) + 50;
    char* result = (char*)malloc(sizeof(char) * sLength);
    //strcpy���ƣ�strcatƴ��
    strcpy(result, scenery[i].number);
    strcat(result, "  ");
    strcat(result, scenery[i].name);
    strcat(result, "\n");
    strcat(result, "Ʊ�ۣ�");
    char q[50];
    //sprintf��%d����
    sprintf(q, "%dԪ\n", scenery[i].ticket);
    strcat(result, q);
    strcat(result, scenery[i].introduce);
    drawtext(result, &r, DT_LEFT | DT_TOP);
}

void setRED(int i)//��Ŀ�꾰������Ϊ��ɫ
{
    setfillcolor(colorPOINT2);//���ú�ɫ
    int dx = leftLength + scenery[i].x * length;
    int dy = upLength + scenery[i].y * length;
    solidcircle(dx, dy, radius);
}

void jumpButton(ExMessage msg)//��ť������ת
{
    const int buttonNum[5] = { 6, 3, 5, 5, 3 };
    for (int k = 0; k < buttonNum[buttonFLAG]; k++)
    {
        if (msg.x >= buttonX[k] && msg.x <= buttonX[k] + buttonW && msg.y >= buttonY[k] && msg.y <= buttonY[k] + buttonH)
        {
            switch (buttonFLAG)
            {
            case 0://������
            {
                if (k == 4)//ʹ��˵��
                {
                    drawOutput(1);
                    outText("��ӭ�ۿ�ʹ��˵����", 0);
                    outText("ͼ���̵��ʾ���㣬��ͬ��ɫ\n\
�߶α�ʾ��·����ʹ�õĽ�ͨ\n\
���߲�ͬ����ɫ��ʾ����·�Σ�\n\
��ɫ��ʾ����·�Σ���ɫ��ʾ\n\
����·�Σ�ÿ��λ����Ϊ1��\n\
�����ʱ���趨Ϊ�����У�\n\
10min/km������3min/km����\n\
��1min/km���۸��趨Ϊ����\n\
����ѣ�������3Ԫ/�Σ�����\n\
5Ԫ/�Ρ�\n\
���⣬��·��ѯ������������\n\
�������·�����۹��ѯ����\n\
�������������·������\"��\n\
��\"�����ѡ�񲥷�/�ر����֡�", 1);
                }
                else if (k == 5)//����
                {
                    drawMap();
                    drawOutput(0);
                    drawOutput(1);
                    outText("��ӭʹ�ó��е���С����", 0);
                }
                else
                {
                    buttonFLAG = k + 1;
                    drawButton();
                }
                break;
            }
            case 1://�����ѯ����
            {
                if (k == 0) button1_1();//ʵ�ֱ�Ų�ѯ
                else if (k == 1) button1_2();//ʵ�����Ʋ�ѯ
                else if (k == 2)//����
                {
                    buttonFLAG = 0;
                    drawButton();
                }
                break;
            }
            case 2://��·
            {
                if (k == 4)
                {
                    buttonFLAG = 0;
                    drawButton();
                }
                else button2(k + 1);//ʵ����·��ѯ
                break;
            }
            case 3://�۹�
            {
                if (k == 4)
                {
                    buttonFLAG = 0;
                    drawButton();
                }
                else button3(k + 1);//ʵ�ֹ۹��ѯ
                break;
            }
            case 4://����
            {
                if (k == 2)
                {
                    buttonFLAG = 0;
                    drawButton();
                }
                //else button4(k + 1);//ʵ�ֲ���/�ر�����
                break;
            }
            }
            break;
        }
    }
}

void button1_1()//ʵ�ְ�ť1_1�������ѯ����Ų�ѯ
{
    char s[3];
    outText("��ӭʹ�ñ�Ų�ѯ��", 0);
    InputBox(s, 3, "��������Ҫ��ѯ�ı��");//��������
    for (int i = 0; i < scnt; i++)
    {
        if (s[0] == scenery[i].number[0] && s[1] == scenery[i].number[1])
        {
            outScenery(i);//�����i��������Ϣ
            drawPoint();
            setRED(i);
            return;
        }
    }
    drawOutput(1);
    outText("�޴˾��㣡", 1);
}

void button1_2()//ʵ�ְ�ť1_2�������ѯ�����Ʋ�ѯ
{
    char s[9];
    outText("��ӭʹ�����Ʋ�ѯ��", 0);
    InputBox(s, 9, "��������Ҫ��ѯ������");
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
    if (flag)//�������
    {
        outScenery(cnt);
        drawPoint();
        setRED(cnt);
    }
    else
    {
        drawOutput(1);
        outText("�޴˾��㣡", 1);
    }
}

void button2(int flag)//ʵ�ְ�ť2_1����·��ѯ�����·��
{
    BeginBatchDraw();
    drawMap();
    char tip[4][50] = { "��ӭ��ѯ���·����", "��ӭ��ѯ���ʱ�䣡", "��ӭ��ѯ��ͻ��ѣ�", "��ӭ��ѯ�������٣�" };
    outText(tip[flag - 1], 0);
    char s[2][3];//���������������
    int nums[2];//�����Ŷ�Ӧ�ĵڼ�������
    char q[2][50] = { "����������ǰ���ھ����ţ�", "������ƻ�ǰ�������ţ�" };
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
            outText("��ӭʹ����·��ѯ��", 0);
            outText("�޴˾��㣡", 1);
            return;
        }
    }
    if (nums[0] == nums[1])
    {
        drawOutput(0);
        drawOutput(1);
        outText("��ӭʹ����·��ѯ��", 0);
        outText("������Ŀ�꾰�㣡", 1);
        return;
    }
    outText("������", 1);
    int a;//�洢����·��ֵ
    int n1 = nums[0];
    int n2 = nums[1];
    a = Dijkstra(n1, n2, flag);//�Ͻ�˹�����㷨
    drawOutput(1);
    char s1[50] = "·����ͼ��ʾ��\n";

    char str1[4][50] = { "���·��Ϊ", "���ʱ��Ϊ", "��ͨ��ͻ���Ϊ", "��ͨ��ͻ���Ϊ" };
    strcat(s1, str1[flag - 1]);

    char s2[10];
    //sprintf(s2, "%d", pointS[a].data);
    sprintf(s2, "%d", a);
    strcat(s1, s2);

    char str2[4][50] = { "����", "����", "Ԫ", "����" };
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

int Dijkstra(int n1, int n2, int flag)//�Ͻ�˹�����㷨������֮�����·��
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
        for (int k = 0; k < 4; k++)//��������
        {
            int nx = i + dx[k];
            int ny = j + dy[k];
            int mx = i + ex[k];
            int my = j + ey[k];
            int mi = ei[k];
            if (nx >= 0 && nx <= 21 && ny >= 0 && ny < 21 && edgeTraffic[mx][my][mi] > 0)
            {
                int temp;//�洢Ȩֵ
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

int preColor(point cur)//Ѱ��ǰһ��·�ε���ɫ
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

void drawLines(int targetX, int targetY, int startX, int startY)//������ͷ�Ĺ켣
{
    //���켣
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

    //����ͷ
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

void button3(int flag)//ʵ�ְ�ť3_1���۹��ѯ�����·��
{
    BeginBatchDraw();
    drawMap();
    char tip[4][50];
    outText(tip[flag - 1], 0);
    char t[3];;
    InputBox(t, 3, "���������������ľ��������");
    int n;
    if (t[1] < '0' || t[1] > '9')
    {
        if (t[0] < '2' || t[0] > '9')
        {
            outText("��ӭʹ�ù۹��ѯ��", 0);
            outText("���벻�Ϸ���", 1);
            return;
        }
        n = t[0] - 48;
    }
    else
    {
        if (t[0] < '0' || t[0] > '9')
        {
            outText("��ӭʹ�ù۹��ѯ��", 0);
            outText("���벻�Ϸ���", 1);
            return;
        }
        n = (t[0] - 48) * 10 + (t[1] - 48);
    }
    if (n == 1)
    {
        outText("��ӭʹ�ù۹��ѯ��", 0);
        outText("���벻�Ϸ���", 1);
        return;
    }
    if (n > scnt)
    {
        outText("��ӭʹ�ù۹��ѯ��", 0);
        outText("�������о�������", 1);
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
        InputBox(s[i], 3, "�����뾰���ţ�");
        bool flag_ = false;
        for (int j = 0; j < scnt; j++)
        {
            if (s[i][0] == scenery[j].number[0] && s[i][1] == scenery[j].number[1])
            {
                for (int k = 0; k < i; k++)
                {
                    if (j == cnt[k])
                    {
                        outText("��ӭʹ�ù۹��ѯ��", 0);
                        outText("���������ظ����㣡", 1);
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
            outText("��ӭʹ�ù۹��ѯ��", 0);
            outText("�޴˾��㣡", 1);
            return;
        }
    }
    preBfs(cnt, n, flag);//���ÿ��������֮�������·��
    outText("������...", 1);
    char s1[1000] = "·����ͼ��ʾ��\n";
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
    char str1[4][50] = { "���·��Ϊ", "���ʱ��Ϊ", "��ͨ��ͻ���Ϊ", "�����貽��" };
    strcat(s1, str1[flag - 1]);
    strcat(s1, s2);
    char str2[4][50] = { "����", "����", "Ԫ", "����" };
    strcat(s1, str2[flag - 1]);
    int allmoney = scenery[cnt2[0]].ticket;
    for (int i = 1; i < n; i++)
    {
        int ni = cnt2[i - 1];//��ʼ����
        int nj = cnt2[i];//Ŀ�����
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
    sprintf(q, "\n�����������Ʊ����%dԪ", allmoney);
    strcat(s1, q);
    outText(s1, 1);
    /*char q[10];
    sprintf(q, "Ϊ��%d", pointDis_[0][1].onePoint[0].pre->x);
    clearOutput2();
    outMessage(q);*/
    EndBatchDraw();
}

void preBfs(int* cnt, int n, int flag)//������洢�������������·��
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

void backtrack(int cur, int n, int flag)//���ݷ����������n�����������·����n>10ʱ�ٶ����Ա���
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

void insertway(int n)//������ٲ���ȷ����
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

//void button4(int flag)//ʵ�ְ�ť5_1����������������
//{
//    switch (flag)
//    {
//    case 1:
//        mciSendString("play 1.mp3 repeat", 0, 0, 0);
//        outText("���ųɹ���", 0);
//        break;
//    case 2:
//        mciSendString("close 1.mp3", 0, 0, 0);
//        outText("�رճɹ���", 0);
//        break;
//    }
//}
