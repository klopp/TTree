/*
 * ttest.c, examples of TernaryTree usage
 *
 *  Created on: 11.05.2015
 *  Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#if defined(__TTREE_TEST__)

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

    struct A { size_t sz; char data[1]; } *a;

    a = (struct A*)Malloc(1024);
    a->data[0] = '0';
    a->data[1] = '1';
    a->data[2] = '2';
    a->data[3] = '3';
    a->data[4] = '4';
    Free( a );

/*
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
*/


    TS_insert( tree, "aa" );
    TS_insert( tree, "a" );


/*
    node = TT_search( tree, "is" );
    printf( "\"is\" is %sfound\n", node ? "" : "NOT " );
    TT_del_key( tree, "is" );
    node = TT_search( tree, "is" );
    printf( "\"is\" is %sfound\n", node ? "" : "NOT " );
*/

    printf( "\nTT_dump( tree, stdout ):\n" );
    TT_dump( tree, stdout );

//    printf( "\nTT_walk_desc( tree, ... ):\n" );
    TS_walk_asc( tree, tt_print );
    TS_walk_desc( tree, tt_print );
//    TS_walk_desc( tree, tt_print );

//    printf( "\nTS_data( tree ):\n" );
    /*
     *  1st way:
     */
/*
    sdata = TS_data( tree, NULL );
    for( i = 0; i < tree->keys; i++ )
        printf( "%s\n", sdata[i] );
*/

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
/*
    free( sdata );

    printf( "\nTS_lookup( tree, \"w\" ):\n" );
    sdata = TS_lookup( tree, "w", &keys );
    for( i = 0; i < keys; i++ )
        printf( "%s\n", sdata[i] );
*/
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
/*
    free( sdata );

    printf( "\nTT_lookup_tree( tree, \"w\" ):\n" );
    rc = TT_lookup_tree( tree, "w" );
    TT_dump( rc, stdout );
    TT_destroy( rc );
*/

    TT_destroy( tree );

    return 0;
}

#endif  /* __TTREE_TEST__ */

