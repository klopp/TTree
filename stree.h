/*
 * stree.h, part of "trees" project.
 *
 *  Created on: 10.06.2015, 02:52
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#ifndef STREE_H_
#define STREE_H_

#include "../klib/config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _ST_Flags
{
    ST_INSERT_IGNORE = 0x01,    // BT_insert() does not replace existing data
    ST_DEFAULTS = (0)
} ST_Flags;

typedef struct _STNode
{
    int key;
    void * data;
    struct _STNode * left;
    struct _STNode * right;
}*STNode;

typedef void (*ST_Destroy)( void * data );
typedef void (*ST_Walk)( STNode node, void * data );
typedef void (*ST_Dump)( void * data, FILE * handle );

typedef struct _STree
{
    ST_Flags flags;
    size_t nodes;
    ST_Destroy destructor;
    STNode head;
}*STree;

STree ST_create( ST_Flags flags, ST_Destroy destructor );
void ST_clear( STree tree );
void ST_destroy( STree tree );
void ST_Free( void * data );

size_t ST_depth( STree tree );

STNode ST_insert( STree tree, int key, void * data );
int ST_delete( STree tree, int key );
STNode ST_search( STree tree, int key );

void ST_walk( STree tree, ST_Walk walker, void * data );
int ST_dump( STree tree, ST_Dump dumper, FILE * handle );


#ifdef __cplusplus
}
#endif

#endif /* STREE_H_ */
