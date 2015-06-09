/*
 * ttest.c, examples of TernaryTree usage
 *
 *  Created on: 11.05.2015
 *  Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#if defined(__TTREE_TEST__)

#include "btree.h"
#include "ttree.h"

static void bt_print( BTNode node, void * data )
{
    printf( "%d %s\n", node->key, (char *)node->data );
}

static void bt_dump( void * data, FILE * handle )
{
    fprintf( handle, " %s", (char *)data );
}

static void tt_print( TTNode node, void * data )
{
    printf( "%s => %s\n", node->key, (char *)node->data );
}

int main()
{
    size_t i;
    BTNode node;
    TTree tt = TT_create( TT_DEFAULTS, NULL );
    BTree tree = BT_create( BT_INSERT_IGNORE, BT_Free );

    /*
     TT_insert( tt, "a", "aa" );
     TT_insert( tt, "b", "bb" );
     TT_insert( tt, "c", "cc" );
     TT_dump( tt, stdout );
     TT_walk( tt, tt_print, NULL );
     TT_insert( tt, "c", "dd" );
     TT_dump( tt, stdout );
     TT_walk( tt, tt_print, NULL );
     */
    for( i = 1; i <= 10; i++ )
    {
        char data[16];
        sprintf( data, "%u", i * 10 );
        BT_insert( tree, i * 10, Strdup( data ) );
    }

    BT_insert( tree, 60, Strdup( "000" ) );
    BT_dump( tree, bt_dump, stdout );
    BT_walk( tree, bt_print, NULL );

    BT_destroy( tree );
    TT_destroy( tt );
    return 0;
}

#endif  /* __TTREE_TEST__ */

