#include"game.h"
#include"TANKDEF.h"
#include"interface.h"
#include<stdio.h>
#include<conio.h>
int main()
{
	SHELLNODE *psn;	//炮弹
	HANDLE hThread;	//线程句柄
	char ch;		//保持按下的是哪个键
	InitGame();		//初始化游戏
	
	int g_state = GAME_RUNNING;
	while (g_state != GAME_OVER)
	{
		if (_kbhit())	//当键盘有被按键被按下返回真，否则返回假
		{
			ch = _getch();	//获取按下的键
			switch (ch)
			{
			case'w':		//向上移动
				/*申请临界区资源保护*/
				EnterCriticalSection(&g_cs);
				if (g_my_tank.dir != UP)
					ChangeDir(&g_my_tank, UP);		//调整方向
				else
					TankMoveUp(&g_my_tank);			//向上移动
				/*临界区资源释放*/
				LeaveCriticalSection(&g_cs);
				break;
			case's':		//向下移动
				/*申请临界区资源保护*/
				EnterCriticalSection(&g_cs);
				if (g_my_tank.dir != DOWN)
					ChangeDir(&g_my_tank, DOWN);	//调整方向
				else
					TankMoveDown(&g_my_tank);		//向下移动
				/*临界区资源释放*/
				LeaveCriticalSection(&g_cs);
				break;
			case'a':		//向左移动
				/*申请临界区资源保护*/
				EnterCriticalSection(&g_cs);
				if (g_my_tank.dir != LEFT)
					ChangeDir(&g_my_tank, LEFT);	//调整方向
				else
					TankMoveLeft(&g_my_tank);		//向左移动
				/*临界区资源释放*/
				LeaveCriticalSection(&g_cs);
				break;
			case'd':		//向右移动
				/*申请临界区资源保护*/
				EnterCriticalSection(&g_cs);
				if (g_my_tank.dir != RIGHT)
					ChangeDir(&g_my_tank, RIGHT);	//调整方向
				else
					TankMoveRight(&g_my_tank);		//向右移动
				/*临界区资源释放*/
				LeaveCriticalSection(&g_cs);
				break;
			case'j':		//发射炮弹
				if (g_self_shell_count < SELF_SHELL_COUNT)
				{
					psn = Shot(&g_my_tank);				//创建一个我方炮弹
					g_self_shell_count++;				//我方炮弹计数增加1
					/*创建一个线程，让炮弹在此线程中运行*/
					hThread = CreateThread(NULL, 0, ShellDispose, psn, 0, NULL);
					CloseHandle(hThread);				//减少句柄的引用计数
				}
				break;
			}
			Sleep(100);
		}
	}
	getchar();
	getchar();
	return 0;
}

int state = GAME_RUNNING;		//定义游戏状态