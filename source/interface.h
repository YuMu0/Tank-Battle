#ifndef _INTERFACE_H_
#define _INTERFACE_H_

/*绘制游戏边框*/
void DrawGameBoard();
/*绘制地图*/
void DrawMap();
/*绘制老家*/
void DrawHome();
/*绘制坦克（1，画出来，2，擦除）*/
void DrawTank(TANK *ptank, const char *letter);
/*坦克转向*/
void ChangeDir(TANK *pTank, DIR dir);
/*绘制炮弹*/
void DrawShell(SHELL *Sshell, const char *letter);
/*击中墙壁*/
void HitWall(SHELL *pShell);
/*击中老家*/
void HitHome(SHELL *pShell);

/*打印游戏失败结束信息*/
void DrawGameOver();
/*打印游戏胜利信息*/
void DrawWin();

#endif