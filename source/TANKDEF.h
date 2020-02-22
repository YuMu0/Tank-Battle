#ifndef _TANKDEF_H_		//为了防止重复的包含
#define _TANKDEF_H_

#include<Windows.h>		//handle
#include<stdio.h>
#include"linklist.h"

/*游戏界面宏*/
#define GAME_AREA_WIDTH  31		//游戏区域宽度
#define GAME_AREA_HEIGHT 20		//游戏区域高度
#define LEFTTOP_X 4				//左上角坐标
#define LEFTTOP_Y 1				//左上角坐标
#define BORDER_LETTER     "□"	//游戏边框字符
#define TANK_LETTER       "■"	//坦克字符
#define HOME_LETTER       "★"	//老家
#define SHELL_LETTER      "◎"	//炮弹
#define WALL_LETTER       "■"	//墙壁

#define OWNER_SELF  1			//我方坦克
#define OWNER_ENEMY 0			//敌方坦克

/*游戏区域数组各值*/
#define AREA_OVERFLOW -1		//越界
#define AREA_SPACE	   0		//空白
#define AREA_WALL      1		//墙壁
#define AREA_HOME      2		//老家
#define AREA_SELF      3		//我方坦克
#define AREA_ENEMY     4		//敌方坦克
#define AREA_SHELL	   5		//敌方炮弹

/*游戏参数信息*/
#define AREA_ENEMY_COUNT 4		//图上敌方坦克数量
#define ENEMY_COUNT		 10		//敌方坦克总数
#define SELF_COUNT		 3		//我方坦克总数
#define ENEMY_SPEED		 400	//敌方坦克速度（ms)
#define SHELL_SPEED		 200	//炮弹速度
#define SELF_SHELL_COUNT 3		//射出炮弹最多数量

/*游戏状态*/
#define GAME_RUNNING	1		//游戏开始
#define GAME_PAUSE		2		//游戏暂停
#define GAME_OVER		0		//游戏结束

/*自定义类型*/
typedef enum{UP,DOWN,LEFT,RIGHT}DIR;	//方向枚举
/*定义坦克结构*/
typedef struct {
	int x;
	int y;			//坐标
	DIR dir;		//位置
	WORD color;		//颜色
	int speed;		//速度(多少毫秒走一步）
	int life;		//生命值
	int owner;		//属主（敌/我）
}TANK;

/*定义炮弹结构*/
typedef struct {
	int x;
	int y;			//坐标
	DIR dir;		//方向
	WORD color;		//颜色
	int speed;		//速度（多少毫秒走一步）
	int owner;		//属主（敌/我）
	int num;		//编号
	int isshow;		//是否显示
	int life;		//生命
}SHELL;

/*定义炮弹节点*/
typedef struct {
	NODE node;			//链表节点
	SHELL shell;		//炮弹节点
}SHELLNODE;

/*全局变量，申明时不能赋值，否则会被视为定义*/
extern HANDLE g_hout;					//标准输出句柄，初始化在game.h中
extern char map[14][GAME_AREA_WIDTH];	//地图数组
extern char tank_data[4][3][3];			//坦克四个方向
extern TANK g_my_tank;					//我的坦克
extern char g_area_data[GAME_AREA_HEIGHT][GAME_AREA_WIDTH];		//游戏区域二维数组
extern TANK g_enemy[AREA_ENEMY_COUNT];	//敌方坦克数组
extern WORD g_colors[11];				//颜色定义
extern int g_state;						//游戏状态声明
extern LINKLIST *g_shell_list;			//炮弹链表
extern int g_self_shell_count;			//当前我方炮弹的数量
extern CRITICAL_SECTION g_cs;			//临界区资源
extern g_died_enemy;					//已经阵亡的坦克数量

#endif