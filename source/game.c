#pragma warning(disable:4996)
#include<stdlib.h>
#include<time.h>
#include"linklist.h"
#include"TANKDEF.h"
#include"interface.h"
#include"game.h"

//游戏初始化
void InitGame()
{
	int i;
	HANDLE hThread;							//定义线程句柄

	/*初始化临界区资源*/
	InitializeCriticalSection(&g_cs);

	g_hout = GetStdHandle(STD_OUTPUT_HANDLE);		//用标准输出初始化
	DrawGameBoard();
	DrawMap();
	DrawHome();	
	InitMyTank();
	DrawTank(&g_my_tank,TANK_LETTER);

	ImportHomeToGameArea();
	ImportMapToGameArea();
	ImportMyTankToGameArray();

	srand((unsigned int)time(NULL));	//初始化随机种子

	/*创建敌方坦克*/
	for (i = 0; i < AREA_ENEMY_COUNT; i++)
	{
		g_enemy[i] = CreateEnemyTank();				//创建一个敌方坦克
		ImportEnemyTankToGameArray(&g_enemy[i]);	//将创建的坦克导入地图数组
		DrawTank(&g_enemy[i], TANK_LETTER);			//画出敌方坦克
	}
	/*创建线程，让敌方坦克自动移动*/
	while (i--)
	{
		hThread = CreateThread(NULL, 0, EnemyMove, &g_enemy[i], 0, NULL);
		CloseHandle(hThread);						//减少对线程句柄的引用计数
	}
	/*初始化炮弹链表*/
	InitLinkList(&g_shell_list);
}

/*游戏结束的释放*/
void GameOver()
{
	/*删除临界区资源*/
	DeleteCriticalSection(&g_cs);
	/*销毁炮弹链表*/
	DestroyLinkList(&g_shell_list);
}

/*我方坦克初始化*/
void InitMyTank()
{
	g_my_tank.x = LEFTTOP_X + 24;
	g_my_tank.y = LEFTTOP_Y + 18;
	g_my_tank.dir = UP;
	g_my_tank.color = FOREGROUND_BLUE|FOREGROUND_INTENSITY;
	g_my_tank.life = 1;
	g_my_tank.owner = OWNER_SELF;
}

/*地图数据导入二维数组*/
void ImportMapToGameArea()
{
	int i, j;
	for (i = 0; i < 14; i++)
	{
		for (j = 0; j < GAME_AREA_WIDTH; j++)
		{
			g_area_data[i + 3][j] = map[i][j];
		}
	}
}

/*将老家导入游戏区域二维数组*/
void ImportHomeToGameArea()
{
	int i, j;
	int row, col;
	row = CoordYConvertToArrayIndex(LEFTTOP_Y + 18);
	col = CoordXConvertToArrayIndex(LEFTTOP_X + 30);
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (i == 1 && j == 1)	//老家
				g_area_data[row + i][col + j] = AREA_HOME;
			else					//墙壁
				g_area_data[row + i][col + j] = AREA_WALL;

		}
	}
}

/*将我方坦克到游戏区域数组*/
void ImportMyTankToGameArray()
{
	int i, j;
	int row, col;
	row = CoordYConvertToArrayIndex(g_my_tank.y);
	col = CoordXConvertToArrayIndex(g_my_tank.x);
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (tank_data[g_my_tank.dir][i][j])
				g_area_data[row + i][col + j] = AREA_SELF;
			else
				g_area_data[row + i][col + j] = AREA_SPACE;
		}
	}
}

/*将敌方坦克到游戏区域数组*/
void ImportEnemyTankToGameArray(TANK *pTank)
{
	int i, j;
	int row, col;
	row = CoordYConvertToArrayIndex(pTank->y);
	col = CoordXConvertToArrayIndex(pTank->x);
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (tank_data[pTank->dir][i][j])
				g_area_data[row + i][col + j] = AREA_ENEMY;
			else
				g_area_data[row + i][col + j] = AREA_SPACE;
		}
	}
}

/*创造一辆敌方坦克*/
TANK CreateEnemyTank()
{
	int i, j, overlap;
	int row, col;
	TANK tank;				//定义一个坦克
	tank.owner = OWNER_ENEMY;
	tank.life = 1;
	tank.dir = rand() % 4;	//一个随机方向
	tank.color = g_colors[rand()%( sizeof g_colors / sizeof g_colors[0])];
	tank.speed = ENEMY_SPEED;
	tank.y = LEFTTOP_Y + 1;
	/*坦克位置不能重叠*/
	while (1)
	{
		overlap = 0;
		tank.x = rand() % 29 * 2 + LEFTTOP_X + 2;//(LEFTTOP_X + 60 - (LEFTTOP_X + 2) + 1) + LEFTTOP_X + 2;
		/*判断坦克出现的位置是否有其他的坦克*/
		row = CoordYConvertToArrayIndex(tank.y);
		col = CoordXConvertToArrayIndex(tank.x); 
		for (i = row; i <row + 3 && !overlap; i++)
		{
			for (j = col; j < col + 3 && !overlap; j++)
			{
				if (g_area_data[i][j])
					overlap = 1;		//发现有重叠
			}
		}
		if (!overlap)					//没有重叠，终止while
			break;
	}

	return tank;
}

/*发射炮弹，参数为发射炮弹的坦克*/
SHELLNODE *Shot(TANK *pTank)
{
	static unsigned int num = 0;			//定义为静态变量，记录炮弹编号
	SHELLNODE *psn = (SHELLNODE*)malloc(sizeof(SHELLNODE));
	if (psn == NULL)
		return NULL;
	psn->node.next = NULL;
	psn->shell.color = pTank->color;		//使用坦克颜色
	psn->shell.dir = pTank->dir;			//与坦克方向一致
	psn->shell.isshow = 0;					//初始状态为不显示
	psn->shell.life = 1;					//活着
	psn->shell.num = num++;					//编号
	psn->shell.owner = pTank->owner;		//与坦克属主相同
	psn->shell.speed = SHELL_SPEED;			//速度
	switch (pTank->dir)
	{
	case UP:
		psn->shell.x = pTank->x + 2;
		psn->shell.y = pTank->y;
		break;
	case DOWN:
		psn->shell.x = pTank->x + 2;
		psn->shell.y = pTank->y + 2;
		break;
	case LEFT:
		psn->shell.x = pTank->x;
		psn->shell.y = pTank->y + 1;
		break;
	case RIGHT:
		psn->shell.x = pTank->x + 4;
		psn->shell.y = pTank->y + 1;
		break;
	}
	/*放入链表之中*/
	AddNode(g_shell_list, (NODE*)psn);
	return psn;
}

/*坦克移动,如果移动成功返回0，否则返回撞到的物体*/
/*向上移动*/
int TankMoveUp(TANK *pTank)
{
	int i, j;
	int row, col;
	row = CoordYConvertToArrayIndex(pTank->y);
	col = CoordXConvertToArrayIndex(pTank->x);
	/*检查是否越界*/
	if (row <= 0)
		return AREA_OVERFLOW;
	/*检查是否遇到障碍物*/
	for (i = col; i < col + 3; i++)
	{
		if (g_area_data[row - 1][i])
			return g_area_data[row - 1][i];		/*返回障碍物*/
	}
	/*可以向上移动*/
	/*调整全局数组，坦克部分整体上移一行*/
	for (i = row - 1; i < row + 3; i++)		//调整4行
	{
		for (j = col; j < col + 3; j++)
		{
			if (i == row + 2)
				g_area_data[i][j] = 0;		//将最后一行赋值为0
			else
			    g_area_data[i][j] = g_area_data[i + 1][j];
		}
	}
	/*擦除原来位置坦克*/
	DrawTank(pTank, "  ");
	/*绘制新的坦克*/
	--pTank->y;
	DrawTank(pTank, TANK_LETTER);
	return 0;
}
/*向下移动*/
int TankMoveDown(TANK *pTank)
{
	int i, j;
	int row, col;
	row = CoordYConvertToArrayIndex(pTank->y);
	col = CoordXConvertToArrayIndex(pTank->x);
	/*检查是否越界*/
	if (row + 2 >= GAME_AREA_HEIGHT - 1)
		return AREA_OVERFLOW;
	/*检查是否遇到障碍物*/
	for (i = col; i < col + 3; i++)
	{
		if (g_area_data[row + 3][i])
			return g_area_data[row + 3][i];	/*返回障碍物*/
	}
	/*可以向下移动*/
	/*调整全局数组，坦克部分整体下移一行*/
	for (i = row+3; i >=row; i--)		//调整4行
	{
		for (j = col; j < col + 3; j++)
		{
			if (i == row)
				g_area_data[i][j] = 0;		//将最后一行赋值为0
			else
				g_area_data[i][j] = g_area_data[i - 1][j];
		}
	}
	/*擦除原来位置坦克*/
	DrawTank(pTank, "  ");
	/*绘制新的坦克*/
	++pTank->y;
	DrawTank(pTank, TANK_LETTER);
	return 0;
}
/*向左移动*/
int TankMoveLeft(TANK *pTank)
{
	int i, j;
	int row, col;
	row = CoordYConvertToArrayIndex(pTank->y);
	col = CoordXConvertToArrayIndex(pTank->x);
	/*检查是否越界*/
	if (col <= 0)
		return AREA_OVERFLOW;
	/*检查是否有障碍物*/
	for (i = row; i < row + 3; i++)
	{
		if (g_area_data[i][col - 1])
			return g_area_data[i][col - 1];
	}
	/*可以向左移动*/
	/*调整全局数组，坦克部分整体左移一行*/
	for (i = row; i < row + 3; i++)		//调整4行
	{
		for (j = col - 1; j < col + 3; j++)
		{
			if (j == col + 2)
				g_area_data[i][j] = 0;		//将最后一行赋值为0
			else
				g_area_data[i][j] = g_area_data[i][j + 1];
		}
	}
	/*擦除原来位置坦克*/
	DrawTank(pTank, "  ");
	/*绘制新的坦克*/
	pTank->x -= 2;
	DrawTank(pTank, TANK_LETTER);
	return 0;
}
/*向右移动*/
int TankMoveRight(TANK *pTank)
{
	int i, j;
	int row, col;
	row = CoordYConvertToArrayIndex(pTank->y);
	col = CoordXConvertToArrayIndex(pTank->x);
	/*检查是否越界*/
	if (col + 2 >= GAME_AREA_WIDTH-1)
		return AREA_OVERFLOW;
	/*检查是否有障碍物*/
	for (i = row; i < row + 3; i++)
	{
		if (g_area_data[i][col + 3])
			return g_area_data[i][col + 3];
	}
	/*可以向右移动*/
	/*调整全局数组，坦克部分整体右移一行*/
	for (i = row; i < row + 3; i++)		//调整4行
	{
		for (j = col + 3; j >= col; j--)
		{
			if (j == col)
				g_area_data[i][j] = 0;		//将最后一行赋值为0
			else
				g_area_data[i][j] = g_area_data[i][j - 1];
		}
	}
	/*擦除原来位置坦克*/
	DrawTank(pTank, "  ");
	/*绘制新的坦克*/
	pTank->x += 2;
	DrawTank(pTank, TANK_LETTER);
	return 0;
}

/*炮弹向上移动*/
int ShellMoveUp(SHELLNODE *psn)
{
	int row, col;
	SHELLNODE *ptmp;
	row = CoordYConvertToArrayIndex(psn->shell.y);
	col = CoordXConvertToArrayIndex(psn->shell.x);

	/*如果之前已经显示过，首先应该擦除掉*/
	if (psn->shell.isshow)
		DrawShell(&psn->shell, " ");

	/*首先判断炮弹是否越界*/
	if (row <= 0)
		return AREA_OVERFLOW;
	/*判断前方是否有墙壁或者坦克*/
	if (g_area_data[row - 1][col] != AREA_SPACE)
	{
		psn->shell.isshow = 0;		//炮弹不再显示
		psn->shell.y--;				//坐标调整
		return g_area_data[row - 1][col];
	}
	/*判断有没有撞到其他炮弹*/
	/*遍历链表，查找有无对方炮弹存在*/
	if (ptmp = CheckLinkList(!psn->shell.owner, psn->shell.x, psn->shell.y - 1))
	{
		/*与上方对方炮弹相撞*/
		ptmp->shell.life = 0;		//让对方炮弹生命结束
		return AREA_SHELL;
	}
	/*其他情况，炮弹显示*/
	psn->shell.y--;					//坐标调整
	if (psn->shell.life)
		DrawShell(&psn->shell, SHELL_LETTER);
	psn->shell.isshow = 1;			//显示
	return 0;
}
/*炮弹向下移动*/
int ShellMoveDown(SHELLNODE *psn)
{
	int row, col;
	SHELLNODE *ptmp;
	row = CoordYConvertToArrayIndex(psn->shell.y);
	col = CoordXConvertToArrayIndex(psn->shell.x);

	/*如果之前已经显示过，首先应该擦除掉*/
	if (psn->shell.isshow)
		DrawShell(&psn->shell, " ");

	/*首先判断炮弹是否越界*/
	if (row >= GAME_AREA_HEIGHT-1)
		return AREA_OVERFLOW;
	/*判断前方是否有墙壁或者坦克*/
	if (g_area_data[row + 1][col] != AREA_SPACE)
	{
		psn->shell.isshow = 0;		//炮弹不再显示
		psn->shell.y++;				//坐标调整
		return g_area_data[row + 1][col];
	}
	/*判断有没有撞到其他炮弹*/
	/*遍历链表，查找有无对方炮弹存在*/
	if (ptmp = CheckLinkList(!psn->shell.owner, psn->shell.x, psn->shell.y + 1))
	{
		/*与上方对方炮弹相撞*/
		ptmp->shell.life = 0;		//让对方炮弹生命结束
		return AREA_SHELL;
	}
	/*其他情况，炮弹显示*/
	psn->shell.y++;					//坐标调整
	if (psn->shell.life)
		DrawShell(&psn->shell, SHELL_LETTER);
	psn->shell.isshow = 1;			//显示
	return 0;
}
/*炮弹向左移动*/
int ShellMoveLeft(SHELLNODE *psn)
{
	int row, col;
	SHELLNODE *ptmp;
	row = CoordYConvertToArrayIndex(psn->shell.y);
	col = CoordXConvertToArrayIndex(psn->shell.x);

	/*如果之前已经显示过，首先应该擦除掉*/
	if (psn->shell.isshow)
		DrawShell(&psn->shell, " ");

	/*首先判断炮弹是否越界*/
	if (col <= 0)
		return AREA_OVERFLOW;
	/*判断前方是否有墙壁或者坦克*/
	if (g_area_data[row][col - 1] != AREA_SPACE)
	{
		psn->shell.isshow = 0;			//炮弹不再显示
		psn->shell.x -= 2;				//坐标调整
		return g_area_data[row][col - 1];
	}
	/*判断有没有撞到其他炮弹*/
	/*遍历链表，查找有无对方炮弹存在*/
	if (ptmp = CheckLinkList(!psn->shell.owner, psn->shell.x - 2, psn->shell.y))
	{
		/*与上方对方炮弹相撞*/
		ptmp->shell.life = 0;		//让对方炮弹生命结束
		return AREA_SHELL;
	}
	/*其他情况，炮弹显示*/
	psn->shell.x -= 2;					//坐标调整
	if (psn->shell.life)
		DrawShell(&psn->shell, SHELL_LETTER);
	psn->shell.isshow = 1;			//显示
	return 0;
}
/*炮弹向右移动*/
int ShellMoveRight(SHELLNODE *psn)
{
	int row, col;
	SHELLNODE *ptmp;
	row = CoordYConvertToArrayIndex(psn->shell.y);
	col = CoordXConvertToArrayIndex(psn->shell.x);

	/*如果之前已经显示过，首先应该擦除掉*/
	if (psn->shell.isshow)
		DrawShell(&psn->shell, " ");

	/*首先判断炮弹是否越界*/
	if (col >= GAME_AREA_WIDTH - 1)
		return AREA_OVERFLOW;
	/*判断前方是否有墙壁或者坦克*/
	if (g_area_data[row][col + 1] != AREA_SPACE)
	{
		psn->shell.isshow = 0;			//炮弹不再显示
		psn->shell.x += 2;				//坐标调整
		return g_area_data[row][col + 1];
	}
	/*判断有没有撞到其他炮弹*/
	/*遍历链表，查找有无对方炮弹存在*/
	if (ptmp = CheckLinkList(!psn->shell.owner, psn->shell.x + 2, psn->shell.y))
	{
		/*与上方对方炮弹相撞*/
		ptmp->shell.life = 0;		//让对方炮弹生命结束
		return AREA_SHELL;
	}
	/*其他情况，炮弹显示*/
	psn->shell.x += 2;					//坐标调整
	if (psn->shell.life)
		DrawShell(&psn->shell, SHELL_LETTER);
	psn->shell.isshow = 1;			//显示
	return 0;
}

/*遍历链表，检查有无对方炮弹，如果有，返回其指针，若无，返回NULL*/
SHELLNODE *CheckLinkList(int owner, int x, int y)
{
	SHELLNODE *psn = NULL;
	int i;
	for (i = 0; i < CountOfLinklist(g_shell_list); i++)
	{
		psn = (SHELLNODE*)GetNode(g_shell_list, i);
		if (psn->shell.owner == owner&&psn->shell.x == x&&psn->shell.y == y)
			return psn;
	}
	return NULL;
}

/*炮弹的线程处理函数*/
DWORD WINAPI ShellDispose(LPVOID lpParam)
{
	int ret;			//返回值
	SHELLNODE *psn = (SHELLNODE*)lpParam;
	while(psn->shell.life)
	{
		/*申请临界区资源保护*/
		EnterCriticalSection(&g_cs);

		switch (psn->shell.dir)
		{
		case UP:
			if (ret = ShellMoveUp(psn))
				HitTarget(psn, ret);
			break;
		case DOWN:
			if (ret = ShellMoveDown(psn))
				HitTarget(psn, ret);
			break;
		case LEFT:
			if (ret = ShellMoveLeft(psn))
				HitTarget(psn, ret);
		case RIGHT:
			if (ret = ShellMoveRight(psn))
				HitTarget(psn, ret);
			break;
		}
		/*临界区资源释放*/
		LeaveCriticalSection(&g_cs);

		Sleep(psn->shell.speed);
	}
	/*如果死亡的是我方炮弹，则修改我方炮弹总数*/
	if (psn->shell.owner == OWNER_SELF)
		g_self_shell_count--;
	/*把没有生命的炮弹从链表里删除*/
	if (DelNode(g_shell_list, (NODE *)psn, ShellCompareByNum))
		free(psn);			//释放炮弹的内存空间
	return 0;
}

/*敌方坦克的线程处理函数*/
DWORD WINAPI EnemyMove(LPVOID lpParam)
{
	unsigned int nStep = 0;				//记步器
	int nDir = rand() % 5 + 5;			//在同一方向最多走多少步
	TANK *pTank = (TANK *)lpParam;
	while (pTank->life)
	{
		/*申请临界区资源保护，保证函数执行的完整性*/
		EnterCriticalSection(&g_cs);

		nStep++;
		if (nStep % nDir == 0)
			ChangeDir(pTank, rand() % 4);
		switch (pTank->dir)
		{
		case UP:
			if (TankMoveUp(pTank))
				ChangeDir(pTank, (pTank->dir + 1) % 4);
			break;
		case DOWN:
			if (TankMoveDown(pTank))
				ChangeDir(pTank, rand() % 4);
			TankMoveDown(pTank);
			break;
		case LEFT:
			if (TankMoveLeft(pTank))
				ChangeDir(pTank, rand() % 4);
			TankMoveLeft(pTank);
			break;
		case RIGHT:
			if (TankMoveRight(pTank))
				ChangeDir(pTank, rand() % 4);
			TankMoveRight(pTank);
			break;
		}
		/*释放临界区资源，让其他线程有机会获得临界区资源*/
		LeaveCriticalSection(&g_cs);					//不要放在睡眠函数之后执行释放

		Sleep(pTank->speed);
	}
	/*敌方坦克死亡*/
	g_died_enemy++;
	/*如果敌方坦克还没有全部出现，则创建一个新的敌方坦克*/
	if (g_died_enemy + AREA_ENEMY_COUNT < ENEMY_COUNT)			//阵亡坦克数+地图上敌方坦克数<总的敌方坦克数
	{
		HANDLE hThread;
		*pTank = CreateEnemyTank();
		hThread = CreateThread(NULL, 0, EnemyMove, pTank, 0, NULL);
		CloseHandle(hThread);
	}
	/*所有的敌方坦克已经被消灭*/
	else if (g_died_enemy == ENEMY_COUNT)
	{
		EnterCriticalSection(&g_cs);
		DrawWin();
		LeaveCriticalSection(&g_cs);
	}
	return 0;
}

/*炮弹击中目标的处理*/
void HitTarget(SHELLNODE *psn, int target)
{
	switch (target)
	{
	case AREA_OVERFLOW:			//越界
		psn->shell.life = 0;
		break;
	case AREA_WALL:				//墙壁
		HitWall(&psn->shell);
		psn->shell.life = 0;
		break;
	case AREA_SELF:				//我方坦克
		break;
	case AREA_ENEMY:			//敌方坦克
		/*将炮弹生命值置为0*/
		psn->shell.life = 0;
		DestroyEnemy(psn);
		break;
	case AREA_HOME:				//老家
		HitHome(&psn->shell);
		DrawGameOver();
		break;
	}
}

/*根据炮弹坐标销毁相应坦克*/
void DestroyEnemy(SHELLNODE *psn)
{
	int i;		//用于遍历敌方坦克数组
	TANK t;		//临时坦克结构变量
	for (i = 0; i < AREA_ENEMY_COUNT; i++)
	{
		t = g_enemy[i];
		if (psn->shell.x >= t.x && psn->shell.x <= t.x + 4
			&& psn->shell.y >= t.y && psn->shell.y <= t.y + 2)
		{
			/*已找到被击中的坦克*/
			/*1、擦除坦克*/
			DrawTank(&t, "  ");
			/*2、让其生命结束*/
			g_enemy[i].life = 0;		//不能直接让t.life置为0
			/*3、清空该坦克在全局数组中的数据*/
			ClearTankForGlobalArray(&t);
		}
	}
}

/*清空某个坦克在全局数组中的数据*/
void ClearTankForGlobalArray(TANK *pTank)
{
	int i, j;
	int row, col;
	row = CoordYConvertToArrayIndex(pTank->y);
	col = CoordXConvertToArrayIndex(pTank->x);
	for(i=row;i<row+3;i++)
		for (j = col; j < col + 3; j++)
		{
			g_area_data[i][j] = AREA_SPACE;
		}
}

/*炮弹节点的比较函数（根据编号来判断）*/
int ShellCompareByNum(NODE *pNode1, NODE *pNode2)
{
	SHELLNODE *p1 = (SHELLNODE *)pNode1;
	SHELLNODE *p2 = (SHELLNODE *)pNode2;
	return p1->shell.num == p2->shell.num ? 0 : 1;
}

/*将基于屏幕的x坐标转换为数组的列下标*/
int CoordXConvertToArrayIndex(int x)
{
	return (x - (LEFTTOP_X + 2)) / 2;
}

/*将基于屏幕的y坐标转换为数组的行下标*/
int CoordYConvertToArrayIndex(int y)
{
	return y - (LEFTTOP_Y + 1);
}
