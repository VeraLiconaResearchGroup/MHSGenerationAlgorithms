# define integer short
# define FALSE 0
# define TRUE 1
# define Boolean int
# define INFNTY_INT 100000000
# define INFNTY_DOUBLE 10E10
# define EPSILON 10E-10
# define MAXALLOC 100000

class Vector;
class Box;
class Problem;
class System;
class IndexArray;

Boolean debug=FALSE;

double numNodes; // num of recursive calls
double numLeaves; // num of subproblems of volume <=1
double clk; // clock
double clockMax;
Boolean clockPos=TRUE;
double interTimeAvg; // Avg. time between two succisevely generated points 
double interTimeAvg2; // Avg. time between two succisevely generated points 
double interTimeMax; // Max. time between two succisevely generated points 
double interTimeMax2; // Max. time between two succisevely generated points 
double interTimeMax22; // Max. time between two succisevely generated points 
double interTimeTot; //sum of Tot times between two succisevely generated points 
double interTimeTot2; //sum of Tot times between two succisevely generated points over the period for output
int numEssMax2,numEssMax22; // Max number of essential coordinates within a period
int nodesVisited; //the number of recursion-tree nodes visited since the last output
int nodeDepth; //the depth of the recursion-tree node at which the current output is produced
int nodesVisitedMax2,nodesVisitedMax22;// the max number of recursion-tree nodes visisted since the last output in a period 
int nodeDepthMax2,nodeDepthMax22; // the max depth in the tree at which the output is generated in a period
int maxNum; // Maximum number of independent elements to be generated
int threshold; // number of independent elements after which tree is renewed
double Tthreshold; // intertime in Sec after which tree is renewed
int numWithShift; // number of independent elements generated with shifts
int numColl;// number of collisions in random shifts
int outputPeriod; // when to output statistics
int orig; // original number of dual points given to the program
char fileName[20];
char logFile[20];
char debugFile[20];
FILE *fout;
Boolean toLog=FALSE;
Boolean toOutput=FALSE;
float cleanUpFactor=0.5; // the factor in [0,1] of the current number of vectors 
                     // after which cleanup is performed  
int numSamples1=1; // number of samples to be taken for finding a random solution 
int numSamples2=30; // number of samples to be taken for count estimation 
int numMultp=10;

int min(int x,int y);
int max(int x,int y);
integer min(integer,integer);
integer max(integer,integer);
double min(double x,double y);
double dabs(double x);
void computeProducts(int num,int* size, long* product);
double map(Vector &v,double *product);
void pam(long int map,Vector *t,int k,long *product);
int uniform(int n1,int n2);
integer uniform(integer n1,integer n2);
void exchange (int *x, int *y);
void init();
void initExp();
void setTimer();
void updateTimes(long num);
void logData(long num,int numEss);
void report1(long num);
void report1_1(long num);
void report1_2(long num);
void report2(long num);
void report2_1(long num);
void report3(long num);
void report4(long num);
void report5(int num);
void fprint(char *fileName,char *s);
double chi(double v);
//int __cdecl lexCompare(const void *xx, const void *yy);
//int __cdecl revLexCompare(const void *xx, const void *yy);
int lexCompare(const void *xx, const void *yy);
int revLexCompare(const void *xx, const void *yy);

typedef struct RBNode *RBptr;	
typedef enum { red, black } Color;

typedef struct RBNode {
  Color colour;
  int index;
  RBptr left,right,parent;
} RB_Node;

class RBTree // Red-Black tree
{
  private:
    RBptr root; // pointer to the root
	void deleteTree(RBptr x);
	void left_rotate(RBptr x);
    void right_rotate(RBptr x);
  public:
	RBTree();
	~RBTree();
    void insert(int i,System &B);
	Boolean found(Vector &v,System &B);
	void print(RBptr x);
	void print(FILE *fout,RBptr x);
	void print();
	void print(FILE *fout);
	void print(RBptr x,System &B);
	void print(FILE *fout,RBptr x,System &B);
	void print(System &B);
	void print(FILE *fout,System &B);
	int height(RBptr x);
	int height();
};

class HNode {
  private:
    int index; // the index of the vector in the vector array 
  public:
    HNode& operator=(HNode &x);
  friend class Heap;
};

class Heap
{
  private:
    int num; // num of elements in the heap;
    HNode *ar; // array of heap nodes
	void swap(HNode *x,HNode *y);
    void heapify(IndexArray &a,int l); // restore the heap property
	void heapifyUp(IndexArray &a,int l); // restore the heap property moving Upward
	int numEss(IndexArray &a,int i);
  public:
	Heap(int size=MAXALLOC);
	~Heap();
	void insert(int index);// insert without maintaining the heap property
	void adjustHeap(IndexArray &a); // restore the heap property
	void heapInsert(IndexArray &a,int index); // insert maintaining the heap property
	void getMin(IndexArray &a,int *index,int *numEss);
	void deleteMin(IndexArray &a,int *index,int *numEss);
	Boolean isEmpty(){return num==0;}
	void init(){num=0;}
	void print();
	void print(FILE *fout);
	void print(IndexArray &a);
	void print(FILE *fout,IndexArray &a);
  friend class Problem;
};
