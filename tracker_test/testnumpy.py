import numpy as np

a = np.array([[1,2],[3,4]])
at = a.transpose()				#transpose
aa = a.dot(at)					#matrix mul
print(a)
print(at)
print(aa)
print(np.linalg.inv(aa))		#matrix inverse


m = [[-2.30413734,  64.51707846 ,  0.44714744],[[0, 0, 0]],[-0.77407852, -1.74835318,  0.55278124]]
m = np.array(m)
m= [np.array([ -2.30413734,  64.51707846,   0.44714744]) ,np.array([ 0.,  0.,  0.]),np.array([-0.77407852, -1.74835318,  0.55278124])]
m = np.array(m)
x = [-2953.5064101219177, -7999.259054660797, -19879.238605499268]
x = np.array(x)

print(m.dot(x))
print(m.dot(x.transpose()))

t = [-518172.49029952,      -0.      ,      5282.90571001]
x = [-541201.265928 ,155795.67474 ,17608.878941]
xx = m.dot(np.array(x))
print('.................')
print(xx)
print(t)
print(xx-t)
