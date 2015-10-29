/** \file     bdd_io_setfam.h
 *  \brief    IO functions for data files that represent set families.
 *  \author   Takahisa Toda
 */
#ifndef BDD_IO_SETFAM_H
#define BDD_IO_SETFAM_H

#include "bdd_interface.h"
#include "my_io_setfam.h"

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

extern  zddp  zcomp   (uintmax_t row_count, uintmax_t entry_count, FILE *in);
extern  int   unzcomp (itemval max, zddp f, FILE *out);

#endif /*BDD_IO_SETFAM_H*/
