/*
 * btree.c, part of "ttree" project.
 *
 *  Created on: 09.06.2015, 02:31
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#include "btree.h"

static int _BN_height( BTNode node )
{
    return node ? node->height : 0;
}

#define BN_bf( node ) _BN_height( (node)->right ) - _BN_height( (node)->left )

static void _BN_seth( BTNode node )
{
    int hl = _BN_height( node->left );
    int hr = _BN_height( node->right );
    node->height = ( hl > hr ? hl : hr ) + 1;
}

static BTNode _BN_rotr( BTNode x )
{
    BTNode y = x->left;
    x->left = y->right;
    y->right = x;
    _BN_seth( x );
    _BN_seth( y );
    return y;
}

static BTNode _BN_rotl( BTNode y )
{
    BTNode x = y->right;
    y->right = x->left;
    x->left = y;
    _BN_seth( y );
    _BN_seth( x );
    return x;
}

static BTNode _BN_balance( BTNode node )
{
    _BN_seth( node );

    if( BN_bf( node ) >= 2 /*== 2*/ ) {
        if( BN_bf( node->right ) < 0 ) {
            node->right = _BN_rotr( node->right );
        }

        return _BN_rotl( node );
    }

    if( BN_bf( node ) <= -2/*== -2*/ ) {
        if( BN_bf( node->left ) > 0 ) {
            node->left = _BN_rotl( node->left );
        }

        return _BN_rotr( node );
    }

    return node;
}

static BTNode _BT_balance( BTNode node )
{
    if( node->left ) {
        node->left = _BT_balance( node->left );
    }

    if( node->right ) {
        node->right = _BT_balance( node->right );
    }

    node = _BN_balance( node );
    return node;
}

/*
 BTree BT_balance( BTree tree )
 {
 if( tree && tree->head ) tree->head = _BT_balance( tree->head );
 return tree;
 }
 */

BTree BT_create( Tree_Flags flags, Tree_Destroy destructor )
{
    BTree tree = Calloc( sizeof( struct _BTree ), 1 );

    if( !tree ) {
        return NULL;
    }

    if( destructor ) {
        tree->destructor = destructor;
    }
    else if( flags & T_FREE_DEFAULT ) {
        tree->destructor = T_Free;
    }

    tree->flags = flags;
    __initlock( tree->lock );
    return tree;
}

static BTNode _BN_min( BTNode node )
{
    if( node->left ) {
        return _BN_min( node->left );
    }

    return node;
}

static BTNode _BN_del_min( BTNode node )
{
    if( !node->left ) {
        return node->right;
    }

    node->left = _BN_del_min( node->left );
    return _BN_balance( node );
}

static BTNode _BN_delete( BTree tree, BTNode *node, int key )
{
    if( *node ) {
        BTNode y = ( *node )->left;
        BTNode z = ( *node )->right;
        BTNode m;

        if( tree->destructor && ( *node )->data ) tree->destructor(
                ( *node )->data );

        Free( *node );
        *node = NULL;
        tree->nodes--;

        if( !z ) {
            return y;
        }

        m = _BN_min( z );
        m->right = _BN_del_min( z );
        m->left = y;
        return _BT_balance( m );
    }

    return NULL;
}

static void _BT_clear( BTree tree, BTNode *node )
{
    if( *node ) {
        _BT_clear( tree, &( *node )->left );
        _BT_clear( tree, &( *node )->right );

        if( tree->destructor && ( *node )->data ) tree->destructor(
                ( *node )->data );

        Free( *node );
        *node = NULL;
        tree->nodes--;
    }
}

void BT_clear( BTree tree )
{
    if( tree ) {
        __lock( tree->lock );
        _BT_clear( tree, &tree->head );
        __unlock( tree->lock );
    }
}

void BT_destroy( BTree tree )
{
    __lock( tree->lock );
    _BT_clear( tree, &tree->head );
    __unlock( tree->lock );
    Free( tree );
}

static BTNode *_BT_search( BTNode *node, int key )
{
    if( key < ( *node )->key ) return
            ( *node )->left ? _BT_search( &( *node )->left, key ) : NULL;

    if( key > ( *node )->key ) return
            ( *node )->right ? _BT_search( &( *node )->right, key ) : NULL;

    return node;
}

BTNode BT_search( BTree tree, TREE_KEY_TYPE key )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        BTNode *node = _BT_search( &tree->head, key );
        __unlock( tree->lock );

        if( node ) {
            return *node;
        }
    }

    return NULL;
}

int BT_delete( BTree tree, TREE_KEY_TYPE key )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        BTNode *node = _BT_search( &tree->head, key );

        if( node ) {
            *node = _BN_delete( tree, node, key );
        }

        __unlock( tree->lock );
        return 1;
    }

    return 0;
}

static BTNode _BT_insert( BTree tree, BTNode node, int key, void *data,
                          size_t depth )
{
    if( !node ) {
        node = Calloc( sizeof( struct _BTNode ), 1 );

        if( !node ) {
            return NULL;
        }

        node->key = key;
        node->data = data;
        node->height = 1;
        return node;
    }

    if( key < node->key ) {
        BTNode n = _BT_insert( tree, node->left, key, data, depth + 1 );

        if( n ) {
            node->left = n;
        }
        else {
            return NULL;
        }
    }
    else if( key > node->key ) {
        BTNode n = _BT_insert( tree, node->right, key, data, depth + 1 );

        if( n ) {
            node->right = n;
        }
        else {
            return NULL;
        }
    }
    else {
        if( tree->flags & T_INSERT_REPLACE ) {
            if( tree->destructor && node->data ) {
                tree->destructor( node->data );
            }

            node->data = data;
            tree->nodes--;
        }
        else {
            /*
             * do not free data
             */
            return NULL;
        }
    }

    return _BN_balance( node );
}

BTNode BT_insert( BTree tree, TREE_KEY_TYPE key, void *data )
{
    if( !tree ) {
        return NULL;
    }

    __lock( tree->lock );
    BTNode node = _BT_insert( tree, tree->head, key, data, 0 );

    if( node ) {
        tree->nodes++;
        tree->head = node;
    }

    __unlock( tree->lock );
    return node;
}

static void _BT_walk_asc( void *node, BT_Walk walker, void *data )
{
    if( node ) {
        _BT_walk_asc( ( ( BTNode )node )->left, walker, data );
        walker( node, data );
        _BT_walk_asc( ( ( BTNode )node )->right, walker, data );
    }
}

static void _BT_walk_desc( void *node, BT_Walk walker, void *data )
{
    if( node ) {
        _BT_walk_desc( ( ( BTNode )node )->right, walker, data );
        walker( node, data );
        _BT_walk_desc( ( ( BTNode )node )->left, walker, data );
    }
}

void BT_walk( BTree tree, BT_Walk walker, void *data )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        _BT_walk_asc( tree->head, walker, data );
        __unlock( tree->lock );
    }
}

void BT_walk_desc( BTree tree, BT_Walk walker, void *data )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        _BT_walk_desc( tree->head, walker, data );
        __unlock( tree->lock );
    }
}

static void _BT_dump( BTNode node, Tree_KeyDump kdumper, Tree_DataDump ddumper,
                      char *indent, int last,
                      FILE *handle )
{
    size_t strip = T_Indent( indent, last, handle );

    if( kdumper ) {
        kdumper( node->key, handle );
    }
    else {
        fprintf( handle, "[%llX]", ( long long )node->key );
    }

    if( ddumper ) {
        ddumper( node->data, handle );
    }

    fprintf( handle, "\n" );

    if( node->left ) _BT_dump( node->left, kdumper, ddumper, indent, !node->right,
                                   handle );

    if( node->right ) {
        _BT_dump( node->right, kdumper, ddumper, indent, 1, handle );
    }

    if( strip ) {
        indent[strip] = 0;
    }
}

static size_t _BT_depth( BTNode node, size_t depth )
{
    size_t left, right;

    if( !node ) {
        return depth;
    }

    left = _BT_depth( node->left, depth + 1 );
    right = _BT_depth( node->right, depth + 1 );
    return left > right ? left : right;
}

size_t BT_depth( BTree tree )
{
    size_t rc;
    __lock( tree->lock );
    rc = _BT_depth( tree->head, 0 );
    __unlock( tree->lock );
    return rc;
}

int BT_dump( BTree tree, Tree_KeyDump kdumper, Tree_DataDump ddumper,
             FILE *handle )
{
    size_t depth = BT_depth( tree );
    char *buf = Calloc( depth + 1, 2 );

    if( buf ) {
        fprintf( handle, "nodes: %zu, depth: %zu\n", tree->nodes, depth );
        __lock( tree->lock );
        _BT_dump( tree->head, kdumper, ddumper, buf, 1, handle );
        __unlock( tree->lock );
        Free( buf );
        return 1;
    }

    return 0;
}

