class Box 
{
  private: 
    int n; // dimension
    Vector *L,*U; // Lower and Upper corners
  public:
    Box(int n,Vector &L,Vector &U);
	Box(int n,integer c=-1);
    ~Box();
    void setL(int i,integer l){L->set(i,l);}
    void setU(int i,integer u){U->set(i,u);}
    integer getL(int i){return(L->get(i));}
    integer getU(int i){return(U->get(i));}
	Vector *getL(){return L;}
	Vector *getU(){return U;}
    int getDim(); // returns the dimesion of the box
    Box& operator=(Box& b);
	void print();
    void print(FILE *fout);
};

