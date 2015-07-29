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


#ifndef _undo_h_
#define _undo_h_

#ifdef ALIST_ID_LONG
 #define UNDO_INT LONG
 #define UNDO_INT_END LONGHUGE
#else
 #define UNDO_INT int
 #define UNDO_INT_END INTHUGE
#endif

#include"base.h"
#include"alist.h"

/* structure for Undo information */
typedef struct _ALIST_UNDO {
  struct _ALIST_UNDO *nxt;
  char type;
  void *s;
  UNDO_INT i;
  UNDO_INT j;
} ALIST_UNDO;

/* enuemration type for Undo operations */
typedef enum {
  U_STOP,
  U_MALIST_ins, 
  U_MALIST_rm,
  U_ALIST_ins,
  U_ALIST_rm,
  U_AGRAPH_edge_num,
  U_AGRAPH_arc_num,
} ALIST_TYPE;

/* initialization of UNDO list */
void ALISTundo_init ();

/* termination */
void ALISTundo_end ();

/* store one operation by inserting to UNDO list */
void ALISTundo_ins ( ALIST_TYPE type, void *s, UNDO_INT i, UNDO_INT j);

/* One undo. return 0 if list is empty, or STOPmark appears, return non-zero otherwise. */
int ALISTundo_iter ();

/* continuously undo until STOPmark appears */
void ALISTundo ();

/* clear UNDO list until STOPmark appears, or become empty list */
void ALISTundo_clear ();

/* insert STOPmark in UNDO list */
void ALISTundo_ins_stop ();


#endif

