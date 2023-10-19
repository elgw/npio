/* A command line "interface" for npio */

#include <assert.h>
#include <getopt.h>
#include <time.h>

#include "npio.h"

void show_endian(void)
{
    uint32_t x = 1;
    uint8_t * x8 = (uint8_t*) &x;

    if(x8[0] == 1)
    {
        printf("This machine is little endian\n");
    } else {
        printf("This machine is big endian\n");
    }
    return;
}

int unittest()
{
    show_endian();

    int M = 2;
    int N = 2;


    int ndim = 2;
    int dim[] = {M, N};
    double * D = malloc(M*N*sizeof(double));
    for(int kk = 0; kk<M*N; kk++)
    {
        D[kk] = (kk+1)+(double) (kk+1) / 10.0;
    }

    char * outname = malloc(100);
    assert(outname != NULL);

    sprintf(outname, "numpy_io_ut_%dx%d.npy", M, N);

    printf("Writing to %s\n", outname);
    int status = npio_save_double(outname, ndim, dim, D);
    if(status != EXIT_SUCCESS)
    {
        free(D);
        free(outname);
        return status;
    }

    printf("Reading from %s\n", outname);
    npio_t * np = npio_load(outname);
    double * in_data = (double*) np->data;
    size_t results_differ = 0;
    for(size_t kk = 0; kk< (size_t) M*N; kk++)
    {
        if(D[kk] != in_data[kk])
        {
            results_differ++;
            return EXIT_FAILURE;
        }
    }

    if(results_differ > 0)
    {
        fprintf(stderr, "FAILED: Results differ in %zu places\n",
                results_differ);
    }
    free(D);
    npio_print(stdout, np);
    npio_free(&np);
    free(outname);
    printf("Written and read data is identical\n");
    return EXIT_SUCCESS;
}

void load(char * from)
{
    npio_t * np = npio_load(from);
    if(np == NULL)
    {
        printf("Failed to load %s\n", from);
        exit(EXIT_FAILURE);
    }
}

void show(char * from)
{
    printf("Loading %s\n", from);
    npio_t * np = npio_load(from);
    if(np == NULL)
    {
        printf("Failed to load %s\n", from);
        exit(EXIT_FAILURE);;
    }
    npio_print(stdout, np);
    npio_free(&np);
}

void resave(char * from, char * to)
{
    npio_t * np = npio_load(from);
    if(np == NULL)
    {
        printf("Failed to load %s\n", from);
        exit(EXIT_FAILURE);;
    }
    if(strncmp(np->descr, "'<f8'", 5) != 0)
    {
        printf("Error: Can only write '<f8', this dataset has format %s\n", np->descr);
        return;
    }
        int status = npio_save_double(to, np->ndim, np->shape, np->data);

    if(status != EXIT_SUCCESS)
    {
        printf("Failed to write to %s\n", to);
        exit(EXIT_FAILURE);
    }
    npio_free(&np);
    return;
}

static double timespec_diff(struct timespec* end, struct timespec * start)
{
    double elapsed = (end->tv_sec - start->tv_sec);
    elapsed += (end->tv_nsec - start->tv_nsec) / 1000000000.0;
    return elapsed;
}

static void bench(char * from, char * to)
{
    size_t N = 1000;

    struct timespec tstart, tend;
    clock_gettime(CLOCK_REALTIME, &tstart);
    for(size_t kk = 0; kk < N; kk++)
    {
    npio_t * np = npio_load(from);
    if(np == NULL)
    {
        printf("Failed to load %s\n", from);
        exit(EXIT_FAILURE);;
    }
    npio_free(&np);
    }
    clock_gettime(CLOCK_REALTIME, &tend);
    double t_load = timespec_diff(&tend, &tstart);

    npio_t * np = npio_load(from);
    if(np == NULL)
    {
        printf("Failed to load %s\n", from);
        exit(EXIT_FAILURE);;
    }

    clock_gettime(CLOCK_REALTIME, &tstart);
    for(size_t kk = 0; kk < N; kk++)
    {
        int status = npio_save_double(to, np->ndim, np->shape, np->data);

    if(status != EXIT_SUCCESS)
    {
        printf("Failed to write to %s\n", to);
        exit(EXIT_FAILURE);
    }
    }
    clock_gettime(CLOCK_REALTIME, &tend);
    double t_write = timespec_diff(&tend, &tstart);
    npio_free(&np);

    printf("To load %s 1000 times took %.4f s\n", from, t_load);
    printf("To write to %s 1000 times took %.4f s\n", to, t_write);

    return;
}

void show_usage(char ** argv)
{
    printf("Usage:\n");
    printf("%s <file.npy>\n\t Show metadata of <file.npy>\n", argv[0]);
    printf("%s --help\n\t Show help message\n", argv[0]);
    printf("%s --usage\n\t Show help message\n", argv[0]);
    printf("%s --benchmark <in> <out>\n\t"
           "Benchmark how fast <in> can be read, and how fast "
           "it can be written to <out>\n", argv[0]);
    printf("%s --unittest\n\t Do some self-testing\n", argv[0]);
    printf("%s --resave <from.npy> <to.npy>\n\t"
           "Read <from.npy> and save as <to.npy>\n", argv[0]);
    return;
}

int main(int argc, char ** argv)
{

    if(argc == 1)
    {
        show_usage(argv);
        return EXIT_SUCCESS;
    }

    struct option longopts[] = {
        { "unittest",   no_argument, NULL, 't' },
        { "usage",      no_argument, NULL, 'u' },
        { "help",       no_argument, NULL, 'h' },
        { "benchmark",  no_argument, NULL, 'b' },
        { "resave",     no_argument, NULL, 'r' },
        { NULL, 0, NULL, 0}
    };

    int ch;
    while((ch = getopt_long(argc, argv, "tuhbr", longopts, NULL)) != -1)
    {
        switch(ch)
        {
        case 't':
            return unittest();
            break;
        case 'u':
            show_usage(argv);
            break;
        case 'h':
            show_usage(argv);
            break;
        case 'b':
            if(optind+2 == argc)
            {
                bench(argv[optind], argv[optind+1]);
            } else {
                show_usage(argv);
                return EXIT_FAILURE;
            }
            break;
        case 'r':
            if(optind+2 == argc)
            {
                resave(argv[optind], argv[optind+1]);
            } else {
                show_usage(argv);
                return EXIT_FAILURE;
            }
            break;
        }
    }

    if(argc == 2 && optind == 1)
    {
        show(argv[1]);
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
