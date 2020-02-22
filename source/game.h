#include"TANKDEF.h"
#ifndef _GAME_H_
#define _GAME_H_

/*游戏初始化*/
void InitGame();
/*游戏结束的释放*/
void GameOver();
/*初始化我方坦克*/
void InitMyTank();
/*将地图导入游戏区域二维数组*/
void ImportMapToGameArea();
/*将老家导入游戏区域二维数组*/
void ImportHomeToGameArea();

/*将基于屏幕的x坐标转换为数组的列下标*/
int CoordXConvertToArrayIndex(int x);
/*将基于屏幕的y坐标转换为数组的行下标*/
int CoordYConvertToArrayIndex(int y);

/*将我方坦克到游戏区域数组*/
void ImportMyTankToGameArray();
/*将敌方坦克到游戏区域数组*/
void ImportEnemyTankToGameArray(TANK *pTank);

/*创造一辆敌方坦克*/
TANK CreateEnemyTank();

/*坦克移动,如果移动成功返回0，否则返回撞到的物体*/
int TankMoveUp(TANK *pTank);
int TankMoveDown(TANK *pTank);
int TankMoveLeft(TANK *pTank);
int TankMoveRight(TANK *pTank);

/*发射炮弹，参数为发射炮弹的坦克*/
SHELLNODE *Shot(pTank);

/*炮弹向上移动*/
int ShellMoveUp(SHELLNODE *psn);
/*炮弹向下移动*/
int ShellMoveDown(SHELLNODE *psn);
/*炮弹向左移动*/
int ShellMoveLeft(SHELLNODE *psn);
/*炮弹向右移动*/
int ShellMoveRight(SHELLNODE *psn);

/*遍历链表，检查有无对方炮弹，如果有，返回其指针，若无，返回NULL*/
SHELLNODE *CheckLinkList(int owner, int x, int y);

/*炮弹的线程处理函数*/
DWORD WINAPI ShellDispose(LPVOID lpParam);

/*敌方坦克的线程处理函数*/
DWORD WINAPI EnemyMove(LPVOID lpParam);

/*炮弹击中目标的处理*/
void HitTarget(SHELLNODE *psn, int target);

/*清空某个坦克在全局数组中的数据*/
void ClearTankForGlobalArray(TANK *pTank);

/*根据炮弹坐标销毁相应坦克*/
void DestroyEnemy(SHELLNODE *psn);

/*炮弹节点的比较函数（根据编号来判断）*/
int ShellCompareByNum(NODE *pNode1, NODE *pNode2);
#endif
