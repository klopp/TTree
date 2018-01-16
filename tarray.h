/*
 * tarray.h, part of "trees" project.
 *
 *  Created on: 16.01.2018, 02:21
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

/*
 * AVL-tree based arrays
 */

#ifndef TARRAY_H_
#define  TARRAY_H_

#include "avltree.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _TArray {
    AVLTree tree;
    int error;
    size_t length;
} *TArray;

TArray TA_create( Tree_Flags flags, Tree_Destroy destructor );
void TA_clear( TArray array );
void TA_destroy( TArray array );

AVLNodeConst TA_set( TArray array, size_t idx, void *data );

/*
 * Sets array->error to 0, ENOENT or ERANGE
 */
int TA_del( TArray array, size_t idx );
void *TA_get( TArray array, size_t idx );

#ifdef __cplusplus
};
#endif

#endif
