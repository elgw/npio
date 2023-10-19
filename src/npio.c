#include "npio.h"

static void nfree(void * p)
{
    if(p != NULL)
    {
        free(p);
    }
}

void npio_free(npio_t ** _np)
{
    npio_t * np = _np[0];
    nfree(np->filename);
    nfree(np->descr);
    nfree(np->shape_str);
    nfree(np->shape);
    nfree(np->data);
    nfree(np);
    _np[0] = NULL;
    return;
}

static void npd_parse_descr(npio_t * npd)
{
    char * descr = npd->descr;
    npd->np_byte_order = descr[1];
    npd->np_type = descr[2];
    // ok since the dictionary is at most 2^16-1 B
    npd->np_bytes = (int) strtol(descr+3, NULL, 10);
    return;
}

static void print_dtype(FILE * fid, const npio_t * npd)
{
    char byte_order = npd->np_byte_order;
    char type = npd->np_type;;
    int nbytes = npd->np_bytes;
    switch(byte_order)
    {
    case '=':
        fprintf(fid, "native");
        break;
    case '<':
        fprintf(fid, "little endian");
        break;
    case '>':
        fprintf(fid, "big endian");
        break;
    case '|':
        fprintf(fid, "not applicable");
        break;
    default:
        fprintf(fid, "?");
    }

    fprintf(fid, ", ");

    switch(type)
    {
    case 'i':
        fprintf(fid, "integer");
        break;
    case 'b':
        fprintf(fid, "boolean");
        break;
    case 'u':
        fprintf(fid, "unsigned integer");
        break;
    case 'f':
        fprintf(fid, "float");
        break;
    case 'c':
        fprintf(fid, "complex float");
        break;
    case 'm':
        fprintf(fid, "timedelta");
        break;
    case 'M':
        fprintf(fid, "datetime");
        break;
    case 'O':
        fprintf(fid, "object");
        break;
    case 'S':
        fprintf(fid, "string");
        break;
    case 'U':
        fprintf(fid, "unicode string");
        break;
    case 'V':
        fprintf(fid, "void");
        break;
    default:
        fprintf(fid, "?");
        break;
    }
    fprintf(fid, ", %d bytes", nbytes);
    }


void npio_print(FILE * fid, const npio_t * np)
{
    fprintf(fid, "filename: %s\n", np->filename);
    fprintf(fid, "descr: %s (", np->descr);
    print_dtype(fid, np);
    fprintf(fid, ")\n");
    fprintf(fid, "np_byte_order: '%c'\n", np->np_byte_order);
    fprintf(fid, "np_type: '%c'\n", np->np_type);

    fprintf(fid, "fortran_order: %d\n", np->fortran_order);
    fprintf(fid, "ndim: %d\n", np->ndim);
    fprintf(fid, "shape_str: '%s'\n", np->shape_str);
    fprintf(fid, "shape: [");
    for(int kk = 0 ; kk < np->ndim ; kk++)
    {
        if(kk>0)
        {
            fprintf(fid, ", ");
        }
        fprintf(fid, "%d", np->shape[kk]);
    }
    fprintf(fid, "]\n");
    fprintf(fid, "nel: %zu\n", np->nel);
    fprintf(fid, "size of data: %zu x %d = %zu B\n",
            np->nel, np->np_bytes, np->data_size);
}


static int write_dictionary(FILE * fid, const int ndim, const int * shape)
{

    if(ndim < 1)
    {
        return EXIT_FAILURE;
    }

    /// Create the shape string i.e. something like (2, 3,)
    char *shape_str;
    size_t size;
    FILE *stream = open_memstream (&shape_str, &size);
    if(stream == NULL)
    {
        return EXIT_FAILURE;
    }

    fprintf(stream, "(");
    for(int kk = 0; kk+1<ndim; kk++)
    {
        fprintf(stream, "%d, ", shape[kk]);
    }
    fprintf(stream, "%d,", shape[ndim-1]);
    fprintf(stream, ")");
    fclose(stream);
    //printf("Will write shape_str: %s\n", shape_str);

    /// Write the header
    // - header should end with '\n' and then be padded by '\x20'
    // so that
    // len(magic string) + 2 + len(length) + HEADER_LEN should be
    // divisible by 64
    // i.e. HEADER_LEN + 10 should be divisible by 64.

    char * dict = malloc(128+strlen(shape_str));
    if(dict == NULL)
    {
        return EXIT_FAILURE;
    }

    sprintf(dict,
            "{'descr': '<f8', 'fortran_order': False, 'shape': %s, }",
            shape_str);
    free(shape_str);

    size_t _HEADER_LEN = strlen(dict);
    if(_HEADER_LEN > 2<<16)
    {
        goto fail;
    }
    uint16_t HEADER_LEN = (uint16_t) _HEADER_LEN;
    while( (10 + HEADER_LEN) % 64 != 63)
    {
        dict[HEADER_LEN++] = '\x20';
    }
    dict[HEADER_LEN++] = '\n';

    // write length of dictionary (including the padding)
    size_t nwritten = fwrite(&HEADER_LEN, 2, 1, fid);
    if(nwritten != 1)
    {
        goto fail;
    }

    // write the dictionary
    nwritten = fwrite(dict, HEADER_LEN, 1, fid);
    if(nwritten != 1)
    {
        goto fail;
    }

    //printf("nelements: %zu\n", nelements);
    free(dict);
    return EXIT_SUCCESS;

fail:
    free(dict);
    return EXIT_FAILURE;
}

int npio_save_double(const char * filename,
              const int ndim, const int * shape, const double * data)
{

    size_t nelements = 1;
    for(int kk = 0; kk<ndim; kk++)
    {
        nelements *= shape[kk];
    }

    /// Write 6 byte magic header
    FILE * fid = fopen(filename, "w");
    if(fid == NULL)
    {
        fprintf(stderr, "npio_save_double: Unable to open %s for writing\n",
                filename);
        return EXIT_FAILURE;
    }
    char d[] = "\x93NUMPY";
    size_t nwritten = fwrite(d, 1, 6, fid);
    if(nwritten != 6)
    {
        goto fail;
    }

    /// Write major and minor version
    d[0] = 1;
    d[1] = 0;
    nwritten = fwrite(d, 1, 2, fid);
    if(nwritten != 2)
    {
        goto fail;
    }

    /// Write the part of the dictionary that describes the shape
    int status = write_dictionary(fid, ndim, shape);
    if(status != EXIT_SUCCESS)
    {
        goto fail;
    }

    /// write the data
    //printf("Will write %zu elements\n", nelements);
    //printf("First element: %f\n", data[0]);
    nwritten = fwrite(data, 8, nelements, fid);
    if(nwritten != nelements)
    {
        goto fail;
    }

    fclose(fid);
    return EXIT_SUCCESS;

fail:
    fclose(fid);
    return EXIT_FAILURE;
}

static int parse_shape_string(npio_t * npd,
                               const char * sstring, const int len)
{
    //printf("To parse shape string: %.*s\n", len, sstring);
    char * str = strndup(sstring, len);
    //printf("<%s>\n", str);
    str[len-1] = ' ';
    str[0] = ' ';
    //printf("<%s>\n", str);

    char * saveptr = NULL;
    char * str1 = str;
    int ndim = 0;
    size_t nel = 1;
    int * shape = malloc(len*sizeof(int)); // more than big enough
    for(str1 = str; ; str1 = NULL)
    {
        char * tok = strtok_r(str1, ",", &saveptr);
        if(tok == NULL)
        {
            break;
        }
        char * endptr;
        int s = (int) strtol(tok, &endptr, 10);//atoi(tok);
        if(endptr == tok)
        {
            // no digits found
        } else {
            //printf("'%s' -> %d\n", tok, s);
        shape[ndim] = s;
        nel *= s;
        ndim++;
        }
    }
    shape[ndim] = 0; // 0-termination
    npd->ndim = ndim;
    npd->shape = shape;
    npd->nel = nel;
    free(str);
    return EXIT_SUCCESS;
}

npio_t * npio_load(const char * filename)
{
    // TODO: for performance read the whole file directly
    // or use mmap
    FILE * fid = fopen(filename, "r");
    if(fid == NULL)
    {
        printf("npio: failed to open %s\n", filename);
        return NULL;
    }

    struct stat info;
    if(fstat(fileno(fid), &info) != 0)
    {
        printf("npio: could not fstat %s\n", filename);
        fclose(fid);
        return NULL;
    }

    size_t filesize = info.st_blksize;

    // Check magic number and version
    char magic[] = "123456";

    size_t nread = fread(magic, 1, 6, fid);
    if(nread != 6)
    {
        printf("npio: Could not the magic number\n");
        fclose(fid);
        return NULL;
    }

    if(strncmp(magic, "\x93NUMPY", 6) != 0)
    {
            printf("npio: Invalid magic number\n");
            fclose(fid);
            return NULL;
    }

    uint8_t version[2];
    nread = fread(&version, 1, 2, fid);
    if(nread != 2)
    {
        fclose(fid);
        printf("npio: Couldn't read version\n");
        return NULL;
    }
    if(version[0] != 1 || version[1] != 0)
    {
        printf("npio: Numpy file is v %d.%d, only tested for v 1.0\n",
               (int) version[0], (int) version[1]);
        fclose(fid);
        return NULL;
    }

    // Read the size of the dictionary
    uint16_t dsize = 0;
    nread = fread(&dsize, 1, 2, fid);
    if(nread != 2)
    {
        printf("npio: could not read dictionary size\n");
        fclose(fid);
        return NULL;
    }
    //printf("Dictionary size: %u\n", dsize);

    // Read the dictionary
    char * dict = malloc(dsize+1);
    nread = fread(dict, 1, dsize, fid);
    if(nread != dsize)
    {
        printf("npio: could not read %d B dictionary\n", dsize);
        free(dict);
        fclose(fid);
        return NULL;
    }
    dict[dsize] = '\0'; // make a proper string
    //printf("Dictionary: %s\n", dict);

    npio_t * npd = malloc(sizeof(npio_t));
    npd->filename = strdup(filename);
    npd->descr = NULL;
    npd->data = NULL;
    npd->shape = NULL;
    npd->shape_str = NULL;
    npd->ndim = 0;
    npd->nel = 0;
    npd->fortran_order = 1;


    // Parse the dictionary
    dp_t dp; // dictionary parser
    dptok_t t[10]; // expect at most 9 tokens
    int r = dp_parse(dp, dict, strlen(dict), t, 10);
    assert(r < 10);

    for(int kk = 0; kk+1<r; kk = kk + 2)
    {
        if(dp_eq(dict, t+kk, "'descr'") == 0)
        {
            npd->descr = strndup(dict+t[kk+1].start,
                                 t[kk+1].end-t[kk+1].start);
            npd_parse_descr(npd);
        }
        else if(dp_eq(dict, t+kk, "'fortran_order'") == 0)
        {
            if(dp_eq(dict, t+kk+1, "'False'"))
            {
                npd->fortran_order = 0;
            }
        }
        else if(dp_eq(dict, t+kk, "'shape'") == 0)
        {
            int ret = parse_shape_string(npd,
                               dict+t[kk+1].start,
                               t[kk+1].end-t[kk+1].start);
            npd->shape_str = strndup(dict+t[kk+1].start,
                                     t[kk+1].end-t[kk+1].start);
            if(ret == EXIT_FAILURE)
            {
                free(dict);
                fclose(fid);
                printf("npio: Could not parse the shape string\n");
                return NULL;
            }
        }
        /*
        printf(" %.*s = ",
               t[kk].end-t[kk].start, dict + t[kk].start);
        printf("%.*s\n",
               t[kk+1].end-t[kk+1].start, dict + t[kk+1].start);
        */
    }

    free(dict);

    // Forward to the data
    long pos = ftell(fid);
    while(((size_t) pos % 64) != 0)
    {
        pos++;
    }
    r = fseek(fid, pos, SEEK_SET);
    if(r != EXIT_SUCCESS)
    {
        printf("npio: fseek failed on line %d\n", __LINE__);
        fclose(fid);
        goto fail;
    }

    /// Read the data
    size_t nBytes = npd->nel*npd->np_bytes;
    // Don't be tricked to allocate more memory than the actual
    // file size
    if(nBytes > filesize)
    {
        printf("npio: corrupt npy file?\n");
        goto fail;
    }
    //printf("To read %zu elements in %zu B\n", npd->nel, nBytes);

    double * data = malloc(nBytes);
    if(data == NULL)
    {
        goto fail1;
    }
    nread = fread(data, npd->np_bytes, npd->nel, fid);
    if(nread != npd->nel)
    {
        goto fail1;
    }
    npd->data_size = npd->np_bytes*npd->nel;
    fclose(fid);

    npd->data = data;
    return npd;

 fail1:
    free(data);
 fail:
    npio_free(&npd);
    fclose(fid);
    return NULL;
}
