#include <stdio.h>
#include <stdlib.h>
#include "my402list.h"
#include "cs402.h"

int  My402ListLength(My402List* list)
{
	return list->num_members;
}

int My402ListEmpty(My402List* emp)
{
	if (emp->num_members == 0)
		return TRUE;
	else
		return FALSE;
}

int My402ListAppend(My402List* q, void* x)
{
	My402ListElem* temp = (My402ListElem*)malloc(sizeof(My402ListElem));
	My402ListElem* p = (My402ListElem*)malloc(sizeof(My402ListElem));
	p=&(q->anchor); //addr of anchor
	
	My402ListElem* a = (My402ListElem*)malloc(sizeof(My402ListElem));
	
	a=My402ListLast(q);
	
	//printf("im in append\n");
	if (temp==NULL)
	{
	return FALSE;
	}

	if(a==NULL)
	{

		a=p;
	}

	if (My402ListEmpty(q)==1)

	{
		temp->obj= x;
		temp->prev=p;
		temp->next=p;
		p->prev=temp;
		p->next=temp;
		
		
	}
		
	else
	{
		temp->obj=x;
		temp->next=p;		
		temp->prev=a;		
		a->next=temp;
		p->prev=temp;		
		
		
	}

	//printf("im at append end\n");
	q->num_members++;	
	return TRUE;
		
	
	
}

int  My402ListPrepend(My402List* q, void* x)
{
	My402ListElem* temp1 = (My402ListElem*)malloc(sizeof(My402ListElem));
	My402ListElem* p = (My402ListElem*)malloc(sizeof(My402ListElem));
	p=&(q->anchor);
	My402ListElem* a = (My402ListElem*)malloc(sizeof(My402ListElem));
	a=My402ListFirst(q);
	
	if (temp1==NULL)
	{
	return FALSE;
	}

	if (a==NULL)
	{

		a=p;
	}

	if (My402ListEmpty(q)==1)
	{
		temp1->obj= x;
		temp1->prev=p;
		temp1->next=p;
		//temp1->next=p->next;
		q->anchor.next=temp1;
		q->anchor.prev=temp1;
		//(temp1->next)->prev=temp1;
	}
	else
	{
		temp1->obj=x;		
		temp1->prev=p;
		q->anchor.next=temp1;		
		//(temp1->prev)->next=temp1;
		temp1->next=a;
		a->prev=temp1;		
		//(temp1->next)->prev=temp1;
	}
	q->num_members++;	
	return TRUE;		
	
	
}

void My402ListUnlink(My402List* q, My402ListElem* n)
{
	
	//My402ListElem* temp_p = (My402ListElem*)malloc(sizeof(My402ListElem*));
	//My402ListElem* temp_n = (My402ListElem*)malloc(sizeof(My402ListElem*));

	My402ListElem* temp_p = NULL;
	My402ListElem* temp_n = NULL;

	//temp_p = (My402ListElem*)malloc(sizeof(My402ListElem*));
	//temp_n = (My402ListElem*)malloc(sizeof(My402ListElem*));

	
	temp_p = n->prev;
	temp_n = n->next;
	temp_p -> next = temp_n;//n->next;
	temp_n -> prev = temp_p;//n->prev;
	
	free(n);
	q->num_members--;

	return;

}


void My402ListUnlinkAll(My402List* q)
{
	My402ListElem* temp1 = (My402ListElem*)malloc(sizeof(My402ListElem));
	temp1=&(q->anchor);
	My402ListElem* temp2 = (My402ListElem*)malloc(sizeof(My402ListElem));
	
	while (temp1!=&(q->anchor))
		{
			temp2=temp1->next;
			free(temp1);
			temp1=temp2;
		}
	q->num_members=0;

	return;
}

int  My402ListInsertAfter(My402List* q, void* x, My402ListElem* n)
{
	
	
	if (n==NULL)
	{
		return My402ListAppend(q,x);
		
	}
	My402ListElem* t = NULL;
	My402ListElem* temp1 = NULL;
	temp1 = (My402ListElem*)malloc(sizeof(My402ListElem));
	t = (My402ListElem*)malloc(sizeof(My402ListElem));
	
	if (temp1==NULL)
	{
		return FALSE;
	}
	
	//else
	//{

		temp1->obj= x;

		t = n->next;

		n->next=temp1;
		temp1->next=t;
		t->prev=temp1;
		temp1->prev=n;
		
		
		
		//(temp1->next)->prev=temp1;
	//}
	
	q->num_members++;
	return TRUE;
	
}

int  My402ListInsertBefore(My402List* q, void* x, My402ListElem* n)
{
	
	
	if (n==NULL)
	{
		return My402ListPrepend(q,x);
		
	}

	My402ListElem* t= NULL;

	My402ListElem* temp1 = NULL;
	temp1 = (My402ListElem*)malloc(sizeof(My402ListElem));
	t = (My402ListElem*)malloc(sizeof(My402ListElem));
	

	if (temp1==NULL)
	{
	return FALSE;
	}

	
	
	//else 
	//{
		temp1->obj= x;

		t = n->prev;

		t->next=temp1;
		temp1->next=n;
		n->prev=temp1;
		temp1->prev=t;
		
		
				
		//(temp1->prev)->next=temp1;
	//}
	
	q->num_members++;			
	return TRUE;
}

My402ListElem *My402ListFirst(My402List* q)
{
	if(q->num_members == 0)
		return NULL;
	else{
		return q->anchor.next;}

}

My402ListElem *My402ListLast(My402List* q)
{
	if(q->num_members == 0)
		return NULL;
	else
        { 
		return q->anchor.prev;
	}
}

My402ListElem *My402ListNext(My402List* q, My402ListElem* n)
{
	if (n==My402ListLast(q))
	return NULL;
	else
	return n->next;
}

My402ListElem *My402ListPrev(My402List* q, My402ListElem* n)
{
	if (n==My402ListFirst(q))
	return NULL;
	else
	return n->prev;
}

My402ListElem *My402ListFind(My402List* q, void*x)
{
	My402ListElem* temp1 = (My402ListElem*)malloc(sizeof(My402ListElem));
	temp1=&(q->anchor);
	while (temp1->obj!=x)
		{
		temp1=temp1->next;
		}
	return temp1;
	
}

int My402ListInit(My402List* q)
{
	(q->anchor).next=NULL;
	(q->anchor).prev=NULL;
	(q->anchor).obj=NULL;
	q->num_members=0;
	
	return 1;
	
}


