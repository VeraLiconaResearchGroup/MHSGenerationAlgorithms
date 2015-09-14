class Problem;
class Vector;

typedef struct stackNode *SNptr;

typedef struct stackNode
{
  int i; // split coordinate
  integer x; // split element
  integer L,U; // lower and Upper bounds in the ith coordinate before split
  int seq; // sequentail # indicating  subproblem being considered
  int rule; // a number indicating which decomposition rule is used
  int ab,ae,bb,be; // indices of A-begin, A-end, B-begin and B-end of the current problem
  int ad,bd; // indices of A-begin and B-begin to deleted by the current node
  int indexToNew; // index of newly added elements at the current node
  int next; // pointer to the next vector to be proceesed in some rules
  SNptr below;
} StackNode;

class Stack // the stack to handle non-recursive dualization
{
  private:
    int num; // number of elements currently the stack
  public:
    SNptr top; // a pointer to the top element of the stack
    Stack();
	int getNum(){return num;}
    ~Stack();
    void push(int i,integer x,int seq,int rule,int ab,int ae,int bb,int be,int ad,int bd,int next);
	void push(int i,integer x,int seq);
    Boolean isEmpty();
	SNptr getTop(){return top;}
    void getTop(int *i,integer *x, int *seq,int *rule,int *ab,int *ae,int *bb,int *be,int *ad,int *bd,int *next);
	void getNext(SNptr ptr,int *ab,int *ae,int *index,int *L,int *U);
    void pop(int *i,integer *x, int *seq,int *rule,int *ab,int *ae,int *bb,int *be,int *ad,int *bd,int *next);
    void pop();
    void empty();
	void print();
	void print(FILE *fout);
    void printTop();
	void printTop(FILE *fout);
	void print2(int n);
	void print2(int n,FILE *fout);
    void print2();
  friend class Problem;
};
