/*
 * tstree.c, part of "ttree" project.
 *
 *  Created on: 16.05.2015, 21:27
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#include "tstree.h"

TTNode __TT_lookup( TTNode node, const char *s, TT_Flags flags );
void _TS_data( TTNode node, void * data );
void _TT_walk_asc( TTNode node, TT_Walk walker, void * data );
TT_Data _TT_lookup( TTree tree, const char * prefix, size_t max,
        size_t * count );

static char ** _TS_lookup( TTree tree, const char * prefix, size_t max,
        size_t * count )
{
    TTNode node;
    struct
    {
        size_t max;
        size_t idx;
        char ** data;
    } data =
    { 0 };

    if( count ) *count = 0;

    if( !tree || !prefix || !*prefix ) return NULL;
    node = __TT_lookup( tree->head->mid, prefix, tree->flags );
    if( !node || !node->mid ) return NULL;

    if( !max ) max = TT_keys( tree );

    data.data = calloc( sizeof(char *), max + 1 );
    if( !data.data ) return NULL;

    data.max = max;
    _TT_walk_asc( node->mid, _TS_data, &data );
    if( count ) *count = data.idx;

    return data.data;
}

TT_Data TT_lookup( TTree tree, const char * prefix, size_t * count )
{
    return _TT_lookup( tree, prefix, 0, count );
}
TT_Data TT_nlookup( TTree tree, const char * prefix, size_t max,
        size_t * count )
{
    return _TT_lookup( tree, prefix, max, count );
}

char ** TS_lookup( TTree tree, const char * prefix, size_t * count )
{
    return _TS_lookup( tree, prefix, 0, count );
}
char ** TS_nlookup( TTree tree, const char * prefix, size_t max,
        size_t * count )
{
    return _TS_lookup( tree, prefix, max, count );
}
