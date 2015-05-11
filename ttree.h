/*
 * ttree.h, part of "ttree" project
 *
 *  Created on: 10.05.2015
 *  Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#ifndef TTREE_H_
#define TTREE_H_

#include <stdlib.h>
#include <stdio.h>

typedef enum _TT_Flags
{
    TT_NOCASE = 0x01,           // ignore case for keys
    TT_FAST_INSERT = 0x02,      // TT_insert() does not return added node
    TT_INSERT_IGNORE = 0x04,    // TT_insert() does not replace existing data
    TT_DEFAULTS = (TT_FAST_INSERT)
} TT_Flags;

typedef struct _TT_Data
{
    char * key;
    void * data;
} *TT_Data;

typedef struct _TernaryTreeNode
{
    char splitter;
    char * key;
    void * data;
    struct _TernaryTreeNode * left;
    struct _TernaryTreeNode * mid;
    struct _TernaryTreeNode * right;
}*TernaryTreeNode;

typedef void (*TT_Destroy)( void * data );
typedef void (*TT_Walk)( TernaryTreeNode node, void * data );

typedef struct _TernaryTree
{
    TT_Flags flags;
    TT_Destroy destroyer;
    TernaryTreeNode head;
}*TernaryTree;

/*
 *  Create and destroy tree:
 */
TernaryTree TT_create( TT_Flags flags, TT_Destroy destroyer );
void TT_destroy( TernaryTree tree );

/*
 *  Insert key / data pair. Data may be NULL. On succcess return inserted
 *  node pointer if TT_FAST_INSERT flag is NOT set or tree head pointer. If
 *  TT_INSERT_IGNORE flag is NOT set existing data will be destroyed and
 *  replaced. Return NULL if operation fails.
 */
TernaryTreeNode TT_insert( TernaryTree tree, const char * key, void * data );
/*
 *  Search tree node with specified key. Return found node pointer or NULL.
 */
TernaryTreeNode TT_search( TernaryTree tree, const char * key );
/*
 *  Delete tree node with specified key. Return 0 if node is not found, or 1.
 */
int TT_delete( TernaryTree tree, const char * key );

/*
 *  Get tree information.
 */
size_t TT_keys( TernaryTree tree );
size_t TT_nodes( TernaryTree tree );
size_t TT_depth( TernaryTree tree );

/*
 *  Get sorted data from tree. Return pointer to allocated TT_Data array
 *  wich must be freed with free(), or NULL.
 */
TT_Data TT_sorted_data( TernaryTree tree );

/*
 *  Tree walking and dumping stuff.
 */
void TT_walk( TernaryTree tree, TT_Walk wakler, void * data );
void TT_reverse_walk( TernaryTree tree, TT_Walk wakler, void * data );
void TT_dump( TernaryTree tree, FILE * handle );

#endif /* TTREE_H_ */
