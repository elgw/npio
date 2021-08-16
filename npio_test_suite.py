#!/usr/bin/python3

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


if __name__ == '__main__':
    tmpfolder = 'testdata'
    if not os.path.exists(tmpfolder):
        os.mkdir(tmpfolder)


    benchmark(folder = tmpfolder)
    test_load_save(folder = tmpfolder)

    print(f"-> npio passed the tests")
