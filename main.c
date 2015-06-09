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

/*
 #if defined(__TTREE_TEST__)

#include "tstree.h"

static void tt_print( TTNode node, void * data )
{
    if( node->key ) printf( "%s\n", node->key );
}

int main()
{
    char ** sdata;
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
*/

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


/*
    TS_insert( tree, "aa" );
    TS_insert( tree, "a" );
*/


/*
    node = TT_search( tree, "is" );
    printf( "\"is\" is %sfound\n", node ? "" : "NOT " );
    TT_del_key( tree, "is" );
    node = TT_search( tree, "is" );
    printf( "\"is\" is %sfound\n", node ? "" : "NOT " );
*/

/*
    printf( "\nTT_dump( tree, stdout ):\n" );
    TT_dump( tree, stdout );
*/

//    printf( "\nTT_walk_desc( tree, ... ):\n" );
//    TS_walk_asc( tree, tt_print );
//    TS_walk_desc( tree, tt_print );
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

//    TT_destroy( tree );

//    return 0;
//}

//#endif

