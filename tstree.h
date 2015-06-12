/*
 * tstree.h, part of "ttree" project.
 *
 *  Created on: 16.05.2015, 21:25
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#ifndef TSTREE_H_
#define TSTREE_H_

#include "ttree.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *  Get char ** data from tree. Return pointer to allocated char ** array
 *  (sorted by key) wich must be freed with free(), or NULL. Last element of
 *  returned array is NULL.
 */
char ** TS_data( TTree tree, size_t * count );
char ** TS_lookup( TTree tree, const char * prefix, size_t * count );
char ** TS_nlookup( TTree tree, const char * prefix, size_t max,
        size_t * count );
int TS_dump( TTree tree, FILE * handle );

/*
 *  String tree shortcuts (with NULL data field)
 */
#define TS_create( flags ) TT_create( (flags), NULL )
#define TS_insert( tree, key ) TT_insert( (tree), (key), NULL )
#define TS_walk( tree, walker ) TT_walk( (tree), (walker), NULL )
#define TS_walk_asc( tree, walker ) TT_walk_asc( (tree), (walker), NULL )
#define TS_walk_desc( tree, walker ) TT_walk_desc( (tree), (walker), NULL )

#ifdef __cplusplus
}
#endif

#endif /* TSTREE_H_ */
