/****************************/

/*   ALIST-based UNDO operators for library functions
            12/Apr/2001   by Takeaki Uno  e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* available for ALIST and AGRAPH */

/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, please
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about the codes for the users. */


/* NOTICE: include all libraries before including "undo.c".
 only the libraries include before is considered, and will be availabale. */

#ifndef _undo_c_
#define _undo_c_

#include"undo.h"
#include"base.c"

ALIST_UNDO *ALISTundo_list;
BASE ALISTundo_base;

/* initialization of UNDO list */
void ALISTundo_init (){
  BASE_alloc (&ALISTundo_base, sizeof(ALIST_UNDO), 16384);
  ALISTundo_list = (ALIST_UNDO *)(&ALISTundo_list);
}

/* termination */
void ALISTundo_end (){
  BASE_end (&ALISTundo_base);
}

/* store one operation by inserting to UNDO list */
void ALISTundo_ins ( ALIST_TYPE type, void *s, UNDO_INT i, UNDO_INT j){
  ALIST_UNDO *m;
  m = (ALIST_UNDO *)BASE_new ( &ALISTundo_base);
  m->type = type;
  m->s = s;
  m->i = i;
  m->j = j;
  m->nxt = ALISTundo_list;
  ALISTundo_list = m;
}

/* One undo. return 0 if list is empty, or STOPmark appears, return non-zero otherwise. */
int ALISTundo_iter (){
  ALIST_UNDO *m;
  if ( ALISTundo_list == (ALIST_UNDO *)(&ALISTundo_list) ) return (0);
  m = ALISTundo_list;
  ALISTundo_list = m->nxt;
  switch ( m->type ){
    case U_STOP: BASE_del (&ALISTundo_base, m); return (0);
    case U_MALIST_rm: MALIST_rm ( (MALIST *)(m->s), m->i, 0);
    break; case U_MALIST_ins: MALIST_ins_nxt ( (MALIST *)(m->s), m->i, m->j, 0);
    break; case U_ALIST_rm: ALIST_rm ( (ALIST *)(m->s), m->i, 0);
    break; case U_ALIST_ins: ALIST_ins_nxt ( (ALIST *)(m->s), m->i, m->j, 0);
#ifdef _agraph_h_
    break; case U_AGRAPH_edge_num: ((AGRAPH *)(m->s))->edge_num += m->i;
    break; case U_AGRAPH_arc_num: ((AGRAPH *)(m->s))->arc_num += m->i; break;
#endif
  }
  BASE_del (&ALISTundo_base, m);
  return (1);
}

/* continuously undo until STOPmark appears */
void ALISTundo (){ while ( ALISTundo_iter () ); }

/* clear UNDO list until STOPmark appears, or become empty list */
void ALISTundo_clear (){
  ALIST_UNDO *m;
  while (1){
    if ( ALISTundo_list == (ALIST_UNDO *)(&ALISTundo_list) ) return;
    m = ALISTundo_list;
    ALISTundo_list = m->nxt;
    if ( m->type == U_STOP ){
      BASE_del (&ALISTundo_base, m);
      return;
    }
    BASE_del (&ALISTundo_base, m);
  }
}

/* insert STOPmark in UNDO list */
void ALISTundo_ins_stop (){ ALISTundo_ins (U_STOP, NULL, -1, -1);}

#endif

