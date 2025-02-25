# Version history

## 0.0.10
 - Identifies nested arrays, to be able to print more useful error
   messages when not reading them.

## 0.0.9
 - Added `npio_write_mem` to write an npy file to a memory buffer,
   this could be useful when in-memory compression is wanted (npz via libzip
   etc).
 - Moved version info from header file to source.
 - Confirmed to build under Windows without modifications, but with
   some squeals from the compiler.

## 0.0.8
- Dropped the `unistd.h` dependency.
- Added `npio_load_metadata` to load only metadata.

## 0.0.7
- Possible to write directly to a file descriptor with `npio_write_FILE`.
- API change, there is now only `npio_write` and `npio_write_FILE` for
  writing data and they return the number of written bytes on success.

## 0.0.6
- Single source file, `dp.c/h` is now integrated into `npio.c/h`.

## 0.0.5
- Fixed issue in opening large files.

## 0.0.4
- renamed npio_save_uint8_t to npio_save_u8 etc.

## 0.0.3
- Changed signature of `np_free`.
- Added a `CMakeLists.txt`
- Minor changes so that `-fanalyzer` does not complain.

## 0.0.2
- Reasonable support also for signed and unsigned integers
- Extended `npio_test_suite.py` to also run valgrind checks for the
  corner cases.

## 0.0.1
- Reasonable support for double and float.
- Compiles without errors or warnings.
- No memory leaks in error free paths.

# Status

## To do
- Decide for a better way to communicate errors. I think that it makes
  sense that the library does not use stdout if not asked for it.
- Facilitate memory mapping of the data.
- NPZ support is planned using libzip.


## Will not

Items on this list could happen if YOU implement them.

- Supported nested arrays, and for the same reason there is no point
  in supporting Numpy files of version 2.0 ( > 65535 bytes header
  length).

- Care about the byte order. The code is only run on little endian
  machines (AARCH64, x86_64).
