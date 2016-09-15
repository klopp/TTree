/*
 * avltree.h, part of "trees" project.
 *
 *  Created on: 09.06.2015, 02:21
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

/*
 * Balanced tree with numeric key
 */

#ifndef AVLTREE_H_
#define AVLTREE_H_

#include "tree.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _AVLNode {
    TREE_KEY_TYPE key;
    int height;
    void *data;
    struct _AVLNode *right;
    struct _AVLNode *left;
} *AVLNode;

typedef struct _AVLNode const *AVLNodeConst;

typedef void ( *AVL_Walk )( const AVLNodeConst node, void *data );

typedef struct _AVLTree {
    Tree_Flags flags;
    Tree_Destroy destructor;
    size_t nodes;
    AVLNode head;
    Tree_Error error;
    __lock_t( lock );
} *AVLTree;

AVLTree AVL_create( Tree_Flags flags, Tree_Destroy destructor );
void AVL_clear( AVLTree tree );
void AVL_destroy( AVLTree tree );

size_t AVL_depth( const AVLTree tree );

AVLNodeConst AVL_insert( const AVLTree tree, TREE_KEY_TYPE key, void *data );
int AVL_delete( const AVLTree tree, TREE_KEY_TYPE key );
AVLNodeConst  AVL_search( AVLTree tree, TREE_KEY_TYPE key );

void AVL_walk( const AVLTree tree, AVL_Walk walker, void *data );
void AVL_walk_desc( const AVLTree tree, AVL_Walk walker, void *data );
int AVL_dump( const AVLTree tree, Tree_KeyDump kdumper, Tree_DataDump ddumper,
              FILE *handle );

#ifdef __cplusplus
}
#endif

#endif /* AVLTREE_H_ */
