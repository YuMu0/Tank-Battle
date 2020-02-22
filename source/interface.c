#include"TANKDEF.h"
#include"game.h"
#include"interface.h"
#include<stdio.h>
HANDLE g_hout;						//标准输出句柄
CONSOLE_SCREEN_BUFFER_INFO csbi;	//控制台屏幕信息缓冲结构
DWORD dwlen;						//填充属性字符的个数

/*绘制游戏边框*/
void DrawGameBoard()
{
	int i, j;
	COORD coord;		//结构体，坐标x,y
	coord.X = LEFTTOP_X;
	coord.Y = LEFTTOP_Y;
	for (i = 0; i < GAME_AREA_HEIGHT+ 2; i++)
	{
		/*把光标定位到行首的位置*/
		SetConsoleCursorPosition(g_hout, coord);
		for (j = 0; j < GAME_AREA_WIDTH + 2; j++)
		{
			if (i == 0 || i == GAME_AREA_HEIGHT + 1 || j == 0 || j == GAME_AREA_WIDTH + 1)
				printf("%s", BORDER_LETTER);
			else
				printf("  ");
		}
		coord.Y++;		//光标位置置于下一行
	}
}

/*绘制地图*/
void DrawMap()
{
	int i, j;
	COORD coord = { LEFTTOP_X + 2,LEFTTOP_Y + 4 };
	for (i = 0; i < GAME_AREA_HEIGHT - 6; i++)
	{
		/*定位光标*/
		SetConsoleCursorPosition(g_hout, coord);
		for (j = 0; j < GAME_AREA_WIDTH; j++)
		{
			if (map[i][j] == 1)
				printf("%s", WALL_LETTER);
			else
				printf("  ");
		}
		coord.Y++;		//移向下一行
	}
}

/*绘制老家*/
void DrawHome()
{
	int i, j;
	COORD coord = { LEFTTOP_X + 30,LEFTTOP_Y + 18 };
	for (i = 0; i < 3; i++)
	{
		SetConsoleCursorPosition(g_hout, coord);
		for (j = 0; j < 3; j++)
		{
			if (i == 1 && j == 1)
				printf("%s", HOME_LETTER);		//画出老家
			else
			    printf("%s", WALL_LETTER);		//画出墙壁
		}
		coord.Y++;								//移向下一行
	}
}

/*画坦克*/
void DrawTank(TANK *pTank, const char *letter)
{
	int i, j;
	COORD coord = { pTank->x,pTank->y };
	for (i = 0; i < 3; i++)
	{
		SetConsoleCursorPosition(g_hout, coord);
		for (j = 0; j < 3; j++)
		{
			if (tank_data[pTank->dir][i][j])
			{
				/*获取当前光标位置信息*/
				GetConsoleScreenBufferInfo(g_hout, &csbi);
				printf("%s", letter);
				/*对坦克字符进行颜色设置*/
				FillConsoleOutputAttribute(g_hout, pTank->color, 2, csbi.dwCursorPosition, &dwlen);
			}
			else
				printf("  ");
		}
		coord.Y++;
	}
}

/*坦克转向*/
void ChangeDir(TANK *pTank, DIR dir)
{
	/*擦除原来的坦克*/
	DrawTank(pTank, "  ");
	pTank->dir = dir;
	/*画新的坦克*/
	DrawTank(pTank, TANK_LETTER);
	/*将新方向的坦克的数据重新导入到游戏区域数组*/
	if (pTank->owner == OWNER_SELF)
		ImportMyTankToGameArray();
	else
		ImportEnemyTankToGameArray(pTank);
}

/*绘制炮弹*/
void DrawShell(SHELL *pShell, const char *letter)
{
	COORD coord = { pShell->x,pShell->y };
	SetConsoleCursorPosition(g_hout, coord);
	printf("%s", letter);
	/*填充颜色*/
	FillConsoleOutputAttribute(g_hout, pShell->color, 2, coord, &dwlen);
}

/*击中墙壁*/
void HitWall(SHELL *pShell)
{
	int row, col;
	int i;
	row = CoordYConvertToArrayIndex(pShell->y);
	col = CoordXConvertToArrayIndex(pShell->x);
	/*根据炮弹方向来击破墙壁*/
	if (pShell->dir == UP || pShell->dir == DOWN)
	{
		/*上下飞行的炮弹，击毁横向的3块墙壁*/
		COORD pos = { pShell->x - 2, pShell->y };
		for (i = col-1; i < col + 2; i++)
		{
			SetConsoleCursorPosition(g_hout, pos);
			printf("  ");		//擦除墙壁
			g_area_data[row][i] = AREA_SPACE;		//修改数组记录
			pos.X += 2;
		}
	}
	else
	{
		/*左右飞行的炮弹，击毁纵向的3块墙壁*/
		COORD pos = { pShell->x, pShell->y-1 };
		for (i = row - 1; i < row + 2; i++)
		{
			SetConsoleCursorPosition(g_hout, pos);
			printf("  ");		//擦除墙壁
			g_area_data[i][col] = AREA_SPACE;		//修改数组记录
			pos.Y++;
		}
	}
}

/*击中老家*/
void HitHome(SHELL *pShell)
{
	COORD pos = { pShell->x,pShell->y };
	SetConsoleCursorPosition(g_hout, pos);
	printf("  ");		//擦除老家

}

/*打印游戏失败结束信息*/
void DrawGameOver()
{
	int i;
	COORD pos = { LEFTTOP_X + GAME_AREA_WIDTH - 4,LEFTTOP_Y + 4 };
	for (i = 0; i < 5; i++)
	{
		SetConsoleCursorPosition(g_hout, pos);
		if (i == 2)
			printf("    GAME  OVER    ");
		else
			printf("                  ");
		FillConsoleOutputAttribute(g_hout, BACKGROUND_RED | FOREGROUND_GREEN, 18, pos, &dwlen);
		pos.Y++;
	}
}

/*打印游戏胜利消息*/
void DrawWin()
{
	int i;
	COORD pos = { LEFTTOP_X + GAME_AREA_WIDTH - 4,LEFTTOP_Y + 4 };
	for (i = 0; i < 5; i++)
	{
		SetConsoleCursorPosition(g_hout, pos);
		if (i == 2)
			printf("    YOU  WIN    ");
		else
			printf("                  ");
		FillConsoleOutputAttribute(g_hout, BACKGROUND_RED | FOREGROUND_GREEN, 18, pos, &dwlen);
		pos.Y++;
	}
}