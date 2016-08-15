/*
 * avltree.c, part of "trees" project.
 *
 *  Created on: 09.06.2015, 02:31
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#include "avltree.h"

static int _BN_height( AVLNode node )
{
    return node ? node->height : 0;
}

#define BN_bf( node ) _BN_height( (node)->right ) - _BN_height( (node)->left )

static void _BN_seth( AVLNode node )
{
    int hl = _BN_height( node->left );
    int hr = _BN_height( node->right );
    node->height = ( hl > hr ? hl : hr ) + 1;
}

static AVLNode _BN_rotr( AVLNode x )
{
    AVLNode y = x->left;
    x->left = y->right;
    y->right = x;
    _BN_seth( x );
    _BN_seth( y );
    return y;
}

static AVLNode _BN_rotl( AVLNode y )
{
    AVLNode x = y->right;
    y->right = x->left;
    x->left = y;
    _BN_seth( y );
    _BN_seth( x );
    return x;
}

static AVLNode _BN_balance( AVLNode node )
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

static AVLNode _BT_balance( AVLNode node )
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

AVLTree AVL_create( Tree_Flags flags, Tree_Destroy destructor )
{
    AVLTree tree = Calloc( sizeof( struct _AVLTree ), 1 );

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

static AVLNode _BN_min( AVLNode node )
{
    if( node->left ) {
        return _BN_min( node->left );
    }

    return node;
}

static AVLNode _BN_del_min( AVLNode node )
{
    if( !node->left ) {
        return node->right;
    }

    node->left = _BN_del_min( node->left );
    return _BN_balance( node );
}

static AVLNode _BN_delete( AVLTree tree, AVLNode *node, int key )
{
    if( *node ) {
        AVLNode y = ( *node )->left;
        AVLNode z = ( *node )->right;
        AVLNode m;

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

static void _BT_clear( AVLTree tree, AVLNode *node )
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

void AVL_clear( AVLTree tree )
{
    if( tree ) {
        __lock( tree->lock );
        _BT_clear( tree, &tree->head );
        __unlock( tree->lock );
    }
}

void AVL_destroy( AVLTree tree )
{
    __lock( tree->lock );
    _BT_clear( tree, &tree->head );
    __unlock( tree->lock );
    Free( tree );
}

static AVLNode *_BT_search( AVLNode *node, int key )
{
    if( key < ( *node )->key ) return
            ( *node )->left ? _BT_search( &( *node )->left, key ) : NULL;

    if( key > ( *node )->key ) return
            ( *node )->right ? _BT_search( &( *node )->right, key ) : NULL;

    return node;
}

AVLNode AVL_search( AVLTree tree, TREE_KEY_TYPE key )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        AVLNode *node = _BT_search( &tree->head, key );
        __unlock( tree->lock );

        if( node ) {
            return *node;
        }
    }

    return NULL;
}

int AVL_delete( AVLTree tree, TREE_KEY_TYPE key )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        AVLNode *node = _BT_search( &tree->head, key );

        if( node ) {
            *node = _BN_delete( tree, node, key );
        }

        __unlock( tree->lock );
        return 1;
    }

    return 0;
}

static AVLNode _BT_insert( AVLTree tree, AVLNode node, int key, void *data,
                           size_t depth )
{
    if( !node ) {
        node = Calloc( sizeof( struct _AVLNode ), 1 );

        if( !node ) {
            return NULL;
        }

        node->key = key;
        node->data = data;
        node->height = 1;
        return node;
    }

    if( key < node->key ) {
        AVLNode n = _BT_insert( tree, node->left, key, data, depth + 1 );

        if( n ) {
            node->left = n;
        }
        else {
            return NULL;
        }
    }
    else if( key > node->key ) {
        AVLNode n = _BT_insert( tree, node->right, key, data, depth + 1 );

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

AVLNode AVL_insert( AVLTree tree, TREE_KEY_TYPE key, void *data )
{
    if( !tree ) {
        return NULL;
    }

    __lock( tree->lock );
    AVLNode node = _BT_insert( tree, tree->head, key, data, 0 );

    if( node ) {
        tree->nodes++;
        tree->head = node;
    }

    __unlock( tree->lock );
    return node;
}

static void _BT_walk_asc( void *node, AVL_Walk walker, void *data )
{
    if( node ) {
        _BT_walk_asc( ( ( AVLNode )node )->left, walker, data );
        walker( node, data );
        _BT_walk_asc( ( ( AVLNode )node )->right, walker, data );
    }
}

static void _BT_walk_desc( void *node, AVL_Walk walker, void *data )
{
    if( node ) {
        _BT_walk_desc( ( ( AVLNode )node )->right, walker, data );
        walker( node, data );
        _BT_walk_desc( ( ( AVLNode )node )->left, walker, data );
    }
}

void AVL_walk( AVLTree tree, AVL_Walk walker, void *data )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        _BT_walk_asc( tree->head, walker, data );
        __unlock( tree->lock );
    }
}

void AVL_walk_desc( AVLTree tree, AVL_Walk walker, void *data )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        _BT_walk_desc( tree->head, walker, data );
        __unlock( tree->lock );
    }
}

static void _BT_dump( AVLNode node, Tree_KeyDump kdumper, Tree_DataDump ddumper,
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

static size_t _BT_depth( AVLNode node, size_t depth )
{
    size_t left, right;

    if( !node ) {
        return depth;
    }

    left = _BT_depth( node->left, depth + 1 );
    right = _BT_depth( node->right, depth + 1 );
    return left > right ? left : right;
}

size_t AVL_depth( AVLTree tree )
{
    size_t rc;
    __lock( tree->lock );
    rc = _BT_depth( tree->head, 0 );
    __unlock( tree->lock );
    return rc;
}

int AVL_dump( AVLTree tree, Tree_KeyDump kdumper, Tree_DataDump ddumper,
              FILE *handle )
{
    size_t depth = AVL_depth( tree );
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

