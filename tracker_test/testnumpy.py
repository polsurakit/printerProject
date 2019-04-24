import numpy as np

a = np.array([[1,2],[3,4]])
at = a.transpose()				#transpose
aa = a.dot(at)					#matrix mul
print(a)
print(at)
print(aa)
print(np.linalg.inv(aa))		#matrix inverse