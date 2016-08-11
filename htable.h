/*
 *  Created on: 11 авг. 2016 г.
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#ifndef HTABLE_H_
#define HTABLE_H_

#include "btree.h"
#include "../klib/_lock.h"

/*
 * Hash table, based on Balanced Trees & CRC32.
 * Presets for integer and C-string keys.
 */

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _HTable {
    BTree bt;
    __lock_t( lock );
} *HTable;

HTable HT_create( Tree_Flags flags, Tree_Destroy destructor );
void HT_clear( HTable ht );
void HT_destroy( HTable ht );

/*
 * HT_set() return internal key value:
 */
unsigned int HT_set( HTable ht, const void *key, size_t key_size, void *data );
void *HT_get( HTable ht, const void *key, size_t key_size );
int HT_delete( HTable ht, const void *key, size_t key_size );
/*
 * HT_get_k() and HT_delete_k() uses internal key values (see HT_set() return, faster):
 */
void *HT_get_k( HTable ht, unsigned int key );
int HT_delete_k( HTable ht, unsigned int key );

/*
 * C-strings keys handling:
 */
unsigned int HT_set_c( HTable ht, const char *key, void *data );
void *HT_get_c( HTable ht, const char *key );
int HT_delete_c( HTable ht, const char *key );

/*
 * HT_set_char( ht, 'c', data );
 * HT_set_int( ht, -1, data );
 * HT_set_ulong( ht, 12345678, data );
 * ... etc
 */
#define HT_INTEGER(tag, type) \
        unsigned int HT_set_##tag( HTable ht, type key, void *data ); \
        void *HT_get_##tag( HTable ht, type key); \
        int HT_delete_##tag( HTable ht, type key);

HT_INTEGER( char, char );
HT_INTEGER( uchar, unsigned char );
HT_INTEGER( short, short );
HT_INTEGER( ushort, unsigned short );
HT_INTEGER( int, int );
HT_INTEGER( uint, unsigned int );
HT_INTEGER( long, long );
HT_INTEGER( ulong, unsigned long );
HT_INTEGER( llong, long long );
HT_INTEGER( ullong, unsigned long long );

#if defined(__cplusplus)
}; /* extern "C" */
#endif

#endif /* HTABLE_H_ */
