/*
 * tstree.c, part of "trees" project.
 *
 *  Created on: 16.05.2015, 21:27
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#include "tstree.h"

TTNode __TT_lookup( TTNode node, const char *s, Tree_Flags flags );
void _TS_data( TTNodeConst node, void *data );
void _TT_walk_asc( TTNodeConst node, TT_Walk walker, void *data );
TT_Data _TT_lookup( TTree tree, const char *prefix, size_t max,
                    size_t *count );

static char const **_TS_lookup( const TTree tree, const char *prefix,
                                size_t max,
                                size_t *count )
{
    TTNode node;
    struct {
        size_t max;
        size_t idx;
        char const **data;
    } data =
    { 0 };

    if( count ) {
        *count = 0;
    }

    if( !tree || !prefix || !*prefix ) {
        return NULL;
    }

    node = __TT_lookup( tree->head->mid, prefix, tree->flags );

    if( !node || !node->mid ) {
        return NULL;
    }

    if( !max ) {
        max = tree->keys/*TT_keys( tree )*/;
    }

    data.data = Calloc( sizeof( char * ), max + 1 );

    if( !data.data ) {
        return NULL;
    }

    data.max = max;
    _TT_walk_asc( node->mid, _TS_data, &data );

    if( count ) {
        *count = data.idx;
    }

    return data.data;
}

TT_DataConst TT_lookup( const TTree tree, const char *prefix, size_t *count )
{
    return _TT_lookup( tree, prefix, 0, count );
}

TT_DataConst TT_nlookup( const TTree tree, const char *prefix, size_t max,
                         size_t *count )
{
    return _TT_lookup( tree, prefix, max, count );
}

char const **TS_lookup( const TTree tree, const char *prefix, size_t *count )
{
    return _TS_lookup( tree, prefix, 0, count );
}

char const **TS_nlookup( const TTree tree, const char *prefix, size_t max,
                         size_t *count )
{
    return _TS_lookup( tree, prefix, max, count );
}

static void _TS_Dump( void *data, FILE *handle )
{
    fprintf( handle, " ''%s''", ( char * )data );
}

int TS_dump( const TTree tree, FILE *handle )
{
    return TT_dump( tree, _TS_Dump, handle );
}
