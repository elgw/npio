#pragma once

/* A library to read/write numpy .npy-files
 * see npio_cli.c for example usage.
*/

#define NPIO_VERSION_MAJOR "0"
#define NPIO_VERSION_MINOR "0"
#define NPIO_VERSION_PATCH "2"
#define NPIO_version NPIO_VERSION_MAJOR "."  \
    NPIO_VERSION_MINOR "."                        \
    NPIO_VERSION_PATCH


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


typedef struct{
    char * filename;
    char * descr; // data type description
    char np_byte_order;
    char np_type;
    int np_bytes;
    int fortran_order;
    int ndim; // number of dimensions
    int * shape;
    char * shape_str;
    size_t nel; // number of elements
    void * data; //
    size_t data_size; // total size of data, in bytes
} npio_t;

/** @brief Save a double array to an npy file */
int npio_save_double(const char * filename,
              const int ndim, const int * shape,
              const double * data);

int npio_save_float(const char * filename,
                     const int ndim, const int * shape,
                     const float * data);

int npio_save_int8_t(const char * filename,
                     const int ndim, const int * shape,
                     const int8_t * data);

int npio_save_int16_t(const char * filename,
                     const int ndim, const int * shape,
                     const int16_t * data);

int npio_save_int32_t(const char * filename,
                     const int ndim, const int * shape,
                     const int32_t * data);

int npio_save_int64_t(const char * filename,
                     const int ndim, const int * shape,
                     const int64_t * data);

int npio_save_uint8_t(const char * filename,
                    const int ndim, const int * shape,
                    const uint8_t * data);

int npio_save_uint16_t(const char * filename,
                      const int ndim, const int * shape,
                      const uint16_t * data);

int npio_save_uint32_t(const char * filename,
                       const int ndim, const int * shape,
                       const uint32_t * data);

int npio_save_uint64_t(const char * filename,
                       const int ndim, const int * shape,
                       const uint64_t * data);


/** @brief Read a .npy file
 *
 * The pointer to the data can be stolen:
 *    double * my_data = (double*) np->data;
 *    np->data = NULL;
 *    npio_free(np); // will ignore np->data
 *
 * Returns NULL on failure.
 */
npio_t * npio_load(const char * filename);

/** @brief Print some info about the npio_t object
 *
*/
void npio_print(FILE *, const npio_t * np);

/* @brief Free an npio_t object
 *
 * Frees *np and sets it to NULL
*/
void npio_free(npio_t ** np);
