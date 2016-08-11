/*
 *  Created on: 11 авг. 2016 г.
 *      Author: klopp
 */

#include "htable.h"
#include "../klib/crc.h"

HTable HT_create( Tree_Flags flags, Tree_Destroy destructor, size_t key_size )
{
    HTable ht = Malloc(sizeof(struct _HTable));
    if( ht )
    {
        ht->key_size = key_size;
        ht->bt = BT_create(flags, destructor);
        if( !ht->bt )
        {
            Free(ht);
            ht = NULL;
        }
    }
    return ht;
}

void HT_clear( HTable ht )
{
    BT_clear( ht->bt);
}
void HT_destroy( HTable ht )
{
    BT_destroy(ht->bt);
    Free(ht);
}

unsigned int HT_set( HTable ht, void *key, void *data )
{
    unsigned int crc = crc32(key, ht->key_size);
    return BT_insert( ht->bt, crc, data ) ? crc : 0;
}

void *HT_get( HTable ht, void *key)
{
    unsigned int crc = crc32(key, ht->key_size);
    BTNode btn = BT_search( ht->bt, crc );
    return btn ? btn->data : NULL;
}

int HT_delete( HTable ht, void *key)
{
    unsigned int crc = crc32(key, ht->key_size);
    return BT_delete( ht->bt, crc );
}


/*
 *  That's All, Folks!
 */

