/** \file   bdd_hit.h
 *  \brief  hit operation (and minhit operation by Knuth)
 *  \author Takahisa Toda
 */
#ifndef BDD_HIT_H
#define BDD_HIT_H

#include "bdd_interface.h"

extern bddp hit_z2b(zddp f);
extern zddp minhit_nonsup(zddp f);

#endif /*BDD_HIT_H*/
