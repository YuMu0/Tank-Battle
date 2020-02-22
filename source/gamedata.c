#include<Windows.h>
#include"TANKDEF.h"
/*地图数组*/
char map[14][GAME_AREA_WIDTH] = 
{
{ 0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0 },
{ 0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0 },
{ 0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1 },
{ 1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0 },
{ 0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0 },
{ 0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0 }
};

/*坦克的四个方向*/
char tank_data[4][3][3] =
{
	{
		0,1,0,
		1,1,1,
		1,0,1
},
	{
		1,0,1,
		1,1,1,
		0,1,0
	},
	{
		0,1,1,
		1,1,0,
		0,1,1
	},
	{
		1,1,0,
		0,1,1,
		1,1,0
	}
};


/*整个游戏区域的二维数组*/
char g_area_data[GAME_AREA_HEIGHT][GAME_AREA_WIDTH];

/*我方坦克*/
TANK g_my_tank;

/*敌方坦克数组*/
TANK g_enemy[AREA_ENEMY_COUNT];

/*颜色数组*/
WORD g_colors[11] = {
	FOREGROUND_RED,
	FOREGROUND_GREEN,
	FOREGROUND_BLUE,
	FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_GREEN ,
	FOREGROUND_RED | FOREGROUND_BLUE ,
	FOREGROUND_GREEN | FOREGROUND_BLUE
};

/*游戏状态声明*/
int g_state = GAME_RUNNING;

/*炮弹链表*/
LINKLIST *g_shell_list;

/*当前我方炮弹的数量*/
int g_self_shell_count = 0;

/*临界区资源*/
CRITICAL_SECTION g_cs;

/*阵亡的敌方坦克数量*/
int g_died_enemy = 0;