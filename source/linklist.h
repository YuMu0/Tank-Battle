#ifndef _LINKLIST_H_
#define _LINKLIST_H_

/*定义节点结构*/
typedef struct node
{
	struct node *next;		//指向下一个节点
}NODE;

/*定义链表结构*/
typedef struct linklist
{
	NODE head;				//头节点
	int size;				//节点数量
}LINKLIST;

/*定义链表操作函数*/
/*初始化链表*/
void InitLinkList(LINKLIST **list);
/*销毁链表*/
void DestroyLinkList(LINKLIST **list);
/*添加节点到链表尾部*/
void AddNode(LINKLIST *list, NODE *pNode);
/*删除一个指定位置处的节点，成功则返回被删除节点的指针，失败返回空指针*/
NODE *DelNode(LINKLIST *list, NODE *pNode, int(*compare)(NODE*, NODE*));		//函数指针
/*返回链表中节点数量*/
int CountOfLinklist(LINKLIST *list);
/*返回指定位置处的节点*/
NODE *GetNode(LINKLIST *list, int pos);


#endif