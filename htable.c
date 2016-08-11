/*
 *  Created on: 11 авг. 2016 г.
 *      Author: klopp
 */

#include "htable.h"
#include "../klib/crc.h"

HTable HT_create( Tree_Flags flags, Tree_Destroy destructor, size_t key_size )
{
    HTable ht = Malloc( sizeof( struct _HTable ) );

    if( ht ) {
        ht->key_size = key_size;
        ht->bt = BT_create( flags, destructor );

        if( !ht->bt ) {
            Free( ht );
            ht = NULL;
        }
    }

    return ht;
}

void HT_clear( HTable ht )
{
    BT_clear( ht->bt );
}
void HT_destroy( HTable ht )
{
    BT_destroy( ht->bt );
    Free( ht );
}

unsigned int HT_set( HTable ht, const void *key, void *data )
{
    unsigned int crc = crc32( key, ht->key_size );
    return BT_insert( ht->bt, crc, data ) ? crc : 0;
}

void *HT_get( HTable ht, const void *key )
{
    unsigned int crc = crc32( key, ht->key_size );
    return HT_get_k( ht, crc );
}

void *HT_get_k( HTable ht, unsigned int key )
{
    BTNode btn = BT_search( ht->bt, key );
    return btn ? btn->data : NULL;
}

int HT_delete( HTable ht, const void *key )
{
    unsigned int crc = crc32( key, ht->key_size );
    return BT_delete( ht->bt, crc );
}

unsigned int HT_set_c( HTable ht, const char *key, void *data )
{
    ht->key_size = strlen( key );
    return HT_set( ht, key, data );
}

void *HT_get_c( HTable ht, const char *key )
{
    ht->key_size = strlen( key );
    return HT_get( ht, key );
}

int HT_delete_c( HTable ht, const char *key )
{
    ht->key_size = strlen( key );
    return HT_delete( ht, key );
}

#define HT_INTEGER_IMPL(tag,type) \
    HTable HT_create_##tag( Tree_Flags flags, Tree_Destroy destructor ) { \
        return HT_create( flags, destructor, sizeof(type) ); \
    } \
    unsigned int HT_set_##tag( HTable ht, type key, void *data ) { \
        return HT_set( ht, &key, data ); \
    } \
    void *HT_get_##tag( HTable ht, type key) {; \
        return HT_get( ht, &key ); \
    } \
    int HT_delete_##tag( HTable ht, type key) { \
        return HT_delete( ht, &key ); \
    }

HT_INTEGER_IMPL( char, char );
HT_INTEGER_IMPL( uchar, unsigned char );
HT_INTEGER_IMPL( short, short );
HT_INTEGER_IMPL( ushort, unsigned short );
HT_INTEGER_IMPL( int, int );
HT_INTEGER_IMPL( uint, unsigned int );
HT_INTEGER_IMPL( long, long );
HT_INTEGER_IMPL( ulong, unsigned long );
HT_INTEGER_IMPL( llong, long long );
HT_INTEGER_IMPL( ullong, unsigned long long );

/*
 *  That's All, Folks!
 */

