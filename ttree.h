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
    TT_INSERT_FAST = 0x02,      // TT_insert() does not return added node
    TT_INSERT_IGNORE = 0x04,    // TT_insert() does not replace existing data
    TT_DEFAULTS = (TT_INSERT_FAST)
} TT_Flags;

typedef struct _TT_Data
{
    char * key;
    void * data;
}*TT_Data;

typedef struct _TTNode
{
    char splitter;
    char * key;
    void * data;
    size_t depth;
    struct _TTNode * left;
    struct _TTNode * mid;
    struct _TTNode * right;
}*TTNode;

typedef void (*TT_Destroy)( void * data );
typedef void (*TT_Walk)( TTNode node, void * data );

typedef struct _TernaryTree
{
    TT_Flags flags;
    TT_Destroy destructor;
    TTNode head;
}*TTree;

/*
 *  Create and destroy tree:
 */
TTree TT_create( TT_Flags flags, TT_Destroy destructor );
void TT_clear( TTree tree );
void TT_destroy( TTree tree );

/*
 *  Insert key / data pair. Data may be NULL. On succcess return inserted
 *  node pointer if TT_FAST_INSERT flag is NOT set or tree head pointer. If
 *  TT_INSERT_IGNORE flag is NOT set existing data will be destroyed and
 *  replaced. Return NULL if operation fails.
 */
TTNode TT_insert( TTree tree, const char * key, void * data );

/*
 *  Search tree node with specified key. Return found node pointer or NULL.
 */
TTNode TT_search( TTree tree, const char * key );
/*
 *  Lookup nodes with key started by prefix. Return pointer to allocated
 *  TT_Data array wich must freed by free() or NULL. Last element of
 *  returned array is { NULL, NULL }. Set 'count' to array length if
 *  'count' is not NULL.
 */
TT_Data TT_lookup( TTree tree, const char * prefix, size_t * count );
TT_Data TT_nlookup( TTree tree, const char * prefix, size_t max,
        size_t * count );
/*
 *  Lookup nodes with key started by prefix in new tree. Field 'destructor' in
 *  new tree is NULL.
 */
TTree TT_lookup_tree( TTree tree, const char * prefix );

/*
 *  Delete tree node with specified key. Return 0 if node is not found, or 1.
 */
int TT_delete( TTree tree, const char * key );

/*
 *  Get tree information.
 */
size_t TT_keys( TTree tree );
size_t TT_nodes( TTree tree );
size_t TT_depth( TTree tree );

/*
 *  Get data from tree. Return pointer to allocated TT_Data array (sorted by
 *  key) wich must be freed with free(), or NULL. Last element of returned
 *  array is { NULL, NULL }.
 */
TT_Data TT_data( TTree tree, size_t * count );

/*
 *  Tree walking and dumping stuff.
 */
void TT_walk( TTree tree, TT_Walk wakler, void * data );
void TT_walk_asc( TTree tree, TT_Walk walker, void * data );
void TT_walk_desc( TTree tree, TT_Walk wakler, void * data );
int TT_dump( TTree tree, FILE * handle );

#endif /* TTREE_H_ */
