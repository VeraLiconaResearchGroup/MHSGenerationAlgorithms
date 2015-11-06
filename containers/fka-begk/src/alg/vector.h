class Stack;
class Box;

class Vector // intergral vector
{
  private:
    int n; //dimension
	integer *c; // integral components
	Boolean mark; // mark for deletions
	Boolean maxMark; // mark for maximization
  public:
    Vector(int n,integer *c);
	int getDim(){return n;}
    Vector(int n);
	Vector();
	void init(int n);
    ~Vector();
	void set(int i,integer v) {this->c[i]=v;}
    integer get(int i){return this->c[i];}
	Boolean marked(){return mark;}
	void setMark(Boolean m){mark=m;}
	Boolean maxMarked(){return maxMark;}
	void setMaxMark(Boolean m){maxMark=m;}
	void inc(int i,int d=1){c[i]+=d;}
	void dec(int i,int d=1){c[i]-=d;}
    Vector& operator=(Vector& v);
	Vector operator=(integer *c);
	Boolean operator==(Vector& v);
    friend std::ostream& operator<<(std::ostream& os,Vector &v);
	friend std::ostream& operator<<(std::ostream& os,Box &b);
	void print();
	void print(FILE *fout);
	int lexComp(Vector &x);
    Vector operator+(Vector &v);
	Vector meet(Vector &v);
	Vector join(Vector &v);
	Boolean above(Vector &x);
    Boolean below(Vector &x);
	Boolean intersect(Box &C,int dir);
};

class System // systems of integral vectors
{
  private:
    int n; //dimension
	int max; // highest occupied index
	int num; // num of vectors
	int size; // size allocated
	Vector *va; // array of vectors
  public:
	System(int n,int size=MAXALLOC);
	System();
	void init(int n,int size);
	int getDim(){return n;}
	int getNum(){return num;}
	int getSize(){return size;}
    void setDim(int n){ this->n=n;}
	void addVector(Vector &v);
	void deleteVector(int i);
    ~System();
    Boolean isEmpty(){return(num==0);}
	void cleanUp();
    void print();
	void print(FILE *fout);
	Boolean read(char *inFile,int *n,integer *c,int *s);
	void sort(int dir);
	void fprint(char *outFile,int n,integer c,int s);
	Boolean found(Vector& x);
	Boolean below(Vector &x,int begin,int end);
	Boolean above(Vector &x,int begin,int end);
	Boolean below(System &A,int begin,int end);
	Boolean above(System &A,int begin,int end);
	Boolean equivalent(System &A);
	void complement(Box &C);
    void maximal();
	void minimal();
	Boolean maxIndep(Vector &x,Box &C,int begin,int end);
	Boolean maxIndep(System &A,Box &C,int begin,int end);
	void shiftUp(int b,int start);
	Vector *get(int i){return &va[i];}
};

class IndexNode {
  private:
    int index;
	short mark,numEss;
  public:
	IndexNode& operator=(IndexNode& x);
  friend class IndexArray;
  friend class Problem;
};

class IndexArray // array of indices
{
  private:
    IndexNode *in;
	int num;
	int size;
  public:
	IndexArray(int size=MAXALLOC);
	~IndexArray();
	void addIndex(int i);
	int deleteIndex();
	void shiftUp(int b,int start);
	void mark(int i){in[i].mark=1;}
    Boolean marked(int i){return in[i].mark;}
    int getIndex(int i){return in[i].index;}
	int getNumEss(int i){return in[i].numEss;}
	void setNumEss(int i,int numEss){in[i].numEss=numEss;}
	int getNum(){return num;}
	void setNum(int n){num=n;}
	void print();
	void print(FILE *fout);
  friend class Problem;
};

class DeleteArray // array of deleted indices
{
  private:
    int *in;
	int num;
	int size;
  public:
	DeleteArray(int size=MAXALLOC);
	~DeleteArray();
	void addIndex(int i);
	int deleteIndex();
    int getIndex(int i){return in[i];}
	void setNum(int n){num=n;}
	int getNum(){return num;}
	void print();
	void print(FILE *fout);
  friend class Problem;
};

class Problem // Dualization subproblem
{
  private:
	int n; // dimesnion
	System *A,*B;
    Box *C,*CO;
	RBTree *TA,*TB; // Red-Black tree for storing Systems A,B
	Stack *S; // stack for recursion
    IndexArray *indexA,*indexB; // arrays of indices used in recursion
    DeleteArray *delA,*delB; // arrays of deleted indices used in recursion
	Heap *heapA,*heapB;
  public:
    Problem(int n,integer c);
    Problem(int n,Box *C,System *A,System *B,RBTree *T,Stack *S,IndexArray *indexA,IndexArray *indexB,Heap *heapA,Heap *heapB,DeleteArray *delA,DeleteArray *delB);
	Problem(int n,Box *C,System *A,System *B,RBTree *TB,RBTree *TA);
    Problem(int n,Box *C,System *A,System *B);
    ~Problem();
    void printABC();
    void printABC(FILE *fout);
    void printA();
    void printA(FILE *fout);
    void printB();
    void printB(FILE *fout);
    void print();
    void print(FILE *fout);
    int numEssentialDown(Vector &x,Box &C);
    int numEssentialUp(Vector &x,Box &C);
    void computeEssentialsDown();
    void computeEssentialsUp();
    double expectation();
    double expectation(Vector &z,int i);
    void solution(Vector *v,int num);
    Boolean findVector(int *i,Boolean *isA);
    int countA(int x,int i);
    int countA(int x,int i,Box &C);
    int countB(int x,int i);
    int countB(int x,int i,Box &C);
    int estCountA(int k,int x,int i,int *num);
    int estCountB(int k,int x,int i,int *num);
    int estSizeA(int k);
    int estSizeB(int k);
    int Ab(){return S->top->ab;}
    void setAb(int ab){S->top->ab=ab;}
    int Ae(){return S->top->ae;}
    void setAe(int ae){S->top->ae=ae;}
    int Bb(){return S->top->bb;}
    void setBb(int bb){S->top->bb=bb;}
    int Be(){return S->top->be;}
    void setBe(int be){S->top->be=be;}
    int Ad(){return S->top->ad;}
    void setAd(int ad){S->top->ad=ad;}
    int Bd(){return S->top->bd;}
    void setBd(int bd){S->top->bd=bd;}
    int ANum(){return S->top->ae-S->top->ab+1;}
    int BNum(){return S->top->be-S->top->bb+1;}
    int seq(){return S->top->seq;}
    void setSeq(int seq){S->top->seq=seq;}
    int rule(){return S->top->rule;}
    void setRule(int rule){S->top->rule=rule;}
    int geti(){return S->top->i;}
    void seti(int i){S->top->i=i;}
    int getx(){return S->top->x;}
    void setx(int x){S->top->x=x;}
    int getL(){return S->top->L;}
    void setL(int L){S->top->L=L;}
    int getU(){return S->top->U;}
    void setU(int U){S->top->U=U;}
	int getIndexToNew(){return S->top->indexToNew;}
    void setIndexToNew(int i){S->top->indexToNew=i;}
	int numEssA(int index){return indexA->getNumEss(index);}
    int numEssB(int index){return indexB->getNumEss(index);}
    int minNumEss();
    void setNumEssA(int index,int numEss){indexA->setNumEss(index,numEss);}
    void setNumEssB(int index,int numEss){indexB->setNumEss(index,numEss);}
    Boolean markedA(int i){return indexA->in[i].mark;}
    Boolean markedB(int i){return indexB->in[i].mark;}
    void markA(int i){indexA->in[i].mark=1;}
    void markB(int i){indexB->in[i].mark=1;}
    void unMarkA(int i){indexA->in[i].mark=0;}
    void unMarkB(int i){indexB->in[i].mark=0;}
    Vector *getA(int i) {return A->get(indexA->in[i].index);}
    Vector *getB(int i) {return B->get(indexB->in[i].index);}
    int getIndexA(int i) {return indexA->in[i].index;}
    int getIndexB(int i) {return indexB->in[i].index;}
    Box *getC(){return C;}
    Boolean ABelow(Vector &x,int begin,int end);
    Boolean BAbove(Vector &x,int begin,int end);
    Boolean maxIndep(Vector &x,int begin,int end);
    Boolean maxIndep(System &B,int begin,int end);
    Boolean sample(int k,Vector *v,int *numTrials,Boolean *isMax);
    Boolean maxIndep(int beginA,int endA,int beginB,int endB);
    Boolean split(Vector *v,int *j,integer *y);
    int randSplit(Vector *v,int k1,int k2,int *j,integer *y,int *cleanUp);
    void BaddVector(Vector &x);
    void eliminate(int num,int *index,int which);
    void project();
    void adjustEssA(int i,integer x,integer L);
    void adjustEssB(int i,integer x,integer U);
    void maximize(Vector *x,Boolean random=FALSE);
    Boolean maximize(Vector *x,Boolean random,int *mark,int ab,int ae,Box &C);
	void maximize2(Vector *x,Boolean random=TRUE);
    Boolean newMaxIndep(Vector &v);
    Boolean dual0(Vector *x);
    void cleanUp(int which);
    void initialize();
	void initialize2();
    void reInitialize();
    void cleanNode();
    Boolean dualA3(Vector *x);
	Boolean dualA3_multiply(Vector *x,int v);
    void cleanUpA(int begin,int end);
    void cleanUpB(int begin,int end);
    void BMaximal();
    Boolean sequential();
    void shift(int num);
    Boolean dualize2A(int withShift);
    Boolean check(Vector *x);
    Boolean intersect();
  friend void logData(long num,int numEss);
};
