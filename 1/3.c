#include <stdio.h>
typedef int DataType;
typedef struct node {
	DataType data;
	struct node *link;
}LinkNode,*LinkList;

LinkList Find(LinkList &L,int k)
{
	LinkList p;
	int i;
	i =1;
	p=L->link;
	while(p!=NULL && i < k)
	{
		p=p->link;
		i++;
	}
	if(p!=NULL && i==k)
		return p;
	
	return NULL;
}

bool Insert(LinkList &L,int k,int elem)
{
	LinkList p,q;
	if(k == 1)
	p=L;
	else p = Find(L,k-1);
	if(p==NULL)
	return false;
	q= new LinkNode;
	if(q==NULL)
	return false;
	q->data = elem;
	q->link = p->link;
	p->link=q;
	return true;
}

bool Delete(LinkList &L,int k)
{
	LinkList p,q;
	if(k == 1)
	p=L;
	else 
	p=Find(L,k-1);
	if(p == NULL)
	return false;
	q=p->link;
	p->link=q->link;
	delete q;
	return true;
}
