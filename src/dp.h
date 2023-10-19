#pragma once

/* A dictionary-string parser,
   parses keywords and values
   and point to the corresponding positions in the dictionary string.
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>


typedef struct {
    size_t pos;
    int toknext;
} dp_t;

/* Token representation, start and end coordinates of the dictionary string */
typedef struct {
    int start; /* start position in dict string */
    int end; /* end position in dict string */
} dptok_t;

/** @brief Parse a simple dictionary in the string dict,
   since dict might not be \0 terminated it also needs to know the
   length of the string.
   The tokens should be pre-allocated to a fixed size supplied by tok_len.
 */
int dp_parse(dp_t dp, const char * dict,
             const size_t dict_len, dptok_t * tok, const int tok_len);

/** @brief Comparison of token string to the string s
*/
int dp_eq(const char *dict, const dptok_t *tok, const char *s);
