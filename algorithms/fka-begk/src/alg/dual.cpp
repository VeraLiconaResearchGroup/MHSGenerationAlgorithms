// read primal/dual form file and dualize

int n,m;

# include <stdio.h>
# include <stdlib.h>
# include <iostream>
# include <math.h>
# include <string.h>
# include <time.h>
# include "basic.h"
# include "stack.h"
# include "vector.h"
# include "box.h"
# include "stack.c"
# include "vector.c"
# include "box.c"
# include "basic.c"

int main(int argc,char *argv[])
{
  int i,n;
  srand(time(0));
  integer c=1;//assume hypergraphs;
  Box *b;
  System *A,*B;
  Problem *P;
  Boolean d;
  maxNum=30000;
  outputPeriod=1000;
  int withShift,whichAlgorithm;
  withShift=0;
  toOutput=1;
  Tthreshold=0.5;
  toLog=TRUE;
  debug=FALSE;
  whichAlgorithm=0;
  n=10;
  char primalName[20],dualName[20],pFile[20];
  if(argc==1){
    std::cout<<"Primal File Name:------->";
    std::cin>>primalName;
	std::cout<<"Dual File Name:------->";
    std::cin>>dualName;
  } else {
    strcpy(primalName,argv[1]);
    strcpy(dualName,argv[2]);
  }
  strcpy(fileName,primalName);
  int sP,sD; // sort  0=lex  1=rev lex   2=no sort
  if(outputPeriod==0)
	toLog=FALSE;
  strcpy(logFile,fileName);
  strcat(logFile,".log");
  strcpy(debugFile,fileName);
  strcat(debugFile,".debug");
  strcat(fileName,".out");
  fout=fopen(fileName,"w");
  fclose(fout);
  fout=fopen(logFile,"w");
  fclose(fout);
  fout=fopen(debugFile,"w");
  fclose(fout);
//  int k=15;
//  n=2*k;
 A=new System();
//  genMatchingDual(A,k);
  A->read(primalName,&n,&c,&sP);
  A->sort(sP);
  n=A->getDim();
  m=A->getNum();
  //A->print();printf("\n");
 //A->fprint(primalName,n,c,sP); exit(0);
    //A->print();printf("\n");
  /*RBTree *T=new RBTree();
//  T->print();printf("\n");
  for(int ii=0;ii<A->getNum();ii++){
    T->insert(ii,*A);
    T->print(*A);printf("\n");
  }
  printf("height=%i\n",T->height());
  for(ii=0;ii<A->getNum();ii++)
    printf("%i\n",T->found(*A->get(ii),*A));
*/
  b=new Box(n);
  for(i=0;i<n;i++){
    b->setL(i,0);
    b->setU(i,c);
  }
  /*genRandom0(*b,A,400);
  A->fprint(primalName,n,c,sP);
  */
  B=new System(n);
  B->init(n);
 /* for(ii=0;ii<A->getNum();ii++)
    B->addVector(*A->get(ii));
  B->complement(*b);
  B->print();
  A->print();
  for(ii=0;ii<A->getNum();ii++)
    printf("%i\n",T->found(*B->get(ii),*A));*/
  B->read(dualName,&n,&c,&sD);
//  B->print();printf("\n");
//  B->sort(sD);
  B->complement(*b);
  if(!A->maxIndep(*B,*b)){
	printf("B is not a partial list of max. indpt. elements of A\n");
	exit(0);
  }
  orig=B->getNum();
  fout=fopen(fileName,"a");
  fprintf(fout,"n=%i\n",n);
  fclose(fout);
  initExp();
  fout=fopen(fileName,"a");
  fprintf(fout,"A.num=%i\n",A->getNum());
  fclose(fout);
  P=new Problem(n,b,A,B);
  if(debug) {
	  fout=fopen(debugFile,"a");
	  fprintf(fout,"A: \n");A->print(fout);fprintf(fout,"\n");
	  fclose(fout);
  }
  if(debug) {
	  fout=fopen(debugFile,"a");
	  fprintf(fout,"B: \n");B->print(fout);fprintf(fout,"\n");
	  fclose(fout);
  }
  setTimer();
  if (whichAlgorithm==0)
    d=P->dualize2A(withShift);
/*  else
    d=P->dualize2B(withShift);
	*/
  report3(B->getNum());
  fout=fopen(fileName,"a");
  fprintf(fout,"By Shift=%i  Total=%i\n",numWithShift,B->getNum());
  if(d)
    fprintf(fout,"Dualization Complete\n");
  fprintf(fout,"---------------------------------------------\n");
  fclose(fout);
  if(toOutput){
    B->complement(*b);
	B->fprint(dualName,n,c,sD);
  }
  delete P;
  return(0);
}
