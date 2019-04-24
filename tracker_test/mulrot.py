import math
import numpy as np

def invMatrix(a):
    return np.linalg.inv(a)


def findangle(pose_mat):
	yaw = 180 / math.pi * math.atan(pose_mat[1][0] /pose_mat[0][0])
	pitch = 180 / math.pi * math.atan(-1 * pose_mat[2][0] / math.sqrt(pow(pose_mat[2][1], 2) + math.pow(pose_mat[2][2], 2)))
	roll = 180 / math.pi * math.atan(pose_mat[2][1] /pose_mat[2][2])
	print('yaw',yaw)
	print('pitch',pitch)
	print('roll',roll)

pose_mat = [[],[],[]]
pose_mat[0] = [float(e) for e in input()[1:-3].split(", ")]
pose_mat[1] = [float(e) for e in input()[1:-3].split(", ")]
pose_mat[2] = [float(e) for e in input()[1:-3].split(", ")]
# pose_mat[2] = [float(e) for e in input().split()]

pose_mat2 = [[],[],[]]
pose_mat2[0] = [float(e) for e in input()[1:-3].split(", ")]
pose_mat2[1] = [float(e) for e in input()[1:-3].split(", ")]
pose_mat2[2] = [float(e) for e in input()[1:-3].split(", ")]

pose_mat = np.array(pose_mat)
pose_mat2 = np.array(pose_mat2)

pose_mat3 = pose_mat2.dot(invMatrix(pose_mat))

print(pose_mat3)
findangle(pose_mat3)



