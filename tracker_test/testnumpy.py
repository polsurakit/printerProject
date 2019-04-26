import numpy as np

a = np.array([[1,2],[3,4]])
at = a.transpose()				#transpose
aa = a.dot(at)					#matrix mul
print(a)
print(at)
print(aa)
print(np.linalg.inv(aa))		#matrix inverse

m=[[ 0.55092027,0.08696687,0.8521884 ],[ 0.,0. ,0.  ],[-0.82663418,  0.33364033,  0.5579869 ]]
n= [[  -447.95916748  -8645.17115885 -10481.52109375]]
m = np.array(m)
n = np.array(n)

print(m)
print(n)
print(m.dot(n))