/*
 * btree.h, part of "ttree" project.
 *
 *  Created on: 09.06.2015, 02:21
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#ifndef BTREE_H_
#define BTREE_H_

#include "../klib/config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _TT_Flags
{
    BT_INSERT_FAST = 0x02,      // BT_insert() does not return added node
    BT_INSERT_IGNORE = 0x04,    // BT_insert() does not replace existing data
    BT_DEFAULTS = (BT_INSERT_FAST)
} BT_Flags;

typedef struct _BTNode
{
    int key;
    int height;
    size_t depth;
    void * data;
    struct _BTNode * right;
    struct _BTNode * left;
}*BTNode;

typedef void (*BT_Destroy)( void * data );
typedef void (*BT_Walk)( BTNode node, void * data );

typedef struct _BTree
{
    BT_Flags flags;
    size_t nodes;
    BT_Destroy destructor;
    BTNode head;
}*BTree;

BTree BT_create( BT_Flags flags, BT_Destroy destructor );
void BT_clear( BTree tree );
void BT_destroy( BTree tree );

size_t BT_depth( BTree tree );

BTNode BT_insert( BTree tree, int key, void * data );
int BT_delete( BTree tree, int key );
BTNode BT_search( BTree tree, int key );

void BT_walk( BTree tree, BT_Walk walker, void * data );
int BT_dump( BTree tree, FILE * handle );
BTree BT_balance( BTree tree );

#ifdef __cplusplus
}
#endif

#endif /* BTREE_H_ */
