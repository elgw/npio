# npio: Numpy Input and Output for C

A tiny (< 20 kB) C library for reading and writing version 1
Python/numpy `.npy` files.  Most likely you should use
[numpy](https://numpy.org/) instead of this.

The reader is quite general and returns a `struct` containing all
information need to use the data, stored as a `void*`, see
`src/npio.h`.

The library contains methods to write the most common numerical data
types.

## Building from source
To build the library and a small command line interface, just do:
``` shell
$ make -B
building for TARGET=PERFORMANCE
gcc -std=gnu99 -fpic -Wall -Wextra -pedantic -O3 -DNDEBUG -c src/npio.c -o npio.o
gcc -std=gnu99 -fpic -Wall -Wextra -pedantic -O3 -DNDEBUG -c src/dp.c -o dp.o
gcc -std=gnu99 -fpic --shared -Wall -Wextra -pedantic -O3 -DNDEBUG npio.o dp.o -o libnpio.a
gcc -std=gnu99 -Wall -Wextra -pedantic -O3 -DNDEBUG src/npio_cli.c -o npio -L./ -lnpio -lm -flto
```

## Validation
Some cases are covered by the Python script `/npio_test_suite.py`
(which calls `./npio`). Self tests are run by `./npio --unittest`

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

## Known bugs?
The main limitation is that it is only tested on little endian
machines (AARCH64, x86_64), see what is planned etc in the
[CHANGELOG](CHANGELOG.md) list.

## References
- [NEP 1 — A Simple File Format for NumPy
  Arrays](https://github.com/numpy/numpy/blob/067cb067cb17a20422e51da908920a4fbb3ab851/doc/neps/nep-0001-npy-format.rst)
- [numpy/format.py](https://github.com/numpy/numpy/blob/main/numpy/lib/format.py)
