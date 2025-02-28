import numpy as np
D = np.array([[1, 2, 3], [4, 5, 6]])
F = np.array([[1, 2, 3], [4, 5, 6]], order='F')
C = np.array([[1, 2, 3], [4, 5, 6]], order='C')
# order_c.npy and order_default.npy are identical
np.save('order_default.npy', D)
np.save('order_c.npy', C)

# this is of course different
np.save('order_fortran.npy', F)
