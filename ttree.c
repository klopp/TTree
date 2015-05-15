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
    TernaryTreeNode node = calloc( sizeof(struct _TernaryTreeNode), 1 );
    if( !node ) return NULL;
    node->splitter = c;
    return node;
}

/*
 *  Create empty tree:
 */
TernaryTree TT_create( TT_Flags flags, TT_Destroy destructor )
{
    TernaryTree tree = calloc( sizeof(struct _TernaryTree), 1 );
    if( tree )
    {
        tree->head = _TT_create_node( 0 );
        if( tree->head )
        {
            tree->flags = flags;
            tree->destructor = destructor;
            return tree;
        }
        free( tree );
    }
    return NULL;
}

/*
 *  Destroy tree / delete tree node stuff:
 */
static void _TT_destroy( TernaryTreeNode node, TT_Destroy destructor,
        int delnode )
{
    if( node->left ) _TT_destroy( node->left, destructor, delnode );
    if( node->mid ) _TT_destroy( node->mid, destructor, delnode );
    if( node->right ) _TT_destroy( node->right, destructor, delnode );
    if( node->data && destructor ) destructor( node->data );
    free( node->key );
    memset( node, 0, sizeof(struct _TernaryTreeNode) );
    if( delnode ) free( node );
}
void TT_destroy( TernaryTree tree )
{
    if( tree->head ) _TT_destroy( tree->head, tree->destructor, 1 );
    memset( tree, 0, sizeof(struct _TernaryTree) );
    free( tree );
}
void TT_clear( TernaryTree tree )
{
    _TT_destroy( tree->head->mid, tree->destructor, 1 );
    tree->head->mid = NULL;
}
int TT_delete( TernaryTree tree, const char * key )
{
    struct _TernaryTreeNode * node = TT_search( tree, key );
    if( node )
    {
        _TT_destroy( node, tree->destructor, 0 );
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
        size_t pos, void * data, TT_Flags flags, TT_Destroy destructor,
        size_t depth )
{
    char c = (flags & TT_NOCASE) ? tolower( s[pos] ) : s[pos];

    if( !node )
    {
        node = _TT_create_node( c );
        if( !node ) return NULL;
        node->depth = depth;
    }
    else if( !node->splitter && !node->key )
    {
        memset( node, 0, sizeof(struct _TernaryTreeNode) );
        node->splitter = c;
        node->depth = depth;
    }

    if( c < node->splitter ) node->left = _TT_insert( node->left, s, pos, data,
            flags, destructor, depth + 1 );
    if( c == node->splitter )
    {
        if( *(s + pos + 1) > 0 )
        {
            node->mid = _TT_insert( node->mid, s, pos + 1, data, flags,
                    destructor, depth + 1 );
        }
        else
        {
            if( !node->key ) node->key = strdup( s );
            if( !node->key ) return NULL;
            if( !(flags & TT_INSERT_IGNORE) )
            {
                if( node->data && destructor ) destructor( node->data );
                node->data = data;
            }
        }
    }
    if( c > node->splitter ) node->right = _TT_insert( node->right, s, pos,
            data, flags, destructor, depth + 1 );
    return node;
}
TernaryTreeNode TT_insert( TernaryTree tree, const char *s, void * data )
{
    if( !tree || !tree->head || !s || !*s ) return NULL;
    tree->head->mid = _TT_insert( tree->head->mid, s, 0, data, tree->flags,
            tree->destructor, 1 );

    if( !tree->head->mid ) return NULL;
    return (tree->flags & TT_INSERT_FAST) ?
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
static void _TT_walk_asc( TernaryTreeNode node, TT_Walk walker, void * data )
{
    if( node )
    {
        walker( node, data );
        _TT_walk_asc( node->left, walker, data );
        _TT_walk_asc( node->mid, walker, data );
        _TT_walk_asc( node->right, walker, data );
    }
}
static void _TT_walk_desc( TernaryTreeNode node, TT_Walk walker, void * data )
{
    if( node )
    {
        _TT_walk_desc( node->right, walker, data );
        _TT_walk_desc( node->mid, walker, data );
        _TT_walk_desc( node->left, walker, data );
        walker( node, data );
    }
}
void TT_walk( TernaryTree tree, TT_Walk walker, void * data )
{
    if( tree ) _TT_walk( tree->head, walker, data );
}
void TT_walk_asc( TernaryTree tree, TT_Walk walker, void * data )
{
    if( tree ) _TT_walk_asc( tree->head, walker, data );
}
void TT_walk_desc( TernaryTree tree, TT_Walk walker, void * data )
{
    if( tree ) _TT_walk_desc( tree->head, walker, data );
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
    if( node->depth > *(size_t*)data ) (*(size_t*)data) = node->depth;
}
size_t TT_depth( TernaryTree tree )
{
    size_t max = 0;
    if( tree ) TT_walk( tree, _TT_depth, &max );
    return max;
}
size_t TT_keys( TernaryTree tree )
{
    size_t keys = 0;
    if( tree ) TT_walk( tree, _TT_keys, &keys );
    return keys;
}
size_t TT_nodes( TernaryTree tree )
{
    size_t nodes = 0;
    if( tree ) TT_walk( tree, _TT_nodes, &nodes );
    return nodes;
}

/*
 *  Get sorted data from tree:
 */
static void _TT_data( TernaryTreeNode node, void * data )
{
    if( node->key )
    {
        struct
        {
            size_t max;
            size_t idx;
            TT_Data data;
        }*ptr = data;
        if( ptr->idx < ptr->max )
        {
            ptr->data[ptr->idx].key = node->key;
            ptr->data[ptr->idx].data = node->data;
            ptr->idx++;
        }
    }
}
TT_Data TT_data( TernaryTree tree, size_t * count )
{
    struct
    {
        size_t max;
        size_t idx;
        TT_Data data;
    } data =
    { 0 };
    size_t keys;

    if( count ) *count = 0;
    if( !tree ) return NULL;

    keys = TT_keys( tree );
    if( !keys ) return NULL;
    data.data = calloc( sizeof(struct _TT_Data), keys + 1 );
    if( !data.data ) return NULL;
    data.max = ((size_t)-1);
    TT_walk_asc( tree, _TT_data, &data );
    if( count ) *count = data.idx;
    return data.data;
}

/*
 *  Dump tree stuff:
 */
static void _TT_dump( TernaryTreeNode node, char * indent, int last,
        FILE * handle )
{
    int strip = 0;
    if( node->splitter )
    {
        fprintf( handle, "%s", indent );
        strip = strlen( indent );
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
    if( strip ) indent[strip] = 0;
}
int TT_dump( TernaryTree tree, FILE * handle )
{
    size_t depth = TT_depth( tree );
    char * buf = calloc( depth + 1, 2 );
    if( buf )
    {
        fprintf( handle, "nodes: %u, keys: %u, depth: %u\n", TT_nodes( tree ),
                TT_keys( tree ), TT_depth( tree ) );
        _TT_dump( tree->head, buf, 0, handle );
        free( buf );
        return 1;
    }
    return 0;
}

/*
 *  Lookup stuff:
 */
static TernaryTreeNode __TT_lookup( TernaryTreeNode node, const char *s,
        TT_Flags flags )
{
    TernaryTreeNode ptr = node;

    while( *s && (ptr && ptr->splitter) )
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
    return ptr;
}
TT_Data _TT_lookup( TernaryTree tree, const char * prefix, size_t max,
        size_t * count )
{
    TernaryTreeNode node;
    struct
    {
        size_t max;
        size_t idx;
        TT_Data data;
    } data =
    { 0 };

    if( count ) *count = 0;

    if( !tree || !prefix || !*prefix ) return NULL;
    node = __TT_lookup( tree->head->mid, prefix, tree->flags );
    if( !node || !node->mid ) return NULL;

    if( !max ) max = TT_keys( tree );

    data.data = calloc( sizeof(struct _TT_Data), max + 1 );
    if( !data.data ) return NULL;

    data.max = max;
    _TT_walk( node->mid, _TT_data, &data );
    if( count ) *count = data.idx;

    return data.data;
}

TT_Data TT_lookup( TernaryTree tree, const char * prefix, size_t * count )
{
    return _TT_lookup( tree, prefix, 0, count );
}
TT_Data TT_nlookup( TernaryTree tree, const char * prefix, size_t max,
        size_t * count )
{
    return _TT_lookup( tree, prefix, max, count );
}

TernaryTree TT_lookup_tree( TernaryTree tree, const char * prefix )
{
    TernaryTree rc = TT_create( tree->flags, NULL );
    TT_Data data, ptr;
    if( !rc ) return NULL;
    data = TT_lookup( tree, prefix, NULL );
    ptr = data;
    while( ptr && ptr->key )
    {
        TT_insert( rc, ptr->key, ptr->data );
        ptr++;
    }
    free( data );

    return rc;
}

