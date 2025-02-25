npio is a C library for reading and writing `.npy` files.

It supports arrays with a single data type, i.e., not [structured
arrays](https://numpy.org/doc/stable/user/basics.rec.html).

Not everything is supported, see some plans in the
  [CHANGELOG](CHANGELOG.md) list.

## Examples
Start with
``` c
#include <npio.h>
```

Open an npy file and print the some info to stdout:

``` c
npio_t * np = npio_load(filename);
if(np)
{
   npio_print(stdout, np);
   npio_free(np);
}
```

Write an array to a file:

``` shell
npio_write("array.npy",
           ndim, &shape,
           array,
           NPIO_F32, NPIO_F32);
```

## Build and install
To build and install as a shared library, use the `CMakeLists.txt`:

``` shell
mkdir build
cd build
cmake ../
make
sudo make install
```

Alternatively copy and add `npio.h`, `npio.c` and `npio_config.h` to
your project.

## Validation

Some self-tests can be run with `./npio --unittest`. A few more cases
are covered by the Python script `npio_test_suite.py`.


## References
- [NEP 1 — A Simple File Format for NumPy
Arrays](https://github.com/numpy/numpy/blob/067cb067cb17a20422e51da908920a4fbb3ab851/doc/neps/nep-0001-npy-format.rst)
- [numpy/format.py](https://github.com/numpy/numpy/blob/main/numpy/lib/format.py)
