/* Scalable Hypergraph Dualization algorithm */
/* 2004/4/10 Takeaki Uno   e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, do not forget to 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about SHD for the users.
   For the commercial use, please make a contact to Takeaki Uno. */



#ifndef _shd_c_
#define _shd_c_

#include"alist.c"
#include"barray.c"
#include"vec.c"
#include"barray.c"
#include"problem.c"

#define SHD_STRAIGHT 1
#define SHD_DFS 2
#define SHD_COMPLEMENT 4
#define SHD_NO_BITMAP 16
#define SHD_PRUNE 32

void SHD_error (){
  ERROR_MES = "command explanation";
  print_err ("SHD: 09DSqc [options] input-filename [output-filename]\n\
%%:show progress, _:no message, +:write solutions in append mode\n\
c:complement input, P:no pruning, B:no bitmap\n\
0:normal version, D:dfs version, 9:naive-minimality-check version\n\
t:transpose the database so that i-th transaction will be item i\n\
[options]\n\
-l [num]:output itemsets with size at least [num]\n\
-u [num]:output itemsets with size at most [num]\n\
-S [num]:stop after outputting [num] solutions\n\
-, [char]:give the separator of the numbers in the output\n\
-Q [filename]:replace the output numbers according to the permutation table given by [filename]\n\
# the 1st letter of input-filename cannot be '-'.n\
# if the output file name is -, the solutions will be output to standard output.\n");
  EXIT;
}


/***********************************************************************/
/*  read parameters given by command line  */
/***********************************************************************/
void SHD_read_param (int argc, char *argv[], PROBLEM *PP){
  ITEMSET *II = &PP->II;
  int c=1;
  if ( argc < c+2 ){ SHD_error (); return; }
  PP->problem |= SHD_PRUNE;
  if ( !strchr (argv[c], '_') ){ II->flag |= SHOW_MESSAGE; PP->FF.flag |= SHOW_MESSAGE; }
  if ( strchr (argv[c], '%') ) II->flag |= SHOW_PROGRESS;
  if ( strchr (argv[c], '+') ) II->flag |= ITEMSET_APPEND;
  if ( strchr (argv[c], '9') ) PP->problem |= SHD_STRAIGHT;
  if ( strchr (argv[c], 'D') ) PP->problem |= SHD_DFS;
  if ( strchr (argv[c], 's') ) PP->FF.flag |= (LOAD_SIZSORT + LOAD_DECROWSORT);  // sort trsacts
  else if ( strchr(argv[c], 'S') ) PP->FF.flag |= LOAD_SIZSORT; // sort trsacts increase order
  if ( strchr (argv[c], 'c') ) PP->problem |= SHD_COMPLEMENT;
  if ( strchr (argv[c], 't') ) PP->FF.flag |= LOAD_TPOSE;
  if ( strchr (argv[c], 'P') ) PP->problem -= SHD_PRUNE;
  if ( strchr (argv[c], 'B') ) PP->problem |= SHD_NO_BITMAP;
  c++;
  
  while ( argv[c][0] == '-' ){
    switch (argv[c][1]){
      case 'K': II->topk.end = atoi (argv[c+1]);
      break; case 'l': II->lb = atoi (argv[c+1]);
      break; case 'u': II->ub = atoi(argv[c+1]);
      break; case '#': II->max_solutions = atoi(argv[c+1]);
      break; case ',': II->separator = argv[c+1][0];
      break; case 'Q': PP->outperm_fname = argv[c+1];
      break; default: goto NEXT;
    }
    c += 2;
    if ( argc < c+1 ){ SHD_error (); return; }
  }

  NEXT:;
  PP->FF.fname = argv[c];
  if ( argc>c+1 ) PP->output_fname = argv[c+1];
}

int SHD_redundant_check (PROBLEM *PP, QUEUE_ID v, VEC_ID tt){
  QUEUE_INT i, *x, *y, *y_end = PP->OQ[v].v+PP->OQ[v].t, flag = 1, t;
  for (y_end=PP->OQ[v].v ; *y_end<tt && y_end<PP->OQ[v].v+PP->OQ[v].t ; y_end++);
  FLOOP (i, 0, PP->FF.clms){
    if ( i == v ) continue;
    y = PP->OQ[v].v; t= -1;
    MQUE_FLOOP (PP->OQ[i], x){
      if ( *x>=tt ) break;
      if ( y >= y_end ) goto END;
      if ( *x != *y ) goto END;
      t = *x;
      y++;
    }
//    if ( y == y_end ){ flag++; printf ("OK(%d %d) %d\n", v, i, y_end-PP->OQ[v].v); continue; }
    if ( y == y_end ){ flag++; continue; }
    END:;
//if ( t>-1 && t<1000) printf ("fail (%d %d) = %d    %d %d\n", v, i, t, *x, *y);
  }
  return (flag);
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

/**************************************/
/* straightforward minimality check */
/* if a subset II->itemset - {i} has intersection to any subset from 
 0 to tt, return 0. Otherwise, II->itemset is minimal hitting set
  and return 1 */
/**************************************/
int SHDstraight_minimality_check (PROBLEM *PP, VEC_ID tt){
  VEC_ID j;
  QUEUE_INT *x, *xx, *y;
  MQUE_FLOOP (PP->II.itemset, x){   // for each item e, 
    ARY_FILL (PP->vecmark, 0, tt+1, 0);
    MQUE_FLOOP (PP->II.itemset, xx){  //  for each item ee!=e
      if ( x==xx ) continue;
      MQUE_FLOOP ( PP->OQ[*xx], y){
        if ( *y>tt ) break;
        PP->vecmark[*y] = 1;  // mark transaction including ee
      }
    }
    FLOOP (j, 0, tt+1) if ( PP->vecmark[j]==0 ) goto END;  // if a transaction includes no item, then II->itemset-{e} is not minimal
//    printf ("okasii %d\n", tt);
    return (0);
    END:;
  }
  return (1);
}

int SHD_minimality_check (PROBLEM *PP, VEC_ID tt){
  VEC_ID i, flag = 1;
  QUEUE_ID m;
  QUEUE_INT *x, mm=0;
  MQUE_FLOOP (PP->II.itemset, x) PP->itemchr[*x] = 2;

  FLOOP (i, 0, tt+1){
    m=0; MQUE_FLOOP (PP->FF.v[i], x)
        if ( PP->itemchr[*x] > 1 ){ m++; mm=*x; }
    if ( m == 1 ) PP->itemchr[mm] = 3;
  }
  MQUE_FLOOP (PP->II.itemset, x){
    if ( PP->itemchr[*x] == 2 ) flag = 0;
    PP->itemchr[*x] = 1;
  }
  return (flag);
}
LONG SHD_hitting_check (PROBLEM *PP, VEC_ID tt){
  VEC_ID i;
  QUEUE_INT *x;

  MQUE_FLOOP (PP->II.itemset, x) PP->itemchr[*x] = 2;
  FLOOP (i, 0, tt+1){
    MQUE_FLOOP (PP->FF.v[i], x)
        if ( PP->itemchr[*x] > 1 ) goto END;
    printf ("not a hitting set\n"); return (i);
    END:;
  }
  MQUE_FLOOP (PP->II.itemset, x) PP->itemchr[*x] = 1;
  return (-1);
}

/*************************************************************************/
/* iteration of SHD reverse search with straightforward minimality check */
/*************************************************************************/
void SHDstraight (PROBLEM *PP, VEC_ID tt){
  ITEMSET *II = &PP->II;
  QUEUE_INT *x;
  II->iters++;

  if ( tt == PP->FF.t ){ ITEMSET_output_itemset (II, NULL, 0); return; }
  MQUE_FLOOP (PP->FF.v[tt], x)
      if ( PP->itemchr[*x] == 1 ){ SHDstraight (PP, tt+1); return; } // tt includes an item of II->itemset, thus go to lower level.
  MQUE_FLOOP (PP->FF.v[tt], x){
    QUE_INS (II->itemset, *x);
    PP->itemchr[*x] = 1;
    if ( SHD_minimality_check(PP, tt) ) SHDstraight (PP, tt+1);
    PP->itemchr[*x] = 0;
    II->itemset.t--;
  }
}



/**************************************/
/* straightforward minimality check for complement */
/* if a subset II->itemset - {i} has intersection to any subset from 
 0 to tt, return 0. Otherwise, II->itemset is minimal hitting set
  and return 1 */
/*  return 1 if minimal */
/**************************************/
int SHDstraight_minimality_check_ (PROBLEM *PP, VEC_ID tt){
  QUEUE_INT *x, *xx, *y;
  VEC_ID i;
//printf ("###########   "); QUEUE_print__ (&PP->II.itemset);
  MQUE_FLOOP (PP->II.itemset, x){  // for each item *x, 
    ARY_FILL (PP->vecmark, 0, tt+1, 0); // clear transaction counter
    MQUE_FLOOP (PP->II.itemset, xx){  // for each item *xx!=*x
      if ( x==xx ) continue;
      MQUE_FLOOP (PP->OQ[*xx], y){
        if ( *y>tt ) break;
        PP->vecmark[*y]++; // increase the counter of transaction including ee
      }
    }
//    FLOOP (i, 0, tt+1) printf ("(%d): %d<%d\n", *x, PP->vecmark[i], PP->II.itemset.t-1); 
    FLOOP (i, 0, tt+1) if ( PP->vecmark[i]==PP->II.itemset.t-1 ) goto END; // if counter is |II->itemset|-1 (including all except for e), then transaction ii includes II->itemset 
    return (0);
    END:;
  }
  return (1);
}


/*************************************************************************/
/* iteration of SHD (complement) reverse search with straightforward minimality check */
/* find all minimal itemsets included in no transactions */
/*************************************************************************/
void SHDstraight_ (PROBLEM *PP, VEC_ID tt){
  ITEMSET *II = &PP->II;
  QUEUE_INT *x, e;
  QUEUE_ID i=0, k=0;
  
  II->iters++;
  if ( tt == PP->FF.t ){ ITEMSET_output_itemset (II, NULL, 0); return; }
  MQUE_FLOOP (PP->FF.v[tt], x) if ( PP->itemchr[*x]==1 ) k++;
  if ( k<II->itemset.t ){ SHDstraight_ (PP, tt+1); return; } // transaction tt does not include II->itemset
  FLOOP (e, 0, PP->FF.clms){
    if ( PP->FF.v[tt].v[i]==e ){ i++; continue; }
    QUE_INS (II->itemset, e);
    if ( SHDstraight_minimality_check_(PP, tt) ){
      PP->itemchr[e] = 1;
      SHDstraight_ (PP, tt+1);
      PP->itemchr[e] = 0;
    }
    II->itemset.t--;
  }
}



/*************************************************************************/
/*************************************************************************/

/*************************************************************************/
/* return (minimum) transaction having no intersection to II->itemset */
/* return -1 if any transaction has non-empty intersection to II->itemset */
/*************************************************************************/
LONG SHDdfs_straight_bottom_check (PROBLEM *PP){
  QUEUE_INT *x, *xx;
  VEC_ID i;
  ARY_FILL (PP->vecmark, 0, PP->FF.t, 1);
  MQUE_FLOOP (PP->II.itemset, x)
      MQUE_FLOOP (PP->OQ[*x], xx) PP->vecmark[*xx] = 0;
  FLOOP (i, 0, PP->FF.t) if ( PP->vecmark[i] ) return (i);
  return (-1);
}

/*************************************************************************/
/* iteration of SHD backtrack with straightforward minimality check */
/*************************************************************************/
void SHDdfs_straight (PROBLEM *PP, QUEUE_INT item){
  ITEMSET *II = &PP->II;
  QUEUE_INT i;
  II->iters++;
  if ( SHDdfs_straight_bottom_check(PP) == -1 )
      { ITEMSET_output_itemset (II, NULL, 0); return; }
  FLOOP (i, 0, item){
    QUE_INS (II->itemset, i);
    if ( SHDstraight_minimality_check(PP, PP->FF.t-1) )
        SHDdfs_straight (PP, i);
    II->itemset.t--;
  }
}



/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/

void SHD_clear_flag (PROBLEM *PP, QUEUE_ID js){
  while (PP->itemjump.t > PP->itemjump.s )
      PP->itemchr[PP->itemjump.v[--PP->itemjump.t]] = 0;
  PP->itemjump.s = js;
}

/*************************************************************************/
/* return (minimum) transaction including II->itemset */
/* return -1 if any transaction has non-empty intersection to II->itemset */
/*************************************************************************/

LONG SHDdfs_straight_bottom_check_ (PROBLEM *PP){
  VEC_ID i;
  QUEUE_INT *x, *xx;
  ARY_FILL (PP->vecmark, 0, PP->FF.t, 0);
  MQUE_FLOOP (PP->II.itemset, x)
      MQUE_FLOOP (PP->OQ[*x], xx) PP->vecmark[*xx]++;
  FLOOP (i, 0, PP->FF.t)
      if ( PP->vecmark[i]==PP->II.itemset.t ) return (i);
  return (-1);
}

/*************************************************************************/
/* iteration of SHD backtrack with straightforward minimality check
  for complement input */
/*************************************************************************/
void SHDdfs_straight_ (PROBLEM *PP, QUEUE_INT item){
  QUEUE_INT i;
  PP->II.iters++;
//printf ("%d :::", item); QUEUE_print__ ( &PP->II.itemset );
  if ( SHDdfs_straight_bottom_check_(PP) == -1 )
      { ITEMSET_output_itemset (&PP->II, NULL, 0); return; }
  FLOOP (i, 0, item){
    QUE_INS (PP->II.itemset, i);
    if ( SHDstraight_minimality_check_ (PP, PP->FF.t-1) )
        SHDdfs_straight_ (PP, i);
    PP->II.itemset.t--;
  }
}





/*****************************************************/
/* SHD with simple crit update                       */
/*****************************************************/

/* check minimality condition */
void SHD_crit_check (PROBLEM *PP, QUEUE_INT tt){
  MALIST *A = &PP->occ;
  QUEUE_INT *x, *t, j, m=0, th = MALIST_HEAD (*A, PP->FF.clms);
  unsigned long *a;

  MQUE_FLOOP (PP->FF.v[tt], x){
    if ( PP->itemchr[*x] ){ if ( PP->itemchr[*x]==4 ) PP->II.iters3++; continue; }
      PP->II.iters2++;
    if ( PP->BA.v && PP->num < PP->OQ[*x].t ){
      m=0; MQUE_FLOOP (PP->II.itemset, t){
        a = &PP->BA.v[(*x) * PP->BA.xend];
        for (j=MALIST_HEAD(*A, *t) ; j<A->end ; j=A->nxt[j]){
//          if ( !(a[j/32]&BITMASK_1[j%32]) ){
          if ( !(a[j/32]&(1<<(j&31))) ){
            ENMAX (m, j);
            if ( m > th ) goto END;
            goto NEXT;
          }
        }
        m = A->end;
        goto END;
        NEXT:;
      }
    } else {
      MQUE_FLOOP (PP->OQ[*x], t) PP->FF.v[*t].end = *x;
      m=0; MQUE_FLOOP (PP->II.itemset, t){
        for (j=MALIST_HEAD(*A, *t) ; j<A->end ; j=A->nxt[j]){
          if ( PP->FF.v[j].end != *x ){
            ENMAX (m, j);
            if ( m > th ) goto END;
            goto NEXT2;
          }
        }
        m = A->end;
        goto END;
        NEXT2:;
      }
    }
    END:;

    if ( m <= th ){
      m=0; MALIST_DO_FORWARD (*A, PP->FF.clms, j)
          if ( PP->FF.v[j].end != *x ){ m = 1; break; }
      if ( m == 0 ){
        QUE_INS (PP->II.itemset, *x);
        ITEMSET_output_itemset (&PP->II, NULL, 0);
        PP->II.itemset.t--;
        PP->itemchr[*x] = 4;
        QUE_INS (PP->itemjump, *x);
      }
    } else if ( m >= PP->FF.t ){ PP->itemchr[*x] = 4; QUE_INS (PP->itemjump, *x); }
    else PP->itemchr[*x] = 1;
  }
}


/* crit update */
int SHD_update (PROBLEM *PP, QUEUE_INT e){
  MALIST *A = &PP->occ;
  QUEUE_INT j, *t, ttt = MALIST_HEAD(*A, PP->FF.clms);
  ALIST_ID tail = A->end + e; // anchor element of list e
  unsigned long *a;

  if ( (PP->problem&(SHD_DFS+SHD_PRUNE)) != SHD_PRUNE ) PP->II.iters2++;
  if ( PP->BA.v && PP->num < PP->OQ[e].t ){  // bitmap version, when bitmap matrix exists, and PP->OQ[e] is larger than crit
    a = &PP->BA.v[e * PP->BA.xend];
    MALIST_DO_FORWARD (*A, PP->FF.clms, j){
     if ( a[j/32]&(1<<(j&31)) ){
        A->list[j] = e;
        QUE_INS (PP->vecjump, A->prv[j]);  // undo list for uncov
        A->nxt[A->prv[j]] = A->nxt[j];
        A->prv[A->nxt[j]] = A->prv[j];
        A->prv[j] = tail;
        A->nxt[tail] = j;
        tail = j;
      }
    }
    MQUE_FLOOP (PP->II.itemset, t){
      MALIST_DO_FORWARD (*A, *t, j){
       if ( a[j/32]&(1<<(j&31)) ){
          A->nxt[A->prv[j]] = A->nxt[j];
          A->prv[A->nxt[j]] = A->prv[j];
          PP->vecchr[j] = 1;
          QUE_INS (PP->veccand, j);  // undo list for crit
        }
      }
    }
  } else {   // set operation version
    MQUE_FLOOP (PP->OQ[e], t){  // loop for hyperedges including e
      if ( A->list[*t] == PP->FF.clms ){  // t is covered by no item in ITEMSET
        A->list[*t] = e;  // insert t to the crit list of e
        QUE_INS (PP->vecjump, A->prv[*t]);  // undo list for uncov
        A->nxt[A->prv[*t]] = A->nxt[*t];
        A->prv[A->nxt[*t]] = A->prv[*t];
        A->prv[*t] = tail;
        A->nxt[tail] = *t;
        tail = *t;
      } else if ( PP->vecchr[*t] ) continue;
      else {
        A->nxt[A->prv[*t]] = A->nxt[*t];
        A->prv[A->nxt[*t]] = A->prv[*t];
        PP->vecchr[*t] = 1;
        QUE_INS (PP->veccand, *t);  // undo list for crit
      }
    }
  }
  PP->num -= PP->veccand.t - PP->veccand.s;  // update #hyperedges in (crit+uncov)
  A->nxt[tail] = A->end + e; A->prv[A->end + e] = tail;
  if ( (PP->problem&(SHD_DFS+SHD_PRUNE)) == SHD_PRUNE ) return(0);
  MQUE_FLOOP (PP->II.itemset, t)
      if ( MALIST_HEAD (*A, *t) >= PP->FF.t ) return (PP->FF.t);
  if ( !(PP->problem&SHD_DFS) ) MQUE_FLOOP (PP->II.itemset, t){
      if ( MALIST_HEAD (*A, *t) > ttt ) return (1);
  }
  return (0);
}


/* recover the update of crit */
void SHD_recov (PROBLEM *PP, QUEUE_INT e){
  QUEUE_INT *x;
  MALIST *A = &PP->occ;
  ALIST_ID tt=MALIST_TAIL(*A, e), ttt=A->prv[tt]; // anchor of list e

  MQUE_SBLOOP (PP->vecjump, x){  // for uncov
    A->list[tt] = PP->FF.clms;
    A->nxt[tt] = A->nxt[*x]; A->nxt[*x] = tt;
    A->prv[tt] = *x; A->prv[A->nxt[tt]] = tt;
    tt = ttt; ttt = A->prv[tt];
  }
  PP->vecjump.t = PP->vecjump.s;
  MQUE_SBLOOP (PP->veccand, x){  // for crit
    A->nxt[A->prv[*x]] = *x; A->prv[A->nxt[*x]] = *x;
    PP->vecchr[*x] = 0;
  }
  PP->num += PP->veccand.t - PP->veccand.s;    // update #hyperedges in (crit+uncov)
  PP->veccand.t = PP->veccand.s;
}


/* main routine for SHD with simple crit update */
void SHD (PROBLEM *PP){
  QUEUE *CAND = &PP->itemcand;
  VEC_ID tt = MALIST_HEAD (PP->occ, PP->FF.clms);
  QUEUE_INT *x, flag, f=PP->problem&SHD_DFS, item;
  QUEUE_ID jt, js=PP->itemjump.s, cs=CAND->s, vs=PP->veccand.s, us=PP->vecjump.s;

    // find the transaction with minimum un-deleted vertices, and check whether there is a hyperedge with no un-deleted vertex
QUEUE_INT i, ii;
if (f){
  item = PP->FF.clms;
  ii=0; MALIST_DO_FORWARD (PP->occ, PP->FF.clms, i){
    flag = 0;
    MQUE_FLOOP (PP->FF.v[i], x) if ( PP->itemchr[*x] == 0 ) flag++;
//    if ( f ){
      if ( flag < item ){ item = flag; tt = i; if ( flag==1 ) break;}
//    } else {
//      if ( flag==0 ){
//        MQUE_FLOOP (PP->FF.v[tt], x) if ( PP->itemchr[*x] == 1 ) PP->itemchr[*x] = 0;
//        goto END;
//      }
//    }
  }
}

//  ITEMSET_output_itemset (&PP->II, NULL, 0);  // output a solution

///////// redundancy counting
//item = 1; MQUE_FLOOP (PP->II.itemset, x) item *= SHD_redundant_check (PP, *x, tt);
//if ( item>1 ){ printf ("redandancy %d %d:::  ", item, tt); QUEUE_print__ (&PP->II.itemset); }
////////

  PP->II.outputs2 += PP->II.itemset.t;
  PP->II.iters++; //
  CAND->s = CAND->t; PP->itemjump.s = PP->itemjump.t;
  PP->veccand.s = PP->veccand.t; PP->vecjump.s = PP->vecjump.t;

  if ( (PP->problem&(SHD_DFS+SHD_PRUNE)) == SHD_PRUNE ) SHD_crit_check (PP, tt);

    // make the list of the vertices to be added (recover the vertex mark, when 0 mode)
PP->II.solutions += PP->FF.v[tt].t;  // counter
  ARY_REALLOCZ (*CAND, PP->FF.v[tt].t + CAND->t, EXIT);
  MQUE_FLOOP (PP->FF.v[tt], x){
    if ( PP->itemchr[*x] == 0 ){
      QUE_INS (*CAND, *x);
      if ( f ) PP->itemchr[*x] = 4;
    } else if ( PP->itemchr[*x] == 1 ) PP->itemchr[*x] = 0;
    else if ( (PP->problem&(SHD_DFS+SHD_PRUNE)) != SHD_PRUNE ) PP->II.iters3++;
  }
  
  for (jt=CAND->t ; jt > CAND->s ; ){
//printf ("call %d: %d\n", *x, PP->itemchr[*x]);
        // update crit //
    item = CAND->v[--jt]; 

    flag = SHD_update (PP, item); //
PP->II.solutions2 += PP->OQ[item].t;  // counter
    if ( flag >= PP->FF.t && (PP->problem&SHD_PRUNE) ){
      PP->itemchr[item] = 4; QUE_INS (PP->itemjump, item);
    } else if ( flag ){ PP->itemchr[item] = 0;
    } else {
      QUE_INS (PP->II.itemset, item); PP->itemchr[item] = 4;
      if ( MALIST_IS_ELM (PP->occ, MALIST_HEAD(PP->occ, PP->FF.clms)) ) SHD (PP); // recursion
      else ITEMSET_output_itemset (&PP->II, NULL, 0);  // output a solution
      PP->itemchr[item] = 0; PP->II.itemset.t--;
    }
    SHD_recov (PP, item);
  } 

  MQUE_SLOOP (PP->itemjump, x) PP->itemchr[*x] = 0;
  CAND->t = CAND->s; CAND->s = cs;
  PP->itemjump.t = PP->itemjump.s; PP->itemjump.s = js;
  PP->veccand.s = vs; PP->vecjump.s = us;
}

/*************************************************************************/
/* SHD for complement input */
/*************************************************************************/

/* check minimality condition */
void SHDC_crit_check (PROBLEM *PP, QUEUE_INT tt, QUEUE_INT prv){
  QUEUE_INT *x = PP->FF.v[tt].v, *t, item, m, u, b=PP->II.itemset.t-1;

  FLOOP (item, 0, PP->FF.clms){
    if ( item == *x ){ x++; continue; }
    if ( PP->itemchr[item] ){ if ( PP->itemchr[*x]==4 ) PP->II.iters3++; continue; }
      PP->II.iters2++;
    u = PP->FF.t; MQUE_FLOOP (PP->II.itemset, t) PP->itemary[*t] = PP->FF.t;
    MQUE_FLOOP (PP->OQ[item], t){
      if ( PP->vecflag[*t] < b ) continue;
      if ( PP->vecflag[*t] == b ){  // for crit
        if ( PP->vecmark[*t] == PP->FF.clms ) ENMIN (PP->itemary[prv], *t);
        else ENMIN (PP->itemary[PP->vecmark[*t]], *t);
      } else ENMIN (u, *t);   // for uncov
    }
    m = 0; MQUE_FLOOP (PP->II.itemset, t) ENMAX (m, PP->itemary[*t]);
    if ( m <= tt ){
      if ( u == PP->FF.t ){
        QUE_INS (PP->II.itemset, item);
        ITEMSET_output_itemset (&PP->II, NULL, 0);
        PP->II.itemset.t--;
        PP->itemchr[item] = 4;
        QUE_INS (PP->itemjump, item);
      }
    } else if ( m >= PP->FF.t ){ PP->itemchr[item] = 4; QUE_INS (PP->itemjump, item); }
    else PP->itemchr[item] = 1;
  }
}

/* crit update for DFS version */
int SHDC_update (PROBLEM *PP, QUEUE_INT e, QUEUE_INT *u, QUEUE_INT *h, QUEUE_INT prv){
  QUEUE_INT *t, z, b=PP->II.itemset.t-1;

  if ( (PP->problem&(SHD_DFS+SHD_PRUNE)) != SHD_PRUNE ) PP->II.iters2++;
  *u = PP->FF.t;
  MQUE_FLOOP (PP->II.itemset, t) PP->itemary[*t] = PP->FF.t;
//fprintf (stderr, "%d::: ", e); QUEUE_print__ (&PP->OQ[e]);
  MQUE_FLOOP (PP->OQ[e], t){
    if ( PP->vecflag[*t] < b ) continue;
    if ( PP->vecflag[*t] == b ){  // for crit
      if ( PP->vecmark[*t] == PP->FF.clms ) PP->vecmark[*t] = prv;
      ENMIN (PP->itemary[PP->vecmark[*t]], *t);
    } else ENMIN (*u, *t);   // for uncov
    PP->vecflag[*t]++;   // increament to write "latest"
  }
  z = 0; MQUE_FLOOP (PP->II.itemset, t) ENMAX (z, PP->itemary[*t]);
//printf ("flag=%d, *u=%d\n", z, *u);
  return (z);
}

/* recover the update of crit, for DFS version (simpler) */
void SHDC_recov (PROBLEM *PP, QUEUE_INT e, QUEUE_INT prv){
  QUEUE_INT *t, b=PP->II.itemset.t;
  MQUE_FLOOP (PP->OQ[e], t){
    if ( PP->vecflag[*t] < b ) continue;
    PP->vecflag[*t]--;
    if ( PP->vecmark[*t] == prv ) PP->vecmark[*t] = PP->FF.clms;
  } 
}

/* main routine for SHD with simple crit update */
void SHDC (PROBLEM *PP, QUEUE_INT prv, VEC_ID tt){
  QUEUE *CAND = &PP->itemcand;
  QUEUE_INT u=0, *x, flag, f=PP->problem&SHD_DFS, item;// *y;
  QUEUE_ID jt, js=PP->itemjump.s, cs=CAND->s; // vt=PP->veccand.t;

// printf ("ITEMSET   "); QUEUE_print__ (&PP->II.itemset);
// printf ("tt=%d\n", tt);
// FLOOP (flag, 0, 6) printf (" %d", PP->itemchr[flag]); printf ("\n");
  PP->II.iters++; //
  CAND->s = CAND->t; PP->itemjump.s = PP->itemjump.t;

  if ( (PP->problem&(SHD_DFS+SHD_PRUNE)) == SHD_PRUNE ) SHDC_crit_check (PP, tt, prv);

  ARY_REALLOCZ (*CAND, (PP->FF.clms-PP->FF.v[tt].t) + CAND->t, EXIT);

PP->II.solutions += PP->FF.clms;  // counter
  x = PP->FF.v[tt].v;
  FLOOP (item, 0, PP->FF.clms){
    if ( item == *x ){ x++; continue; }
    if ( PP->itemchr[item] == 0 ){
      QUE_INS (*CAND, item);
      if ( f ) PP->itemchr[item] = 4;
    } else if ( PP->itemchr[item] == 1 ) PP->itemchr[item] = 0;
    else if ( (PP->problem&(SHD_DFS+SHD_PRUNE)) != SHD_PRUNE ) PP->II.iters3++;
  }

  if ( f ) tt = PP->FF.t-1;
  for ( jt=CAND->t ; jt > CAND->s ; ){
//printf ("call %d: %d\n", *x, PP->itemchr[*x]);
        // update crit //
    item = CAND->v[--jt];
PP->II.solutions2 += PP->OQ[item].t;  // counter
    if ((flag=SHDC_update (PP,item,&u,&PP->OQ[item].t,prv))>=PP->FF.t && (PP->problem&SHD_PRUNE)){
      PP->itemchr[item] = 4; QUE_INS (PP->itemjump, item);
    } else if ( flag > tt ){ PP->itemchr[item] = 0;
    } else {
      QUE_INS (PP->II.itemset, item); PP->itemchr[item] = 4;
      if ( u < PP->FF.t ) SHDC (PP, item, u); // recursion
      else ITEMSET_output_itemset (&PP->II, NULL, 0);  // output a solution
      PP->itemchr[item] = 0; PP->II.itemset.t--;
    }
    SHDC_recov (PP, item, prv);
  } 

  MQUE_SLOOP (PP->itemjump, x) PP->itemchr[*x] = 0;
  CAND->t = CAND->s; CAND->s = cs;
  PP->itemjump.t = PP->itemjump.s; PP->itemjump.s = js; // PP->veccand.t = vt;
}

/* find all hitting set of size 1, and remove them from the data */
void SHD_output_single (PROBLEM *PP){
  QUEUE_INT item, *xx, *x;
  VEC_ID t;

  FLOOP (item, 0, PP->FF.clms){
    if ( ( !(PP->problem&SHD_COMPLEMENT) && PP->OQ[item].t == PP->FF.t ) 
       || ( (PP->problem&SHD_COMPLEMENT) && PP->OQ[item].t == 0) ){
      QUE_INS (PP->II.itemset, item);
      ITEMSET_output_itemset (&PP->II, NULL, 0);  // output a solution
      PP->II.itemset.t--;
      PP->itemchr[item] = 4;  // forbid to add
    }
  }
  FLOOP (t, 0, PP->FF.t){
    xx = PP->FF.v[t].v;
    MQUE_FLOOP (PP->FF.v[t], x){
      if ( PP->itemchr[*x] == 0 ){
        if ( x != xx ) *xx = *x;
        xx++;
      }  
    }
    PP->FF.v[t].t = xx - PP->FF.v[t].v;
  }
}


/*************************************************************************/
/* main of SHD */
/*************************************************************************/
int SHD_main (int argc, char *argv[]){
  PROBLEM PP;
  int f=0;
  QUEUE_INT i;

  PROBLEM_init (&PP);
  SHD_read_param (argc, argv, &PP);
if ( ERROR_MES ) return (1);

  PP.FF.flag |= LOAD_RM_DUP + LOAD_INCSORT;
// + TRSACT_DELIVERY;
  PROBLEM_load (&PP);
if ( ERROR_MES ) goto END;
  if ( PP.problem & SHD_STRAIGHT ) f |= PROBLEM_VECMARK;
  else {
    f |= PROBLEM_ITEMJUMP;
    if ( PP.problem & SHD_COMPLEMENT ) f |= PROBLEM_VECMARK +PROBLEM_VECFLAG +PROBLEM_ITEMARY;
    else f |= PROBLEM_VECCHR + PROBLEM_VECCAND + PROBLEM_VECJUMP;
  }
  PROBLEM_alloc (&PP, PP.FF.clms, PP.FF.t, PP.FF.t, NULL, PROBLEM_ITEMCHR + PROBLEM_ITEMCAND + f);

// delivery
  ARY_FILL (PP.itemcand.v, 0, PP.FF.clms+1, 0);
  QUEUE_delivery(NULL, PP.itemcand.v, NULL, PP.FF.v, NULL, PP.FF.t, PP.FF.clms);
  MQUE_ALLOC (PP.OQ, PP.FF.clms, PP.itemcand.v, 0, 1, EXIT);
  QUEUE_delivery (PP.OQ, NULL, NULL, PP.FF.v, NULL, PP.FF.t, PP.FF.clms);
  FLOOP (i, 0, PP.FF.clms) PP.OQ[i].v[PP.OQ[i].t] = PP.FF.t;  // put endmark 
  if ( (PP.problem&(SHD_DFS+SHD_PRUNE)) == SHD_PRUNE )
      FLOOP (i, 0, PP.FF.t) PP.FF.v[i].end = PP.FF.clms;
  if ( !(PP.problem&SHD_STRAIGHT) ) SHD_output_single (&PP); // output all minimal hitting sets of size one
  PP.num = PP.FF.t; // #hyperedges in ( crit and uncov)

    // initialization for bitmap matrix
  if ( !(PP.problem&(SHD_NO_BITMAP+SHD_COMPLEMENT)) && ((size_t)PP.FF.clms)*PP.FF.t/64 <= PP.FF.eles ){
    BARRAY_init (&PP.BA, PP.FF.t, PP.FF.clms);
    i=0; FLOOP (f, 0, PP.FF.clms){
      BARRAY_set_subset (&PP.BA.v[i], &PP.OQ[f]);
      i += PP.BA.xend;
    }
  }  //      SETFAMILY_to_BARRAY (&PP.BA, &PP.FF);

  if ( (PP.problem & SHD_STRAIGHT) == 0 ){
    if ( PP.problem & SHD_COMPLEMENT ){
      ARY_FILL (PP.vecmark, 0, PP.FF.t, PP.FF.clms);
//      QUEUE_alloc (&PP.veccand, PP.FF.clms);
//      FLOOP (i, 0, PP.FF.clms) QUE_INS (PP.veccand, i);
    } else {
      MALIST_alloc (&PP.occ, PP.FF.t, PP.FF.clms+2); // element=>
      FLOOP (i, 0, PP.FF.t) MALIST_ins_tail (&PP.occ, PP.FF.clms, i, 0);
    }
  }

///////
//FLOOP (i, 0, PP.FF.clms) SHD_redundant_check (&PP, i, PP.FF.t);

//////

// main routines
  if ( PP.problem&SHD_STRAIGHT ){
    if ( PP.problem&SHD_DFS )
      if ( PP.problem&SHD_COMPLEMENT ) SHDdfs_straight_(&PP, PP.FF.clms);
      else SHDdfs_straight (&PP, PP.FF.clms);
    else if ( PP.problem&SHD_COMPLEMENT ) SHDstraight_(&PP, 0); else SHDstraight (&PP, 0);
  } else {
    if ( PP.problem&SHD_COMPLEMENT ) SHDC (&PP, 0, 0);
    else SHD (&PP);
  }

  ITEMSET_last_output (&PP.II);
  print_mes (&PP.II, "#iterations= %lld ,#min_check= %lld ,#pruning= %lld\n", PP.II.iters, PP.II.iters2, PP.II.iters3);
  print_mes (&PP.II, "ave_cost_for_F(e)= %f ,ave_cost_for_CAND= %f ,ave_internal_sol.= %f\n", ((double)PP.II.solutions2)/PP.II.iters, ((double)PP.II.solutions)/PP.II.iters, ((double)PP.II.outputs2)/PP.II.iters);
  END:;
  PROBLEM_end (&PP);

  return (ERROR_MES?1:0);
}

/*******************************************************************************/
#ifndef _NO_MAIN_
#define _NO_MAIN_
int main (int argc, char *argv[]){
  return (SHD_main (argc, argv));
}
#endif
/*******************************************************************************/

#endif


