# npio: a lib for reading and writing NPY 1.0 files

NPY files are nice for passing array data between programs. They
contain metadata about the array shape and data type which eliminates
the risk of silly mistakes which typically occurs when passing around
raw data.

- This library compiles to about 25 kb and should be simple to
  integrate into small projects that are only interested that want to
  import and or export NPY files.

- It supports n-dimensional arrays with a single data type, i.e., not
[structured
arrays](https://numpy.org/doc/stable/user/basics.rec.html).

- Up to 20X faster loading and 2X faster saving than Numpy for small
  files

Quirks:

- Always saves arrays with `'fortran_order': False`, i.e. up to the
  user to navigate around this potential problem.

- Ignores the byte order. The code is only run on little endian
  machines (AARCH64, x86_64) (to do).

To do:

- Decide for a better way to communicate errors without relying on
  `stdout`.

- Facilitate memory mapping of the data.

- Load data to a user-provided buffer.

- NPZ support using libzip.

Will not:

- Supported nested arrays, and for the same reason there is no point
  in supporting Numpy files of version 2.0 ( > 65535 bytes header
  length).

For what has been done, see the [CHANGELOG](CHANGELOG.md).

## Build and install
To build and install as a shared library, use the `CMakeLists.txt`:

``` shell
mkdir build
cd build
cmake ../
make
sudo make install
```

Alternatively just copy `npio.h`, `npio.c` and `npio_config.h` to
your project source folder and add them to the build procedure.

## Example usage

Open an NPY file and print the some info to stdout:

``` c
#include <npio.h>

...

npio_t * np = npio_load(filename);
if(np)
{
   npio_print(stdout, np);
   npio_free(np);
}
```

Write an array to a file:

``` shell
int ndim = 3;
float * my_data = output_of_my_alg();
int shape[3] = {1024, 1024, 121};
if(npio_write(filename,
           ndim, &shape,
           array,
           NPIO_F32, NPIO_F32))
   {
      print("Failed to writ to %s\n", filename);
   }
```

## Validation

Some self-tests can be run with `./npio --unittest`. A few more cases
are covered by the Python script `npio_test_suite.py`:

<details><summary>Example output from npio_test_suite.py</summary>

``` shell
$ cd test
$ python npio_test_suite.py
Using ../build/npio
- Running benchmark
-- Using numpy:
To load testdata/bench.npy 1000 times took 0.1456 s
To write testdata/bench_out.npy 1000 times took 0.0830 s
 -- using c_numpy_io:
cmd: ../build/npio --benchmark testdata/bench.npy testdata/bench_out.npy
To load testdata/bench.npy 1000 times took 0.0067 s
To write to testdata/bench_out.npy 1000 times took 0.0517 s
- Load-Save validation
-- double
-- float
-- uint8
-- uint16
-- uint32
-- uint64
-- int8
-- int16
-- int32
-- int64
- Corner cases
-- Incomplete magic number
-- Wrong magic number
-- Only magic number
-- Wrong version
-- Correct version, but no dictionary
-- Correct version, missing dictionary
-- Correct version, incomplete dictionary
-> npio passed the tests
```

</details>

## References
- [NEP 1 — A Simple File Format for NumPy
Arrays](https://github.com/numpy/numpy/blob/067cb067cb17a20422e51da908920a4fbb3ab851/doc/neps/nep-0001-npy-format.rst)
- [numpy.lib.format](https://numpy.org/doc/stable/reference/generated/numpy.lib.format.html)
- [numpy/format.py](https://github.com/numpy/numpy/blob/main/numpy/lib/format.py)
