# Version history

## 0.0.9
Planned:
 - Added `npio_write_mem` to write an npy file to a memory buffer,
   this could be useful when in-memory compression is wanted (npz via libzip
   etc).
 - Removed usage of `open_memstream` so that the library can be
   compiled on windows (not tested).
 - Moved version info from header file to source.

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
- Care about endianness either in a proper big endian machine or via [https://www.qemu.org]
- Test all numerical types.
- Figure out a cleaner way to communicate errors.
- Need to mention refactor?

## Maybe
- NPZ support?
- Support read/write version 2.0 files ( > 65535 bytes header length).

## Will not
- Supported nested arrays.
