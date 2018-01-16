/*
 * tarray.h, part of "trees" project.
 *
 *  Created on: 16.01.2018, 02:21
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */
#include "tarray.h"
#include <errno.h>

TArray TA_create( Tree_Flags flags, Tree_Destroy destructor )
{
    TArray array = Malloc( sizeof(struct _TArray) );
    if( array ) {
        array->tree = AVL_create( flags | T_INSERT_REPLACE, destructor );
        array->length = 0;
        array->error = 0;
        if( !array->tree ) {
            Free( array );
            array = NULL;
        }
    }
    return array;
};

void TA_clear( TArray array )
{
    array->error = 0;
    AVL_clear( array->tree );
    array->length = 0;
};

void TA_destroy( TArray array )
{
    TA_clear( array );
    Free( array );
};

AVLNodeConst TA_set( TArray array, size_t idx, void * data )
{
    return AVL_insert( array->tree, idx, data );
};

void * TA_get( TArray array, size_t idx )
{
    if( idx < array->length )
    {
        AVLNodeConst node = AVL_search( array->tree, idx );
        if( node ) {
            return node->data;
        }
        array->error = ENOENT;
    }
    else
    {
        array->error = ERANGE;
    }
    return NULL;
};

/*
 *  That's All, Folks!
 */
