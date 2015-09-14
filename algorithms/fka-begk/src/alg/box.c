class Vector;

Box::Box(int n,integer c)
// create a new box with dimesion n
{
  this->n=n;
  this->L=new Vector(n);
  this->U=new Vector(n);
  if(c!=-1)
	for(int i=0;i<n;i++){
	  L->set(i,0);
	  U->set(i,c);
	}
}

Box::Box(int n,Vector &L,Vector &U)
// create a new box with dimesion n, whose corners are the integer vectors L,U 
{
  this->n=n;
  this->L=new Vector(n);
  this->U=new Vector(n);
  *(this->L)=L;
  *(this->U)=U;
}

Box::~Box()
{
  delete L;
  delete U;
}

int Box::getDim()
{
  return n;
}

Box& Box::operator=(Box& b)
{
  this->n=b.n;
  *(this->L)=*(b.L);
  *(this->U)=*(b.U);
  return(*this);
}

void Box::print()
{
  for(int i=0;i<n-1;i++)
    printf("[%i:%i]  x ",L->get(i),U->get(i));
  printf("[%i:%i]",L->get(n-1),U->get(n-1));
}

void Box::print(FILE *fout)
{
  for(int i=0;i<n-1;i++)
    fprintf(fout,"[%i:%i]  x ",L->get(i),U->get(i));
  fprintf(fout,"[%i:%i]",L->get(n-1),U->get(n-1));
}

