# npio: Numpy Input and Output for C

Features

- A tiny (~ 20 kB) C library for reading and writing version 1
Python/numpy `.npy` files.
- No dependencies.

Limitations

- Only tested on little endian machines (AARCH64, x86_64).
- Not everything is supported, see some plans in the [CHANGELOG](CHANGELOG.md) list.


## Minimal example

This code will open an npy file and show the metadata:

``` c
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
```

Which will give the following output:
``` shell
$ gcc -Wall -Wextra -pedantic -fanalyzer minimal.c -lnpio
$ ./a.out numpy_io_ut_2x2.npy
filename: numpy_io_ut_2x2.npy
descr: '<f4' (little endian, float, 4 bytes)
np_byte_order: '<'
np_type: 'f'
fortran_order: 0
ndim: 2
shape_str: '(2, 2,)'
shape: [2, 2]
nel: 4
size of data: 4 x 4 = 16 B
```

Actual data can be found in the `npio_t` object.

## Build and install

``` shell
mkdir build
cd build
cmake ../
sudo make install
```

## Validation
- [x] Passes valgrind.
- [x] Passes `-fanalyzer`.
- Some cases are covered by the Python script `/npio_test_suite.py`
(which calls `./npio`).
- Self tests are run by `./npio --unittest`.
- [ ] Fuzzing?
- [ ] Other evil edge cases.

## Command Line Utility

The command line utility exists mostly to demonstrate how to use the
library. However it can also be used to quickly inspect `.npy` files:

``` shell
./npio numpy_io_ut_2x2.npy
Loading numpy_io_ut_2x2.npy
filename: numpy_io_ut_2x2.npy
descr: '<f8' (little endian, float, 8 bytes)
np_byte_order: '<'
np_type: 'f'
fortran_order: 0
ndim: 2
shape_str: '(2, 2,)'
shape: [2, 2]
nel: 4
size of data: 4 x 8 = 32 B
```

build it from the makefile.

## References
- [NEP 1 — A Simple File Format for NumPy
Arrays](https://github.com/numpy/numpy/blob/067cb067cb17a20422e51da908920a4fbb3ab851/doc/neps/nep-0001-npy-format.rst)
- [numpy/format.py](https://github.com/numpy/numpy/blob/main/numpy/lib/format.py)
