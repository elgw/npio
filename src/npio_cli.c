/* A command line "interface" for npio */

#include <assert.h>
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>


#include "npio.h"

typedef int64_t i64;

static void gettime(struct timespec * t)
{
#ifdef _WIN32
    timespec_get(t, TIME_UTC); // since C11
#else
    clock_gettime(CLOCK_REALTIME, t);
#endif
    return;
}


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

int test_double(int mem)
{
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
    if(mem == 1)
    {
        i64 buff_size;
        void * buff = npio_write_mem(ndim, dim, (void*) D, NPIO_F64, NPIO_F64, &buff_size);
        FILE * fid = fopen(outname, "wb");
        fwrite(buff, buff_size, 1, fid);
        fclose(fid);
        free(buff);
    } else {
        i64 nwritten = npio_write(outname, ndim, dim, (void*) D,
                            NPIO_F64, NPIO_F64);

        printf("Wrote %ld bytes\n", nwritten);
        if(nwritten <= 0)
    {
        printf("%ld bytes written\n", nwritten);
        free(D);
        free(outname);
        return EXIT_FAILURE;
    }
    }
    printf("Reading from %s\n", outname);
    npio_t * np = npio_load(outname);
    if(np == NULL)
    {
        fprintf(stderr, "Unable to open %s\n",
               outname);
               exit(EXIT_FAILURE);
    }
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
    npio_free(np);
    np = NULL;
    free(outname);
    printf("Written and read data is identical\n");
    return EXIT_SUCCESS;
}

int test_float(void)
{
    int M = 2;
    int N = 2;


    int ndim = 2;
    int dim[] = {M, N};
    float * D = malloc(M*N*sizeof(float));
    for(int kk = 0; kk<M*N; kk++)
    {
        D[kk] = (kk+1)+(float) (kk+1) / 10.0;
    }

    char * outname = malloc(100);
    assert(outname != NULL);

    sprintf(outname, "numpy_io_ut_%dx%d.npy", M, N);

    printf("Writing float array to %s\n", outname);
    i64 nwritten = npio_write(outname, ndim, dim, (void*) D,
                            NPIO_F32, NPIO_F32);
    printf("Wrote %ld bytes\n", nwritten);
    if(nwritten < 0)
    {
        free(D);
        free(outname);
        return EXIT_FAILURE;
    }

    printf("Reading from %s\n", outname);
    npio_t * np = npio_load(outname);
    if(np == NULL)
    {
        fprintf(stderr, "Unable to read from %s\n", outname);
    }
    float * in_data = (float*) np->data;
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
    npio_free(np);
    np = NULL;
    free(outname);
    printf("Written and read data is identical\n");
    return EXIT_SUCCESS;
}


int unittest()
{
    show_endian();
    printf("-> Testing write/read double (npio_write)\n");
    if(test_double(0))
    {
        fprintf(stderr, "test_double(0) failed\n");
        return EXIT_FAILURE;
    }
    printf("-> Testing write/read double (npio_write_mem\n");
    if(test_double(1))
    {
        fprintf(stderr, "test_double(1) failed\n");
        return EXIT_FAILURE;
    }
    printf("-> Testing write/read float\n");
    if(test_float())
    {
        fprintf(stderr, "test_float failed\n");
        return EXIT_FAILURE;
    }
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
    npio_free(np);
    np = NULL;
}

static void
resave(const char * from, const char * to)
{
    npio_t * np = npio_load(from);
    if(np == NULL)
    {
        printf("Failed to load %s\n", from);
        exit(EXIT_FAILURE);;
    }
    i64 nwritten = npio_write(to, np->ndim, np->shape, np->data,
                              np->dtype, np->dtype);

    if(nwritten <= 0)
    {
        printf("Failed to write to %s\n", to);
        printf("This is what was read:\n");
        npio_print(stdout, np);
        exit(EXIT_FAILURE);
    }

    npio_free(np);
    np = NULL;

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
    gettime(&tstart);
    for(size_t kk = 0; kk < N; kk++)
    {
        npio_t * np = npio_load(from);
        if(np == NULL)
        {
            printf("Failed to load %s\n", from);
            exit(EXIT_FAILURE);;
        }
        npio_free(np);
        np = NULL;
    }
    gettime(&tend);
    double t_load = timespec_diff(&tend, &tstart);

    npio_t * np = npio_load(from);
    if(np == NULL)
    {
        printf("Failed to load %s\n", from);
        exit(EXIT_FAILURE);;
    }

    gettime(&tstart);
    for(size_t kk = 0; kk < N; kk++)
    {
        i64 nwritten = npio_write(to, np->ndim, np->shape, np->data,
                                NPIO_F64, NPIO_F64);

        if(nwritten < 0)
        {
            printf("Failed to write to %s\n", to);
            exit(EXIT_FAILURE);
        }
    }
    gettime(&tend);
    double t_write = timespec_diff(&tend, &tstart);
    npio_free(np);
    np = NULL;

    printf("To load %s 1000 times took %.4f s\n", from, t_load);
    printf("To write to %s 1000 times took %.4f s\n", to, t_write);

    return;
}

void show_usage(char ** argv)
{
    printf("NPIO version %s\n",
           npio_version());
    printf("\n");
    printf("A program to inspect Numpy npy files using libnpio\n");
    printf("\n");
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
    printf("\n");
    printf("Webpage: https://github.com/elgw/npio\n");
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
