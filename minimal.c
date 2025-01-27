#include <stdlib.h>
#include <npio.h>

int main(int argc, char **argv)
{
    if(argc > 1)
    {
        npio_t * np = npio_load(argv[1]);
        if(np)
        {
            npio_print(stdout, np);
            npio_free(np);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}
