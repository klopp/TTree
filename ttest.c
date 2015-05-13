/*
 * ttest.c, examples of TernaryTree usage
 *
 *  Created on: 11.05.2015
 *  Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */
#include "ttree.h"

static void t_print( TernaryTreeNode node, void * data )
{
    if( node->key ) printf( "%s\n", node->key );
}

int main()
{
    TT_Data data;
    TT_Data ptr;
    size_t keys, i;
    TernaryTreeNode node;
    TernaryTree tree = TT_create( TT_DEFAULTS, NULL );

    TT_insert( tree, "make", NULL );
    TT_insert( tree, "love", NULL );
    TT_insert( tree, "not", NULL );
    TT_insert( tree, "war", NULL );

    TT_insert( tree, "bird", NULL );
    TT_insert( tree, "is", NULL );
    TT_insert( tree, "the", NULL );
    TT_insert( tree, "word", NULL );

    TT_insert( tree, "all", NULL );
    TT_insert( tree, "you", NULL );
    TT_insert( tree, "need", NULL );
    TT_insert( tree, "is", NULL );
    TT_insert( tree, "love", NULL );

    node = TT_search( tree, "is" );
    printf( "\"is\" is %sfound\n", node ? "" : "NOT " );
    TT_delete( tree, "is" );
    node = TT_search( tree, "is" );
    printf( "\"is\" is %sfound\n", node ? "" : "NOT " );

    printf( "\nTT_dump( tree, stdout ):\n" );
    TT_dump( tree, stdout );

    printf( "\nTT_walk_desc( tree, ... ):\n" );
    TT_walk_desc( tree, t_print, NULL );

    printf( "\nTT_data( tree ):\n" );
    /*
     *  1st way:
     */
    data = TT_data( tree, NULL );
    keys = TT_keys( tree );
    for( i = 0; i < keys; i++ )
        printf( "%s\n", data[i].key );

/*
 *  2nd way:
 */
/*
    data = TT_data( tree, &keys );
    for( i = 0; i < keys; i++ )
        printf( "%s\n", data[i].key );
*/

/*
 *  3rd way:
 */
/*
    ptr = data;
    while( ptr && ptr->key )
    {
        printf( "%s\n", ptr->key );
        ptr++;
    }
*/
    free( data );

    printf( "\nTT_lookup( tree, \"w\" ):\n" );
    data = TT_lookup( tree, "w", &keys );
    ptr = data;
    while( ptr && ptr->key )
    {
        printf( "%s\n", ptr->key );
        ptr++;
    }
    free( data );

    TT_destroy( tree );
    return 0;
}
