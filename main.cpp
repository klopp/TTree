#include "ttree.h"
#include <map>
#include <unordered_map>

/* ----------------------------------------------------------------- */
#define N_STRINGS (10 * 1000)
#define R_STRINGS (1000)

/* ----------------------------------------------------------------- */
int main()
{
    char** strings = new char*[N_STRINGS];
    size_t rstrings = new size_t[R_STRINGS];

    delete[] strings;
    delete[] rstrings;
    return 0;
}

