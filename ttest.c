/*
 * ttest.c, examples of TernaryTree usage
 *
 *  Created on: 11.05.2015
 *  Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */
#include "tstree.h"

static void tt_print( TTNode node, void * data )
{
    if( node->key ) printf( "%s\n", node->key );
}

int main()
{
/*    TT_Data data;*/
    char ** sdata;
    /*    TT_Data ptr;*/
    size_t keys, i;
    TTNode node;
    TTree rc;
    TTree tree = TS_create( TT_DEFAULTS );

    TS_insert( tree, "make" );
    TS_insert( tree, "love" );
    TS_insert( tree, "not" );
    TS_insert( tree, "war" );

    TS_insert( tree, "bird" );
    TS_insert( tree, "is" );
    TS_insert( tree, "the" );
    TS_insert( tree, "word" );

    TS_insert( tree, "all" );
    TS_insert( tree, "you" );
    TS_insert( tree, "need" );
    TS_insert( tree, "is" );
    TS_insert( tree, "love" );

    node = TT_search( tree, "is" );
    printf( "\"is\" is %sfound\n", node ? "" : "NOT " );
    TT_delete( tree, "is" );
    node = TT_search( tree, "is" );
    printf( "\"is\" is %sfound\n", node ? "" : "NOT " );

    printf( "\nTT_dump( tree, stdout ):\n" );
    TT_dump( tree, stdout );

    printf( "\nTT_walk_desc( tree, ... ):\n" );
    TS_walk_desc( tree, tt_print );

    printf( "\nTS_data( tree ):\n" );
    /*
     *  1st way:
     */
    sdata = TS_data( tree, NULL );
    keys = TT_keys( tree );
    for( i = 0; i < keys; i++ )
        printf( "%s\n", sdata[i] );

    /*
     *  2nd way:
     */
    /*
     sdata = TS_data( tree, &keys );
     for( i = 0; i < keys; i++ )
     printf( "%s\n", sdata[i] );
     */

    /*
     *  3rd way:
     */
    /*
     ptr = sdata;
     while( ptr && *ptr )
     {
     printf( "%s\n", ptr );
     ptr++;
     }
     */
    free( sdata );

    printf( "\nTS_lookup( tree, \"w\" ):\n" );
    sdata = TS_lookup( tree, "w", &keys );
    for( i = 0; i < keys; i++ )
        printf( "%s\n", sdata[i] );
    /*
     *  OR
     */
    /*
     ptr = sdata;
     while( ptr && ptr )
     {
     printf( "%s\n", ptr );
     ptr++;
     }
     */
    free( sdata );

    printf( "\nTT_lookup_tree( tree, \"w\" ):\n" );
    rc = TT_lookup_tree( tree, "w" );
    TT_dump( rc, stdout );
    TT_destroy( rc );

    TT_destroy( tree );

    return 0;
}
