/*
 *  Created on: 11 авг. 2016 г.
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#include "htable.h"
#include "../klib/crc.h"

HTable HT_create( Tree_Flags flags, Tree_Destroy destructor )
{
    HTable ht = Malloc( sizeof( struct _HTable ) );

    if( ht ) {
        /*
         * Always replace values:
         */
        ht->bt = BT_create( flags | T_INSERT_REPLACE, destructor );

        if( !ht->bt ) {
            Free( ht );
            ht = NULL;
        }

        __initlock( ht->lock );
    }

    return ht;
}

void HT_clear( HTable ht )
{
    __lock( ht->lock );
    BT_clear( ht->bt );
    __unlock( ht->lock );
}

void HT_destroy( HTable ht )
{
    __lock( ht->lock );
    BT_destroy( ht->bt );
    __unlock( ht->lock );
    Free( ht );
}

unsigned int HT_set( HTable ht, const void *key, size_t key_size, void *data )
{
    unsigned int crc;
    BTNode btn;
    __lock( ht->lock );
    crc = crc32( key, key_size );
    btn = BT_insert( ht->bt, crc, data );
    __unlock( ht->lock );
    return btn ? crc : 0;
}

void *HT_get( HTable ht, const void *key, size_t key_size )
{
    return HT_get_k( ht, crc32( key, key_size ) );
}

void *HT_get_k( HTable ht, unsigned int key )
{
    BTNode btn;
    __lock( ht->lock );
    btn = BT_search( ht->bt, key );
    __unlock( ht->lock );
    return btn ? btn->data : NULL;
}

int HT_delete( HTable ht, const void *key, size_t key_size )
{
    return HT_delete_k( ht, crc32( key, key_size ) );
}

int HT_delete_k( HTable ht, unsigned int key )
{
    int rc;
    __lock( ht->lock );
    rc = BT_delete( ht->bt, key );
    __unlock( ht->lock );
    return rc;
}

unsigned int HT_set_c( HTable ht, const char *key, void *data )
{
    return HT_set( ht, key, strlen( key ), data );
}

void *HT_get_c( HTable ht, const char *key )
{
    return HT_get( ht, key, strlen( key ) );
}

int HT_delete_c( HTable ht, const char *key )
{
    return HT_delete( ht, key, strlen( key ) );
}

#define HT_INTEGER_IMPL(tag, type) \
    unsigned int HT_set_##tag( HTable ht, type key, void *data ) { \
        return HT_set( ht, &key, sizeof(key), data ); \
    } \
    void *HT_get_##tag( HTable ht, type key) {; \
        return HT_get( ht, &key, sizeof(key) ); \
    } \
    int HT_delete_##tag( HTable ht, type key) { \
        return HT_delete( ht, &key, sizeof(key) ); \
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

