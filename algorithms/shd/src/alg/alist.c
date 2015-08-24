/*
    array-based list/multi-list library
            12/Apr/2001   by Takeaki Uno  e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, pleas
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about codes for the users. */

#ifndef _alist_c_
#define _alist_c_

#include"alist.h"
#include"stdlib2.c"
#include"undo.c"

/* if error check is unnecessary, comment out the next line */
//#define ERROR_CHECK

ALIST INIT_ALIST = {TYPE_ALIST,NULL,NULL,0,0};
MALIST INIT_MALIST = {TYPE_MALIST,NULL,NULL,0,NULL,NULL,0};
QSORT_TYPE(ALIST_ID, ALIST_ID)
/************* initialization/termination ************/

/* initialization */
void ALIST_alloc (ALIST *A, ALIST_ID siz){
  ALIST_ID i;
  *A = INIT_ALIST;
  malloc2 (A->prv, siz+1, EXIT);  // allocate memory
  malloc2 (A->nxt, siz+1, {free2(A->prv);EXIT;});  // allocate memory
  A->end = siz;
  A->prv[siz] = siz;  // make empty list by connecting root to itself in bidirection
  A->nxt[siz] = siz;  
  for ( i=0 ; i<siz ; i++ ) A->prv[i] = -1; // set prv[i]=-1 for each element since they are not inserted in the list
}

/* termination */
void ALIST_end (ALIST *A){
  mfree (A->prv, A->nxt);
  *A = INIT_ALIST;
}

/* copy list A2 to A1. If the size of A1 is less than current size of A2, error */
void ALIST_cpy (ALIST *A1, ALIST *A2){
  ALIST_ID i;
  ALIST_rmall (A1);
  ALIST_DO_FORWARD (*A2, i){
#ifdef ERROR_CHECK
  if ( !ALIST_IS_ELM(*A1,i) )
    error ("ALIST_cpy: the size of A1 is smaller than current size of A2", EXIT);
#endif
    ALIST_ins_tail (A1, i, 0);
  }
}

/* duplication */
ALIST ALIST_dup (ALIST *A){
  ALIST AA;
  ALIST_alloc (&AA, A->end);    /* initialize the new list */
  ALIST_cpy (&AA, A);    /* copy A to AA */
  return (AA);
}


/************* addition/deletion ************/

/* insert an element e next/previous to element i or haed/tail of the list
   error is checked for already inserted elements or out of range */
void ALIST_ins_nxt (ALIST *A, ALIST_ID e, ALIST_ID ee, int undo){
#ifdef ERROR_CHECK
  if ( e < 0 || e >= A->end ) error_num ("ALIST_ins: e is out of range", e, EXIT);
  if ( ee < 0 || ee > A->end ) error_num ("ALIST_ins: ee is out of range", ee, EXIT);
  if ( A->prv[e] >= 0 ) error_num ("ALIST_ins: element is already in the list", e, EXIT);
#endif
  
  A->num++;      /* increment the length of the list */
  A->nxt[e] = A->nxt[ee]; /* change the four links */
  A->prv[A->nxt[ee]] = e;
  A->nxt[ee] = e;
  A->prv[e] = ee;
  if ( undo ) ALISTundo_ins (U_ALIST_rm, (void *)A, e, e);
}

void ALIST_ins_prv (ALIST *A, ALIST_ID e, ALIST_ID ee, int undo){
#ifdef ERROR_CHECK
  if ( ee < 0 || ee > A->end ) error_num ("ALIST_ins_prv: ee is out of range", ee, EXIT);
#endif
  ALIST_ins_nxt (A, e, A->prv[ee], undo);
}
void ALIST_ins_head (ALIST *A, ALIST_ID e, int undo){ ALIST_ins_prv (A, e, ALIST_HEAD(*A), undo);}
void ALIST_ins_tail (ALIST *A, ALIST_ID e, int undo){ ALIST_ins_nxt (A, e, ALIST_TAIL(*A), undo);}

/* remove head/tail of the list. do nothing if e is root
 with check removal of removed element/range check  */
void ALIST_rm (ALIST *A, ALIST_ID e, int undo){
#ifdef ERROR_CHECK
  if ( e < 0 || e >= A->end ) error_num ("ALIST_rm: e is out of range", e, EXIT);
  if ( A->prv[e] < 0 ) error_num ("ALIST_rm: element is not in the list", e, EXIT );
#endif
  if ( e == A->end ) return;
  if ( undo ) ALISTundo_ins (U_ALIST_ins, (void *)A, e, A->prv[e]);
  A->num--;  /* decrement the length of the list */
  A->nxt[A->prv[e]] = A->nxt[e];    /* short cut the previous & next elements to e */
  A->prv[A->nxt[e]] = A->prv[e];
  A->prv[e] = -1;   /* set prv[i]=-1 so that i is out */
}


/* remove head/tail of the list, and return it. return root if empty list
 with check removal of removed element/range check  */
ALIST_ID ALIST_rm_head (ALIST *A, int undo){
  ALIST_ID e = ALIST_HEAD(*A);  /* set e to the head of list */
  if ( ALIST_IS_ELM(*A,e) ) ALIST_rm (A, e, undo);  /* if e is element, remove e */
  return (e);
}

ALIST_ID ALIST_rm_tail (ALIST *A, int undo){
  ALIST_ID e = ALIST_TAIL(*A);   /* set e to the tail */
  if ( ALIST_IS_ELM(*A,e) ) ALIST_rm (A, e, undo); /* if e is element, remove e */
  return (e);
}

/* re-insert removed element by ALIST_rm to its original position. precisely, 
 insert removed element e previous to the next element when e is in the list
 with check insertion of non-removed element/range check  */
void ALIST_recov (ALIST *A, ALIST_ID e, int undo){
  ALIST_ID i;
#ifdef ERROR_CHECK
  if ( e < 0 || e >= A->end ) error_num ("ALIST_recov: e is out of range", e, EXIT);
  if ( A->prv[e] >= 0 ) error_num ("ALIST_rm: recover element %d in the list\n", e, EXIT);
#endif
  i = A->prv[A->nxt[e]];  /* i = element previous to the next to e */
  A->num++;  /* increment the length of the list */
  A->nxt[i] = e;    /* insert e by changing the links */
  A->prv[A->nxt[e]] = e;
  A->prv[e] = i;    /* re-set prv[e], since it was -1 */
  if ( undo ) ALISTundo_ins (U_ALIST_rm, (void *)A, e, -1);
}

/* remove all elements in a list */
void ALIST_rmall (ALIST *A){
  ALIST_ID i;
  ALIST_DO_FORWARD (*A, i) A->prv[i] = -1; /* set prv[i]=-1 for all elements */
  A->num = 0;  /* set length of the list to 0 */
  A->nxt[A->end] = A->end;  /* make empty list */
  A->prv[A->end] = A->end;
}

/************* print routines ************/

/* print all elements in a list
  flag&1: not print newline after the print, flag&2: not print size,
  flag&4: print all elements not in the list */
void ALIST_print (ALIST *A, int flag){
  ALIST_ID i;
  if ( flag&2 ) printf (ALIST_IDF": ", flag&4?A->end-A->num:A->num );
  if ( flag&4 ){
     FLOOP (i, 0, A->end) if ( A->prv[i] == -1 ) printf (ALIST_IDF",", i);
  } else ALIST_DO_FORWARD (*A, i) printf (ALIST_IDF",", i);
  if ( (flag&1) == 0 ) printf ("\n");
}

/************* search ************/

/* return the kth element next/previous to i */
/* note that return i when k=1, and return root if root is reached */
ALIST_ID ALIST_knxt (ALIST *A, ALIST_ID i, ALIST_ID k){
  for ( ; k>=1&& ALIST_IS_ELM(*A,i) ; k--) i = A->nxt[i];  /* go next k-1 times from i */
  return (i);
}
ALIST_ID ALIST_kprv (ALIST *A, ALIST_ID i, ALIST_ID k){
  for ( ; k>=1 && ALIST_IS_ELM(*A,i) ; k--) i = A->prv[i];  /* go previous k-1 times from i */
  return (i);
}

/* return kth element from the head/tail. return the head/tail if k<1.
   return root if k>current size */
ALIST_ID ALIST_find_kth (ALIST *A, ALIST_ID k){ return (ALIST_knxt (A, ALIST_HEAD (*A), k-1) );}
ALIST_ID ALIST_find_tkth (ALIST *A, ALIST_ID k){ return (ALIST_kprv (A, ALIST_TAIL (*A), k-1) );}

/* return element randomly chosen from all element in A */
ALIST_ID ALIST_find_rnd (ALIST *A){
  ALIST_ID k = rand()%A->num +1;     /* generate random number ranging the size of list */
  if ( k < A->num/2 ) return (ALIST_find_kth (A, k) );
  else return (ALIST_find_tkth (A, A->num-k+1) );
}

/* return the most tailmost element which does not exceed i. 
   return root if there is no such element */
ALIST_ID ALIST_find_i (ALIST *A, ALIST_ID i){
  ALIST_ID e;
  ALIST_DO_FORWARD (*A, e) if ( e>i ) break;
  return ( A->prv[e] );
}

/* return the element most close to i and not exceed i */
/* return -1 if no element does not exceed i */
ALIST_ID ALIST_find_max_le_i (ALIST *A, ALIST_ID i){
  ALIST_ID e, m=-1;
  ALIST_DO_FORWARD (*A, e) if ( e<=i ) ENMAX (m, e);
  return (m);
}

/* sort elements in the list by merge sort without recursion */
/* increasing order if f==0 */
void ALIST_sort ( ALIST *A, int f, int undo ){
  ALIST_ID k, i, ii, j1, j2;
  for (k=1 ; k<A->num; k=k*2){  /* twice the size of partial sequence to be sorted */
    i = ALIST_HEAD (*A);
    while (1){
      j1 = ALIST_knxt (A, i, k-1);  /* to merge from ith to (i+k-1)th and (i+k)th to (i+k+k)th, set pointers to the position */
      if ( !ALIST_IS_ELM (*A, j1) ) break;
      j2 = ALIST_knxt(A, j1, k+1);
      while (1){
        ii = A->nxt[j1];
        if ( i > ii ){
          ALIST_rm (A, ii, undo);
          ALIST_ins_prv (A, ii, i, undo); /* insert ii previous to i if i>ii */ 
          if ( A->nxt[j1] == j2 ) break;
        } else {
          i = A->nxt[i];  /* finish if i reach to the last */ 
          if ( i == ii ) break;
        }
      }
      i = j2;
    }
  }
}

/************* rich operations ************/

/*  remove elements in B from A  */
void ALIST_minus (ALIST *A, ALIST *B, int undo){
  ALIST_ID i;
  ALIST_DO_FORWARD (*B, i)
      if ( A->prv[i] >= 0 ) ALIST_rm (A, i, undo);
}

/*  remove elements not in B from A (intersection of A and B) */
void ALIST_and (ALIST *A, ALIST *B, int undo){
  ALIST_ID i;
  ALIST_DO_FORWARD (*A, i)
      if ( B->prv[i] == -1 ) ALIST_rm (A, i, undo);
}

/* concatinate A2 following to A1, A2 remains in ALIST_concat, not remain in ALIST_append */
void ALIST_concat (ALIST *A1, ALIST *A2, int undo){
  ALIST_ID e;
  ALIST_DO_FORWARD (*A2, e) ALIST_ins_tail (A1, e, undo);
}

void ALIST_append (ALIST *A1, ALIST *A2, int undo){
  ALIST_ID e;
  while (1){
    e = ALIST_rm_head (A2, undo);
    if ( !ALIST_IS_ELM(*A2, e) ) return;
    ALIST_ins_tail (A1, e, undo);
  }
}


/***********************************************************************/
/* Multi list structure                        */
/***********************************************************************/


/************* initialization/termination ************/

/* initialization. parameters are
   pointer to the structure, #elements, #lists. No range check  */
void MALIST_alloc (MALIST *A, ALIST_ID siz, ALIST_ID list_siz){
  ALIST_ID i;
  *A = INIT_MALIST;
  malloc2 (A->prv, siz+list_siz, EXIT);
  malloc2 (A->nxt, siz+list_siz, {MALIST_end(A);EXIT;});
  malloc2 (A->num, list_siz, {MALIST_end(A);EXIT;});
  malloc2 (A->list, siz, {MALIST_end(A);EXIT;});
  A->end = siz;
  A->list_end = list_siz;
  ARY_FILL (A->prv, 0, siz, -1); /* each element belongs no list */
  FLOOP (i, 0, list_siz){
    A->num[i] = 0;  /* set the length of each list to 0 */
    A->prv[siz+i] = siz+i;   /* set each list to the empty list */
    A->nxt[siz+i] = siz+i;
  }
}

/* termination */
void MALIST_end (MALIST *A){
  mfree (A->prv, A->nxt, A->num, A->list);
  *A = INIT_MALIST;
}

/* copy AA to A1. error if max #elements in A1 is length of A2 */
void MALIST_cpy (MALIST *A1, MALIST *A2){
  ALIST_ID l, e;
  FLOOP (l, 0, A1->list_end) MALIST_rmall (A1, l);  /* remove all elements in A1 */
  FLOOP (l, 0, A2->list_end){  /* For each list in A2 */
    if ( A2->num[l] >0 ){  /* if lth list is non-empty, do ... */
#ifdef ERROR_CHECK
     if ( A1->list_end <= l ) error ("MALIST_cpy: #lists in A1 < #lists in A2.", EXIT);
#endif
      MALIST_DO_FORWARD (*A2, l, e){  /* cpoy lth element of list A2 */
        if ( MALIST_IS_ELM(*A1,e) ) MALIST_ins_tail (A1, l, e, 0);
         error ("MALIST_cpy: #elements in A1 < #current elements in A2.", EXIT);
      }
    }
  }
}

/* duplication */
MALIST MALIST_dup (MALIST *A){
  MALIST AA;
  MALIST_alloc (&AA, A->end, A->list_end);  /* initialize AA as same as A */
  MALIST_cpy (&AA, A); /* copy AA to A */
  return (AA);
}


/************* addition/deletion ************/

/* insert an element e previous/next to element ee or haed/tail of the list l
   error is checked for already inserted elements or out of range */
void MALIST_ins_nxt (MALIST *A, ALIST_ID e, ALIST_ID ee, int undo){
  ALIST_ID f, l;
#ifdef ERROR_CHECK
  if ( e < 0 || e >= A->end ) error_num ("MALIST_ins: e is out of range", e, EXIT);
  if ( ee < 0 || ee >= A->end+A->list_end ) error_num ("MALIST_ins: ee is out of range", ee, EXIT);
  if ( A->prv[e] >= 0 ) error_num ("MALIST_ins: element is already in some list", e, EXIT);
#endif
  f = A->nxt[ee];
  if ( MALIST_IS_ELM(*A,ee) ) l=A->list[ee]; else l=ee-A->end;
  A->num[l]++;       /* increment the length of the list ee belongs */
  A->list[e] = l;    /* set list[e] to the list ee belongs */
  A->nxt[e] = f;     /* change the links to insert e */
  A->prv[f] = e;
  A->nxt[ee] = e;
  A->prv[e] = ee;
  if ( undo ) ALISTundo_ins (U_MALIST_rm, (void *)A, e, -1);
}

void MALIST_ins_prv (MALIST *A, ALIST_ID e, ALIST_ID ee, int undo){
#ifdef ERROR_CHECK
  if ( ee < 0 || ee >= A->end+A->list_end ) error_num ("MALIST_ins_prv: ee is out of range", ee, EXIT);
#endif
  MALIST_ins_nxt (A, e, A->prv[ee], undo);
}

/* insert an element e to the head/tail of the list l
   error is checked for already inserted elements or out of range */
void MALIST_ins_head ( MALIST *A, ALIST_ID l, ALIST_ID e, int undo ){
#ifdef ERROR_CHECK
  if ( l < 0 || l >= A->list_end ) error_num ("MALIST_ins_prv: l is out of range.%d\n", l, EXIT);
#endif
  MALIST_ins_prv (A, e, MALIST_HEAD (*A, l), undo);
}

void MALIST_ins_tail (MALIST *A, ALIST_ID l, ALIST_ID e, int undo){
#ifdef ERROR_CHECK
  if ( l < 0 || l >= A->list_end ) error_num ("MALIST_ins_prv: l is out of range", l, EXIT);
#endif
  MALIST_ins_nxt (A, e, MALIST_TAIL (*A, l), undo);
}

/* remove the head of l-th list, and return it. return the root if it is empty. no range check */
ALIST_ID MALIST_rm_head (MALIST *A, ALIST_ID l, int undo){
  ALIST_ID e = MALIST_HEAD (*A,l);
  if ( MALIST_IS_ELM(*A,e) ){ MALIST_rm (A, e, undo); return (e); } else return (-1);
}

/* remove head/tail of the list, and return it. return root if empty list
 with check removal of removed element/range check  */
ALIST_ID MALIST_rm_tail (MALIST *A, ALIST_ID l, int undo){
  ALIST_ID e = MALIST_TAIL(*A,l);
  if ( MALIST_IS_ELM(*A,e) ){ MALIST_rm (A, e, undo); return (e); } else return (-1);
}

/* remove an element e from A.
 with check removal of removed element/range check  */
void MALIST_rm ( MALIST *A, ALIST_ID e, int undo ){
#ifdef ERROR_CHECK
  if ( e < 0 || e >= A->end ) error_num ("MALIST_rm: e is out of range", e, EXIT);
  if ( A->prv[e] < 0 ) error_num ("MALIST_rm: element is not in any list", e, EXIT);
#endif
  if ( undo ) ALISTundo_ins (U_MALIST_ins, (void *)A, e, A->prv[e]);
  A->num[A->list[e]]--;
  A->nxt[A->prv[e]] = A->nxt[e];
  A->prv[A->nxt[e]] = A->prv[e];
  A->prv[e] = -1;
}

/* re-insert removed element by MALIST_rm to its original position. precisely, 
 insert removed element e previous to the next element when e is in the list
 with check insertion of non-removed element/range check  */
void MALIST_recov (MALIST *A, ALIST_ID e, int undo){
  ALIST_ID i, l;
#ifdef ERROR_CHECK
  if ( e < 0 || e >= A->end ) error_num ("MALIST_recov: e is out of range", e, EXIT);
  if ( A->prv[e] >= 0 ) error_num ("MALIST_rm: element %d is already in some list.\n", e, EXIT);
#endif
  i = A->prv[A->nxt[e]];
  l = MALIST_IS_ELM(*A,i)? A->list[i]: i - A->end;
  A->list[e] = l;
  A->num[l]++;
  A->prv[e] = i;
  A->nxt[i] = e;
  A->prv[A->nxt[e]] = e;
  if ( undo ) ALISTundo_ins (U_MALIST_rm, (void *)A, e, 0);
}

/* remove all elements in the lth list */
void MALIST_rmall (MALIST *A, ALIST_ID l){
  ALIST_ID i;
  MALIST_DO_FORWARD (*A, l, i) A->prv[i] = -1;
  A->num[l] = 0;
  A->nxt[A->end+l] = A->end+l;
  A->prv[A->end+l] = A->end+l;
}

/* move element e to the lth list
 with check insertion of non-removed element/range check  */
void MALIST_mv (MALIST *A, ALIST_ID l, ALIST_ID e, int undo){
  MALIST_rm (A, e, undo);
  MALIST_ins_tail (A, l, e, undo);
}
void MALIST_mv_head (MALIST *A, ALIST_ID l, ALIST_ID e, int undo){
  MALIST_rm (A, e, undo);
  MALIST_ins_head (A, l, e, undo);
}

/* move all elements in llth list to lth list. do nothing if l==ll */
void MALIST_mvall (MALIST *A, ALIST_ID l, ALIST_ID ll, int undo){
  ALIST_ID i, ii;
#ifdef ERROR_CHECK
  if ( l == ll ) return;
#endif
  MALIST_DO_FORWARD2 (*A, ll, i, ii) MALIST_mv (A, l, i, undo);
}

/************* print routine ************/

/* print lth list */
void MALIST_print (MALIST *A, ALIST_ID l){
  ALIST_ID i;
  MALIST_DO_FORWARD (*A, l, i) printf (ALIST_IDF",", i);
  printf ("\n");
}

/* print all lists */
void MALIST_print_all (MALIST *A){
  ALIST_ID l, i;
  FLOOP (l, 0, A->list_end){
    printf ("%d:", l);
    MALIST_DO_FORWARD (*A, l, i) printf (ALIST_IDF",", i);
    printf ("\n");
  }
}


/************* åüçıån ************/

/* return kth element from the head/tail. return the head/tail if k<1.
   return root if k>current size */
/* for large k, pass through the root element. */
ALIST_ID MALIST_find_kth (MALIST *A, ALIST_ID l, ALIST_ID k){
  int x = MALIST_HEAD(*A,l);  /* set x to the head of lth list */
  for ( ; k>1 ; k--) x = A->nxt[x];  /* go next from x k-1 times */
  return (x);
}

ALIST_ID MALIST_find_tkth (MALIST *A, ALIST_ID l, ALIST_ID k){
  ALIST_ID x = MALIST_TAIL(*A,l);   /* set x to the tail of lth list */
  for ( ; k>1 ; k--) x = A->prv[x];  /* go forward from x k-1 times */
  return (x);
}

/* return a random element of lth list */
ALIST_ID MALIST_find_rnd (MALIST *A, ALIST_ID l){
  ALIST_ID k = rand() % A->num[l]+1;
  if ( k < A->num[l]/2 ) return (MALIST_find_kth (A, l, k) );
  return (MALIST_find_tkth (A, l, A->num[l]-k+1) );
}

/* find minimum/maximum weight of the element in lth list.
   weights are given by array odr. find minimum if f==0, otherwise find the maximum
   if odr is NULL, use index as weights */
ALIST_ID MALIST_find_min_odr ( MALIST *A, ALIST_ID l, ALIST_ID *odr, int f ){
  ALIST_ID m = -1, i;
  MALIST_DO_FORWARD (*A, l, i){
    if ( m == -1 ) m = i;
    else {
      if ( f ){
        if ( odr ){
          if ( odr[m] < odr[i] ) m = i;
        } else if ( m < i ) m = i;
      } else if ( odr ){
          if ( odr[m] > odr[i] ) m = i;
      } else if ( m > i ) m = i;
    }
  }
  return (m);
}

/* partition the sub-list from head to tail, in the l-th list so that the former part
 is composed of elements smaller than ind
 and the latter is the remaining. if f!=0, then reverse order */
/* return the position at which idx should be placed */
ALIST_ID MALIST_seg_partition (MALIST *A, ALIST_ID l, ALIST_ID head, ALIST_ID tail, ALIST_ID idx, int f, int undo){
  ALIST_ID e1, e2, e, k=head;
#ifdef ERROR_CHECK
  if ( l < 0 || l >= A->list_end ) error_num ("MALIST_partition: l is out of range", l, EXIT0);
#endif
  if ( head >= A->num[l] ) return (-1);
  e1 = MALIST_find_kth (A, l, head);
  if ( tail>A->num[l] ) e2 = MALIST_TAIL (*A, l);
  else e2 = MALIST_find_kth (A, l, tail);
  
  while (1){
    while ( f? (e1>idx): (e1<idx) ){
      e1 = A->nxt[e1];
      k++;
      if ( e1==e2 ) goto END;
    }
    while ( f? (e2<=idx): (e2>idx) ){
      e2 = A->prv[e2];
      if ( e1==e2 ) goto END;
    }
    e = A->prv[e1];
    MALIST_rm (A, e1, undo);
    MALIST_ins_nxt (A, e1, e2, undo);
    MALIST_rm (A, e2, undo);
    MALIST_ins_nxt (A, e2, e, undo);
    e=e1; e1=e2; e2=e;
  }
  END:;
  return (k);
}

/* partition the l-th list so that the former part is composed of elements smaller than ind
 and the latter is the remaining. if f!=0, then reverse order */
/* return the position at which idx should be placed */
ALIST_ID MALIST_partition (MALIST *A, ALIST_ID l, ALIST_ID idx, int f, int undo){
  ALIST_ID e, e1, e2, k=1;
#ifdef ERROR_CHECK
  if ( l < 0 || l >= A->list_end ) error_num ("MALIST_partition: l is out of range", l, EXIT0);
#endif
  e1 = MALIST_HEAD ( *A, l );
  e2 = MALIST_TAIL ( *A, l );
  while (1){
    while ( f? (e1>idx): (e1<idx) ){
      e1 = A->nxt[e1];
      k++;
      if ( e1==e2 ) goto END;
    }
    while ( f? (e2<=idx): (e2>idx) ){
      e2 = A->prv[e2];
      if ( e1==e2 ) goto END;
    }
    e = A->prv[e1];
    MALIST_rm (A, e1, undo);
    MALIST_ins_nxt (A, e1, e2, undo);
    MALIST_rm (A, e2, undo);
    MALIST_ins_nxt (A, e2, e, undo);
    e=e1; e1=e2; e2=e;
  }
  END:;
  if ( e2 == MALIST_TAIL (*A, l) && ( f? (e2<=idx): (e2>idx) ) ) k++;
  return (k);
}

/* partition the l-th list so that any of the former k elements is smaller than any of the 
   lattter. if f!=0, then reverse order */
/* return the k-th element */
ALIST_ID MALIST_k_partition (MALIST *A, ALIST_ID l, ALIST_ID k, int f, int undo){
  ALIST_ID e, ee, head=1, tail=A->num[l], kk;
#ifdef ERROR_CHECK
  if ( l < 0 || l >= A->list_end ) error_num ("MALIST_partition: l is out of range", l, EXIT0);
#endif
  while (1){
    e = MALIST_find_kth (A, l, k);
    if ( tail == head ) break;
    kk = MALIST_seg_partition (A, l, head, tail, e, f, undo);
    if ( kk == head ){
      if ( k == head ) return (e);
      ee = MALIST_find_kth (A, l, head);
      kk = A->prv[ee];
      MALIST_rm (A, ee, undo);
      MALIST_ins_nxt (A, ee, e, undo);
      MALIST_rm (A, e, undo);
      MALIST_ins_nxt (A, e, kk, undo);
      head++;
    }
    else if ( kk<=k ) head = kk;
    else tail = kk-1;
  }
  return (e);
}



/* sort lth list in the order of indices. increasing order if f==0, decreasing otherwise */
void MALIST_sort (MALIST *A, ALIST_ID l, int f, int undo){
  ALIST_ID i, ii, j;
  MALIST_DO_FORWARD2 (*A, l, i, ii){
    j=A->prv[i];
    MALIST_rm (A, i, undo);
    if ( f ) while (MALIST_IS_ELM(*A, j) && j<i ) j = A->prv[j];
    else while (MALIST_IS_ELM(*A, j) && j>i ) j = A->prv[j];
    MALIST_ins_nxt (A, i, j, undo);
  }
}

void MALIST_sort_odr (MALIST *A, ALIST_ID l, ALIST_ID f, int *odr, int undo){
  ALIST_ID i, ii, j;
  MALIST_DO_FORWARD2 (*A, l, i, ii){
    j = A->prv[i];
    MALIST_rm (A, i, undo);
    if ( f ) while (MALIST_IS_ELM(*A, j) && odr[j]<odr[i]) j = A->prv[j];
    else while (MALIST_IS_ELM(*A, j) && odr[j]>odr[i]) j = A->prv[j];
    MALIST_ins_nxt (A, i, j, undo);
  }
}

/* remove all elements included in ALIST B, from lth list of MALIST A */
void MALIST_minus_ALIST ( MALIST *A, ALIST_ID l, ALIST *B ){
  ALIST_ID i;
  ALIST_DO_FORWARD (*B, i) if ( A->list[i] == l ) MALIST_rm (A, i, 0);
}


#endif



