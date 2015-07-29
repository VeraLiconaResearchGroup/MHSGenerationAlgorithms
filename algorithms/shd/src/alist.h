/*    array-based list/multi-list library
            12/Apr/2001   by Takeaki Uno  e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, please
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about the codes for the users. */

#ifndef _alist_h_
#define _alist_h_

#include"stdlib2.h"
#include"undo.h"

#ifndef ALIST_ID
 #ifdef ALIST_ID_LONG
  #define ALIST_ID LONG
  #define ALIST_ID_END LONGHUGE
  #define ALIST_IDF LONGF
 #else
  #define ALIST_ID int
  #define ALIST_ID_END INTHUGE
  #define ALIST_IDF "%d"
 #endif
#endif

/* macros for getting head, tail of (multi) list */
#define ALIST_HEAD(A)     ((A).nxt[(A).end])
#define ALIST_TAIL(A)     ((A).prv[(A).end])
#define MALIST_HEAD(A,l)  ((A).nxt[(A).end+l])
#define MALIST_TAIL(A,l)  ((A).prv[(A).end+l])

/*  macro to check if e is an element(or root element) */
#define ALIST_IS_ELM(A,e)  (e<(A).end)
#define MALIST_IS_ELM(A,e)  (e<(A).end)

/* macros for loops */
/* each ***2 works correctly even when we remove the current element from the list */
#define ALIST_DO_FORWARD(A,i)  for((i)=(A).nxt[(A).end];(i)<(A).end;(i)=(A).nxt[(i)])
#define ALIST_DO_FORWARD2(A,i,ii)  for(i=(A).nxt[(A).end],ii=(A).nxt[i];(i)<(A).end;i=ii,ii=(A).nxt[i])
#define ALIST_DO_BACKWARD(A,i) for((i)=(A).prv[(A).end];(i)<(A).end;i=(A).prv[(i)])
#define ALIST_DO_BACKWARD2(A,i,ii) for((i)=(A).prv[(A).end],ii=(A).prv[i];(i)<(A).end;i=ii,ii=(A).prv[i])
#define MALIST_DO_FORWARD(A,l,i)  for((i)=(A).nxt[(A).end+l];(i)<(A).end;(i)=(A).nxt[(i)])
#define MALIST_DO_FORWARD2(A,l,i,ii)  for((i)=(A).nxt[(A).end+l],ii=(A).nxt[i];(i)<(A).end;i=ii,ii=(A).nxt[i])
#define MALIST_DO_BACKWARD(A,l,i) for((i)=(A).prv[(A).end+l];(i)<(A).end;(i)=(A).prv[(i)])
#define MALIST_DO_BACKWARD2(A,l,i,ii) for((i)=(A).prv[(A).end+l],ii=(A).prv[i];(i)<(A).end;i=ii,ii=(A).prv[i])

/* marco for getting the list number of a root element e */
#define MALIST_LIST(A,e) ((e)-((A).end))

/* struct for array list. the "end"th element is the root of the list */
/* the element e not included in the list is identified by "prv[e] = -1  */
typedef struct {
  unsigned char type;
  int *prv;        /* index of the previous element of each element */
  int *nxt;        /* index of the next element of each element */
  int end;         /* maximum number of elements */
  int num;         /* the length of the current list */
} ALIST;

extern ALIST INIT_ALIST;
QSORT_TYPE_HEADER(ALIST_ID, ALIST_ID)

/************* initialization/termination ************/

void ALIST_alloc (ALIST *A, ALIST_ID siz); /* initialization */
void ALIST_end (ALIST *A); /* termination */
void ALIST_cpy (ALIST *A1, ALIST *A2);  /* copy A2 to A1 */
ALIST ALIST_dup (ALIST *A); /* duplication */

/************* addition/deletion ************/

/* insert an element e previous/next to element i or haed/tail of the list
   error is checked for already inserted elements or out of range */
void ALIST_ins_nxt (ALIST *A, ALIST_ID e, ALIST_ID i, int undo);
void ALIST_ins_prv (ALIST *A, ALIST_ID e, ALIST_ID i, int undo);
void ALIST_ins_head (ALIST *A, ALIST_ID e, int undo);
void ALIST_ins_tail (ALIST *A, ALIST_ID e, int undo);

/* remove head/tail of the list, and return it. return root if empty list
 with check removal of removed element/range check  */
ALIST_ID ALIST_rm_head (ALIST *A, int undo);
ALIST_ID ALIST_rm_tail (ALIST *A, int undo);

/* remove an element e from list A.
 with check removal of removed element/range check  */
void ALIST_rm (ALIST *A, ALIST_ID e, int undo);

/* re-insert removed element by ALIST_rm to its original position. precisely, 
 insert removed element e previous to the next element when e is in the list
 with check insertion of non-removed element/range check  */
void ALIST_recov (ALIST *A, ALIST_ID e, int undo);

/* remove all elements in a list */
void ALIST_rmall (ALIST *A);

/************* print routines ************/

/* print all elements in the list */
void ALIST_print (ALIST *A, int flag);

/************* search ************/

/* return the kth element next/previous to i */
/* for large k, pass through the root element. */
ALIST_ID ALIST_knxt (ALIST *A, ALIST_ID i, ALIST_ID k);
ALIST_ID ALIST_kprv (ALIST *A, ALIST_ID i, ALIST_ID k);

/* return kth element from the head/tail. return the head/tail if k<1.
   return root if k>current size */
ALIST_ID ALIST_find_kth (ALIST *A, ALIST_ID k);
ALIST_ID ALIST_find_tkth (ALIST *A, ALIST_ID k);

/* return element randomly chosen from all element in A */
ALIST_ID ALIST_find_rnd (ALIST *A);


/* sort elements in the list by merge sort without recursion */
/* increasing order if f==0 */
void ALIST_sort (ALIST *A, int f, int undo);

/* return the most tailmost element which does not exceed i. 
   return root if there is no such element */
ALIST_ID ALIST_find_i (ALIST *A, ALIST_ID i);

/************* rich operations ************/

/*  remove elements in B/not in B from A */
void ALIST_minus (ALIST *A, ALIST *B, int undo);
void ALIST_and (ALIST *A, ALIST *B, int undo);

/* concatinate A2 following to A1, A2 remains in ALIST_concat, not remain in ALIST_append */
void ALIST_append (ALIST *A1, ALIST *A2, int undo);
void ALIST_concat (ALIST *A1, ALIST *A2, int undo);



/********************************************************************/
/*******************   à»å„ÅAMALIST *********************************/
/********************************************************************/

/* structure for array based multi list. each element can belong to at most one list */
typedef struct {
  unsigned char type;
  ALIST_ID *prv;        /* previous element to i */
  ALIST_ID *nxt;        /* next element to i */
  ALIST_ID end;         /* total #of elements */
  ALIST_ID *num;        /* current length of each list */
  ALIST_ID *list;       /* index of the list that each element belongs. If not belong, -1 */
  ALIST_ID list_end;    /* maximum #lists */
} MALIST;

extern MALIST INIT_MALIST;

/************* initialization/termination ************/

void MALIST_alloc (MALIST *A, ALIST_ID siz, ALIST_ID list_siz); /* initialization with #elements siz, and #list list_siz */
void MALIST_end (MALIST *A); /* termination */
void MALIST_cpy (MALIST *A1, MALIST *A2); /* copy. with size overflow check  */
MALIST MALIST_dup (MALIST *A); /* duplication */


/************* addition/deletion ************/

/* insert an element e previous/next to element ee or haed/tail of the list l
   error is checked for already inserted elements or out of range */
void MALIST_ins_nxt (MALIST *A, ALIST_ID e, ALIST_ID ee, int undo);
void MALIST_ins_prv (MALIST *A, ALIST_ID e, ALIST_ID ee, int undo);
void MALIST_ins_head (MALIST *A, ALIST_ID l, ALIST_ID e, int undo);
void MALIST_ins_tail (MALIST *A, ALIST_ID l, ALIST_ID e, int undo);

/* remove head/tail of the list, and return it. return root if empty list
 with check removal of removed element/range check  */
ALIST_ID MALIST_rm_head (MALIST *A, ALIST_ID l, int undo);
ALIST_ID MALIST_rm_tail (MALIST *A, ALIST_ID l, int undo);

/* remove an element e from A.
 with check removal of removed element/range check  */
void MALIST_rm (MALIST *A, ALIST_ID e, int undo);

/* re-insert removed element by MALIST_rm to its original position. precisely, 
 insert removed element e previous to the next element when e is in the list
 with check insertion of non-removed element/range check  */
void MALIST_recov (MALIST *A, ALIST_ID e, int undo);

/* remove all elements in the lth list */
void MALIST_rmall (MALIST *A, ALIST_ID l);

/* move element e to the lth list
 with check insertion of non-removed element/range check  */
void MALIST_mv (MALIST *A, ALIST_ID l, ALIST_ID e, int undo);
void MALIST_mv_head (MALIST *A, ALIST_ID l, ALIST_ID e, int undo);
/* move all elements in llth list to lth list. do nothing if l==ll */
void MALIST_mvall (MALIST *A, ALIST_ID l, ALIST_ID ll, int undo);

/************* print routines ************/

void MALIST_print (MALIST *A, ALIST_ID l); /* print lth list */
void MALIST_print_all (MALIST *A); /* print all lists */

/************* search ************/

/* return the kth element from the head/tail of the lth list */
ALIST_ID MALIST_find_kth (MALIST *A, ALIST_ID l, ALIST_ID k);
ALIST_ID MALIST_find_tkth (MALIST *A, ALIST_ID l, ALIST_ID k);

/* return a random element of lth list */
ALIST_ID MALIST_find_rnd (MALIST *A, ALIST_ID l);

/* find minimum/maximum weight of the element in lth list.
   weights are given by array odr. find minimum if f==0, otherwise find the maximum
   if odr is NULL, use index as weights */
ALIST_ID MALIST_find_min_odr (MALIST *A, ALIST_ID l, ALIST_ID *odr, int f);

/* sort lth list in the order of indices/weights. increasing order if f==0, decreasing otherwise */
void MALIST_sort (MALIST *A, ALIST_ID l, int f, int undo);
void MALIST_sort_odr (MALIST *A, ALIST_ID l, int f, int *odr, int undo);

/* remove all elements included in ALIST B, from lth list of MALIST A */
void MALIST_minus_ALIST (MALIST *A, ALIST_ID l, ALIST *B);

#endif
