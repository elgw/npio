/* A library to read/write numpy .npy-files
 * see npio_cli.c for example usage.
*/

#ifndef __npio_h__
#define __npio_h__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "dp.h"

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
    void * data;
    size_t data_size; // total size of data, in bytes
} npio_t;

int npio_save_double(const char * filename,
              const int ndim, const int * shape,
              const double * data);

/* Read a .npy file

The pointer to the data can be stolen:
   double * my_data = (double*) np->data;
   np->data = NULL;
   npio_free(np); // will ignore np->data

Returns NULL on failure.
 */
npio_t * npio_load(const char * filename);

/* Print some info about the npio_t object */
void npio_print(FILE *, const npio_t * np);

/* Free *np and set it to NULL */
void npio_free(npio_t ** np);

#endif
