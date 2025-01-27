# Version history

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
