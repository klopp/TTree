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
    /*
     * Replace elements on insert. For existing elements (*Tree_Destroy) function will be called:
     */
    T_INSERT_REPLACE = 1,
    /*
     * Use T_Free() function to destroy elements:
     */
    T_FREE_DEFAULT = 2,
    /*
     * Caseless comparison for TS_Tree data:
     */
    T_NOCASE = 1024,
    /*
     * With this flag TT_insert() function (ternary trees) will return tree root.
     * Without - link to created/replaced node.
     */
    T_INSERT_FAST = 2096,
    T_DEFAULT_FLAGS = ( T_INSERT_REPLACE | T_FREE_DEFAULT )
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
 * Default free function (used with T_FREE_DEFAULT flag)
 */
void T_Free( void *data );

#ifdef __cplusplus
}
#endif

#endif /* TREE_H_ */
