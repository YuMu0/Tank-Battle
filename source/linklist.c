#include"linklist.h"
#include<stdlib.h>

/*初始化链表*/
void InitLinkList(LINKLIST **list)
{
	*list = (LINKLIST*)malloc(sizeof(LINKLIST));
	if (*list == NULL)
		return;
	(*list)->head.next = NULL;
	(*list)->size = 0;
}

/*销毁链表*/
void DestroyLinkList(LINKLIST **list)
{
	if (list && *list)
	{
		free(*list);
		*list = NULL;
	}
}

/*添加节点到链表尾部*/
void AddNode(LINKLIST *list, NODE *pNode)
{
	NODE *p = &list->head;
	/*先找到最后一个节点*/
	while (p->next)
		p = p->next;
	p->next = pNode;
	pNode->next = NULL;	
	list->size++;		//链表节点数加一
}

/*删除一个指定位置处的节点，成功则返回被删除节点的指针，失败返回空指针*/
NODE *DelNode(LINKLIST *list, NODE *pNode, int(*compare)(NODE*, NODE*))		//函数指针
{
	NODE *pPrev = &list->head;
	NODE *p = NULL;
	while (pPrev->next)
	{
		if (compare(pPrev->next, pNode) == 0)
		{
			/*找到了想要删除的节点*/
			p = pPrev->next;
			pPrev->next = pPrev->next->next;
			list->size--;		//链表节点数减一

			/*节点还没有空间，不需要free*/

		}
		pPrev = pPrev->next;	//让指针指向下一个节点，继续循环
	}
	return p;
}

/*返回链表中节点数量*/
int CountOfLinklist(LINKLIST *list)
{
	return list->size;
}

/*返回指定位置处的节点，成功则返回该节点指针，失败则返回空指针*/
NODE *GetNode(LINKLIST *list, int pos)
{
	NODE *p = list->head.next;
	int i;
	if (pos < 0)
		return NULL;
	for (i = 0; i < pos && p != NULL; i++)
		p = p->next;
	return p;
}
