#include "dp.h"

int dp_eq(const char *dict, const dptok_t *tok, const char *s) {
    if ((int)strlen(s) == tok->end - tok->start &&
        strncmp(dict + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int dp_parse(dp_t dp, const char * dict,
             const size_t dict_len, dptok_t * tok, const int tok_len)
{
    dp.pos = 0; // position in dict string
    dp.toknext = 0; // next token to write to

    // Find first '{' while ignoring white spaces
    while(dict[dp.pos] != '{' && dp.pos < dict_len)
    {
        dp.pos++;
    }
    if((dp.pos + 1) == dict_len)
    {
        return 0;
    }

    // States:
    // 0: looking for key to start '
    // 1: looking for key to end ''
    // 2: looking for ':'
    // 3: looking for value to start nonwhite
    // 4: looking for value to end ,
    int state = 0;
    int nest = 0; // Keep track of parentheses
    while( dp.pos < dict_len)
    {
        char c = dict[dp.pos];
        if(state == 0) // looking for key to start
        {
            if(c == '\'')
            {
                state = 1;
                tok[dp.toknext].start = dp.pos;
                goto nextchar;
            }
        }
        if(state == 1) // looking for key to end
        {
            if(c == '\'')
            {
                state = 2;
                tok[dp.toknext].end = dp.pos+1;
                dp.toknext++;
                goto nextchar;
            }
        }
        if(state == 2)
        {
            if(c == ':') // looking for key:value separator
            {
                state = 3;
                goto nextchar;
            }
        }
        if(state == 3) // looking for start of value
        {
            nest = 0;
            if(c != ' ')
            {
                if(c == '(')
                {
                    nest ++;
                }

                state = 4;
                tok[dp.toknext].start = dp.pos;
                goto nextchar;
            }
        }
        if(state == 4) // looking for end of value
        {
            if( c == ')')
            {
                nest--;
            }
            if( c == '(')
            {
                nest++;
            }
            if( nest == 0 && c == ',')
            {
                state = 0;
                tok[dp.toknext].end = dp.pos;
                dp.toknext++;
                goto nextchar;
            }
        }
    nextchar:
        //printf("(pos: %d, state: %d), ", dp.pos, state);
        dp.pos++;
        if(dp.toknext == tok_len)
        {
            // Could warn here...
            return dp.toknext;
        }
    }
    //printf("\n");
    return dp.toknext;
}
