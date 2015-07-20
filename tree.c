/*
 * tree.c, part of "trees" project.
 *
 *  Created on: 14.06.2015, 03:11
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#include "tree.h"

void T_Free( void * data )
{
    Free( data );
}

size_t T_Indent( char * indent, int last, FILE * handle )
{
    size_t strip = strlen( indent );
    fprintf( handle, "%s", indent );
    if( last )
    {
        fprintf( handle, "+-" );
        strcat( indent, "  " );
    }
    else
    {
        fprintf( handle, "|-" );
        strcat( indent, "| " );
    }
    return strip;
}


