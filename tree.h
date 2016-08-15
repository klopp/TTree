/*
 * tree.h, part of "trees" project.
 *
 *  Created on: 14.06.2015, 03:04
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#ifndef TREE_H_
#define TREE_H_

/*
 * OS and compiler dependent stuff here. Make own if needed.
 */
#include "../klib/config.h"
#include "../klib/_lock.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * Common tree
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef TREE_KEY_TYPE
# define TREE_KEY_TYPE int
#endif

typedef enum _Tree_Flags
{
    T_INSERT_REPLACE = 1,
    /*T_DESTROY_ON_FAIL = 2,*/
    T_FREE_DEFAULT = 4,
    T_NOCASE = 1024,
    T_INSERT_FAST = 2096,
    T_DEFAULT_FLAGS = ( T_INSERT_REPLACE /*| T_DESTROY_ON_FAIL */ | T_FREE_DEFAULT )
}
Tree_Flags;

/*
 * Destroy tree data:
 */
typedef void ( *Tree_Destroy )( void *data );
/*
 * Dump tree node data, used by dumpers:
 */
typedef void ( *Tree_DataDump )( void *data, FILE *handle );
typedef void ( *Tree_KeyDump )( TREE_KEY_TYPE key, FILE *handle );
/*
 * Internal : indent tree node, used by dumpers.
 * Return initial indent  length.
 */
size_t T_Indent( char *indent, int last, FILE *handle );

/*
 * Used by walkers:
 */
//typedef void (*Tree_Walk)( void * node, void * data );
/*
 * Default free function (used with T_FREE_DEFAULT flag)
 */
void T_Free( void *data );

#ifdef __cplusplus
}
#endif

#endif /* TREE_H_ */
