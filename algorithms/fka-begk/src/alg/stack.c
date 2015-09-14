class Vector;

Stack::Stack()
{
  top=NULL;
  num=0;
}

void Stack::push(int i,integer x,int seq,int rule,int ab,int ae,int bb,int be,int ad,int bd,int next=-1)
{
  num++;
  SNptr p=new stackNode;
  p->i=i;
  p->x=x;
  p->seq=seq;
  p->rule=rule;
  p->ab=ab;
  p->ae=ae;
  p->bb=bb;
  p->be=be;
  p->ad=ad;
  p->bd=bd;
  p->L=0;
  p->U=1;
  p->next=next;
  p->below=top;
  top=p;
}

void Stack::push(int i,integer x,int seq)
{
  num++;
  SNptr p=new stackNode;
  p->i=i;
  p->x=x;
  p->seq=seq;
  p->rule=0;
  p->ab=0;
  p->ae=-1;
  p->bb=0;
  p->be=-1;
  p->ad=0;
  p->bd=0;
  p->L=0;
  p->U=1;
  p->next=-1;
  p->below=top;
  top=p;
}

Boolean Stack::isEmpty()
// return TRUE iff stack is empty
{
  if(top==NULL)
    return(TRUE);
  return(FALSE);
}

void Stack::getTop(int *i,integer *x,int *seq,int *rule,int *ab,int *ae,int *bb,int *be,int *ad,int *bd,int *next)
{
  if(top==NULL)
    return;
  *i=top->i;
  *x=top->x;
  *seq=top->seq;
  *rule=top->rule;
  *ab=top->ab;
  *ae=top->ae;
  *bb=top->bb;
  *be=top->be;
  *ad=top->ad;
  *bd=top->bd;
  *next=top->next;
}

void Stack::getNext(SNptr ptr,int *ab,int *ae,int *index,int *L,int *U)
{
  if(ptr==NULL)
    return;
  *ab=ptr->ab;
  *ae=ptr->ae;
  *index=ptr->i;
  *L=ptr->L;
  *U=ptr->U;
}

void Stack::pop(int *i,integer *x,int *seq,int *rule,int *ab,int *ae,int *bb,int *be,int *ad,int *bd,int *next)
{
  if(top==NULL)
    return;
  *i=top->i;
  *x=top->x;
  *seq=top->seq;
  *rule=top->rule;
  *ab=top->ab;
  *ae=top->ae;
  *bb=top->bb;
  *be=top->be;
  *ad=top->ad;
  *bd=top->bd;
  *next=top->next;
  SNptr q=top;
  top=top->below;
  delete q;
  num--;
}

void Stack::pop()
{
  if(top==NULL)
    return;
  SNptr q=top;
  top=top->below;
  delete q;
  num--;
}

void Stack::empty()
{
  int i,seq,rule,ab,ae,bb,be,ad,bd;
  int next;
  integer x;
  while(!isEmpty()){
    pop(&i,&x,&seq,&rule,&ab,&ae,&bb,&be,&ad,&bd,&next);
  }
  num=0;
}

Stack::~Stack()
{
  empty();
}

void Stack::print()
{
  printf("--------------------------- Stack -------------------------\n");
  for(SNptr p=top;p!=NULL;p=p->below){
	printf("rule=%i,seq=%i,i=%i,x=%i\n",p->rule,p->seq,p->i,p->x);
    /*if(p->next!=NULL){
	  printf("next="); p->next->v->print(); printf("\n");
	} else
	    printf("next=NULL\n");*/
  }
}

void Stack::print(FILE *fout)
{
  fprintf(fout,"--------------------------- Stack -------------------------\n");
  for(SNptr p=top;p!=NULL;p=p->below){
	fprintf(fout,"rule=%i,seq=%i,i=%i,x=%i\n",p->rule,p->seq,p->i,p->x);
    /*if(p->next!=NULL){
	  fprintf(fout,"next="); p->next->v->print(fout); fprintf(fout,"\n");
	} else
	    fprintf(fout,"next=NULL\n");*/
  }
  fprintf(fout,"-----------------------------------------------------------\n");
}

void Stack::printTop()
{
  printf("--------------------------- Stack Top -------------------------\n");
  printf("rule=%i,seq=%i,i=%i,x=%i,ab=%i,ae=%i,bb=%i,be=%i ad=%i bd=%i\n",top->rule,top->seq,top->i,top->x,top->ab,top->ae,top->bb,top->be,top->ad,top->bd);
  printf("-----------------------------------------------------------\n");
}

void Stack::printTop(FILE *fout)
{
  fprintf(fout,"--------------------------- Stack Top -------------------------\n");
  fprintf(fout,"rule=%i,seq=%i,i=%i,x=%i,ab=%i,ae=%i,bb=%i,be=%i ad=%i bd=%i\n",top->rule,top->seq,top->i,top->x,top->ab,top->ae,top->bb,top->be,top->ad,top->bd);
  fprintf(fout,"-----------------------------------------------------------\n");
}

void Stack::print2(int n)
{
  Vector v(n);
  int i;
  for(i=0;i<n;i++)
	v.set(i,-1);
  for(SNptr p=top;p!=NULL;p=p->below){
	if(p->seq==1)
	  v.set(p->i,p->x-1);
	else
	  v.set(p->i,p->x);
  }
  v.print();printf("\n");
}

void Stack::print2(int n,FILE *fout)
{
  Vector v(n);
  int i;
  for(i=0;i<n;i++)
	v.set(i,-1);
  for(SNptr p=top;p!=NULL;p=p->below){
	if(p->seq==1)
	  v.set(p->i,p->x-1);
	else
	  v.set(p->i,p->x);
  }
  v.print(fout);fprintf(fout,"\n");
}
