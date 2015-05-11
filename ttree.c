/*
 * ttree.c, part of "ttree" project
 *
 *  Created on: 10.05.2015
 *  Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */
#include "ttree.h"
#include <string.h>
#include <ctype.h>

/*
 *  Internal, create empty node:
 */
static TernaryTreeNode _TT_create_node( char c )
{
    TernaryTreeNode node = (TernaryTreeNode)calloc(
            sizeof(struct _TernaryTreeNode), 1 );
    if( !node ) return NULL;

    node->splitter = c;
    node->left = node->right = node->mid = NULL;
    return node;
}

/*
 *  Create empty tree:
 */
TernaryTree TT_create( TT_Flags flags, TT_Destroy destroyer )
{
    TernaryTree tree = calloc( sizeof(struct _TernaryTree), 1 );
    if( tree )
    {
        tree->head = _TT_create_node( 0 );
        if( tree->head )
        {
            tree->flags = flags;
            tree->destroyer = destroyer;
            return tree;
        }
        free( tree );
    }
    return NULL;
}

/*
 *  Destroy tree / delete tree node stuff:
 */
static void _TT_destroy( TernaryTreeNode node, TT_Destroy destroyer )
{
    if( node->left ) _TT_destroy( node->left, destroyer );
    if( node->mid ) _TT_destroy( node->mid, destroyer );
    if( node->right ) _TT_destroy( node->right, destroyer );
    if( node->data && destroyer ) destroyer( node->data );
    if( node->key ) memset( node->key, 0, strlen( node->key ) );
    free( node->key );
    memset( node, 0, sizeof(struct _TernaryTreeNode) );
    free( node );
}
void TT_destroy( TernaryTree tree )
{
    _TT_destroy( tree->head, tree->destroyer );
    memset( tree, 0, sizeof(struct _TernaryTree) );
    free( tree );
}
int TT_delete( TernaryTree tree, const char * key )
{
    struct _TernaryTreeNode * node = TT_search( tree, key );
    if( node )
    {
        _TT_destroy( node, tree->destroyer );
        return 1;
    }
    return 0;
}

/*
 *  Search nodes stuff:
 */
static TernaryTreeNode _TT_search( TernaryTreeNode node, const char *s,
        TT_Flags flags )
{
    TernaryTreeNode ptr = node;

    while( *s && (ptr && (ptr->splitter || ptr->key)) )
    {
        char c = (flags & TT_NOCASE) ? tolower( *s ) : *s;
        if( c < ptr->splitter )
        {
            ptr = ptr->left;
        }
        else if( c > ptr->splitter )
        {
            ptr = ptr->right;
        }
        else
        {
            s++;
            if( *s ) ptr = ptr->mid;
        }
    }
    return (ptr && ptr->key) ? ptr : NULL;
}
TernaryTreeNode TT_search( TernaryTree tree, const char *s )
{
    if( !tree || !tree->head || !s || !*s ) return NULL;
    return _TT_search( tree->head->mid, s, tree->flags );
}

/*
 *  Insert nodes stuff:
 */
static TernaryTreeNode _TT_insert( TernaryTreeNode node, const char *s,
        size_t pos, void * data, TT_Flags flags, TT_Destroy destroyer )
{
    char c = (flags & TT_NOCASE) ? tolower( s[pos] ) : s[pos];

    if( !node || (!node->splitter && !node->key) )
    {
        node = _TT_create_node( c );
        if( !node ) return NULL;
    }

    if( c < node->splitter ) node->left = _TT_insert( node->left, s, pos, data,
            flags, destroyer );
    if( c == node->splitter )
    {
        if( *(s + pos + 1) > 0 )
        {
            node->mid = _TT_insert( node->mid, s, pos + 1, data, flags,
                    destroyer );
        }
        else
        {
            if( !node->key ) node->key = strdup( s );
            if( !node->key ) return NULL;
            if( !(flags & TT_INSERT_IGNORE) )
            {
                if( node->data && destroyer ) destroyer( node->data );
                node->data = data;
            }
        }
    }
    if( c > node->splitter ) node->right = _TT_insert( node->right, s, pos,
            data, flags, destroyer );
    return node;
}
TernaryTreeNode TT_insert( TernaryTree tree, const char *s, void * data )
{
    if( !tree || !tree->head || !s || !*s ) return NULL;
    tree->head->mid = _TT_insert( tree->head->mid, s, 0, data, tree->flags,
            tree->destroyer );

    if( !tree->head->mid ) return NULL;
    return (tree->flags & TT_FAST_INSERT) ?
            tree->head : _TT_search( tree->head->mid, s, tree->flags );
}

/*
 *  Tree walking stuff:
 */
static void _TT_walk( TernaryTreeNode node, TT_Walk walker, void * data )
{
    if( node )
    {
        _TT_walk( node->left, walker, data );
        _TT_walk( node->mid, walker, data );
        _TT_walk( node->right, walker, data );
        walker( node, data );
    }
}
static void _TT_reverse_walk( TernaryTreeNode node, TT_Walk walker,
        void * data )
{
    if( node )
    {
        _TT_reverse_walk( node->right, walker, data );
        _TT_reverse_walk( node->mid, walker, data );
        _TT_reverse_walk( node->left, walker, data );
        walker( node, data );
    }
}
void TT_walk( TernaryTree tree, TT_Walk walker, void * data )
{
    if( tree ) _TT_walk( tree->head, walker, data );
}
void TT_reverse_walk( TernaryTree tree, TT_Walk walker, void * data )
{
    if( tree ) _TT_reverse_walk( tree->head, walker, data );
}

/*
 *  Tree information stuff, get:
 *   - tree depth
 *   - nodes number
 *   - keys number
 */
static void _TT_keys( TernaryTreeNode node, void * data )
{
    if( node->key ) (*(size_t*)data)++;
}
static void _TT_nodes( TernaryTreeNode node, void * data )
{
    (*(size_t*)data)++;
}
static void _TT_depth( TernaryTreeNode node, void * data )
{
    if( node )
    {
        struct
        {
            size_t count;
            size_t max;
        }* ptr = data;
        ptr->count++;
        if( ptr->max < ptr->count ) ptr->max = ptr->count;
        _TT_depth( node->left, data );
        _TT_depth( node->mid, data );
        _TT_depth( node->right, data );
        ptr->count--;
    }
}
size_t TT_depth( TernaryTree tree )
{
    struct
    {
        size_t count;
        size_t max;
    } data =
    { 0, 0 };
    TT_walk( tree, _TT_depth, &data );
    return data.max;
}
size_t TT_keys( TernaryTree tree )
{
    size_t keys = 0;
    TT_walk( tree, _TT_keys, &keys );
    return keys;
}
size_t TT_nodes( TernaryTree tree )
{
    size_t nodes = 0;
    TT_walk( tree, _TT_nodes, &nodes );
    return nodes;
}

/*
 *  Get sorted data from tree:
 */
static void _TT_sorted_data( TernaryTreeNode node, void * data )
{
    if( node->key )
    {
        struct
        {
            size_t idx;
            TT_Data data;
        }*ptr = data;
        ptr->data[ptr->idx].key = node->key;
        ptr->data[ptr->idx].data = node->data;
        ptr->idx++;
    }
}
TT_Data TT_sorted_data( TernaryTree tree )
{
    struct
    {
        size_t idx;
        TT_Data data;
    } data =
    { 0 };
    size_t i = 0;
    size_t keys = TT_keys( tree );

    data.data = calloc( sizeof(struct _TT_Data), keys );
    if( !data.data ) return NULL;
    TT_reverse_walk( tree, _TT_sorted_data, &data );

    while( i < --keys )
    {
        char * k = data.data[i].key;
        void * d = data.data[i].data;
        data.data[i].key = data.data[keys].key;
        data.data[i].data = data.data[keys].data;
        data.data[keys].key = k;
        data.data[keys].data = d;
        i++;
    }

    return data.data;
}

/*
 *  Dump tree stuff:
 */
static void _TT_dump( TernaryTreeNode node, char * indent, int last,
        FILE * handle )
{
    if( node->splitter )
    {
        fprintf( handle, "%s", indent );

        if( last )
        {
            fprintf( handle, "+-" );
            strcat( indent, "  " );
        }
        else
        {
            fprintf( handle, "|-" );
            strcat( indent, "| " );
        }

        if( node->key )
        {
            fprintf( handle, "%c => [%s]\n",
                    (isprint(node->splitter) ? node->splitter : '?'),
                    node->key );
        }
        else
        {
            fprintf( handle, "%c => ()\n",
                    (isprint(node->splitter) ? node->splitter : '?') );
        }
    }
    if( node->left ) _TT_dump( node->left, indent,
            (node->right || node->mid) ? 0 : 1, handle );
    if( node->mid ) _TT_dump( node->mid, indent, node->right ? 0 : 1, handle );
    if( node->right ) _TT_dump( node->right, indent, 1, handle );
    indent[strlen( indent ) - 2] = 0;
}
void TT_dump( TernaryTree tree, FILE * handle )
{
    size_t depth = TT_depth( tree );
    char * buf = calloc( depth + 1, 2 );
    if( buf )
    {
        fprintf( handle, "nodes: %u, keys: %u\n", TT_nodes( tree ),
                TT_keys( tree ) );
        _TT_dump( tree->head, buf, 1, handle );
        free( buf );
    }
}
