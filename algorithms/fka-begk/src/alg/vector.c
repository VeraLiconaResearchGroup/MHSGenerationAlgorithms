class Box;

/***************************************************
*****              class Vector                *****
****************************************************/

Vector::Vector(int n,integer *c)
{
  this->n=n;
  this->mark=0;
  this->maxMark=0;
  this->c=new integer[n];
  for(int i=0;i<n;i++){
    this->c[i]=c[i];
  }
}

Vector::Vector(int n)
{
  this->n=n;
  this->mark=0;
  this->maxMark=0;
  this->c=new integer[n];
}

Vector::Vector()
{
  this->n=0;
  this->mark=0;
  this->maxMark=0;
  this->c=NULL;
}

void Vector::init(int n)
{
  this->n=n;
  this->mark=0;
  this->maxMark=0;
  this->c=new integer[n];
}

Vector::~Vector()
{
  delete [] c;
}

Vector& Vector::operator=(Vector& v)
// assign one vector to another
{
  this->n=v.n;
  for(int i=0;i<n;i++)
    this->c[i]=v.c[i];
  this->mark=v.mark;
  this->maxMark=v.maxMark;
  return(*this);
}

Vector Vector::operator=(integer *c)
// assign an array to a vector
{
  for(int i=0;i<n;i++)
    this->c[i]=c[i];
  return(*this);
}

Vector Vector::operator+(Vector& v)
{
  Vector result(n);
  this->n=v.n;
  for(int i=0;i<n;i++)
    result.c[i]=this->c[i]+v.c[i];
  return(result);
}

Vector Vector::meet(Vector &v)
// return the componenet wise min of v and this vector
{
  Vector result(n);
  int i;
  for(i=0;i<n;i++)
	result.c[i]=min(this->c[i],v.c[i]);
  return(result);
}

Vector Vector::join(Vector &v)
// return the componenet wise min of v and this vector
{
  Vector result(n);
  int i;
  for(i=0;i<n;i++)
	result.c[i]=max(this->c[i],v.c[i]);
  return(result);
}

Boolean Vector::above(Vector &x)
// Check if this vector is above x  
{
  for(int i=0;i<n;i++){
    if(c[i]<x.c[i])
      return(FALSE);
  }
  return(TRUE);
}

Boolean Vector::below(Vector &x)
// Check if this vector is below x  
{
  for(int i=0;i<n;i++){
    if(c[i]>x.c[i])
      return(FALSE);
  }
  return(TRUE);
}

Boolean Vector::intersect(Box &C,int dir)
// chack if this vector intersects the box
// dir=0 if ideal and 1 if filter
{
  if(!dir)
	return(below(*C.getU()));
  else
    return(above(*C.getL()));
}

int Vector::lexComp(Vector &x)
// Check if this vector is lexcographically less than, equal to, or greater than
// vector x.    Return -1.0, or 1 respectively  
{
  for(int i=0;i<n;i++)
	if(c[i]<x.c[i])
      return(-1);
	else if(c[i]>x.c[i])
      return(1);
  return(0);
}

Boolean Vector::operator==(Vector& v)
{
  return (lexComp(v)==0);
}

void Vector::print()
{
  printf("(");
  for(int i=0;i<n-1;i++)
    printf("%i,",c[i]);
  printf("%i)",c[n-1]);
}

void Vector::print(FILE *fout)
{
  fprintf(fout,"(");
  for(int i=0;i<n-1;i++)
    fprintf(fout,"%i,",c[i]);
  fprintf(fout,"%i)",c[n-1]);
}

/***************************************************
*****              class System                *****
****************************************************/

System::System(int n,int size)
// construct an empty system of vectors of dimension n and size size
{
  this->va=new Vector[size];
  this->n=n;
  this->size=size;
  for(int i=0;i<size;i++)
    va[i].init(n);
  this->num=this->max=0;
}

System::System()
// construct an empty system of vectors
{
  this->num=this->max=this->size=0;
  this->va=NULL;
}

void System::init(int n,int size=MAXALLOC)
// construct an empty system of vectors of dimension n and size size
{
  this->va=new Vector[size];
  this->n=n;
  this->size=size;
  for(int i=0;i<size;i++)
    va[i].init(n);
  this->num=this->max=0;
}

System::~System()
{
  delete [] va;
}

void System::addVector(Vector &v)
{
  if(max==size){
	size=2*size;  // use doubling
	Vector *va=new Vector[size];
	int i;
    for(i=0;i<size;i++)
      va[i].init(n);
    for(i=0;i<max;i++)
	  va[i]=this->va[i];
    delete [] this->va;
	this->va=va;
  }
  va[max]=v;
  num++;
  max++;
}

void System::deleteVector(int i)
// marks for deletion a specific vector from the system
{
  va[i].setMark(TRUE);
  num--;
}

void System::cleanUp()
// deletes all vectors marked for deletion in the system
{
  if(num<size/4)
	size=size/4; //use 1/4 ing
  Vector *va=new Vector[size];
  int i;
  for(i=0;i<size;i++)
    va[i].init(n);
  num=0;
  for(i=0;i<max;i++)
	if(!this->va[i].marked()){
	  va[num]=this->va[i];
	  va[num++].setMark(FALSE);
	}
  delete [] this->va;
  this->va=va;
  max=num;
}

void System::print()
{
  if (num==0){
	printf("System is empty");
	return;
  }
  printf("{");
  for(int i=0;i<max-1;i++)
    if(!va[i].marked()) {
      va[i].print();
	  printf(",");
	}
  if(!va[max-1].marked())
    va[max-1].print();
  printf("}");
}

void System::print(FILE *fout)
{
  if (num==0){
	fprintf(fout,"System is empty");
	return;
  }
  fprintf(fout,"{");
  for(int i=0;i<max-1;i++)
    if(!va[i].marked()) {
      va[i].print(fout);
	  fprintf(fout,",");
	}
  if(!va[max-1].marked())
    va[max-1].print(fout);
  fprintf(fout,"}");
}

Boolean System::read(char *inFile,int *n,integer *c,int *s)
{
  FILE *fin=fopen(inFile,"r");
  *s=2;
  if(fin==NULL)
	return(FALSE);
  int x;
//  fscanf(fin,"%i %i %i",&x,c,s);
  fscanf(fin,"%i %i",&x,c); 
  *n=x;
  if(size==0)
	init(*n,MAXALLOC/20);
  integer v;
  while(!feof(fin)){
	 Vector x(*n);
	 for(int i=0;i<*n;i++){
	   fscanf(fin,"%i",&v);
	   x.set(i,v);
	 }
	 if(feof(fin))
	   break;
	 addVector(x);
  }
  fclose(fin);
  minimal();
  return(TRUE);
}

void System::sort(int dir)
{
// sorts the system into (reverse) lexcographic order
// dir=0 lex  dir=1 reverse lex  dir=anything else  no sorting
  if((dir!=0)&&(dir!=1))
	return;
  if(dir==0)
    qsort((char *) va,num, sizeof(Vector), revLexCompare);
  else
    qsort((char *) va,num, sizeof(Vector), lexCompare);
}

void System::fprint(char *outFile,int n,integer c,int s)
{
  FILE *fout=fopen(outFile,"w");
//  fprintf(fout,"%i %i %i\n",n,c,s);
  fprintf(fout,"%i %i \n",n,c);
  for(int k=0;k<num;k++){
    for(int i=0;i<n;i++)
      fprintf(fout,"%i ",get(k)->get(i));
    fprintf(fout,"\n");
  }
  fclose(fout);
}

Boolean System::found(Vector& x)
// Check if some vector in the system is equal to x 
{
  for(int i=0;i<max;i++)
    if ((!va[i].marked())&&(va[i]==x))
	  return(TRUE);
  return(FALSE);
}

Boolean System::below(Vector &x,int begin=0,int end=-1)
// Check if some vector in the subsystem {begin,...,end} is below x 
{
  if(end==-1)
	end=max-1;
  for(int i=begin;i<=end;i++)
    if ((!va[i].marked())&&(va[i].below(x)))
	  return(TRUE);
  return(FALSE);
}

Boolean System::above(Vector &x,int begin=0,int end=-1)
// Check if some vector in the subsystem {begin,...,end} is above x 
{
  if(end==-1)
	end=max-1;
  for(int i=begin;i<=end;i++)
    if ((!va[i].marked())&&(va[i].above(x)))
	  return(TRUE);
  return(FALSE);
}

Boolean System::below(System &A,int begin=0,int end=-1)
// check if every vector in the subsystem {begin,...,end} is below some vector in A
{
  if(end==-1)
	end=max-1;
  for(int i=begin;i<=end;i++)
    if ((!va[i].marked())&&(!A.above(va[i])))
      return(FALSE);
  return(TRUE); 
}

Boolean System::above(System &A,int begin=0,int end=-1)
// check if every vector in the subsystem {begin,...,end} is above some vector in A
{
  if(end==-1)
	end=max-1;
  for(int i=begin;i<=end;i++)
    if ((!va[i].marked())&&(!A.below(va[i])))
      return(FALSE);
  return(TRUE); 
}

Boolean System::equivalent(System &A)
// check if two antichains are equivalent 
{
  return(above(A)&&below(A));
}

void System::complement(Box &C)
// Complemet the vectors of the system with respect ot the box
{ 
  for(int i=0;i<num;i++)
    for(int j=0;j<n;j++)
      va[i].set(j,C.getU(j)-va[i].get(j));
}

void System::maximal()
// delete all non-maximal elements from the system 
{
  // mark for deletion
  int numOld=num;
  for(int i=0;i<max;i++)
    if(!va[i].marked())
	  for(int j=i+1;j<max;j++)
        if(!va[j].marked())
		  if(va[i].above(va[j])){
		    va[j].setMark(TRUE);
			num--;
		  }
	      else if(va[j].above(va[i])){
		    va[i].setMark(TRUE);
			num--;
		    break;
		  }
  // then delete
  cleanUp();
}

void System::minimal()
// delete all non-minimal elements from the system 
{
  // mark for deletion
  int numOld=num;
  for(int i=0;i<max;i++)
    if(!va[i].marked())
	  for(int j=i+1;j<max;j++)
        if(!va[j].marked())
		  if(va[i].below(va[j])){
		    va[j].setMark(TRUE);
			num--;
		  }
	      else if(va[j].below(va[i])){
		    va[i].setMark(TRUE);
			num--;
		    break;
		  }
  // then delete
  if(num<numOld) 
	cleanUp();
}

Boolean System::maxIndep(Vector &x,Box &C,int begin=0,int end=-1)
// check if x is a maximal independent element of the subsystem {begin,...,end}
{
  if(below(x,begin,end))
	return(FALSE);
  for(int i=0;i<n;i++)
	if(x.get(i)<C.getU(i)){
	  x.inc(i);
      if(!below(x,begin,end))
	    return(FALSE);
      x.dec(i);
	}
  return(TRUE);
}

Boolean System::maxIndep(System &A,Box &C,int begin=0,int end=-1)
// check if A is a set of maximal independent elements of the subsystem {begin,...,end}
{
  for(int i=0;i<A.num;i++)
    if(!maxIndep(A.va[i],C,begin,end))
	  return(FALSE);
  return(TRUE);
}

void System::shiftUp(int b,int start)
// shift the elements {b,...,end of System} to start from {start,...} where start>=b
{
  if(b>=num)
	return;
  int j=start;
  for(int i=b;i<num;i++)
    va[j++]=va[i];
  num-=(b-start);
  max-=(b-start);
}

/***************************************************
*****              class IndexNode            *****
****************************************************/

IndexNode& IndexNode::operator=(IndexNode& x)
// assign one index node to another
{
  this->index=x.index;
  this->mark=x.mark;
  return(*this);  
}

/***************************************************
*****              class IndexArray            *****
****************************************************/

IndexArray::IndexArray(int size)
{
  this->in=new IndexNode[size];
  this->size=size;
  this->num=0;
}

IndexArray::~IndexArray()
{
  delete [] in;
}

void IndexArray::addIndex(int j)
{
  if(num==size){
	size=2*size;  // use doubling
	IndexNode *in=new IndexNode[size];
	for(int i=0;i<size;i++)
      in[i]=this->in[i];
    delete [] this->in;
	this->in=in;
  }
  in[num].index=j;
  in[num++].mark=0;
}

int IndexArray::deleteIndex()
{
  if(num==0)
	return -1;
  if(num<size/4){
	size=size/4; //use 1/4 ing
    IndexNode *in=new IndexNode[size];
    for(int i=0;i<num;i++) 
	  in[i]=this->in[i];
    delete [] this->in;
    this->in=in;
  }
  return(in[--num].index);
}

void IndexArray::shiftUp(int b,int start)
// shift the elements {b,...,end of array} to start from {start,...} where start>=b
{
  int j=start;
  for(int i=b;i<num;i++)
    in[j++]=in[i];
  num-=(b-start);
}

void IndexArray::print()
{
  for(int i=0;i<num;i++)
	printf("%i %i %i \n",in[i].index,in[i].mark,in[i].numEss);
}

void IndexArray::print(FILE *fout)
{
  for(int i=0;i<num;i++)
	fprintf(fout,"%i %i %i \n",in[i].index,in[i].mark,in[i].numEss);
}

/***************************************************
*****              class DeleteArray           *****
****************************************************/

DeleteArray::DeleteArray(int size)
{
  this->in=new int[size];
  this->size=size;
  this->num=0;
}

DeleteArray::~DeleteArray()
{
  delete [] in;
}

void DeleteArray::addIndex(int j)
{
  if(num==size){
	size=2*size;  // use doubling
	int *in=new int[size];
	for(int i=0;i<size;i++)
      in[i]=this->in[i];
    delete [] this->in;
	this->in=in;
  }
  in[num++]=j;
}

int DeleteArray::deleteIndex()
{
  if(num==0)
	return 0;
/*  if(num<size/4){
	size=size/4; //use 1/4 ing
    int *in=new int[size];
    for(int i=0;i<num;i++) 
	  in[i]=this->in[i];
    delete [] this->in;
    this->in=in;
  }*/
  return(in[--num]);
}

void DeleteArray::print()
{
  for(int i=0;i<num;i++)
	printf("%i \n",in[i]);
}

void DeleteArray::print(FILE *fout)
{
  for(int i=0;i<num;i++)
	fprintf(fout,"%i \n",in[i]);
}

/***************************************************
*****              class Problem               *****
****************************************************/

Problem::Problem(int n,integer c=1)
{
  this->n=n;
  this->C=new Box(n,c);
  this->CO=new Box(n,c); // for internal use
  this->A=new System(n);
  this->B=new System(n);
  this->TB=new RBTree();
  this->S=new Stack;
  this->indexA=new IndexArray();
  this->indexB=new IndexArray();
  this->heapA=new Heap();
  this->heapB=new Heap();
  this->delA=new DeleteArray();
  this->delB=new DeleteArray();
  initialize();
}

Problem::Problem(int n,Box *C,System *A,System *B,RBTree *TB,Stack *S,IndexArray *indexA,IndexArray *indexB,Heap *heapA,Heap *heapB,DeleteArray *delA,DeleteArray *delB)
{
  this->n=n;
  this->C=C;
  this->CO=new Box(n);
  *CO=*C;
  this->A=A;
  this->B=B;
  this->TB=TB;
  this->S=S;
  this->heapA=heapA;
  this->heapB=heapB;
  this->indexA=indexA;
  this->indexB=indexB;
  this->delA=delA;
  this->delB=delB;
  initialize();
}

Problem::Problem(int n,Box *C,System *A,System *B,RBTree *TB,RBTree *TA)
{
  this->n=n;
  this->C=C;
  this->CO=new Box(n);
  *CO=*C;
  this->A=A;
  this->B=B;
  this->S=new Stack;
  this->TA=TA;
  this->TB=TB;
  this->indexA=new IndexArray();
  this->indexB=new IndexArray();
  this->heapA=new Heap();
  this->heapB=new Heap();
  this->delA=new DeleteArray();
  this->delB=new DeleteArray();
  initialize2();
}

Problem::Problem(int n,Box *C,System *A,System *B)
{
  this->n=n;
  this->C=C;
  this->CO=new Box(n);
  *CO=*C;
  this->A=A;
  this->B=B;
  this->S=new Stack;
  this->TB=new RBTree();
  this->indexA=new IndexArray();
  this->indexB=new IndexArray();
  this->heapA=new Heap();
  this->heapB=new Heap();
  this->delA=new DeleteArray();
  this->delB=new DeleteArray();
  initialize();
}

Problem::~Problem()
{
  delete A;
  delete B;
  delete CO;
  delete TB;
  delete S;
  delete indexA;
  delete indexB;
  delete heapA;
  delete heapB;
  delete delA;
  delete delB;
}

void Problem::printABC()
{
  printf("Box: "); C->print(); printf("\n");
  printf("A: "); A->print(); printf("\n");
  printf("B: "); B->print(); printf("\n");
}

void Problem::printABC(FILE *fout)
{
  fprintf(fout,"Box: "); C->print(fout); fprintf(fout,"\n");
  fprintf(fout,"A: "); A->print(fout); fprintf(fout,"\n");
  fprintf(fout,"B: "); B->print(fout); fprintf(fout,"\n");
}

void Problem::printA()
{
  printf("current A: ");
  if(ANum()==0){
	printf("System is empty");
	return;
  }
  for(int k=Ab();k<=Ae();k++){
	printf("[%i,%i,%i]:",getIndexA(k),markedA(k),numEssA(k));
	getA(k)->print();
  }
  printf("\n");
}

void Problem::printA(FILE *fout)
{
  fprintf(fout,"current A: ");
  if(ANum()==0){
	fprintf(fout,"System is empty");
	return;
  }
  for(int k=Ab();k<=Ae();k++){
	fprintf(fout,"[%i,%i,%i]:",getIndexA(k),markedA(k),numEssA(k));
	getA(k)->print(fout);
  }
  fprintf(fout,"\n");
}

void Problem::printB()
{
  printf("current B: ");
  if(BNum()==0){
	printf("System is empty");
	return;
  }
  for(int k=Bb();k<=Be();k++){
	printf("[%i,%i,%i]:",getIndexB(k),markedB(k),numEssB(k));
	getB(k)->print();
  }
  printf("\n");
}

void Problem::printB(FILE *fout)
{
  fprintf(fout,"current B: ");
  if(BNum()==0){
	fprintf(fout,"System is empty");
	return;
  }
  for(int k=Bb();k<=Be();k++){
	fprintf(fout,"[%i,%i,%i]:",getIndexB(k),markedB(k),numEssB(k));
	getB(k)->print(fout);
  }
  fprintf(fout,"\n");
}

void Problem::print()
{
  printf("Box: "); getC()->print(); printf("\n");
  printA(); printf("\n");
  printB(); printf("\n");
}


void Problem::print(FILE *fout)
{
  fprintf(fout,"Box: "); getC()->print(fout); fprintf(fout,"\n");
  printA(fout); fprintf(fout,"\n");
  printB(fout); fprintf(fout,"\n");
}

int Problem::numEssentialDown(Vector &x,Box &C)
// returns the number of essential coordinate of vector x (as ideal) in box C
{
  int num=0;
   for(int i=0;i<x.getDim();i++){
	 if(x.get(i)>C.getL(i)) 
	   num++;
   }
   return(num);
}

int Problem::numEssentialUp(Vector &x,Box &C)
// returns the number of essential coordinate of vector x (as filter) in box C
{
   int num=0;
   for(int i=0;i<x.getDim();i++){
	 if(x.get(i)<C.getU(i)) 
	   num++;
   }
   return(num);
}

void Problem::computeEssentialsDown()
// Compute the number of essential coordinates of vectors in A in box C
{
  for(int k=Ab();k<=Ae();k++)
	setNumEssA(k,numEssentialDown(*getA(k),*getC()));
}

void Problem::computeEssentialsUp()
// Compute the number of essential coordinates of vectors in B in box C
{
  for(int k=Bb();k<=Be();k++){
	setNumEssB(k,numEssentialUp(*getB(k),*getC()));
  }
}

double Problem::expectation()
     // compute the expectation of the coverage of A,B
{
  double E=0;
  int k;
  for(k=Ab();k<=Ae();k++)
    E+=exp(numEssA(k)*log(0.5));
  for(k=Bb();k<=Be();k++)
    E+=exp(numEssB(k)*log(0.5));
  return(E);
}

double Problem::expectation(Vector &z,int i)
     // compute the expectation of the coverage of A,B, when fixing z0,...,zi
{
  double E=0;
  int num;
  Boolean nonCovering;
  int  k;
  for(k=Ab();k<=Ae();k++){
	nonCovering=FALSE;
    for(int j=0;j<=i;j++)
	  if(getA(k)->get(j)>z.get(j)){ // assuming all the vectors intersect the box
	    nonCovering=TRUE;
	    break;
	  }
    if(!nonCovering){
      num=0;
      for(int j=i+1;j<n;j++){
        if(getA(k)->get(j)>getC()->getL(j)) 
		  num++;
      }
      E+=exp(num*log(0.5));
    }
  }
  for(k=Bb();k<=Be();k++){
	nonCovering=FALSE;
    for(int j=0;j<=i;j++)
	  if(getB(k)->get(j)<z.get(j)){ // assuming all the vectors intersect the box 
	    nonCovering=TRUE;
	    break;
	  }
    if(!nonCovering){
      num=0;
      for(int j=i+1;j<n;j++){  
        if(getB(k)->get(j)<getC()->getU(j)) 
		  num++;
      }
      E+=exp(num*log(0.5));
    }
  }
  return(E);
}

Boolean Problem::ABelow(Vector &x,int begin=-1,int end=-1)
// check if some vector in A is below x
{
  if(begin==-1)
	begin=Ab();
  if(end==-1)
	end=Ae();
  for(int k=begin;k<=end;k++){
	if(debug==10){
	  x.print();printf(" ");
	  getA(k)->print();printf("\n");
	}
	if(!markedA(k)&&(getA(k)->below(x)))
	  return(TRUE);
  }
  return(FALSE);
}

Boolean Problem::BAbove(Vector &x,int begin=-1,int end=-1)
// check if some vector in B is above x
{
  if(begin==-1)
	begin=Bb();
  if(end==-1)
	end=Be();
  for(int k=begin;k<=end;k++)
	if(!markedB(k)&&(getB(k)->above(x)))
	  return(TRUE);
	return(FALSE);
}

Boolean Problem::maxIndep(Vector &x,int begin=-1,int end=-1)
// check if x is a maximal independent element of the subsystem {begin,...,end} of A
{
  if(ABelow(x,begin,end))
	return(FALSE);
  for(int i=0;i<n;i++)
	if(x.get(i)<CO->getU(i)){
	  x.inc(i);
      if(!ABelow(x,begin,end))
	    return(FALSE);
      x.dec(i);
	}
  return(TRUE);
}

Boolean Problem::maxIndep(System &B,int begin=-1,int end=-1)
// check if B is a set of maximal independent elements of the subsystem {begin,...,end} of A
{
  for(int i=0;i<B.getNum();i++)
    if(!maxIndep(*B.get(i),begin,end))
	  return(FALSE);
  return(TRUE);
}

Boolean Problem::maxIndep(int beginA=-1,int endA=-1,int beginB=-1,int endB=-1)
// check if the subsystem {beginB,...,endB} of B is a set of maximal independent elements of the subsystem {beginA,...,endB} of A
{
  if(beginA==-1)
	beginA=Ab();
  if(endA==-1)
	endA=Ae();
  if(beginB==-1)
	beginB=Bb();
  if(endB==-1)
	endB=Be();
  for(int i=beginB;i<=endB;i++)
    if(!maxIndep(*getB(i),beginA,endA))
	  return(FALSE);
  return(TRUE);
}

void Problem::solution(Vector *v,int num=0)
// Assuming E[coverage]<1/2
// do sampling num times,
// and if no solution is found, compute a solution by conditinal expectations
{
  int i,j;
  for(j=0;j<num;j++){
	for(i=0;i<n;i++){
	  if((float)(rand())/RAND_MAX<0.5)
		v->set(i,getC()->getL(i));
	  else
		v->set(i,getC()->getU(i));
	}
	if(ABelow(*v)) 
	  continue;
	maximize(v);
	if(!TB->found(*v,*B))
	  return;
  }
  double E1,E2,E;
  int k;
  int *markA=new int[ANum()]; // mark for deletion
  int *markB=new int[BNum()]; // mark for deletion
  computeEssentialsDown();
  computeEssentialsUp();
  for(i=0;i<ANum();i++)
	markA[i]=0;
  for(i=0;i<BNum();i++)
	markB[i]=0;
  for(i=0;i<n;i++){
	E1=E2=0;
    for(k=Ab();k<=Ae();k++)
	  if((!markA[k-Ab()])&&(getA(k)->get(i)<=getC()->getL(i))){
		E1+=(E=exp(numEssA(k)*log(0.5)));
		E2+=E;
	  }
	  else
        E2+=exp((numEssA(k)-1)*log(0.5));
	for(k=Bb();k<=Be();k++)
	  if(!markB[k-Bb()]&&(getB(k)->get(i)>=getC()->getU(i))){
		E1+=(E=exp(numEssB(k)*log(0.5)));
		E2+=E;
	  }
	  else
        E1+=exp((numEssB(k)-1)*log(0.5));
	if(E1<E2){
	  v->set(i,getC()->getL(i));
	  for(k=Ab();k<=Ae();k++)
		if(getA(k)->get(i)>getC()->getL(i))
		  markA[k-Ab()]=1;
	  for(k=Bb();k<=Be();k++)
		if(!markB[k-Bb()]&&(getB(k)->get(i)<getC()->getU(i)))
		  setNumEssB(k,numEssB(k)-1);
	}
	else{
	  v->set(i,getC()->getU(i));
      for(k=Ab();k<=Ae();k++)
		if(!markA[k-Ab()]&&(getA(k)->get(i)>getC()->getL(i)))
		  setNumEssA(k,numEssA(k)-1);
	  for(k=Bb();k<=Be();k++)
		if(getB(k)->get(i)<getC()->getU(i))
		  markB[k-Bb()]=1;
	}
  }
  computeEssentialsDown();
  computeEssentialsUp();
  delete [] markA;
  delete [] markB;
}

Boolean Problem::sample(int k,Vector *v,int *numTrials,Boolean *isMax)
// Try to find a vector not in A+ nor B- by sampling
// k is the maximum number of samples to be taken before returning false
// numTrials points to the number of trials before a success 
// isMax=True if the point returned is already maximal
{
  *numTrials=0;
  *isMax=FALSE;
  for(int j=0;j<k;j++){
	for(int i=0;i<n;i++){
	  if((float)(rand())/RAND_MAX<0.5)
		v->set(i,C->getL(i));
	  else
		v->set(i,C->getU(i));
	}
	if(ABelow(*v,Ab(),Ae())) 
	  continue;
	if(BNum()<=A->getNum()){
	  if(!BAbove(*v)){
		*numTrials=j;
	    return(TRUE);
	  }
	} else {
	  maximize(v);
	  if(!TB->found(*v,*B)){
	    *numTrials=j;
		*isMax=TRUE;
	    return(TRUE);
	  }
	}
  }
  return(FALSE);
}

Boolean Problem::findVector(int *i,Boolean *isA)
// Find a vector v in A or B with few essential coordinates in the current box C
// return TRUE in isA if the vector is in A and FALSE if it is in B
// return FALSE if no such vector exists
// return in i the index of the found vector in the vector array
{
  int numEssA,numEssB,iA,iB;
  Boolean exists;
  do {
    heapA->getMin(*indexA,&iA,&numEssA);
	heapB->getMin(*indexB,&iB,&numEssB);
	if(numEssA<=numEssB){
	  *isA=1;
      *i=iA;
	} else {
	  *isA=0;
	  *i=iB;
	}
    if(*isA)
	  exists=!markedA(*i)&&getA(*i)->intersect(*getC(),0);
	else
	  exists=!markedB(*i)&&getB(*i)->intersect(*getC(),1);
	if(!exists){
	  if(*isA){
		heapA->deleteMin(*indexA,i,&numEssA);
		if(!markedA(*i)){
		  indexA->mark(*i);
	      delA->addIndex(-*i-1);
		}
	  }else{
		heapB->deleteMin(*indexB,i,&numEssB);
		if(!markedB(*i)){
          indexB->mark(*i);
		  delB->addIndex(-*i-1);
		}
	  }
	} 
  } while(!exists&&(!heapA->isEmpty()||!heapB->isEmpty()));
/*  int m=ANum()+BNum();
  if((exists)&&(numEss<=log(m)/log(2))){
    return(TRUE);
  }
  return(FALSE);*/
  return(exists);
}

void Problem::eliminate(int num=INFNTY_INT,int *index=NULL,int which=0)
// mark for elimination in the coordinates determined by index[0],...,index[num-1]
// which=0 both A,B  1=A   2=B
{
  int i;
  if(num==INFNTY_INT){
    num=n;
	for(i=0;i<n;i++)
	  index[i]=i;
  }
  int k;
  if((which==0)||(which==1))
    for(k=Ab();k<=Ae();k++) 
      if(!markedA(k))
	    for(i=0;i<num;i++)
	      if(getA(k)->get(index[i])>getC()->getU(index[i])){
		    markA(k);
		    break;
		  } 
  if((which==0)||(which==2))
    for(k=Bb();k<=Be();k++) 
      if(!markedB(k))
	    for(i=0;i<num;i++)
	      if(getB(k)->get(index[i])<getC()->getL(index[i])){
		    markB(k);
		    break;
		  }
}

void Problem::project()
// perform projection
{
  int k;
  for(k=Ab();k<=Ae();k++){
	for(int i=0;i<n;i++)
	  if(getA(k)->get(i)<getC()->getL(i))
        getA(k)->set(i,getC()->getL(i));
  }
  for(k=Bb();k<=Be();k++){
	for(int i=0;i<n;i++)
	  if(getB(k)->get(i)>getC()->getU(i))
        getB(k)->set(i,getC()->getU(i));
  }
}

int Problem::countA(int x,int i)
// Given x and i, return the number of elements of A that have a_i>x 
{
  int count=0;
  for(int k=Ab();k<=Ae();k++){
	if(getA(k)->get(i)>x) 
	  count++;
  } 
  return(count);
}

int Problem::countB(int x,int i)
// Given x and i, return the number of elements of B that have b_i<x 
{
  int count=0;
  for(int k=Bb();k<=Be();k++){
	if(getB(k)->get(i)<x) 
	  count++;
  } 
  return(count);
}

int Problem::countA(int x,int i,Box &C)
// Given x and i, return the number of elements of A that have a_i>x 
{
  int count=0;
  for(int k=Ab();k<=Ae();k++){
	if((getA(k)->get(i)>x)&&(getA(k)->get(i)<=C.getU(i))) 
	  count++;
  } 
  return(count);
}

int Problem::countB(int x,int i,Box &C)
// Given x and i, return the number of elements of B that have b_i<x 
{
  int count=0;
  for(int k=Bb();k<=Be();k++){
	if((getB(k)->get(i)<x)&&(getB(k)->get(i)>=C.getL(i))) 
	  count++;
  } 
  return(count);
}

int Problem::estCountA(int k,int x,int i,int *num)
// Given x and i, return an estimation of the number of elements of A that have a_i>x 
// k is the number of random samples used for estimation
// Mark those elements, encountered, that do not intersect the current box
// return in num the number of vectors of B that do not intersect the current box
{
  *num=0;
  int count=0;
  Vector *v;
  for(int j=0;j<k;j++){
    int index=uniform(Ab(),Ae());
    if(!markedA(index)&&((v=getA(index))->intersect(*getC(),0))){
	  if(v->get(i)>x)
		count++;
	} else {
	  if(!markedA(index)){
	    markA(index);
	    delA->addIndex(index+1);
	  }
	  *num++;
	}
  }
  count=count*(Ae()-Ab()+1)/k;
  return(count);
}

int Problem::estCountB(int k,int x,int i,int *num)
// Given x and i, return an estimation of the number of elements of B that have b_i<x 
// k is hte number of random samples used for estimation
// Mark those elements, encountered, that do not intersect the current box
// return in num the number of vectors of B that do not intersect the current box
{
  *num=0;
  int count=0;
  Vector *v;
  for(int j=0;j<k;j++){
    int index=uniform(Bb(),Be());
    if(!markedB(index)&&((v=getB(index))->intersect(*getC(),1))){
	  if(v->get(i)<x)
		count++;
	} else {
	  if(!markedB(index)){
	    markB(index);
	    delB->addIndex(index+1);
	  }
	  *num++;
	}
  }
  count=count*(Be()-Bb()+1)/k;
  return(count);
}

int Problem::estSizeA(int k)
// return an estimation of the number of elements of A 
// k is the number of random samples used for estimation
// Mark those elements, encountered, that do not intersect the current box
{
  int count=0;
  for(int j=0;j<k;j++){
    int index=uniform(Ab(),Ae());
    if(!markedA(index)&&(getA(index))->intersect(*getC(),0)){
		count++;
	} else {
	  if(!markedA(index)){
	    markA(index);
	    delA->addIndex(index+1);
	  }
	}
  }
  count=count*(Ae()-Ab()+1)/k;
  return(count);
}

int Problem::estSizeB(int k)
// return an estimation of the number of elements of A 
// k is the number of random samples used for estimation
// Mark those elements, encountered, that do not intersect the current box
{
  int count=0;
  for(int j=0;j<k;j++){
    int index=uniform(Bb(),Be());
    if(!markedB(index)&&(getB(index))->intersect(*getC(),1)){
		count++;
	} else {
	  if(!markedB(index)){
	    markB(index);
	    delB->addIndex(index+1);
	  }
	}
  }
  count=count*(Be()-Bb()+1)/k;
  return(count);
}

Boolean Problem::split(Vector *v,int *j,integer *y)
// either return a non-covered vector or a volume descreasing split
// A split is defined by a coordinate j and a point y in C_j
// return TRUE if there is a split and FALSE otherwise
{
  if(debug==1) {
	fout=fopen(debugFile,"a");
	fprintf(fout,"Problem:\n" ); print(fout); fprintf(fout,"\n");
	printf("Problem:\n" ); print(); printf("\n");
	fclose(fout);
  }
  double E=expectation();
  if(E<1.0){
	solution(v);
	if(debug==1) {
	  fout=fopen(debugFile,"a");
	  fprintf(fout,"Solution= "); v->print(fout); fprintf(fout,"\n");
	  printf("Solution= "); v->print(); printf("\n");
      fclose(fout);
	}
	return(FALSE);
  }
  double epsilon=log(2)/(log(ANum()+BNum()));
  Boolean isA=FALSE;
  int k;
  if(!findVector(&k,&isA)){ // no frequnet coordinate, shouldn't happen if E<1
     solution(v);
	 return(FALSE);
  }
  Vector *x=(isA==1)?getA(k):getB(k);
  if(debug==1) {
	fout=fopen(debugFile,"a");
	fprintf(fout,"Short (%i): ",isA); x->print(fout); fprintf(fout,"\n");
	printf("Short (%i): ",isA); x->print(); printf("\n");
    fclose(fout);
  }
  if(isA){ // a in A has |Ess(a)|<= log m
    for(int i=0;i<n;i++)
	  if(x->get(i)>getC()->getL(i))
        if(countB(x->get(i),i)>=(BNum())*epsilon){
          *j=i;
		  *y=x->get(i);
		}
  }else{ // b in B has |Ess(b)|<= log m
    for(int i=0;i<n;i++) 
	  if(x->get(i)<getC()->getU(i))
	    if(countA(x->get(i),i)>=(ANum())*epsilon){
		  *j=i;
		  *y=x->get(i)+1;
		}
  }
  return(TRUE);
}

int Problem::randSplit(Vector *v,int k1,int k2,int *j,integer *y,int *cleanUp)
// Randomized split: either return a non-covered vector or a volume descreasing split
// A split is defined by a coordinate j and a point y in C_j
// return 1 if there is a split, 0 if there is a non-covered vector,
//        2 if duality is discovered, 3 if A has zero size, and 4 if B has zero size 
// k is the number of samples to be taken for estimation
// CleanUp=1, 2 if it is suspected that many vectors need to be cleaned up from A, B
// respectively, and 0 otherwise
{
  *cleanUp=0; 
  int isMax;
/*  if(debug==1) {
	fout=fopen(debugFile,"a");
	fprintf(fout,"Problem:\n" ); print(fout); fprintf(fout,"\n");
	printf("Problem:\n" ); print(); printf("\n");
	fclose(fout);
  }*/
  int numTrials;
  if(sample(k1,v,&numTrials,&isMax)){
	if(debug==1) {
	  fout=fopen(debugFile,"a");
	  fprintf(fout,"Solution= "); v->print(fout); fprintf(fout,"   numTrials=%i\n",numTrials);
	  printf("Solution= "); v->print(); printf("   numTrials=%i\n",numTrials);
      fclose(fout);
	}
	if(!isMax)
	  maximize(v);
	return(0);
  }
  Boolean isA=FALSE;
  int index;
  if(!findVector(&index,&isA)){ // no short implicant, shouldn't happen with high prob.
    if(heapA->isEmpty())
	  *cleanUp=1;
	else // heapB->isEmpty()
	  *cleanUp=2;
    return(3);  
  }
  Vector *x=(isA==1)?getA(index):getB(index);
  if(debug==1) {
	fout=fopen(debugFile,"a");
	fprintf(fout,"Short (%i): ",isA); x->print(fout); fprintf(fout,"\n");
	printf("Short (%i): ",isA); x->print(); printf("\n");
    fclose(fout);
  }
  int i;
  int max=-1;
  int count,num;
  if(isA){ // a in A has |Ess(a)|<= log m
    for(i=0;i<n;i++)
	  if((x->get(i)>getC()->getL(i))&&((count=estCountB(k2,x->get(i),i,&num))>max)){
        max=count;
		*j=i;
		*y=x->get(i);
	  }
	if(max<=0){ // do deterministic counting
	  for(i=0;i<n;i++)
	    if((x->get(i)>getC()->getL(i))&&((count=countB(x->get(i),i,*getC()))>max)){
          max=count;
		  *j=i;
		  *y=x->get(i);
		}
	}
	if(max==-1)
	  return(2); // must be dual
	else if(max==0){ // or |B|=0
	  *cleanUp=2;
      return(4);  
	}
	if(num>cleanUpFactor*BNum())
	  *cleanUp=2;
  } else { // b in B has |Ess(b)|<= log m
    for(i=0;i<n;i++) 
	  if((x->get(i)<getC()->getU(i))&&((count=estCountA(k2,x->get(i),i,&num))>max)){
        max=count;
		*j=i;
		*y=x->get(i)+1;
	  }
	if(max<=0){ // do deterministic counting
      for(i=0;i<n;i++) 
	    if((x->get(i)<getC()->getU(i))&&((count=countA(x->get(i),i,*getC()))>max)){
          max=count;
		  *j=i;
		  *y=x->get(i)+1;
		}
	}
	if(max==-1)
	  return(2); // must be dual
	else if(max==0){ // or |A|=0
	  *cleanUp=1;
      return(3);  
	}
	if(num>cleanUpFactor*ANum())
	  *cleanUp=1;
  }
  return(1);
}

void Problem::maximize(Vector *x,Boolean random)
// extend the given vector x to a maximal independent vector of A in the box CO
// If random=TRUE, then randomize coordinates first
{
  int *counter=new int[A->getNum()];
  int *order=new int[n];
  int index,i,k,j=0;
  for(i=0;i<n;i++)
	order[i]=i;
  if(random)
    for(i=0;i<n;i++){
	  index=uniform(i,n-1);
	  exchange(&order[i],&order[index]);
	}
  for(k=0;k<A->getNum();k++){
	counter[j]=0;
    for(i=0;i<n;i++)
	  if(A->get(k)->get(i)>x->get(i))
		counter[j]++;
	j++;
  }  
  int Min;
  for(i=0;i<n;i++){
    Min=CO->getU(order[i]); 
	j=0;
	for(k=0;k<A->getNum();k++){
	  if((counter[j]==1)&&(A->get(k)->get(order[i])>x->get(order[i]))&&(A->get(k)->get(order[i])-1<Min))
	    Min=A->get(k)->get(order[i])-1; 
      j++;
	}
    j=0;
    for(k=0;k<A->getNum();k++){
	  if((A->get(k)->get(order[i])>x->get(order[i]))&&(A->get(k)->get(order[i])<=Min))
	    counter[j]--;
      j++;  
    }
	x->set(order[i],Min);
  }
  delete [] order;
  delete [] counter;
}

Boolean Problem::maximize(Vector *x,Boolean random,int *mark,int ab,int ae,Box &C)
// extend the given vector x to a maximal independent vector of A in the box CO
// If random=TRUE, then randomize coordinates first
// the binary mark[0..n-1] tells which coordinates are allowed to be increased
// returns true if there are still coordinates that can be increased
{
  int *counter=new int[ae-ab+1];
  int *order=new int[n];
  int index,i,k;
  for(i=0;i<n;i++)
	order[i]=i;
  if(random)
    for(i=0;i<n;i++){
	  index=uniform(i,n-1);
	  exchange(&order[i],&order[index]);
	}
  for(k=ab;k<=ae;k++)
	if(!getA(k)->maxMarked()){
//		getA(k)->print();printf("\n");
	  counter[k-ab]=0;
      for(i=0;i<n;i++)
	    if (mark[order[i]]){
	      if(getA(k)->get(order[i])>x->get(order[i]))
		    counter[k-ab]++;
		} else {
		   if(getA(k)->get(order[i])>x->get(order[i])){
		     getA(k)->setMaxMark(1);
			 break;
		   }
		}
	}  
  int Min,mm=0;
  for(i=0;i<n;i++)
	if(mark[order[i]]){
      Min=C.getU(order[i]); 
	  for(k=ab;k<=ae;k++)
        if(!getA(k)->maxMarked()){
    	  if((counter[k-ab]==1)&&(getA(k)->get(order[i])>x->get(order[i]))&&(getA(k)->get(order[i])-1<Min))
	        Min=getA(k)->get(order[i])-1; 
	  }
      for(k=ab;k<=ae;k++)
	    if(!getA(k)->maxMarked()){
	      if((getA(k)->get(order[i])>x->get(order[i]))&&(getA(k)->get(order[i])<=Min))
	        counter[k-ab]--;
	  }
	  x->set(order[i],Min);
	  if((Min==C.getU(order[i]))&&(Min!=CO->getU(order[i])))
		mark[order[i]]=1;
	  else
		mark[order[i]]=0;
	  mm=mark[order[i]]||mm;
	}
  delete [] order;
  delete [] counter;
  return(mm);
}

void Problem::maximize2(Vector *x,Boolean random)
// a more efficient maximize
{
  int *mark=new int[n];
  int i,ab,ae,index,L,U,mm;
  ab=ae=0;
  Box C(n);
  C=*getC();
  for(i=0;i<n;i++)
	mark[i]=1;
//  C.print();printf("\n");
//  x->print();printf("\n");
  if(mm=maximize(x,random,mark,Ab(),Ae(),C)){
    SNptr p=this->S->getTop()->below;
    while(p!=NULL){
	  this->S->getNext(p,&ab,&ae,&index,&L,&U);
//	  C.print();printf("\n");
//	  x->print();printf("\n");
	  if(!(mm=maximize(x,random,mark,ab,ae,C)))
	    break;
	  C.setL(index,L);
      C.setU(index,U);
	  p=p->below;
	}
	if(mm)
	  maximize(x,random,mark,0,A->getNum()-1,C);
  }
  for(i=ab;i<=ae;i++)
	getA(i)->setMaxMark(0);
  delete mark;
}

Boolean Problem::newMaxIndep(Vector &v)
// check if v is a new maximal independent element
{
  return(A->maxIndep(v,*CO)&&!B->above(v));
}

int Problem::minNumEss()
{
  int numEssA,numEssB,i;
  heapA->getMin(*indexA,&i,&numEssA);
  heapB->getMin(*indexB,&i,&numEssB);
  if(numEssA<=numEssB)
	return(numEssA);
  else
    return(numEssB);
}

Boolean Problem::dual0(Vector *x)
// performs trivial dualization
// returns true if A, B are dual in C
// returns FALSE and a non-covered vector x, otherwise
// Assumes the necessary duality condition holds: A^+ \cap B^-=\emptyset
{
  int i,k;
  long j;
  if((ANum()==1)&&(BNum()==1)){
    for(i=0;i<n;i++){
      if(getA(Ab())->get(i)>getB(Bb())->get(i))
		j=i; // gauranteed to be found by the necessary duality condition
	  x->set(i,getC()->getU(i));
	}
	if(getB(Bb())->get(j)<getA(Ab())->get(j)-1){
	  x->set(j,getA(Ab())->get(j)-1);
	  maximize(x);
      return(FALSE);
	}
	for(i=0;i<n;i++)
	  if((i!=j)&&(getA(Ab())->get(i)>getC()->getL(i))){
        x->set(j,getA(Ab())->get(j));
        x->set(i,getA(Ab())->get(i)-1);
		maximize(x);
        return(FALSE);
	  }
	for(i=0;i<n;i++)
	  if((i!=j)&&(getB(Bb())->get(i)<getC()->getU(i))){
        x->set(j,getB(Bb())->get(j));
		maximize(x);
        return(FALSE);
	  }	
	return(TRUE);  // trivially dual
  }
  if (ANum()<=BNum()){
	for(i=0;i<n;i++)
      x->set(i,getC()->getU(i));
	if(ANum()==0){
	  if(BNum()<=A->getNum()){
	    if(!BAbove(*x)){
		  maximize(x);
	      return(FALSE);
		} else 
          return(TRUE);
	  } else {
        maximize(x);
	    if(!TB->found(*x,*B))
	      return(FALSE);	    
	    return(TRUE);
	  }
	}
    long int* product=new long int[ANum()+1];
    int *size=new int[ANum()];
	for(i=0;i<ANum();i++)
	  size[i]=n;
	computeProducts(ANum(),size,product);
	Vector t(ANum()); 
	for(j=0;j<product[ANum()];j++){
	  for(i=0;i<n;i++)
        x->set(i,getC()->getU(i));
	  pam(j,&t,ANum(),product);
	  i=0;
	  Boolean qualify=TRUE;
      for(k=Ab();k<=Ae();k++){
		if(getA(k)->get(t.get(i))<=getC()->getL(t.get(i))){
		  qualify=FALSE;
		  break;
		}
		if(!qualify)
		  break;
		if(getA(k)->get(t.get(i))-1<x->get(t.get(i)))
          x->set(t.get(i),getA(k)->get(t.get(i))-1);
		i++;
	  }
      if(qualify &&!BAbove(*x,Bb(),Be())){
		delete [] product;
		delete [] size;
		maximize(x);
        return(FALSE);    
	  }
	}
	delete [] product;
	delete [] size;
    return(TRUE); 
  } else {
	for(i=0;i<n;i++)
      x->set(i,getC()->getL(i));
	if(BNum()==0){
	  if(ABelow(*x,Ab(),Ae()))
		return(TRUE);
	  else {
		maximize(x);
		return(FALSE);
	  }
	}
    long int* product=new long int[BNum()+1];
    int *size=new int[BNum()];
	for(i=0;i<BNum();i++)
	  size[i]=n;
	computeProducts(BNum(),size,product);
	Vector t(BNum()); 
	for(j=0;j<product[BNum()];j++){
	  for(i=0;i<n;i++)
        x->set(i,getC()->getL(i));
	  pam(j,&t,BNum(),product);
	  i=0;
	  Boolean qualify=TRUE;
      for(k=Bb();k<=Be();k++){
		if(getB(k)->get(t.get(i))>=getC()->getU(t.get(i))){
		  qualify=FALSE;
		  break;
		}
		if(!qualify)
		  break;
		if(getB(k)->get(t.get(i))+1>x->get(t.get(i)))
          x->set(t.get(i),getB(k)->get(t.get(i))+1);
		i++;
	  }
      if(qualify &&!ABelow(*x,Ab(),Ae())){
		delete [] product;
		delete [] size;
		maximize(x);
        return(FALSE);    
	  }
	}
	delete [] product;
	delete [] size;
    return(TRUE); 
  }
}

void Problem::BaddVector(Vector &x)
// add a new maximal independent element
{
  B->addVector(x); // add to the array of vectors
  TB->insert(B->getNum()-1,*B); // insert in the RB-tree
//  TB->print(*B);printf("\n");
  indexB->addIndex(B->getNum()-1); // insert in index array
  setBe(Be()+1);
  setNumEssB(indexB->getNum()-1,numEssentialUp(x,*getC()));
  heapB->heapInsert(*indexB,indexB->getNum()-1); //insert in heap
//  heapB->print();printf("\n");
}

void Problem::cleanUpA(int begin=-1,int end=-1)
// delete indices marked for deletion
{
  if(begin==-1)
	begin=Ab();
  if(end==-1)
	end=Ae();
  int ptr=begin;
  for(int i=begin;i<=end;i++)
	if(!markedA(i)){
	  indexA->in[ptr].index=indexA->in[i].index;
	  unMarkA(ptr++);
	}
  setAb(ptr-1);
}

void Problem::cleanUpB(int begin=-1,int end=-1)
// delete indices marked for deletion
{
  if(begin==-1)
	begin=Bb();
  if(end==-1)
	end=Be();
  int ptr=begin;
  for(int i=begin;i<=end;i++)
	if(!markedB(i)){
	  indexB->in[ptr].index=indexB->in[i].index;
	  unMarkB(ptr++);
	}
  setBb(ptr-1);
}

void Problem::BMaximal()
// delete all non-maximal elements from the current B 
{
  // mark for deletion
  for(int i=Bb();i<=Be();i++)
    if(!markedB(i))
	  for(int j=i+1;j<=Be();j++)
        if(!markedB(j))
		  if(getB(i)->above(*getB(j)))
		    markB(j);
	      else if(getB(j)->above(*getB(i))){
		    markB(i);
		    break;
		  }
  // then delete
  cleanUpB(Bb(),Be());
}

Boolean Problem::sequential()
// Performs the sequential method for dualization
// Adds the dual of A to B
{
  if(debug==1) {
	  printf("A= "); printA(); printf("\n");
	  fout=fopen(debugFile,"a");
	  fprintf(fout,"A= "); printA(fout); fprintf(fout,"\n");
	  fclose(fout);
  }
  if(ANum()==0) 
    return(TRUE);
  int i,k;
  System B(n);
  Vector x(n);
  for(i=0;i<n;i++)
    if(getA(Ab())->get(i)>getC()->getL(i)){
      x=*(getC()->getU());
      x.set(i,getA(Ab())->get(i)-1);
      B.addVector(x);
      if(debug==1) {
		fout=fopen(debugFile,"a");
		fprintf(fout,"B= "); B.print(fout); fprintf(fout,"\n");
		printf("B= "); B.print(); printf("\n");
		fclose(fout);
	  }
    }
  if(B.getNum()==0)
    return(TRUE); // no indepdendent elements
  if(debug==1) {
	fout=fopen(debugFile,"a");
	fprintf(fout,"B= "); B.print(fout); fprintf(fout,"\n");
	printf("B= "); B.print(); printf("\n");
	fclose(fout);
  }
  for(k=Ab()+1;k<=Ae();k++){
    if(debug==1) {
	  fout=fopen(debugFile,"a");
	  fprintf(fout,"a= "); getA(k)->print(fout); fprintf(fout,"\n");
	  printf("a= "); getA(k)->print(); printf("\n");
	  fclose(fout);
	}
    int q,num;
	num=B.getNum();
    for(q=0;q<num;q++){
	  for(i=0;i<n;i++)
	    if(getA(k)->get(i)>getC()->getL(i)){
	      x=*B.get(q);
	      x.set(i,min((integer)(getA(k)->get(i)-1),B.get(q)->get(i)));
	      B.addVector(x);
	      if(debug==1) {
			  fout=fopen(debugFile,"a");
			  fprintf(fout,"B= "); B.print(fout); fprintf(fout,"\n");
			  printf("B= "); B.print(); printf("\n");
			  fclose(fout);
		  }
	    }
    }
    if(debug==1) {
		fout=fopen(debugFile,"a");
		fprintf(fout,"B= "); B.print(fout); fprintf(fout,"\n");
		printf("B= "); B.print(); printf("\n");
		fclose(fout);
	}
	B.shiftUp(num,0);
	if(debug==1) {
		fout=fopen(debugFile,"a");
		fprintf(fout,"B= "); B.print(fout); fprintf(fout,"\n");
		printf("B= "); B.print(); printf("\n");
		fclose(fout);
	}
    B.maximal();
	if(debug==1) {
		fout=fopen(debugFile,"a");
		fprintf(fout,"B= "); B.print(fout); fprintf(fout,"\n");
		printf("B= "); B.print(); printf("\n");
		fclose(fout);
	}
    //updateTimes(B.getNum());
    //report1(B.getNum());
    if(debug==1) {
		fout=fopen(debugFile,"a");
		fprintf(fout,"B= "); B.print(fout); fprintf(fout,"\n");
		printf("B= "); B.print(); printf("\n");
		fclose(fout);
	}
/*    if(!maxIndep(B,Ab(),k))
      printf("ERROR!!!!\n");
    else 
      printf("OK!!!!\n");
*/
    //if(debug==1) {printf("B= "); B->print(); printf("\n");}
  }
  for(k=0;k<B.getNum();k++){
    if(BNum()<=A->getNum()){
	  if(!BAbove(*B.get(k))){
		//  B.get(k)->print();printf("\n");
		maximize(B.get(k));
		//  B.get(k)->print();printf("\n");
	    BaddVector(*B.get(k));  
		logData(this->B->getNum(),numEssentialUp(*B.get(k),*CO));      
	    report2(this->B->getNum());
	  }
	} else {
      maximize(B.get(k));
	  if(!TB->found(*B.get(k),*this->B)){
	    BaddVector(*B.get(k));  
	    logData(this->B->getNum(),numEssentialUp(*B.get(k),*CO));      
	    report2(this->B->getNum());    
	  }
	}
  }
  return(TRUE);
}

/*
Boolean Problem::sequentialB()
// Performs the sequential method for dualization
// multiplies B, to get a new dual elements and 
// adds them to B
{
  if(debug==1) {
	  printf("B= "); B->print(); printf("\n");
	  fout=fopen(debugFile,"a");
	  fprintf(fout,"B= "); B->print(fout); fprintf(fout,"\n");
	  fclose(fout);
  }
  if(BNum()==0) 
    return(TRUE);
  int i,k;
  System A(n);
  Vector x(n);
  for(i=0;i<n;i++)
    if(getB(Bb())->get(i)>getC()->getU(i)){
      x=*(getC()->getU());
      x.set(i,getA(Bb())->get(i)-1);
      A.addVector(x);
      if(debug==1) {
		fout=fopen(debugFile,"a");
		fprintf(fout,"A= "); A.print(fout); fprintf(fout,"\n");
		printf("A= "); A.print(); printf("\n");
		fclose(fout);
	  }
    }
  if(A.getNum()==0)
    return(TRUE); // no indepdendent elements
  if(debug==1) {
	fout=fopen(debugFile,"a");
	fprintf(fout,"A= "); A.print(fout); fprintf(fout,"\n");
	printf("A= "); A.print(); printf("\n");
	fclose(fout);
  }
  for(k=Bb()+1;k<=Be();k++){
    if(debug==1) {
	  fout=fopen(debugFile,"a");
	  fprintf(fout,"b= "); getB(k)->print(fout); fprintf(fout,"\n");
	  printf("b= "); getB(k)->print(); printf("\n");
	  fclose(fout);
	}
    int q,num;
	num=A.getNum();
    for(q=0;q<num;q++){
	  for(i=0;i<n;i++)
	    if(getB(k)->get(i)>getC()->getU(i)){
	      x=*A.get(q);
	      x.set(i,max((integer)(getB(k)->get(i)-1),A.get(q)->get(i)));
	      A.addVector(x);
	      if(debug==1) {
			  fout=fopen(debugFile,"a");
			  fprintf(fout,"A= "); A.print(fout); fprintf(fout,"\n");
			  printf("A= "); A.print(); printf("\n");
			  fclose(fout);
		  }
	    }
    }
    if(debug==1) {
		fout=fopen(debugFile,"a");
		fprintf(fout,"A= "); A.print(fout); fprintf(fout,"\n");
		printf("A= "); A.print(); printf("\n");
		fclose(fout);
	}
	A.shiftUp(num,0);
	if(debug==1) {
		fout=fopen(debugFile,"a");
		fprintf(fout,"A= "); A.print(fout); fprintf(fout,"\n");
		printf("A= "); A.print(); printf("\n");
		fclose(fout);
	}
    A.minimal();
	if(debug==1) {
		fout=fopen(debugFile,"a");
		fprintf(fout,"A= "); A.print(fout); fprintf(fout,"\n");
		printf("A= "); A.print(); printf("\n");
		fclose(fout);
	}
    if(debug==1) {
		fout=fopen(debugFile,"a");
		fprintf(fout,"B= "); B.print(fout); fprintf(fout,"\n");
		printf("B= "); B.print(); printf("\n");
		fclose(fout);
	}
    if(!minIndep(A,Bb(),k))
      printf("ERROR!!!!\n");
    else 
      printf("OK!!!!\n");
 
   //if(debug==1) {printf("A= "); A->print(); printf("\n");}
  }
  for(k=0;k<A.getNum();k++)
    if (!ABelow(*A.get(k))){
      maximize(*A.get(k));
	  if (!TB->found(*A.get(k),*(this->B))){
	  BaddVector(*A.get(k));  
	  logData(B->getNum(),numEssentialUp(*A.get(k),*CO));      
      //report1_2(B->getNum());
	  report2(B->getNum());
	}
  return(TRUE);
}
*/

void Problem::shift(int num=1)
// Perform shift down and then up and generate the resulting maximal independent elements
// goes up at random for num times
// assumes that A is an antichain
{
  int k;
  for(k=Ab();k<=Ae();k++)
    for(int i=0;i<n;i++)
      if(getA(k)->get(i)>getC()->getL(i)){
		for(int j=0;j<num;j++){
          Vector x(n);
	      x=*getA(k);
	      x.dec(i);
          maximize(&x,num>1);
	      if(!TB->found(x,*B)){
	        BaddVector(x);
	        logData(B->getNum(),numEssentialUp(x,*getC()));
            report1(B->getNum());
	        //printf("By Shift-B.num=%i",B->getNum());printf("\n"); 
			//x.print();printf("\n");
	        if(B->getNum()>=maxNum)
		      return;
		  }
	      else{
    		numColl++;
	      }
		}
	  }
  report4(B->getNum());
  if(!A->maxIndep(*B,*C))
    printf("ERROR!!!!\n");
}

void Problem::adjustEssA(int i,integer x,integer L)
{
  for(int k=Ab();k<=Ae();k++)
	if((getA(k)->get(i)<=x)&&(getA(k)->get(i)>L))
	  setNumEssA(k,numEssA(k)-1);  
}


void Problem::adjustEssB(int i,integer x,integer U)
{
  for(int k=Bb();k<=Be();k++)
	if((getB(k)->get(i)>=x)&&(getB(k)->get(i)<U))
	  setNumEssB(k,numEssB(k)-1);  
}

void Problem::cleanUp(int which)
// create a new set of vectors that intersect the current box
// which=1 if cleanUp A and 2 if cleanUp B
// also create a new cleaned up heap
{
  int k,index;
  int num=0;
  if(which==1){
	for(k=delA->getNum()-1;k>=Ad();k--){
	  index=delA->deleteIndex();
	  if(index<0){
	    unMarkA(-index-1);
	    heapA->heapInsert(*indexA,-index-1);
	  } else 
	    unMarkA(index-1);
	}
	setAd(-1); //to indicate that a new heap is created at this point
    for(k=Ab();k<=Ae();k++){
	  //getA(k)->print();printf("%i \n",k);
	  //C->print();printf("\n");
	  if(!markedA(k)&&(getA(k)->intersect(*getC(),0))){
        indexA->addIndex(indexA->getIndex(k));
		num++;
	  }
	}
	setAb(Ae()+1);
    setAe(Ab()+num-1);
	heapA->init();
	computeEssentialsDown();
    for(k=Ab();k<=Ae();k++)
	  heapA->insert(k);
	heapA->adjustHeap(*indexA);
  }
  else {// which=2
	for(k=delB->getNum()-1;k>=Bd();k--){
	  index=delB->deleteIndex();
	  if(index<0){
	    unMarkB(-index-1);
	    heapB->heapInsert(*indexB,-index-1);
	  } else 
	    unMarkB(index-1);
	}
	setBd(-1); //to indicate that a new heap is created at this point
    for(k=Bb();k<=Be();k++)
	  if(!markedB(k)&&(getB(k)->intersect(*getC(),1))){
        indexB->addIndex(indexB->getIndex(k));
		num++;
	  }
    setBb(Be()+1);
    setBe(Bb()+num-1);
	heapB->init();
	computeEssentialsUp();
    for(k=Bb();k<=Be();k++)
	  heapB->insert(k);
	heapB->adjustHeap(*indexB);
  }
}

void Problem::initialize()
// initialize stack other data structure before generation
{
  int k;
  for(k=0;k<A->getNum();k++){
	indexA->addIndex(k);
	heapA->insert(k);
  }
  if(B->getSize()==0)
	B->init(n,MAXALLOC);
  for(k=0;k<B->getNum();k++){
	indexB->addIndex(k);
	heapB->insert(k);
	TB->insert(k,*B);
  }
  S->push(0,0,0,0,0,A->getNum()-1,0,B->getNum()-1,0,0);
  computeEssentialsDown();
  computeEssentialsUp();
  if(debug==1){
	print();printf("\n");
	fout=fopen(debugFile,"a");
    print(fout);fprintf(fout,"\n");
	fclose(fout);
  }
  heapA->adjustHeap(*indexA);
  heapB->adjustHeap(*indexB);
  setIndexToNew(Be()+1);
//  heapA->print(*indexA);printf("\n");
//  heapB->print(*indexB);printf("\n");
}

void Problem::initialize2()
// initialize stack other data structure before generation
{
  int k;
  if(A->getSize()==0)
	A->init(n,MAXALLOC);
  for(k=0;k<A->getNum();k++){
	indexA->addIndex(k);
	heapA->insert(k);
  }
  if(B->getSize()==0)
	B->init(n,MAXALLOC);
  for(k=0;k<B->getNum();k++){
	indexB->addIndex(k);
	heapB->insert(k);
	TB->insert(k,*B);
  }
  S->push(0,0,0,0,0,A->getNum()-1,0,B->getNum()-1,0,0);
  computeEssentialsDown();
  computeEssentialsUp();
  if(debug==1){
	print();printf("\n");
	fout=fopen(debugFile,"a");
    print(fout);fprintf(fout,"\n");
	fclose(fout);
  }
  heapA->adjustHeap(*indexA);
  heapB->adjustHeap(*indexB);
  setIndexToNew(Be()+1);
//  heapA->print(*indexA);printf("\n");
//  heapB->print(*indexB);printf("\n");
}

void Problem::reInitialize()
// reInitialize stack other data structure before generation
{
  int k;
  S->empty();
  *C=*CO;
  indexA->setNum(A->getNum());
  delA->setNum(0);
  indexB->setNum(0);
  delB->setNum(0);
  heapB->init();
  for(k=0;k<B->getNum();k++){
	indexB->addIndex(k);
	heapB->insert(k);
  }
  S->push(0,0,0,0,0,A->getNum()-1,0,B->getNum()-1,0,0);
  for(k=Ab();k<=Ae();k++)
	unMarkA(k);
  for(k=Bb();k<=Be();k++)
	unMarkB(k);
  computeEssentialsUp();
  computeEssentialsDown();
  if(debug==1){
	print();printf("\n");
	fout=fopen(debugFile,"a");
    print(fout);fprintf(fout,"\n");
	fclose(fout);
  }
  heapB->adjustHeap(*indexB);
  setIndexToNew(Be()+1);
}

void Problem::cleanNode()
// performs clean up after a recursion tree node is completed
{
  int i,index,oldBd,oldAd,oldIndexToNew;
  oldAd=Ad();
  oldBd=Bd();
  oldIndexToNew=getIndexToNew();
  S->pop();
  if(S->isEmpty())
    return;
  C->setL(geti(),getL());
  C->setU(geti(),getU());
  indexA->setNum(Ae()+1);
  indexB->shiftUp(oldIndexToNew,Be()+1);
  setBe(indexB->getNum()-1);
  if(oldAd==-1){// a new A heap was created
	heapA->init();
    for(i=Ab();i<=Ae();i++)
	  if(!markedA(i))
        heapA->insert(i);
	heapA->adjustHeap(*indexA);
  } else {
	for(i=delA->getNum()-1;i>=oldAd;i--){
	  index=delA->deleteIndex();
	  if(index<0){
	    unMarkA(-index-1);
	    heapA->heapInsert(*indexA,-index-1);
	  } else 
	  unMarkA(index-1);
	}
  }
  if(oldBd==-1){// a new B heap was created
	heapB->init();
    for(i=Bb();i<=Be();i++)
	  if(!markedB(i))
        heapB->insert(i);
	heapB->adjustHeap(*indexB);
  } else {
	for(i=delB->getNum()-1;i>=oldBd;i--){
	  index=delB->deleteIndex();
	  if(index<0){
	    unMarkB(-index-1);
	    heapB->heapInsert(*indexB,-index-1);
	  } else 
 	    unMarkB(index-1);
	}
  }
}

Boolean Problem::dualA3(Vector *x)
// performs NON-recursive dualization-Algorithm A
// starts from the state determined by stack
// returns TRUE if A, B are dual in C
// returns FALSE and a non-covered vector x, otherwise
// Assumes the necessary duality condition holds: A^+ \cap B^-=\emptyset
{
/*  if(debug==1) {
	  fout=fopen(debugFile,"a");
	  fprintf(fout,"Subproblem:\n" ); print(fout); fprintf(fout,"\n");
	  printf("Subproblem:\n" ); print(); printf("\n");
	  fclose(fout);
  }*/
  if(A->getNum()*B->getNum()<=1){ 
    numLeaves++;
    return(dual0(x));
  }
  int i,which;
  integer y;
  int rs;
  if((rs=randSplit(x,numSamples1,numSamples2,&i,&y,&which))==0){
    return(FALSE);
  } else if(rs==2){
	cleanNode();
  } else if(rs==3){
	cleanUp(1);
    if(!dual0(x)){
      return(FALSE);
	} else {
	  cleanNode();
	}
  } else if(rs==4){
	  cleanUp(2);
      if(!dual0(x)){
      return(FALSE);
	} else {
	  cleanNode();
	} 
  } else if(!S->isEmpty()){
    seti(i);
    setx(y);
  }
  while(!S->isEmpty()){ // assume stack initially has one node
//	if(debug==2)
//	  printf("Depth= %i Nodes=%i BNum=%i\n",S->num,nodesVisited,indexB->getNum());
	if(nodesVisited%2000==1000) report5(S->num);
    if(debug==1) {
	  fout=fopen(debugFile,"a");
	  S->printTop();
	  S->printTop(fout);
	  fclose(fout);
	}    
	if(debug==1) S->print2(n);
    if(seq()==2){
      cleanNode();
    } else {
      // Form left/right subproblem
      numNodes++;
	  setL(C->getL(geti()));
	  setU(C->getU(geti()));
      if(seq()==0){
        C->setU(geti(),getx()-1);
        if(debug==1) {
			fout=fopen(debugFile,"a");
			fprintf(fout,"Left sub:\n" );
			printf("Left sub:\n" ); 
			fclose(fout);
		}    
      } else { // if(s->top->seq==1)
        C->setL(geti(),getx());	
        if(debug==1) {
			fout=fopen(debugFile,"a");
			fprintf(fout,"Right sub:\n" ); 
			printf("Right sub:\n" );
			fclose(fout);
		}
      }
	  setSeq(seq()+1);             
	  S->push(0,0,0,0,Ab(),Ae(),Bb(),Be(),0,0);
	  nodesVisited++;
      //P->eliminate(1,&s->top->i);
	  if(estSizeA(numSamples2)<=cleanUpFactor*ANum())
		cleanUp(1);
	  if(estSizeB(numSamples2)<=cleanUpFactor*BNum())
		cleanUp(2);
	  setIndexToNew(Be()+1);
	  if(debug==1) {
		fout=fopen(debugFile,"a");
		print(fout); fprintf(fout,"\n");
    	print(); printf("\n");
	 	printf("Heap A: ");heapA->print();printf("\n");
		C->print();printf("\n");
		fclose(fout);
	  }
      if(ANum()*BNum()<=1) {
    	numLeaves++;
	    if(!dual0(x)){
          return(FALSE);
		} else {
          cleanNode(); 
          continue;
		}
      }
      if((rs=randSplit(x,numSamples1,numSamples2,&i,&y,&which))==0){
        return(FALSE);
	  } else if(rs==1){
		seti(i);
        setx(y);
      } else if(rs==2){//dual
	    cleanNode();
	  } else if(rs==3){
		cleanUp(1);
        if(!dual0(x)){
          return(FALSE);
		} else {
		  cleanNode();
		}
	  } else if(rs==4){
        cleanUp(2);
        if(!dual0(x)){
          return(FALSE);
		} else {
		  cleanNode();
		}
	  }

	}
  }
  return(TRUE);
}

Boolean Problem::dualA3_multiply(Vector *x,int v=1)
// performs NON-recursive dualization-Algorithm A
// performs multiplication at the lowest levels 
// starts from the state determined by stack
// returns TRUE if A, B are dual in C
// returns FALSE and a non-covered vector x, otherwise
// Assumes the necessary duality condition holds: A^+ \cap B^-=\emptyset
{
/*  if(debug==1) {
	  fout=fopen(debugFile,"a");
	  fprintf(fout,"Subproblem:\n" ); print(fout); fprintf(fout,"\n");
	  printf("Subproblem:\n" ); print(); printf("\n");
	  fclose(fout);
  }*/
  if(A->getNum()<=v){ 
    numLeaves++;
    return(sequential());
  }
  if(A->getNum()*B->getNum()<=1){ 
    numLeaves++;
    return(dual0(x));
  }
  int i,which;
  integer y;
  int rs;
  if((rs=randSplit(x,numSamples1,numSamples2,&i,&y,&which))==0){
    return(FALSE);
  } else if(rs==2){
	cleanNode();
  } else if(rs==3){
    if(!dual0(x)){
      return(FALSE);
	} else {
	  cleanNode();
	} 
  } else if(!S->isEmpty()){
    seti(i);
    setx(y);
  }
  while(!S->isEmpty()){ // assume stack initially has one node
//	if(debug==2)
//	  printf("Depth= %i Nodes=%i BNum=%i\n",S->num,nodesVisited,indexB->getNum());
	if(nodesVisited%2000==1000) report5(S->num);
    if(debug==1) {
	  fout=fopen(debugFile,"a");
	  S->printTop();
	  S->printTop(fout);
	  fclose(fout);
	}    
	if(debug==1) S->print2(n);
    if(seq()==2){
      cleanNode();
    } else {
      // Form left/right subproblem
      numNodes++;
	  setL(C->getL(geti()));
	  setU(C->getU(geti()));
      if(seq()==0){
        C->setU(geti(),getx()-1);
        if(debug==1) {
			fout=fopen(debugFile,"a");
			fprintf(fout,"Left sub:\n" );
			printf("Left sub:\n" ); 
			fclose(fout);
		}    
      } else { // if(s->top->seq==1)
        C->setL(geti(),getx());	
        if(debug==1) {
			fout=fopen(debugFile,"a");
			fprintf(fout,"Right sub:\n" ); 
			printf("Right sub:\n" );
			fclose(fout);
		}
      }
	  setSeq(seq()+1);             
	  S->push(0,0,0,0,Ab(),Ae(),Bb(),Be(),0,0);
	  nodesVisited++;
      //P->eliminate(1,&s->top->i);
	  if(estSizeA(numSamples2)<=cleanUpFactor*ANum())
		cleanUp(1);
	  if(estSizeB(numSamples2)<=cleanUpFactor*BNum())
		cleanUp(2);
	  setIndexToNew(Be()+1);
	  if(debug==1) {
		fout=fopen(debugFile,"a");
		print(fout); fprintf(fout,"\n");
    	print(); printf("\n");
	 	printf("Heap A: ");heapA->print();printf("\n");
		C->print();printf("\n");
		fclose(fout);
	  }
      if(ANum()*BNum()<=1) {
    	numLeaves++;
	    if(!dual0(x)){
          return(FALSE);
		} else {
          cleanNode(); 
          continue;
		}
      }
	  if(ANum()<=v) {
    	numLeaves++;
	    sequential();
        cleanNode(); 
        continue;
	  }
      if((rs=randSplit(x,numSamples1,numSamples2,&i,&y,&which))==0){
        return(FALSE);
	  } else if(rs==1){
		seti(i);
        setx(y);
      } else if(rs==2){//dual
	    cleanNode();
	  } else if(rs==3){
        if(!dual0(x)){
          return(FALSE);
		} else {
		  cleanNode();
		}
	  }
	}
  }
  return(TRUE);
}

Boolean Problem::intersect()
// check if the duality condition does not hold
// return TRUE if A^+\cap B^- \neq \emptyset
{
  for(int k=0;k<A->getNum();k++)
	if(B->above(*A->get(k)))
	  return(TRUE);
  return(FALSE);
}

Boolean Problem::check(Vector *x)
// exhaustively check if A,B are dual in C
// return a vector x \in C\setminus(A^+\cup B^-) if not
{
  long int numA=A->getNum();
  long int numB=B->getNum();
  A->minimal();
  B->maximal();
  if((A->getNum()!=numA)||(B->getNum()!=numB)||intersect())
	return(FALSE);
  int *size=new int[n];
  for(int i=0;i<n;i++)
	size[i]=C->getU(i)-C->getL(i)+1;
  long int* product=new long int[n+1];
  computeProducts(n,size,product);
  for(long int j=0;j<product[n];j++){
    pam(j,x,n,product);
	if(!A->below(*x)&&!B->above(*x)){
	  delete [] size;
      delete [] product;
      return(FALSE);
	}
  }
  delete [] size;
  delete [] product;
  return(TRUE);
}

Boolean Problem::dualize2A(int withShift=1)
// Performs the incremental dualization
// works on the same tree multiple times 
// Assuming B is empty, returns the dual of A in B
{
  Vector x(n);
  if(withShift) 
    shift(withShift);
  numWithShift=B->getNum()-orig;
  fout=fopen(fileName,"a");
  fprintf(fout,"By Shift=%i\n",numWithShift);
  fclose(fout);
  Boolean d=FALSE;
  int tot=0;
  threshold=1000;
  nodesVisited=0;
  for(int i=B->getNum();i<maxNum;i++){
    init();
	debug=0;
    d=dualA3_multiply(&x,numMultp);
	//d=dualA3(&x);
	//d=sequential();
	nodeDepth=S->getNum();
    if(d){
      logData(B->getNum(),0);
      //report1_2(B->getNum());
	  report2(B->getNum());
      break;
    }else{
//     if(!newMaxIndep(x)){
//	    printf("ERROR!!!!\n");
//	  }
	  if(debug==1){
		  printf("%i : Solution =",i);x.print(); printf("\n");
		  fout=fopen(debugFile,"a");
		  fprintf(fout,"%i : Solution =",i);x.print(fout); fprintf(fout,"\n");
		  fclose(fout);
	  }
      BaddVector(x);
	  logData(B->getNum(),numEssentialUp(x,*CO));      
      //report1_2(B->getNum());
	  report2(B->getNum());
	  //reInitialize();
      tot++;
/*      if((tot>threshold)&&(interTimeAvg>Tthreshold)){
        tot=0;
	    threshold=B->getNum()*4;
		Tthreshold=Tthreshold*4;
        s.empty();
		fprint(fileName,"Rebuilding tree .....\n");
      }*/
      if(debug==1){
		printf("A: "); A->print(); printf("\n"); 
    	printf("B: "); B->print(); printf("\n"); 
		fout=fopen(debugFile,"a");
	    fprintf(fout,"A: "); A->print(fout); fprintf(fout,"\n"); 
    	fprintf(fout,"B: "); B->print(fout); fprintf(fout,"\n"); 
		fclose(fout);
      }
    } 
  }
/*  if(!check(&x))
	  printf("ERRRRRRRROR!!!!!\n");
  else
	  printf("OKKKKKKKKKKKKK!!!!\n");*/
  return(d);
}