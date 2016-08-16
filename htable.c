/*
 *  Created on: 11 авг. 2016 г.
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#include "htable.h"
#include "../klib/crc.h"
#include "../klib/hash.h"

/*
 * Because crc16() return short:
 */
static inline unsigned int _crc16( const void *buf, size_t size )
{
    return crc16( buf, size );
}

static struct {
    HT_Hash_Functions idx;
    HT_Hash_Function hf;

} _hf[] = { { HF_HASH_FAQ6, hash_faq6 }, { HF_HASH_LY, hash_ly }, { HF_HASH_ROT13, hash_rot13 }, {
        HF_HASH_RS,
        hash_rs
    }, { HF_HASH_CRC16, _crc16 }, { HF_HASH_CRC32, crc32 }
};

HTable HT_create( HT_Hash_Functions hf, size_t size, Tree_Destroy destructor )
{
    HTable ht = NULL;
    ht = Malloc( sizeof( struct _HTable ) );

    if( ht ) {
        size_t i;
        ht->bt = Calloc( sizeof( struct _AVLTree ), size + 1 );

        if( !ht->bt ) {
            Free( ht );
            return NULL;
        }

        for( i = 0; i <= /*UCHAR_MAX*/size; i++ ) {
            /*
             * Always replace values (T_INSERT_REPLACE flag):
             */
            ht->bt[i] = AVL_create( 0, NULL );

            if( !ht->bt[i] ) {
                while( i ) {
                    i--;
                    AVL_destroy( ht->bt[i] );
                }

                Free( ht->bt );
                Free( ht );
                return NULL;
            }
        }

        ht->hf = NULL;

        for( i = 0; i < sizeof( _hf ) / sizeof( _hf[0] ); i++ ) {
            if( hf == _hf[i].idx ) {
                ht->hf = _hf[i].hf;
            }
        }

        if( !ht->hf ) {
            ht->hf = _hf[0].hf;
        }

        ht->destructor = destructor;
        ht->error = TE_NO_ERROR;
        ht->size = size;
        __initlock( ht->lock );
    }

    return ht;
}

static void _HT_Free_Elem( HTElem e, HTable ht )
{
    if( e->next ) {
        _HT_Free_Elem( e->next, ht );
    }

    if( ht->destructor ) {
        ht->destructor( e->data );
    }

    Free( e->key );
    Free( e );
}

static void _HT_Destructor( AVLNode node, void *data )
{
    HTElem e = node->data;
    unused( data );
    _HT_Free_Elem( e, data );
}

void HT_clear( HTable ht )
{
    size_t i;
    __lock( ht->lock );

    for( i = 0; i <= /*UCHAR_MAX*/ht->size; i++ ) {
        AVL_walk( ht->bt[i], _HT_Destructor, ht );
        AVL_clear( ht->bt[i] );
    }

    ht->error = TE_NO_ERROR;
    __unlock( ht->lock );
}

void HT_destroy( HTable ht )
{
    size_t i;
    __lock( ht->lock );

    for( i = 0; i <= /*UCHAR_MAX*/ht->size; i++ ) {
        AVL_walk( ht->bt[i], _HT_Destructor, ht );
        AVL_destroy( ht->bt[i] );
    }

    Free( ht );
}

size_t HT_size( HTable ht )
{
    size_t rc = 0;
    size_t i = 0;
    __lock( ht->lock );

    while( i <= /*UCHAR_MAX*/ht->size ) {
        rc += ht->bt[i]->nodes;
        i++;
    }

    __unlock( ht->lock );
    return rc;
}

size_t HT_maxdepth( HTable ht )
{
    size_t maxdepth = 0;
    size_t i = 0;
    __lock( ht->lock );

    while( i <= /*UCHAR_MAX*/ht->size ) {
        size_t depth = AVL_depth( ht->bt[i] );

        if( maxdepth < depth ) {
            maxdepth = depth;
        }

        i++;
    }

    __unlock( ht->lock );
    return maxdepth;
}

unsigned int HT_set( HTable ht, const void *key, size_t key_size, void *data )
{
    unsigned int hash;
    AVLNode node;
    HTElem e;
    HTElem cursor;
    size_t idx;
    __lock( ht->lock );
    e = Malloc( sizeof( struct _HTElem ) );

    if( !e ) {
        ht->error = TE_MEMORY;
        __unlock( ht->lock );
        return 0;
    }

    e->key = Malloc( key_size );

    if( !e->key ) {
        Free( e );
        ht->error = TE_MEMORY;
        __unlock( ht->lock );
        return 0;
    }

    memcpy( e->key, key, key_size );
    e->key_size = key_size;
    e->data = data;
    e->next = NULL;
    hash = ht->hf( key, key_size );
    idx = hash & ht->size;
    node = AVL_insert( ht->bt[idx], hash, e );
    ht->error = ht->bt[idx]->error;

    if( node || ht->error != TE_FOUND ) {
        __unlock( ht->lock );
        return node ? hash : 0;
    }

    node = AVL_search( ht->bt[idx], hash );

    if( !node ) {
        ht->error = ht->bt[idx]->error;
        __unlock( ht->lock );
        return 0;
    }

    cursor = node->data;

    while( 1 ) {
        /*
         * Node found, check key:
         */
        if( cursor->key_size == key_size && !memcmp( cursor->key, key, key_size ) ) {
            /*
             * Keys equals, destroy & replace old data:
             */
            if( ht->destructor ) {
                ht->destructor( cursor->data );
            }

            cursor->data = data;
            Free( e->key );
            Free( e );
            __unlock( ht->lock );
            return hash;
        }

        if( cursor->next ) {
            cursor = cursor->next;
            continue;
        }

        cursor->next = e;
        break;
    }

    __unlock( ht->lock );
    return hash;
}

void *HT_get( HTable ht, const void *key, size_t key_size )
{
    AVLNode node;
    unsigned int hash;
    HTElem e;
    __lock( ht->lock );
    e = NULL;
    hash = ht->hf( key, key_size );
    node = AVL_search( ht->bt[hash & /*UCHAR_MAX*/ht->size], hash );

    if( node ) {
        e = node->data;

        if( !e->next ) {
            __unlock( ht->lock );
            return e->data;
        }

        do {
            if( e->key_size == key_size && !memcmp( e->key, key, key_size ) ) {
                break;
            }

            e = e->next;
        }
        while( e );
    }

    ht->error = ht->bt[hash & /*UCHAR_MAX*/ht->size]->error;
    __unlock( ht->lock );
    return e ? e->data : NULL;
}

int HT_delete( HTable ht, const void *key, size_t key_size )
{
    int rc = 0;
    AVLNode node;
    unsigned int hash;
    HTElem e;
    __lock( ht->lock );
    hash = ht->hf( key, key_size );
    node = AVL_search( ht->bt[hash & /*UCHAR_MAX*/ht->size], hash );

    if( node ) {
        e = node->data;

        if( !e->next ) {
            if( ht->destructor ) {
                ht->destructor( e->data );
            }

            Free( e->key );
            Free( e );
            rc = AVL_delete( ht->bt[hash & /*UCHAR_MAX*/ht->size], hash );
            ht->error = ht->bt[hash & /*UCHAR_MAX*/ht->size]->error;
        }
        else {
            HTElem cursor = e;

            while( cursor ) {
                if( cursor->key_size == key_size && !memcmp( cursor->key, key, key_size ) ) {
                    /*
                     * First element:
                     */
                    if( e == node->data ) {
                        node->data = e->next;
                    }
                    else {
                        e->next = cursor->next;
                    }

                    if( ht->destructor ) {
                        ht->destructor( cursor->data );
                    }

                    Free( cursor->key );
                    Free( cursor );
                    rc = 1;
                    break;
                }

                e = cursor;
                cursor = cursor->next;
            }

            if( !cursor ) {
                ht->error = TE_NOT_FOUND;
            }
        }
    }

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

int HT_dump( HTable ht, Tree_KeyDump kdumper, Tree_DataDump ddumper,
             FILE *handle )
{
    size_t i;
    int errors = 0;

    for( i = 0; i < /*UCHAR_MAX*/ht->size; i++ ) {
        if( ht->bt[i]->nodes ) {
            printf( "Tree idx: %zu, ", i );
            errors += AVL_dump( ht->bt[i], kdumper, ddumper, handle );
        }
    }

    return errors;
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

HT_INTEGER_IMPL( szt, size_t );
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

