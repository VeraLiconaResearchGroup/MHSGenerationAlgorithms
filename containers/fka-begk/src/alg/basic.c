void computeProducts(int num,int* size, long* product)
// auxiliary fucntion to help in the base case for dualization
{
  product[0]=1;
  for(int i=1;i<num;i++)
    product[i]=product[i-1]*size[i-1];
  product[num]=product[num-1]*size[num-1];
}

double map(Vector &v,double *product)
{
  /* given a vector, map it into a single number */
  double sum;
  int i;
  sum=v.get(0);
  for(i=1;i<v.getDim();i++){
    sum+=v.get(i)*product[i];
  }
  return sum;
}

void pam(long int map,Vector *t,int k,long *product)
{
  /*  inverse of map */
  for(int i=k-1;i>=0;i--){
    t->set(i,(int)(map/product[i]));
      map-=t->get(i)*product[i];
  }
}

void init()
{
  numNodes=0;
  numLeaves=0;
}

void initExp()
{
  clockMax=exp(log(2)*(8*sizeof(clock_t)-1));
  interTimeTot=0;
  interTimeTot2=0;
  interTimeAvg=0;
  interTimeMax=-INFNTY_DOUBLE;
  interTimeMax22=-INFNTY_DOUBLE;
  interTimeMax2=0;
  numEssMax22=-INFNTY_INT;
  numEssMax2=0;
  nodesVisitedMax22=-INFNTY_INT;
  nodesVisitedMax2=0;
  nodesVisited=0;
  nodeDepthMax22=-INFNTY_INT;
  nodeDepthMax2=0;
  nodeDepth=0;
}

void setTimer()
{
  clk=clock();  
}

void updateTimes(long currentNum)
{
  double interTime;
  if((clock()<0)&&(clockPos)){
    clockPos=FALSE;
    interTime=(double)(2*clockMax+clock()-clk)/CLOCKS_PER_SEC;
  }
  else{
    interTime=(double)(clock()-clk)/CLOCKS_PER_SEC;
    if(clock()>=0)
      clockPos=TRUE;
  }
  clk=clock();
  interTimeTot+=interTime;
  interTimeTot2+=interTime;
  interTimeAvg=interTimeTot/currentNum;
  if(interTime>interTimeMax)
    interTimeMax=interTime;
  if(interTime>interTimeMax22)
    interTimeMax22=interTime;
  if(currentNum%outputPeriod==0){
    interTimeAvg2=interTimeTot2/outputPeriod;
    interTimeMax2=interTimeMax22;
    interTimeTot2=0;
    interTimeMax22=-INFNTY_DOUBLE;
  }  
}

void logData(long num,int numEss)
{
  updateTimes(num);
  if(numEss>numEssMax22)
    numEssMax22=numEss;
  if(nodesVisited>nodesVisitedMax22)
    nodesVisitedMax22=nodesVisited;
  nodesVisited=0;
  if(nodeDepth>nodeDepthMax22)
    nodeDepthMax22=nodeDepth;
  if(num%outputPeriod==0){
    numEssMax2=numEssMax22;
    numEssMax22=-INFNTY_INT;
    nodesVisitedMax2=nodesVisitedMax22;
	nodesVisitedMax22=-INFNTY_INT;
	nodeDepthMax2=nodeDepthMax22;
	nodeDepthMax22=-INFNTY_INT;
  }
}

int min(int x,int y)
{
  if (x<=y) 
	return(x);
  else
	return(y);
}

integer min(integer x,integer y)
{
  if (x<=y) 
	return(x);
  else
	return(y);
}

integer max(integer x,integer y)
{
  if (x>=y) 
	return(x);
  else
	return(y);
}

int max(int x,int y)
{
  if (x>=y) 
	return(x);
  else
	return(y);
}

double min(double x,double y)
{
  if (x<=y) 
	return(x);
  else
	return(y);
}

double dabs(double x)
{
  if(x<0)
    return(-x);
  else 
    return(x);  
}

integer round(float x)
{
  integer y;
  y=(integer)x;
  if(x-y<0.5) return(y);
  else return(y+1);
}

int uniform(int n1,int n2)
{
  //return a random number distributed uniformly in [n1:n2]
  float y;
  y=((float)(rand())/RAND_MAX*(n2-n1+1));
  return(min(n1+(int)y,n2));
}

integer uniform(integer n1,integer n2)
{
  //return a random number distributed uniformly in [n1:n2]
  float y;
  y=((float)(rand())/RAND_MAX*(n2-n1+1));
  return(min((integer)(n1+(integer)y),(integer)n2));
}

void genRandom0(Box &C,System *A,int num)
// generates an antichain A of size num at random inside the box C
{
  int i,j;
  int n=C.getDim();
  A->init(n,num);
  for(j=0;j<num;j++){
	Vector *x=new Vector(n);
    for (i=0;i<n;i++)
	  x->set(i,uniform(C.getL(i),C.getU(i)));
	A->addVector(*x);
  }
  A->minimal();
} 

void exchange (int *x, int *y)
{
  int t=*x;
  *x=*y;
  *y=t;
}

void genRandom1(Box &C,System *A,int num,int numEss)
// generates an antichain A of size num at random inside the box C
// numEss coordinates is set random from 0 to numEss
{
  int i,j,k,index;
  int n=C.getDim();
  numEss=min(numEss,n-1);
  A->init(n,num);
  int *list=new int[n];
  for(j=0;j<num;j++){
	Vector x(n);
	for(i=0;i<n;i++){
	    x.set(i,C.getL(i));
		list[i]=i;
	}
    //k=uniform(2,numEss);
	k=numEss;
	for(i=0;i<k;i++){
	  index=uniform(i,n-1);
	  x.set(list[index],uniform((integer)(C.getL(list[index])+1),C.getU(list[index])));
	  exchange(&list[i],&list[index]);
	}
	A->addVector(x);
  }
  A->minimal();
  delete [] list;
} 

void genRandom2(Box &C,System *A,int num=INFNTY_INT)
// generates an antichain A of size at most num at random inside the box C
// Ess coordinates are determined by the edges of the threshold graph on n vertites
{
  int i,j;
  int count=0;
  int n=C.getDim();
  if(num==INFNTY_INT)
	num=n*n/4; //assuming n is even
  A->init(n,num);
  for(i=0;i<n;i++)
	if(i%2==1)
	  for(j=0;j<i;j++)
		if(count++<num){
		  Vector x(n);
          x=*C.getL();
		  x.set(i,uniform((integer)(C.getL(i)+1),C.getU(i)));
          x.set(j,uniform((integer)(C.getL(j)+1),C.getU(j)));
		  A->addVector(x);
		}
}

void genMatching(System *A,int k,int d=2)
// generates a matching of k edges each has d vertices
{
  int n=d*k;
  int i,j;
  A->init(n,k);
  for(i=0;i<k;i++){
    Vector x(d*k);
    for(j=0;j<d*k;j++)
      x.set(j,0);
	for(j=0;j<d;j++)
      x.set(d*i+j,1);
    A->addVector(x);    
  }
}

void dary(long j,int *x, int k,int d=2)
{
  for(int i=0;i<k;i++){
	x[i]=j%d;
	j/=d;
  }
}

int myRound(float x)
{
  if(x-(int)x<0.5)
	return (int)x;
  else
	return (int)x+1;
}

void genMatchingDual(System *A,int k,int d=2)
// generates the dual to a k-matching in d dimensions
{
  int n=d*k;
  int i;
  int num=myRound((float)exp(k*log(d)));
  A->init(n,num);
  int *x=new int[n];
  for(long int j=0;j<num;j++){
	Vector v(n); 
	for(i=0;i<n;i++)
      v.set(i,0);
	dary(j,x,k,d);
	for(i=0;i<k;i++){
	  for(int jj=0;jj<d;jj++)
	  v.set(d*i+x[i],1);
	}
	A->addVector(v);
  }
}

void genNearMatching(System *A,int k,int num)
// generates a k-matching and num random hyperedges of its dual
{
  genMatching(A,k);
  int x;
  int n=2*k;
  for(int j=0;j<num;j++){
    Vector v(n); 
	for(int i=0;i<k;i++){
      if((float)(rand())/RAND_MAX<0.5)
        x=0;
	  else 
        x=1;
	  v.set(2*i,x);
      v.set(2*i+1,(x+1)%2);
	}
	A->addVector(v);
  }
  A->minimal();
}

void genNearMatchingDual(System *A,int k,int num)
// generates the dual to a k-matching in d=2 dimensions minus num randomly 
//  selected hyperedges
{
  genMatchingDual(A,k);
  int x;
  int n=2*k;
  int m=A->getNum();
  for(int j=0;j<num;j++){
    x=uniform(0,m);	
	A->deleteVector(x);
  }
  A->cleanUp();
}

void genFano(System *A,int k)
// generates k disjoint copies of the Fano plane
{
  int a[7][7]={{1,1,1,0,0,0,0},{1,0,0,1,0,0,1},{1,0,0,0,1,1,0},
	{0,1,0,0,0,1,1},{0,1,0,1,1,0,0},{0,0,1,0,1,0,1},{0,0,1,1,0,1,0}};
  int n=7*k;
  A->init(n,n);
  int i,j,l;
  Vector v(n);
  for(j=0;j<k;j++)
   	for(i=0;i<7;i++){
      for(l=0;l<n;l++)
	    v.set(l,0);
      for(l=0;l<7;l++)
	    v.set(7*j+l,a[i][l]);
//	  v.print();printf("\n");
	  A->addVector(v);
	 }
}

void genFanoDual(System *A,int k)
// generates the dual to k disjoint copies of the Fano plane
{
  int a[7][7]={{1,1,1,0,0,0,0},{1,0,0,1,0,0,1},{1,0,0,0,1,1,0},
	{0,1,0,0,0,1,1},{0,1,0,1,1,0,0},{0,0,1,0,1,0,1},{0,0,1,1,0,1,0}};
  int i,l;
  long j;
  int n=7*k;
  Vector v(n);
  long int* product=new long int[k+1];
  int *size=new int[k];
  Vector t(k); 
  for(i=0;i<k;i++)
	size[i]=7;
  computeProducts(k,size,product);
  A->init(n,product[k]);
  for(j=0;j<product[k];j++){
    pam(j,&t,k,product);
	for(i=0;i<k;i++)
	  for(l=0;l<7;l++)
	    v.set(7*i+l,a[t.get(i)][l]);
	A->addVector(v);
  }
  delete [] product;
  delete [] size;
}

void genSelfDual(System *A,System *B,System *D)
// given two hypergraphs A and B=Tr(A) return  a self dual hypergraph D 
{
   int i;
   long j;
   int n=B->getDim()+2;
   long num=A->getNum()+B->getNum()+1;
   D->init(n,num);
   Vector v(n); 
   v.set(0,1);
   v.set(1,1);
   for(i=2;i<n;i++)
     v.set(i,0);
   D->addVector(v);
   v.set(0,1);
   v.set(1,0);
   for(j=0;j<A->getNum();j++){
	 for(i=2;i<n;i++)
       v.set(i,A->get(j)->get(i-2));
     D->addVector(v);
   }
   v.set(0,0);
   v.set(1,1);
   for(j=0;j<B->getNum();j++){
     for(i=2;i<n;i++)
       v.set(i,B->get(j)->get(i-2));
     D->addVector(v);
   }
}

void report1(long num)
{
  if(!toLog) 
	  return;
  num=num-orig;
  if(num%outputPeriod==0){
    printf("  %6i  %9.2f   %7.2f   %7.2f   %7.2f    %7.2f  %4i  %9i  %4i\n",num,interTimeTot,interTimeAvg,interTimeMax,interTimeAvg2,interTimeMax2,numEssMax2,num+numColl,nodeDepthMax2);    
    fout=fopen(logFile,"a");
    fprintf(fout,"  %6i  %9.2f   %7.2f   %7.2f   %7.2f    %7.2f  %4i  %9i  %4i\n",num,interTimeTot,interTimeAvg,interTimeMax,interTimeAvg2,interTimeMax2,numEssMax2,num+numColl,nodeDepthMax2); 
    fclose(fout);
  }
}

void report1_1(long num)
{
  if(!toLog) 
	  return;
  num=num-orig;
  if(num%outputPeriod==0){
    printf("  %6i  %9.5f\n",num/1000,interTimeAvg*1000);    
    fout=fopen(logFile,"a");
    fprintf(fout,"  %6i  %9.5f\n",num/1000,interTimeAvg*1000); 
    fclose(fout);
  }
}

void report2(long num)
{
  if(!toLog) 
	  return;
  num=num-orig;
  if(num%outputPeriod==0){
    printf("* %6i  %9.2f   %7.2f   %7.2f   %7.2f    %7.2f  %4i  %9i  %4i\n",num,interTimeTot,interTimeAvg,interTimeMax,interTimeAvg2,interTimeMax2,numEssMax2,nodesVisitedMax2,nodeDepthMax2);    
    fout=fopen(logFile,"a");
    fprintf(fout,"* %6i  %9.2f   %7.2f   %7.2f   %7.2f    %7.2f  %4i  %9i  %4i\n",num,interTimeTot,interTimeAvg,interTimeMax,interTimeAvg2,interTimeMax2,numEssMax2,nodesVisitedMax2,nodeDepthMax2); 
    fclose(fout);
  }
}

void report2_1(long num)
{
  if(!toLog) 
	  return;
  num=num-orig;
  if(num%outputPeriod==0){
    printf("* %6i %6i  %6i %9.5f   \n",m,num/1000,n,interTimeAvg*1000);    
    fout=fopen(logFile,"a");
    fprintf(fout,"* %6i %6i  %6i  %9.5f   \n",m,num/1000,n,interTimeAvg*1000); 
    fclose(fout);
  }
}

void report1_2(long num)
{
  if(num%1000==0)
    printf("  %6i  %6i %6i %9.5f \n",num,n,m,interTimeTot);
  if(!toLog) 
	  return;
  num=num-orig;
  if(num%outputPeriod==0){
    printf("  %6i  %6i  %6i  %9.5f \n",num,n,m,interTimeTot);    
    fout=fopen(logFile,"a");
    fprintf(fout,"  %6i  %6i  %6i  %9.5f\n",num,n,m,interTimeTot); 
    fclose(fout);
  }
}
void report3(long num)
{
	if(!toLog) 
	  return;
	num=num-orig;
	printf("* %6i  %9.2f   %7s   %7s   %7s    %7s  %4s  %9i  %4i\n",num,interTimeTot," "," "," "," "," ",nodesVisitedMax2,nodeDepthMax2);    
    fout=fopen(logFile,"a");
    fprintf(fout,"* %6i  %9.2f   %7s   %7s    %7s   %7s  %4s  %9i  %4i\n",num,interTimeTot," "," "," "," "," ",nodesVisitedMax2,nodeDepthMax2);    
    fclose(fout);
}

void report4(long num)
{
	if(!toLog) 
	  return;
	num=num-orig;
	printf("  %6i  %9.2f   %7s   %7s   %7s    %7s  %4s  %9i  %4i\n",num,interTimeTot," "," "," "," "," ",num+numColl,nodeDepthMax2);    
    fout=fopen(logFile,"a");
    fprintf(fout,"  %6i  %9.2f   %7s   %7s    %7s   %7s  %4s  %9i  %4i\n",num,interTimeTot," "," "," "," "," ",num+numColl,nodeDepthMax2);    
    fclose(fout);
}

void report5(int num)
{
  printf("Dualization in progress.....	  Num of nodes visited=%i, Depth=%i\n",nodesVisited,num);
}

void fprint(char *fileName,char *s)
{
  printf("%s",s);
  fout=fopen(fileName,"a");
  fprintf(fout,"%s",s);
  fclose(fout);
}

double f(double x,double v)
{
  return(exp(x*log(x))-v);
}

double chi(double v)
// use Bisection to compute the root of x^x=v
{
  double x0,x1,x;
  x0=log(v)/log(log(v)/log(2));
  x1=log(v)/log(2);
  while(f(x1,v)<0) x1=2*x1;
  while(dabs(x1-x0)>EPSILON){
    x=(x0+x1)/2;
    if (f(x,v)<0)
      x0=x;
    else
      x1=x;
  }
  return(x0);
}
/*
int __cdecl lexCompare(const void *xx, const void *yy)
{
  Vector *y=(Vector *)xx;
  Vector *x=(Vector *)yy;
  int i;
  for(i=0;(i<x->getDim())&&(x->get(i)==y->get(i));i++);
  if(i<x->getDim()){
    if (x->get(i) > y->get(i))
      return (1);
    else
      return (-1);
  }
  return (0);
}

int __cdecl revLexCompare(const void *xx, const void *yy)
{
  Vector *y=(Vector *)xx;
  Vector *x=(Vector *)yy;
  int i;
  for(i=0;(i<x->getDim())&&(x->get(i)==y->get(i));i++);
  if(i<x->getDim()){
    if (x->get(i) > y->get(i))
      return (-1);
    else
      return (1);
  }
  return (0);
}
/*/
int lexCompare(const void *xx, const void *yy)
{
  Vector *y=(Vector *)xx;
  Vector *x=(Vector *)yy;
  int i;
  for(i=0;(i<x->getDim())&&(x->get(i)==y->get(i));i++);
  if(i<x->getDim()){
    if (x->get(i) > y->get(i))
      return (1);
    else
      return (-1);
  }
  return (0);
}

int revLexCompare(const void *xx, const void *yy)
{
  Vector *y=(Vector *)xx;
  Vector *x=(Vector *)yy;
  int i;
  for(i=0;(i<x->getDim())&&(x->get(i)==y->get(i));i++);
  if(i<x->getDim()){
    if (x->get(i) > y->get(i))
      return (-1);
    else
      return (1);
  }
  return (0);
}
/***************************************************
*****             Red-Black tree               *****
****************************************************/

RBTree::RBTree()
{
  root=NULL;
}

void RBTree::deleteTree(RBptr x)
{
  if(x->left!=NULL)
	deleteTree(x->left);
  if(x->right!=NULL)
	deleteTree(x->right);
  delete x;
}

RBTree::~RBTree()
{
  deleteTree(root);
}

void RBTree::left_rotate(RBptr x) 
{
  RBptr y;
  y = x->right;
  /* Turn y's left sub-tree into x's right sub-tree */
  x->right = y->left;
  if ( y->left != NULL )
    y->left->parent = x;
  /* y's new parent was x's parent */
  y->parent = x->parent;
  /* Set the parent to point to y instead of x */
  /* First see whether we're at the root */
  if ( x->parent == NULL ) 
	root = y;
  else
    if ( x == (x->parent)->left ) // x was on the left of its parent
      x->parent->left = y;
    else // x must have been on the right 
      x->parent->right = y;
    /* Finally, put x on y's left */
  y->left = x;
  x->parent = y;
}

void RBTree::right_rotate(RBptr x) 
{
  RBptr y;
  y = x->left;
  /* Turn y's right sub-tree into x's left sub-tree */
  x->left = y->right;
  if ( y->right != NULL )
    y->right->parent = x;
  /* y's new parent was x's parent */
  y->parent = x->parent;
  /* Set the parent to point to y instead of x */
  /* First see whether we're at the root */
  if ( x->parent == NULL ) 
	root = y;
  else
    if ( x == (x->parent)->left ) // x was on the left of its parent
      x->parent->left = y;
    else // x must have been on the right 
      x->parent->right = y;
    /* Finally, put x on y's right */
  y->right = x;
  x->parent = y;
}

void RBTree::insert(int i,System &B)
{
    /* Insert in the tree in the usual way */
    RBptr p,q;
	p=root;
	q=NULL;
	while(p!=NULL){
	  q=p;
	  if(B.get(i)->lexComp(*B.get(p->index))<=0)
		p=p->left;
	  else
		p=p->right;
	}
	RBptr x=new RBNode;
	x->index=i;
	x->left=x->right=NULL;
	if(p==root){
	  root=x;
	  root->parent=NULL;
	} else {
	  x->parent=q;
	  if(B.get(i)->lexComp(*B.get(q->index))<=0)
		q->left=x;
	  else
		q->right=x;
	}
    /* Now restore the red-black property */
    x->colour = red;
	RBptr y;
    while ((x !=root) && (x->parent->colour == red)) {
      if (x->parent == x->parent->parent->left) {
           /* If x's parent is a left, y is x's right 'uncle' */
           y = x->parent->parent->right;
           if ((y!=NULL)&&(y->colour == red)) {
               /* case 1 - change the colours */
               x->parent->colour = black;
               y->colour = black;
               x->parent->parent->colour = red;
               /* Move x up the tree */
               x = x->parent->parent;
               }
           else {
               /* y is a black node */
               if ( x == x->parent->right ) {
                   /* and x is to the right */ 
                   /* case 2 - move x up and rotate */
                   x = x->parent;
                   left_rotate(x);
                   }
               /* case 3 */
               x->parent->colour = black;
               x->parent->parent->colour = red;
               right_rotate(x->parent->parent);
               }
           }
       else {
           /* repeat the "if" part with right and left
              exchanged */
		   /* If x's parent is a right, y is x's left 'uncle' */
           y = x->parent->parent->left;
           if ((y!=NULL)&&(y->colour == red)) {
               /* case 1 - change the colours */
               x->parent->colour = black;
               y->colour = black;
               x->parent->parent->colour = red;
               /* Move x up the tree */
               x = x->parent->parent;
               }
           else {
               /* y is a black node */
               if ( x == x->parent->left ) {
                   /* and x is to the right */ 
                   /* case 2 - move x up and rotate */
                   x = x->parent;
                   right_rotate(x);
                   }
               /* case 3 */
               x->parent->colour = black;
               x->parent->parent->colour = red;
               left_rotate(x->parent->parent);
               }
           
	   }
	}
  /* Colour the root black */
  root->colour = black;
}

Boolean RBTree::found(Vector &v,System &B)
// Check if the vector is in the tree
{
  RBptr p;
  p=root;
  while(p!=NULL){
    if(v.lexComp(*B.get(p->index))==0)
	  return(TRUE);
    if(v.lexComp(*B.get(p->index))<0)
	  p=p->left;
	else
	  p=p->right;
  }
  return(FALSE); 
}

void RBTree::print(RBptr x)
// print inorder traversal
{
  if(x==NULL)
	return;
  print(x->left);
  printf("%i ",x->index);
  print(x->right);
}

void RBTree::print(FILE *fout,RBptr x)
// print inorder traversal
{
  if(x==NULL)
	return;
  print(fout,x->left);
  fprintf(fout,"%i ",x->index);
  print(fout,x->right);
}

void RBTree::print()
{
  printf("Tree:\n");
  print(root);
}

void RBTree::print(FILE *fout)
{
  fprintf(fout,"Tree:\n");
  print(fout,root);

}

void RBTree::print(RBptr x,System &B)
// print inorder traversal
{
  if(x==NULL)
	return;
  print(x->left,B);
  printf("%i:",x->index);
  B.get(x->index)->print(); printf(" ");
  print(x->right,B);
}

void RBTree::print(FILE *fout,RBptr x,System &B)
// print inorder traversal
{
  if(x==NULL)
	return;
  print(fout,x->left,B);
  fprintf(fout,"%i ",x->index);
  B.get(x->index)->print(fout); fprintf(fout," ");
  print(fout,x->right,B);
}

void RBTree::print(System &B)
{
  printf("Tree:\n");
  print(root,B);
}

void RBTree::print(FILE *fout,System &B)
{
  fprintf(fout,"Tree:\n");
  print(fout,root,B);

}

int RBTree::height(RBptr x)
{
  if(x==NULL)
	return 0;
  return (1+max(height(x->left),height(x->right)));
}

int RBTree::height()
{
  return(height(root));
}

/***************************************************
*****                   Heap                   *****
****************************************************/

HNode& HNode::operator=(HNode &x)
// assign a heap node to another
{
  this->index=x.index;
  return(*this);
}

Heap::Heap(int size)
{
  ar=new HNode[size];
  num=0;
}

Heap::~Heap()
{
  delete [] ar;
}

void Heap::swap(HNode *x,HNode *y)
{
  HNode t=*x;
  *x=*y;
  *y=t;
}

int Heap::numEss(IndexArray &a,int i)
{
  if(num==0)
	return(INFNTY_INT);
  return(a.getNumEss(ar[i].index));
}

void Heap::heapify(IndexArray &a,int l=0)
// heapify starting from l
{
  for(int i=l;i<num;){
	if(2*i+2<num)
		if((numEss(a,i)>numEss(a,2*i+1))&&(numEss(a,2*i+1)<=numEss(a,2*i+2))){
	      swap(&ar[i],&ar[2*i+1]);
		  i=2*i+1;
		} else if((numEss(a,i)>numEss(a,2*i+2))&&(numEss(a,2*i+2)<=numEss(a,2*i+1))){
	      swap(&ar[i],&ar[2*i+2]);
		  i=2*i+2;
		} else
		  break;
	else if(2*i+1<num)
        if((numEss(a,i)>numEss(a,2*i+1))){
	      swap(&ar[i],&ar[2*i+1]);
		  i=2*i+1;
		} else
		  break;
	else
	  break;
  }
}

void Heap::heapifyUp(IndexArray &a,int l)
// heapify Upward starting from l
{
  for(int i=l;i>0;i=(i-1)/2)
    if(numEss(a,i)<numEss(a,(i-1)/2))
      swap(&ar[i],&ar[(i-1)/2]);
	else 
	  break;
}

void Heap::adjustHeap(IndexArray &a)
// restore the heap property
{
  for(int i=(num-1)/2;i>=0;i--)
    heapify(a,i);
}

void Heap::insert(int index)
{
  ar[num].index=index;
  num++;
}

void Heap::heapInsert(IndexArray &a,int index)
{
  ar[num].index=index;
  heapifyUp(a,num);
  num++;
}

void Heap::getMin(IndexArray &a,int *index,int *ess)
{
  *index=ar[0].index;
  *ess=numEss(a,0);
}

void Heap::deleteMin(IndexArray &a,int *index,int *ess)
{
  *index=ar[0].index;
  *ess=numEss(a,0);
  ar[0]=ar[num-1];
  num--;
  heapify(a);
}

void Heap::print()
{
  if(num==0){
	printf("Heap is empty\n");
	return;
  }
  printf("Heap (%i elements):",num);
  for(int i=0;i<num;i++)
	printf("%i ",ar[i].index);
  printf("\n");
}

void Heap::print(FILE *fout)
{
  if(num==0){
	fprintf(fout,"Heap is empty\n");
	return;
  }
  fprintf(fout,"Heap (%i elements):",num);
  for(int i=0;i<num;i++)
	fprintf(fout,"%i ",ar[i].index);
  fprintf(fout,"\n");
}

void Heap::print(IndexArray &a)
{
  if(num==0){
	printf("Heap is empty\n");
	return;
  }
  printf("Heap (%i elements):",num);
  for(int i=0;i<num;i++)
	printf("(%i:%i:%i) ",i,ar[i].index,numEss(a,i));
  printf("\n");
}

void Heap::print(FILE *fout,IndexArray &a)
{
  if(num==0){
	fprintf(fout,"Heap is empty\n");
	return;
  }
  fprintf(fout,"Heap (%i elements):",num);
  for(int i=0;i<num;i++)
	fprintf(fout,"(%i:%i:%i) ",i,ar[i].index,numEss(a,i));
  fprintf(fout,"\n");
}
