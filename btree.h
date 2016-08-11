/*
 * btree.h, part of "ttree" project.
 *
 *  Created on: 09.06.2015, 02:21
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

/*
 * Balanced tree with numeric key
 */

#ifndef BTREE_H_
#define BTREE_H_

#include "tree.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _BTNode {
    TREE_KEY_TYPE key;
    int height;
    void *data;
    struct _BTNode *right;
    struct _BTNode *left;
} *BTNode;

typedef void ( *BT_Walk )( BTNode node, void *data );

typedef struct _BTree {
    Tree_Flags flags;
    Tree_Destroy destructor;
    size_t nodes;
    BTNode head;
} *BTree;

BTree BT_create( Tree_Flags flags, Tree_Destroy destructor );
void BT_clear( BTree tree );
void BT_destroy( BTree tree );

size_t BT_depth( BTree tree );

BTNode BT_insert( BTree tree, TREE_KEY_TYPE key, void *data );
int BT_delete( BTree tree, TREE_KEY_TYPE key );
BTNode BT_search( BTree tree, TREE_KEY_TYPE key );

void BT_walk( BTree tree, BT_Walk walker, void *data );
void BT_walk_desc( BTree tree, BT_Walk walker, void *data );
int BT_dump( BTree tree, Tree_KeyDump kdumper, Tree_DataDump ddumper,
             FILE *handle );

#ifdef __cplusplus
}
#endif

#endif /* BTREE_H_ */
