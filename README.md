# npio: Numpy Input and Output for C

A tiny (< 20 kB) C library for reading and writing version 1
Python/numpy `.npy` files.  Most likely you should use
[numpy](https://numpy.org/) instead of this.

The reader is quite general and returns a `struct` containing all
information need to use the data, stored as a `void*`, see
`src/npio.h`.

For writing, there is an interface to write `double` data, it should
be trivial to add support for other formats, at least numerical ones.

## Building from source
To build the command line interface:
``` shell
$ make -B
building for TARGET=PERFORMANCE
gcc -std=gnu99 -fpic -Wall -Wextra -pedantic -O3 -DNDEBUG -c src/npio.c -o npio.o
gcc -std=gnu99 -fpic -Wall -Wextra -pedantic -O3 -DNDEBUG -c src/dp.c -o dp.o
gcc -std=gnu99 -fpic --shared -Wall -Wextra -pedantic -O3 -DNDEBUG npio.o dp.o -o libnpio.a
gcc -std=gnu99 -Wall -Wextra -pedantic -O3 -DNDEBUG src/npio_cli.c -o npio -L./ -lnpio -lm -flto
```

## Validation
Some cases are covered by a Python script:

``` shell
$ ./npio_test_suite.py
- Running benchmark
-- Using numpy:
To load testdata/bench.npy 1000 times took 0.2621 s
To write testdata/bench_out.npy 1000 times took 0.1333 s
 -- using c_numpy_io:
To load testdata/bench.npy 1000 times took 0.0033 s
To write to testdata/bench_out.npy 1000 times took 0.0310 s
- Load-Save validation
-> npio passed the tests
```

In the CLI interface does also have a few self-tests, run with:


``` shell
$ ./npio --unittest
```

## Command Line Utility
The command line utility was written to show example usage. However it can also
be used to quickly inspect `.npy` files:

``` shell
$ time ./npio numpy_io_ut_2x2.npy
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

real	0m0,001s
user	0m0,001s
sys	0m0,000s

$ time python3 -c 'import numpy as np; A = np.load("numpy_io_ut_2x2.npy"); print(A.shape)'
(2, 2)

real	0m0,065s
user	0m0,054s
sys	0m0,012s

```

## Known bugs?
The main limitation is that it is only tested on little endian
machines (AARCH64, x86_64), see what is planned etc in the
[CHANGELOG](CHANGELOG.md) list.

## References
- [NEP 1 — A Simple File Format for NumPy Arrays](https://github.com/numpy/numpy/blob/067cb067cb17a20422e51da908920a4fbb3ab851/doc/neps/nep-0001-npy-format.rst)
- [numpy/format.py](https://github.com/numpy/numpy/blob/main/numpy/lib/format.py)
