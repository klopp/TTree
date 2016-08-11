/*
 *  Created on: 11 авг. 2016 г.
 *      Author: klopp
 */

/*
 *  That's All, Folks!
 */
#ifndef HTABLE_H_
#define HTABLE_H_

#include "btree.h"

/*
 * Hash table, based on Balanced Trees
 */

#if defined(__cplusplus)
  extern "C" {
#endif

typedef struct _HTable
{
   BTree bt;
   size_t key_size;

} *HTable;

HTable HT_create( Tree_Flags flags, Tree_Destroy destructor, size_t key_size );
void HT_clear( HTable ht );
void HT_destroy( HTable ht );

unsigned int HT_set( HTable ht, void *key, void *data );
void *HT_get( HTable ht, void *key);
int HT_delete( HTable ht, void *key);

#if defined(__cplusplus)
  }; /* extern "C" */
#endif

#endif /* HTABLE_H_ */
