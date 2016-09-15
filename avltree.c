/*
 * avltree.c, part of "trees" project.
 *
 *  Created on: 09.06.2015, 02:31
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#include "avltree.h"

static int _AVN_height( AVLNode node )
{
    return node ? node->height : 0;
}

#define AVN_bf( node ) _AVN_height( (node)->right ) - _AVN_height( (node)->left )

static void _AVN_seth( AVLNode node )
{
    int hl = _AVN_height( node->left );
    int hr = _AVN_height( node->right );
    node->height = ( hl > hr ? hl : hr ) + 1;
}

static AVLNode _AVN_rotr( AVLNode x )
{
    AVLNode y = x->left;
    x->left = y->right;
    y->right = x;
    _AVN_seth( x );
    _AVN_seth( y );
    return y;
}

static AVLNode _AVN_rotl( AVLNode y )
{
    AVLNode x = y->right;
    y->right = x->left;
    x->left = y;
    _AVN_seth( y );
    _AVN_seth( x );
    return x;
}

static AVLNode _AVN_balance( AVLNode node )
{
    _AVN_seth( node );

    if( AVN_bf( node ) >= 2 /*== 2*/ ) {
        if( AVN_bf( node->right ) < 0 ) {
            node->right = _AVN_rotr( node->right );
        }

        return _AVN_rotl( node );
    }

    if( AVN_bf( node ) <= -2/*== -2*/ ) {
        if( AVN_bf( node->left ) > 0 ) {
            node->left = _AVN_rotl( node->left );
        }

        return _AVN_rotr( node );
    }

    return node;
}

static AVLNode _AVL_balance( AVLNode node )
{
    if( node->left ) {
        node->left = _AVL_balance( node->left );
    }

    if( node->right ) {
        node->right = _AVL_balance( node->right );
    }

    node = _AVN_balance( node );
    return node;
}

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
    tree->error = TE_NO_ERROR;
    return tree;
}

static AVLNode _AVL_min( AVLNode node )
{
    if( node->left ) {
        return _AVL_min( node->left );
    }

    return node;
}

static AVLNode _AVL_del_min( AVLNode node )
{
    if( !node->left ) {
        return node->right;
    }

    node->left = _AVL_del_min( node->left );
    return _AVL_balance( node );
}

static AVLNode _AVL_delete( AVLTree tree, AVLNode *node, TREE_KEY_TYPE key )
{
    if( *node ) {
        AVLNode y = ( *node )->left;
        AVLNode z = ( *node )->right;
        AVLNode m;

        if( tree->destructor && ( *node )->data ) {
            tree->destructor( ( *node )->data );
        }

        Free( *node );
        *node = NULL;
        tree->nodes--;

        if( !z ) {
            return y;
        }

        m = _AVL_min( z );
        m->right = _AVL_del_min( z );
        m->left = y;
        return _AVL_balance( m );
    }

    return NULL;
}

static void _AVL_clear( AVLTree tree, AVLNode *node )
{
    if( *node ) {
        _AVL_clear( tree, &( *node )->left );
        _AVL_clear( tree, &( *node )->right );

        if( tree->destructor && ( *node )->data ) {
            tree->destructor( ( *node )->data );
        }

        Free( *node );
        *node = NULL;
        tree->nodes--;
    }
}

void AVL_clear( AVLTree tree )
{
    if( tree ) {
        __lock( tree->lock );
        _AVL_clear( tree, &tree->head );
        tree->error = TE_NO_ERROR;
        __unlock( tree->lock );
    }
}

void AVL_destroy( AVLTree tree )
{
    __lock( tree->lock );
    _AVL_clear( tree, &tree->head );
    __unlock( tree->lock );
    Free( tree );
}

static AVLNode *_AVL_search( AVLNode *node, TREE_KEY_TYPE key )
{
    if( key < ( *node )->key ) {
        return ( *node )->left ? _AVL_search( &( *node )->left, key ) : NULL;
    }

    if( key > ( *node )->key ) {
        return ( *node )->right ? _AVL_search( &( *node )->right, key ) : NULL;
    }

    return node;
}

AVLNodeConst AVL_search( const AVLTree tree, TREE_KEY_TYPE key )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        AVLNode *node = _AVL_search( &tree->head, key );
        __unlock( tree->lock );

        if( node ) {
            tree->error = TE_NO_ERROR;
            return *node;
        }

        tree->error = TE_NOT_FOUND;
    }

    return NULL;
}

int AVL_delete( const AVLTree tree, TREE_KEY_TYPE key )
{
    int rc = 0;

    if( tree && tree->head ) {
        AVLNode *node;
        __lock( tree->lock );
        node = _AVL_search( &tree->head, key );

        if( node ) {
            *node = _AVL_delete( tree, node, key );
            rc = 1;
        }
        else {
            tree->error = TE_NOT_FOUND;
        }

        __unlock( tree->lock );
        return rc;
    }

    return 0;
}

static AVLNode _AVL_insert( AVLTree tree, AVLNode node, TREE_KEY_TYPE key,
                            void *data, size_t depth )
{
    if( !node ) {
        node = Calloc( sizeof( struct _AVLNode ), 1 );

        if( !node ) {
            tree->error = TE_MEMORY;
            return NULL;
        }

        node->key = key;
        node->data = data;
        node->height = 1;
        tree->error = TE_NO_ERROR;
        return node;
    }

    if( key < node->key ) {
        AVLNode n = _AVL_insert( tree, node->left, key, data, depth + 1 );

        if( n ) {
            node->left = n;
        }
        else {
            return NULL;
        }
    }
    else if( key > node->key ) {
        AVLNode n = _AVL_insert( tree, node->right, key, data, depth + 1 );

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
            tree->error = TE_FOUND;
            return NULL;
        }
    }

    return _AVL_balance( node );
}

AVLNodeConst AVL_insert( const AVLTree tree, TREE_KEY_TYPE key, void *data )
{
    AVLNode node = NULL;

    if( tree ) {
        __lock( tree->lock );
        node = _AVL_insert( tree, tree->head, key, data, 0 );

        if( node ) {
            tree->error = TE_NO_ERROR;
            tree->nodes++;
            tree->head = node;
        }

        __unlock( tree->lock );
    }

    return node;
}

static void _AVL_walk_asc( void *node, AVL_Walk walker, void *data )
{
    if( node ) {
        _AVL_walk_asc( ( ( AVLNode ) node )->left, walker, data );
        walker( node, data );
        _AVL_walk_asc( ( ( AVLNode ) node )->right, walker, data );
    }
}

static void _AVL_walk_desc( void *node, AVL_Walk walker, void *data )
{
    if( node ) {
        _AVL_walk_desc( ( ( AVLNode ) node )->right, walker, data );
        walker( node, data );
        _AVL_walk_desc( ( ( AVLNode ) node )->left, walker, data );
    }
}

void AVL_walk( const AVLTree tree, AVL_Walk walker, void *data )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        _AVL_walk_asc( tree->head, walker, data );
        __unlock( tree->lock );
    }
}

void AVL_walk_desc( const AVLTree tree, AVL_Walk walker, void *data )
{
    if( tree && tree->head ) {
        __lock( tree->lock );
        _AVL_walk_desc( tree->head, walker, data );
        __unlock( tree->lock );
    }
}

static void _AVL_dump( AVLNode node, Tree_KeyDump kdumper,
                       Tree_DataDump ddumper, char *indent, int last,
                       FILE *handle )
{
    size_t strip = T_Indent( indent, last, handle );

    if( kdumper ) {
        kdumper( node->key, handle );
    }
    else {
        fprintf( handle, "[%llX]", ( long long ) node->key );
    }

    if( ddumper ) {
        ddumper( node->data, handle );
    }

    fprintf( handle, "\n" );

    if( node->left ) {
        _AVL_dump( node->left, kdumper, ddumper, indent, !node->right, handle );
    }

    if( node->right ) {
        _AVL_dump( node->right, kdumper, ddumper, indent, 1, handle );
    }

    if( strip ) {
        indent[strip] = 0;
    }
}

static size_t _AVL_depth( AVLNode node, size_t depth )
{
    size_t left, right;

    if( !node ) {
        return depth;
    }

    left = _AVL_depth( node->left, depth + 1 );
    right = _AVL_depth( node->right, depth + 1 );
    return left > right ? left : right;
}

size_t AVL_depth( const AVLTree tree )
{
    size_t rc;
    __lock( tree->lock );
    rc = _AVL_depth( tree->head, 0 );
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
        _AVL_dump( tree->head, kdumper, ddumper, buf, 1, handle );
        __unlock( tree->lock );
        Free( buf );
        return 1;
    }

    return 0;
}

