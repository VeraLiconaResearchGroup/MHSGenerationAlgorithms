/** \file     my_io_setfam.h
 *  \brief    IO functions for data files that represet set families.
 *  \author   Takahisa Toda
 *  \par      Requirement for a datafile format of Set Families
 * - Entries must be sorted in increasing order and separated by at least one black character.
 * - No duplicated entries appear in the same row.
 * - Each entry must be a nonzero positive number.
 * - Empty lines and duplicated lines are allowed.
 * - The length of a row must be less than the value indicated by the macro MAX_ROWLEN.
 *
 * \code
 * ------EXAMPLE------
 * 2 4 7 
 * 7 8 
 * 9 
 * 9 10 
 * 4 8 9 
 * 1 2 3 10 
 * 5 6 8 
 * -------------------
 * \endcode
 *
 */
#ifndef MY_IO_SETFAM_H
#define MY_IO_SETFAM_H

#define SF_DELIMITER ' '        //!< delimiter
#define MAX_ROWLEN  (1UL << 20) //!< the maximum length of a row

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/** \brief  A uncompressed data represention for set families
  *  \note
  *  - Let sf point to a setfam data structure.
  *  - sf->elem[i-1][j-1] corresponds to the j-th element in the i-th set. 
  *  - Each set ends with INFTY, that is,
  *  - sf->elem[i-1][k] == INFTY if the i-th set contains exactly k elements.
  */
struct setfam {
  uintmax_t card;      //!< the number of sets with repetition, i.e. cardinality
  uintmax_t memlen;    //!< the length of mem
  itemval   **elem;    //!< double pointer to an element in mem
  itemval   *mem;      //!< memory area (array) of elements, which are partitioned into sets using INFTY as delimiter.
};


/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/
extern int            getfileinfo     (uintmax_t *pn, itemval *pm, uintmax_t *pt, FILE *in);
extern struct setfam  *create_setfam  (uintmax_t n, uintmax_t t, FILE *in);
extern void           destroy_setfam  (struct setfam *sf);


#endif /*MY_IO_SETFAM_H*/
