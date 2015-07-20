/*
 * stree.c, part of "trees" project.
 *
 *  Created on: 10.06.2015, 02:58
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#include "stree.h"
#include <limits.h>

STree ST_create( Tree_Flags flags, Tree_Destroy destructor )
{
    STree tree = Calloc( sizeof(struct _STree), 1 );
    if( !tree ) return NULL;
    if( destructor )
    {
        tree->destructor = destructor;
    }
    else if( flags & T_FREE_DEFAULT )
    {
        tree->destructor = T_Free;
    }
    tree->flags = flags;
    return tree;
}

static void _ST_clear( STree tree, STNode * node )
{
    if( *node )
    {
        _ST_clear( tree, &(*node)->left );
        _ST_clear( tree, &(*node)->right );
        if( tree->destructor && (*node)->data ) tree->destructor(
                (*node)->data );
        Free( *node );
        *node = NULL;
        tree->nodes--;
    }
}

void ST_clear( STree tree )
{
    if( tree ) _ST_clear( tree, &tree->head );
}

void ST_destroy( STree tree )
{
    _ST_clear( tree, &tree->head );
    Free( tree );
}

static STNode _rotr( STNode x )
{
    STNode y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

static STNode _rotl( STNode x )
{
    STNode y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

static STNode _ST_insert( STree tree, STNode * node, int key, void * data,
        size_t depth )
{
    if( !*node )
    {
        STNode n = Calloc( sizeof(struct _STNode), 1 );
        if( !n ) return NULL;
        n->key = key;
        n->data = data;
        *node = n;
        return *node;
    }

    if( key < (*node)->key )
    {
        return _ST_insert( tree, &(*node)->left, key, data, depth + 1 );
    }
    else if( key > (*node)->key )
    {
        return _ST_insert( tree, &(*node)->right, key, data, depth + 1 );
    }

    if( tree->flags & T_INSERT_REPLACE )
    {
        if( tree->destructor && (*node)->data ) tree->destructor(
                (*node)->data );
        (*node)->data = data;
        tree->nodes--;
        return *node;
    }
    /*
     * TODO free data?
     */
    return NULL;
}

STNode ST_insert( STree tree, int key, void * data )
{
    if( !tree ) return NULL;
    STNode node = _ST_insert( tree, &tree->head, key, data, 0 );
    if( node ) tree->nodes++;
    return node;
}

/*
 static STNode _ST_search( STNode node, int key )
 {
 struct _STNode header;
 STNode LeftTreeMax = &header;
 STNode RightTreeMin = &header;

 header.left = header.right = NULL;

 while( 1 )
 {
 if( key < node->key )
 {
 if( !node->left ) break;
 if( key < node->left->key )
 {
 node = _rotr( node );
 if( !node->left ) break;
 }
 RightTreeMin->left = node;
 RightTreeMin = RightTreeMin->left;
 node = node->left;
 RightTreeMin->right = NULL;
 }
 else if( key > node->key )
 {
 if( !node->right ) break;
 if( key > node->right->key )
 {
 node = _rotl( node );
 if( !node->right ) break;
 }
 LeftTreeMax->right = node;
 LeftTreeMax = LeftTreeMax->right;
 node = node->right;
 LeftTreeMax->right = NULL;
 }
 else break;
 }
 LeftTreeMax->right = node->left;
 RightTreeMin->left = node->right;
 node->left = header.right;
 node->right = header.left;
 return node;
 }
 */

static STNode * _ST_search( STNode * node, int key )
{
    STNode * workhorse;
    if( !*node || (*node)->key == key ) return node;

    if( (*node)->key > key )
    {
        if( (*node)->left == NULL ) return node;

        if( (*node)->left->key > key )
        {
            workhorse = _ST_search( &(*node)->left->left, key );
            (*node)->left->left = *workhorse;
            *node = _rotr( *node );
        }
        else if( (*node)->left->key < key )
        {
            workhorse = _ST_search( &(*node)->left->right, key );
            (*node)->left->right = *workhorse;

            if( (*node)->left->right != NULL ) (*node)->left = _rotl(
                    (*node)->left );
        }

        if( (*node)->left == NULL ) return node;
        *node = _rotr( *node );
        return node;
    }
    if( (*node)->right == NULL ) return node;

    if( (*node)->right->key > key )
    {
        workhorse = _ST_search( &(*node)->right->left, key );
        (*node)->right->left = *workhorse;

        if( (*node)->right->left != NULL ) (*node)->right = _rotr(
                (*node)->right );
    }
    else if( (*node)->right->key < key )
    {
        workhorse = _ST_search( &(*node)->right->right, key );
        (*node)->right->right = *workhorse;
        *node = _rotl( *node );
    }

    if( (*node)->right == NULL ) return node;
    *node = _rotl( *node );
    return node;
}

STNode ST_search( STree tree, int key )
{
    if( tree && tree->head )
    {
        STNode * node = _ST_search( &tree->head, key );
        if( node && *node && (*node)->key == key ) return *node;
    }
    return NULL;
}

static size_t _ST_depth( STNode node, size_t depth )
{
    size_t left, right;
    if( !node ) return depth;
    left = _ST_depth( node->left, depth + 1 );
    right = _ST_depth( node->right, depth + 1 );
    return left > right ? left : right;
}

size_t ST_depth( STree tree )
{
    return _ST_depth( tree->head, 0 );
}

/*
 static size_t _ST_nodes( STNode node, size_t nodes )
 {
 size_t left, right;
 if( !node ) return nodes;
 left = _ST_depth( node->left, 1 );
 right = _ST_depth( node->right, 1 );
 return left + right;
 }

 static int _ST_max( STNode node, int max )
 {
 if( node )
 {
 int lmax, rmax;
 if( node->key > max ) max = node->key;
 lmax = _ST_max( node->left, max );
 rmax = _ST_max( node->right, max );
 max = rmax > lmax ? rmax : lmax;
 }
 return max;
 }

 static int _ST_min( STNode node, int min )
 {
 if( node )
 {
 int lmin, rmin;
 if( node->key < min ) min = node->key;
 lmin = _ST_min( node->left, min );
 rmin = _ST_min( node->right, min );
 min = lmin < rmin ? lmin : rmin;
 }
 return min;
 }
 */

int ST_delete( STree tree, int key )
{
    if( tree && tree->head )
    {
        STNode *node = _ST_search( &tree->head, key );
        if( node && *node && (*node)->key == key )
        {
            STNode tmp = tree->head;
            if( !tree->head->left )
            {
                tree->head = tree->head->right;
            }
            else
            {
                node = _ST_search( &tree->head->left, key );
                tree->head = *node;
                tree->head->right = tmp->right;
            }
            if( tree->destructor && tmp->data ) tree->destructor( tmp->data );
            Free( tmp );
            tree->nodes--;
        }
        return 1;
    }
    return 0;
}

static void _ST_walk( STNode node, ST_Walk walker, void * data )
{
    if( node )
    {
        _ST_walk( ((STNode)node)->left, walker, data );
        walker( node, data );
        _ST_walk( ((STNode)node)->right, walker, data );
    }
}

void ST_walk( STree tree, ST_Walk walker, void * data )
{
    if( tree && tree->head ) _ST_walk( tree->head, walker, data );
}

static void _ST_dump( STNode node, Tree_Dump dumper, char * indent, int last,
        FILE * handle )
{
    size_t strip = T_Indent( indent, last, handle );

    fprintf( handle, "[%d]", node->key );
    if( dumper ) dumper( node->data, handle );
    fprintf( handle, "\n" );
    if( node->left ) _ST_dump( node->left, dumper, indent, !node->right,
            handle );
    if( node->right ) _ST_dump( node->right, dumper, indent, 1, handle );
    if( strip ) indent[strip] = 0;
}

int ST_dump( STree tree, Tree_Dump dumper, FILE * handle )
{
    if( tree && tree->head )
    {
        size_t depth = ST_depth( tree );
        char * buf = Calloc( depth + 1, 2 );
        if( buf )
        {
            fprintf( handle, "nodes: %zu, depth: %zu\n", tree->nodes, depth );
            _ST_dump( tree->head, dumper, buf, 1, handle );
            Free( buf );
            return 1;
        }
    }
    return 0;
}

