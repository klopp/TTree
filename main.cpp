#include "ttree.h"
#include <vector>
#include <map>
#include <string>
#include <sys/time.h>
#include <unordered_map>

/* ----------------------------------------------------------------- */
#define N_STRINGS   (500 * 1000)
#define S_LENGTH    (32)
#define R_STRINGS   (1000)

/* ----------------------------------------------------------------- */
static std::string random_string( void )
{
    std::string s;

    for( size_t i = 0; i < S_LENGTH; ++i ) {
        s.push_back( char( (rand() % 23) + 'a' ) );
    }

    return s;
}

/* -------------------------------------------------------------------------- */
static void print_elapsed( struct timeval *start, const char *title )
{
    struct timeval tend;
    gettimeofday( &tend, 0 );

    double elapsed = ((tend.tv_sec - start->tv_sec) * 1000000 + tend.tv_usec - start->tv_usec) / 1000000;
    printf( "%s :: %.4f\n", title, elapsed );
}

/* ----------------------------------------------------------------- */
int main()
{
    size_t* rstrings = new size_t[R_STRINGS];
    std::vector<std::string> sarray;

    for( size_t i = 0; i < N_STRINGS; ++i ) {
        sarray.push_back( std::move( random_string() ) );
    }

    for( size_t i = 0; i < R_STRINGS; ++i ) {
        rstrings[i] = rand() % N_STRINGS;
    }

    std::map<std::string, std::string> map;
    std::unordered_map<std::string, std::string> umap;
    TTree tree = TT_create( T_NO_FLAGS, NULL );

    struct timeval tstart;

    gettimeofday( &tstart, 0 );
    for( size_t i = 0; i < N_STRINGS; ++i ) {
        map[sarray[i]] = sarray[i];
    }
    for( size_t i = 0; i < R_STRINGS; ++i ) {
        std::string s = map[sarray[rstrings[i]]];
    }
    print_elapsed( &tstart, "std::map" );

    gettimeofday( &tstart, 0 );
    for( size_t i = 0; i < N_STRINGS; ++i ) {
        umap[sarray[i]] = sarray[i];
    }
    for( size_t i = 0; i < R_STRINGS; ++i ) {
        std::string s = umap[sarray[rstrings[i]]];
    }
    print_elapsed( &tstart, "std::unordered_map" );

    gettimeofday( &tstart, 0 );
    for( size_t i = 0; i < N_STRINGS; ++i ) {
        TT_insert( tree, sarray[i].c_str(), sarray[i].c_str() );
    }
    for( size_t i = 0; i < R_STRINGS; ++i ) {
        TTNodeConst s = TT_search( tree, sarray[rstrings[i]].c_str() );
        //printf(" > %s\n", (const char *)s->data);
    }
    print_elapsed( &tstart, "TTree" );

    delete[] rstrings;
    TT_destroy( tree );

    return 0;
}

