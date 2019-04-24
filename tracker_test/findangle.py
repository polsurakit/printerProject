import math


pose_mat = [[],[],[]]
pose_mat[0] = [float(e) for e in input()[1:-3].split(", ")]
pose_mat[1] = [float(e) for e in input()[1:-3].split(", ")]
pose_mat[2] = [float(e) for e in input()[1:-3].split(", ")]
# pose_mat[2] = [float(e) for e in input().split()]

print(pose_mat)


def findangle(pose_mat):
	yaw = 180 / math.pi * math.atan(pose_mat[1][0] /pose_mat[0][0])
	pitch = 180 / math.pi * math.atan(-1 * pose_mat[2][0] / math.sqrt(pow(pose_mat[2][1], 2) + math.pow(pose_mat[2][2], 2)))
	roll = 180 / math.pi * math.atan(pose_mat[2][1] /pose_mat[2][2])
	print('yaw',yaw)
	print('pitch',pitch)
	print('roll',roll)

findangle(pose_mat)