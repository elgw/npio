#include "mex.h"
#include "npio.h"

/* # STATUS
 *
 * This is not finished, and will not be continued since Matlab already
 * can do this:
 *
 * >>> array = py.numpy.load('file.npy')
 * and
 * >>> py.numpy.save('file.npy', array)
 *
 * At the moment it can:
 *
 * - Save float and double matrices.
 * - When loading, returns a matrix of 0s of.
 *
 * # BUILD
 *
 * mex -largeArrayDims -output npio npio.c mex_npio.c
 *
 *
 * # Useful resources on the Matlab MEX API:
 *
 * https://se.mathworks.com/help/matlab/call-mex-files-1.html
 * https://se.mathworks.com/help/matlab/cc-mx-matrix-library.html
 */

static void
show_usage(void)
{
    mexPrintf("npio usage:\n"
              "\n"
              "To read an npy file:\n"
              "data = npio('file.npy');\n"
              "\n"
              "To write:\n"
              "status = npio(array, 'file.npy')\n"
              "\n");
    mexErrMsgTxt("Wrong number of arguments");
}

void write_npy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if(!(mxIsChar(prhs[1])))
    {
        mexErrMsgTxt("The 2nd argument must be a file name (char).");
    }

    if (!(mxIsNumeric(prhs[0]))) {
        mexErrMsgTxt("First argument must be a numeric array.");
    }

    if (mxIsComplex(prhs[0])) {
        mexErrMsgTxt("Complex data is not supported");
    }

    void * data = mxGetPr(prhs[0]);

    mwSize _ndim = mxGetNumberOfDimensions(prhs[0]);
    const mwSize * _dim = mxGetDimensions(prhs[0]);
    npio_dtype dtype = NPIO_NOSUPPORT;

    // TODO: Add the other common data types here

    if(mxIsSingle(prhs[0]))
    {
        dtype = NPIO_F32;
    }

    if(mxIsDouble(prhs[0]))
    {
        dtype = NPIO_F64;
    }

    if(dtype == NPIO_NOSUPPORT)
    {
        mexErrMsgTxt("Unsupported data format");
    }

    int ndim = _ndim;
    int dim[ndim];
    for(int kk = 0; kk < ndim; kk++)
    {
        dim[kk] = _dim[ndim-kk-1];
    }

    size_t n_name = mxGetNumberOfElements(prhs[1]) + 1;
    char * name = calloc(n_name, 1);
    if(mxGetString(prhs[1], name, n_name))
    {
        free(name);
        mexErrMsgTxt("Something is funky with the file name");
    }
    if(npio_write(name, ndim, dim, data, dtype, dtype) == -1)
    {
        free(name);
        mexErrMsgTxt("Failed writing");
    }
    free(name);
}

void load_npy(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if(!(mxIsChar(prhs[0])))
    {
        mexErrMsgTxt("The argument must be a file name (char).");
    }
    size_t n_name = mxGetNumberOfElements(prhs[0]) + 1;
    char * name = calloc(n_name, 1);
    if(mxGetString(prhs[0], name, n_name))
    {
        free(name);
        mexErrMsgTxt("Something is funky with the file name");
    }
    mexPrintf("Loading %s\n", name);
    npio_t * meta = npio_load_metadata(name);

    if(meta == NULL)
    {
        free(name);
        mexErrMsgTxt("Failed to load the file");
    }

    /* Figure out if supported data type. */

    mwSize outdim[meta->ndim];
    for(int kk = 0; kk < meta->ndim; kk++)
    {
        outdim[kk] = meta->shape[meta->ndim-kk-1];
    }

    plhs[0] = mxCreateNumericArray((mwSize) meta->ndim, outdim, mxDOUBLE_CLASS, mxREAL);
    void * D = mxGetPr(plhs[0]);

    // TODO: load the npy data directly to the buffer allocated by
    //       mxCreateNumericArray, i.e. something like:
    //       npio_load_buff(name, D, D_nb);

    free(name);
    npio_free(meta);
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    switch(nrhs)
    {
    case 1:
        load_npy(nlhs, plhs, nrhs, prhs);
        break;
    case 2:
        write_npy(nlhs, plhs, nrhs, prhs);
        break;
    default:
        show_usage();
        break;
    }
}
