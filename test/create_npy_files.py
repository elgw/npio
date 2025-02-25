import numpy as np

# Arrays taken from https://numpy.org/doc/stable/user/basics.rec.html

x0 = np.array([('Rex', 9, 81.0), ('Fido', 3, 27.0)],
             dtype=[('name', 'U10'), ('age', 'i4'), ('weight', 'f4')])
np.save('structured_1.npy', x0)

x1 = np.array([(1, 2, 3), (4, 5, 6)], dtype='i8, f4, f8')
np.save('structured_2.npy', x1)

x2 = np.zeros(2, dtype='i8, f4, ?, S1')
np.save('structured_3.npy', x2)


a = np.zeros(3, dtype=[('a', 'i8'), ('b', 'f4'), ('c', 'S3')])
b = np.ones(3, dtype=[('x', 'f4'), ('y', 'S3'), ('z', 'O')])
b[:] = a
np.save('structured_4.npy', b)

recordarr = np.rec.array([(1, 2., 'Hello'), (2, 3., "World")],

                   dtype=[('foo', 'i4'),('bar', 'f4'), ('baz', 'S10')])
np.save('structured_5', recordarr);
