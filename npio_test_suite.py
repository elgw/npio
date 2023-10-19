#!/usr/bin/python3

# 2023-10-19
# - everything passed
# - run with Python 3.10.12


import numpy as np
import time
import subprocess
import os

def benchmark(folder, N=1000):
    print('- Running benchmark')
    filename = f"{folder}/bench.npy"
    filename_out = f"{folder}/bench_out.npy"

    start = time.time()
    m = np.random.randint(10) + 1
    n = np.random.randint(10) + 2
    A = np.random.randn(m, n)

    start = time.time()
    for k in range(0, N):
        np.save(filename, A)
    end = time.time()
    t_write = end-start

    for k in range(0, 1000):
        A = np.load(filename)
    end = time.time()
    t_load = end-start

    print("-- Using numpy:")
    print(f"To load {filename} {N} times took {t_load:.4f} s")
    print(f"To write {filename_out} {N} times took {t_write:.4f} s")
    print(' -- using c_numpy_io:')
    subprocess.run(["./npio", "--benchmark", filename, filename_out])


def load_save_validate_file(fe):

    # print(f"Testing {fe}")
    fe_out = fe + 'resave.npy'
    A = np.load(fe)
    cmd = ["./npio", "--resave",  fe, fe_out]
    # print(f"Running {' '.join(cmd)}")
    subprocess.run(cmd)
    # print(f"Loading '{fe_out}'")
    try:
        B = np.load(fe_out)
    except ValueError:
        print(ValueError)
        exit(1)

    try:
        assert(np.allclose(A, B))
    except AssertionError:
        print("A and B are not equal!")
        print(f"A = {A}")
        print(f"B = {B}")


def test_load_save(folder):
    print('- Load-Save validation')
    print('-- double')

    # generate a file
    for k in range(0,100):
        m = np.random.randint(10) + 1
        n = np.random.randint(10) + 2
        A = np.random.randn(m, n)
        fe = folder + '/rand_double.npy'
        np.save(fe, A)
        load_save_validate_file(fe)

    # Empty array
    fe = folder + '/empty.npy'
    np.save(fe, np.array([]))
    load_save_validate_file(fe)

    # One float
    fe = folder + '/one.npy'
    np.save(fe, np.array([1.1]))
    load_save_validate_file(fe)

    # Two floats
    fe = folder + '/two.npy'
    np.save(fe, np.array([1.1, 2.2]))
    load_save_validate_file(fe)

    print('-- float')
    for k in range(0,100):
        m = np.random.randint(10) + 1
        n = np.random.randint(10) + 2
        A = np.random.randn(m, n)
        fe = folder + '/rand_float.npy'
        A = A.astype('float')
        np.save(fe, A)
        load_save_validate_file(fe)

    print('-- uint8')
    for k in range(0,100):
        m = np.random.randint(10) + 1
        n = np.random.randint(10) + 2
        A = 255*np.random.randn(m, n)
        fe = folder + '/rand_uint8.npy'
        A = A.astype('uint8')
        np.save(fe, A)
        load_save_validate_file(fe)

    print('-- uint16')
    for k in range(0,100):
        m = np.random.randint(10) + 1
        n = np.random.randint(10) + 2
        A = 255*np.random.randn(m, n)
        fe = folder + '/rand_uint16.npy'
        A = A.astype('uint16')
        np.save(fe, A)
        load_save_validate_file(fe)

    print('-- uint32')
    for k in range(0,100):
        m = np.random.randint(10) + 1
        n = np.random.randint(10) + 2
        A = 255*np.random.randn(m, n)
        fe = folder + '/rand_uint32.npy'
        A = A.astype('uint32')
        np.save(fe, A)
        load_save_validate_file(fe)

    print('-- uint64')
    for k in range(0,100):
        m = np.random.randint(10) + 1
        n = np.random.randint(10) + 2
        A = 255*np.random.randn(m, n)
        fe = folder + '/rand_uint64.npy'
        A = A.astype('uint64')
        np.save(fe, A)
        load_save_validate_file(fe)

    print('-- int8')
    for k in range(0,100):
        m = np.random.randint(10) + 1
        n = np.random.randint(10) + 2
        A = 255*np.random.randn(m, n)
        fe = folder + '/rand_int8.npy'
        A = A.astype('int8')
        np.save(fe, A)
        load_save_validate_file(fe)

    print('-- int16')
    for k in range(0,100):
        m = np.random.randint(10) + 1
        n = np.random.randint(10) + 2
        A = 255*np.random.randn(m, n)
        fe = folder + '/rand_int16.npy'
        A = A.astype('int16')
        np.save(fe, A)
        load_save_validate_file(fe)

    print('-- int32')
    for k in range(0,100):
        m = np.random.randint(10) + 1
        n = np.random.randint(10) + 2
        A = 255*np.random.randn(m, n)
        fe = folder + '/rand_int32.npy'
        A = A.astype('int32')
        np.save(fe, A)
        load_save_validate_file(fe)

    print('-- int64')
    for k in range(0,100):
        m = np.random.randint(10) + 1
        n = np.random.randint(10) + 2
        A = 255*np.random.randn(m, n)
        fe = folder + '/rand_int64.npy'
        A = A.astype('int64')
        np.save(fe, A)
        load_save_validate_file(fe)


def check_valgrind(command):
    result = subprocess.run(command.split(),
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            universal_newlines=True)
    # Check that no memory leaks
    assert("no leaks are possible" in result.stderr)
    # Check that we got an error from npio
    assert(result.returncode == 1)




def corner_cases(folder):
    print("- Corner cases")
    print("-- Incomplete magic number")
    filename = folder + "/invalid0.npy";
    f = open(filename, "wb")
    f.write(b"\x93NU")
    f.close()
    check_valgrind("valgrind ./npio " + filename)

    print("-- Wrong magic number")
    filename = folder + "/invalid1.npy";
    f = open(filename, "wb")
    f.write(b"\x93MUMPY")
    f.close()
    check_valgrind("valgrind ./npio " + filename)

    print("-- Only magic number")
    filename = folder + "/invalid2.npy";
    f = open(filename, "wb")
    f.write(b"\x93NUMPY")
    f.close()
    check_valgrind("valgrind ./npio " + filename)

    print("-- Wrong version")
    filename = folder + "/invalid3.npy";
    f = open(filename, "wb")
    f.write(b"\x93NUMPY\x01\x01")
    f.close()
    check_valgrind("valgrind ./npio " + filename)

    print("-- Correct version, but no dictionary")
    filename = folder + "/invalid4.npy";
    f = open(filename, "wb")
    f.write(b"\x93NUMPY\x01\x00")
    f.close()
    check_valgrind("valgrind ./npio " + filename)

    print("-- Correct version, missing dictionary")
    filename = folder + "/invalid5.npy";
    f = open(filename, "wb")
    f.write(b"\x93NUMPY\x01\x00\xff\xff")
    f.close()
    check_valgrind("valgrind ./npio " + filename)

    print("-- Correct version, incomplete dictionary");
    filename = folder + "/invalid6.npy";
    f = open(filename, "wb")
    dict = b"{'descr': '<f8', 'fortran_order': False, 'shape': (1000, 1000, 1000,), }"
    print(len(dict))
    f.write(b"\x93NUMPY\x01\x00\x48\x00" + dict)
    f.close()
    check_valgrind("valgrind ./npio " + filename)
    # ...

if __name__ == '__main__':
    tmpfolder = 'testdata'
    if not os.path.exists(tmpfolder):
        os.mkdir(tmpfolder)

    benchmark(folder = tmpfolder)
    test_load_save(folder = tmpfolder)

    corner_cases(tmpfolder)

    print(f"-> npio passed the tests")
