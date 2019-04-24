import triad_openvr
import time
import sys
import numpy as np
import math

RECTANGLE_W = 3300.0        #in 0.1 mm.
RECTANGLE_H = 3300.0        #in 0.1 mm.

ROT_MATRIX_FILENAME = "rotationMatrix.txt"
SYS_MATRIX_FILENAME = "systemMatrix.txt"

def rearrange(s):
    result = [[],[],[],[]]
    for i in range(12):
        if (i+1) %4 == 0:
            result[3].append(s[i]*1000)
        else:
            result[i//4].append(s[i]/10)
    return result

def testRearrange():
    s = []
    for i in range(12):
        s.append(i)
    result = rearrange(s)
    print(result)

def getControllerInfo(v,interval):
    while(True):
        x = []
        s = [0]*12
        for i in range(10):
            start = time.time()
            txt = ""
            xi = v.devices["controller_1"].get_pose()
            x.append(xi)
            for j in range(3):
                for k in range(4):
                    s[j*4+k] += xi[j][k]
            sleep_time = interval-(time.time()-start)
            if sleep_time>0:
                time.sleep(sleep_time)
        #return rearrange(s)
        std = [0]*12
        isP = True
        for i in range(12):
            for j in range(10):
                std[i] += (float(s[i])/10-float(x[j][i//4][i%4]))**2
            std[i] = (std[i]/9)**0.5
        if max(std) < 0.0001:
            return rearrange(s)

def saveRotationMatrix(rot):
    #get and save to file
    txt = ""
    for i in range(3):
        for j in range(3):
            txt += str(rot[i][j]) + ' '
        txt += "\n"
    file = open(ROT_MATRIX_FILENAME, 'w')
    file.write(txt)
    file.close()
    print("Saved rot to", ROT_MATRIX_FILENAME)

def loadRotationMatrix():
    #load from file and return as 3x3 matrix
    file = open(ROT_MATRIX_FILENAME, 'r')
    rot = [[],[],[]]
    idx = 0
    for line in file:
        rot[idx] = [float(e) for e in line.split()]
        idx += 1
    file.close()
    print("Loaded rot from", ROT_MATRIX_FILENAME)
    return rot

def saveSystemMatrix(M,t):
    #save M, t to file
    txt = ""
    for i in range(3):
        for j in range(3):
            txt += M[i][j] + ' '
        txt += "\n"
    for i in range(3):
        txt += t[i] + ' '
    txt += "\n"
    file = open(SYS_MATRIX_FILENAME, 'w')
    file.write(txt)
    file.close()
    print("Saved M,t to", SYS_MATRIX_FILENAME)

def loadSystemMatrix():
    #load M, t from file
    #return as [3x3 M matrix, 3x1 t Matrix]
    file = open(SYS_MATRIX_FILENAME, 'r')
    M = [[],[],[]]
    t = []
    idx = 0
    for line in file:
        if(idx==3):
            t = [float(e) for e in line.split()]
        else:
            M[idx] = [float(e) for e in line.split()]
        idx+=1
    file.close()
    print("Loaded M,t from", SYS_MATRIX_FILENAME)
    return [M,t]

def invMatrix(a):
    return np.linalg.inv(a)

v = triad_openvr.triad_openvr()
v.print_discovered_objects()
interval = 1/250

if len(sys.argv) > 1:
    #get position and angle in our system as string
    #one call
    rot = invMatrix(np.array(loadRotationMatrix()))
    [M,t] = loadSystemMatrix()
    M = np.array(M)
    t = np.array([t])
    
    info = getControllerInfo(v,interval)
    y = M.dot(np.array([info[3]])) - t
    rot_mat = rot.dot(np.array(info[:3]))
    yaw = 180 / math.pi * math.atan(rot_mat[1][0] /rot_mat[0][0])
    pitch = 180 / math.pi * math.atan(-1 * rot_mat[2][0] / math.sqrt(pow(rot_mat[2][1], 2) + math.pow(rot_mat[2][2], 2)))
    roll = 180 / math.pi * math.atan(rot_mat[2][1] /rot_mat[2][2])
    
    txt = str(y[0]) + ' ' + str(y[1]) + ' ' + str(y[2])
    txt += ' ' + yaw + ' ' + pitch + ' ' + roll
    print(txt)
    sys.exit()

print("Please select the mode")
print("0: print raw data")
print("1: full calibration")
print("2: position calibration")
print("3: rotation calibration")
print("4: print data in our system")
print("5: get data")
print("--------------------------------")

mode = int(input())

if mode == 0:
    #review raw data
    while(True):
        g = getControllerInfo(v,interval)
        txt = ''
        for x in g:
            for y in x:
                txt += str(y) + ' '
        print(txt)
        print('@@@@@@@@@@@@@@@@@@@@@@@@')

elif mode == 1:
    #full calibration (rot+pos)
    #requires 1+4 moves (1 correct rotation + 4 angles of rec)
    
    print("move printer to the top left corner")
    print("Press ENTER after finished moving")
    input()
    tl = getControllerInfo(v,interval)
    print(tl)
    tl = tl[3]
    print("Data collected")

    print("move printer to the top right corner")
    print("Press ENTER after finished moving")
    input()
    tr = getControllerInfo(v,interval)
    print(tr)
    tr = tr[3]
    print("Data collected")

    print("move printer to the bottom left corner")
    print("Press ENTER after finished moving")
    input()
    bl = getControllerInfo(v,interval)
    print(bl)
    bl = bl[3]
    print("Data collected")

    print("move printer to the bottom left corner")
    print("Press ENTER after finished moving")
    input()
    br = getControllerInfo(v,interval)
    print(br)
    br = br[3]
    print("Data collected")

    print("move printer to the origin(0,0) with right rotation")
    print("Press ENTER after finished moving")
    input()
    origin = getControllerInfo(v,interval)
    print(origin)

    #save rot
    saveRotationMatrix(origin[:3])

    #find M
    Ax = np.array([[tr[0]-tl[0], tr[1]-tl[1], tr[2]-tl[2]],[br[0]-tl[0],br[1]-tl[1],br[2]-tl[2]]])
    bx = np.array([RECTANGLE_W, RECTANGLE_W])
    M0 = (np.linalg.inv(Ax.transpose().dot(Ax)).dot(Ax.transpose())).dot(bx)
    print(M0)

    M1 = np.array([[0.,0.,0.]])

    Az = np.array([[bl[0]-tl[0], bl[1]-tl[1], bl[2]-tl[2]],[br[0]-tl[0],br[1]-tl[1],br[2]-tl[2]]])
    bz = np.array([RECTANGLE_H, RECTANGLE_H])
    M2 = (np.linalg.inv(Az.transpose().dot(Az)).dot(Az.transpose())).dot(bz)
    print(M2)

    M = np.array([M0,M1,M2])
    print(M)

    #find t
    t = M.dot(np.array([origin[3]]).transpose()).transpose()
    print(t)
    #save M,t
    saveSystemMatrix(M,t)

    print("Finished calibration")

    

elif mode == 2:
    #pos calibration and save (find M[:2] and t[:2])
    #requires 4 moves (4 angles of rec)
    #note that M[2] and t[2] is not necessary for our work
    print("move printer to the top left corner")
    print("Press ENTER after finished moving")
    input()
    tl = getControllerInfo(v,interval)
    tl = tl[3]
    print("Data collected")
    print("move printer to the top right corner")
    print("Press ENTER after finished moving")
    input()
    tr = getControllerInfo(v,interval)
    tr = tr[3]
    print("Data collected")
    print("move printer to the bottom left corner")
    print("Press ENTER after finished moving")
    input()
    bl = getControllerInfo(v,interval)
    bl = bl[3]
    print("Data collected")
    print("move printer to the bottom left corner")
    print("Press ENTER after finished moving")
    input()
    br = getControllerInfo(v,interval)
    br = br[3]
    print("Data collected")
    print("move printer to the origin(0,0) with right rotation")
    print("Press ENTER after finished moving")
    input()
    origin = getControllerInfo(v,interval)

    #find M
    Ax = np.array([[tr[0]-tl[0], tr[1]-tl[1], tr[2]-tl[2]],[br[0]-tl[0],br[1]-tl[1],br[2]-tl[2]]])
    bx = np.array([RECTANGLE_W, RECTANGLE_W])
    M0 = (np.linalg.inv(Ax.transpose().dot(Ax)).dot(Ax.transpose())).dot(bx)
    print(M0)
    M1 = np.array([[0,0,0]])
    Az = np.array([[bl[0]-tl[0], bl[1]-tl[1], bl[2]-tl[2]],[br[0]-tl[0],br[1]-tl[1],br[2]-tl[2]]])
    bz = np.array([RECTANGLE_H, RECTANGLE_H])
    print(Az.transpose().dot(Az))
    M2 = (np.linalg.inv(Az.transpose().dot(Az)).dot(Az.transpose())).dot(bz)
    print(M2)

    M = [M0,M1,M2]

    #find t
    t = M.dot(np.array([origin[3]]))

    #save M,t
    saveSystemMatrix(M,t)

    print("Finished calibration")

elif mode == 3:
    #rot calibration and save
    #requires 1 move with correct rotation
    print("move printer to the right rotation")
    print("Press ENTER after finished moving")
    input()
    origin = getControllerInfo(v,interval)

    #save rot
    saveRotationMatrix(origin[:3])
    print("Finished calibration")

elif mode == 4:
    #review pos and angel in our system
    #compare to information of M,t,first rotation in our file
    #infinity call
    rot = invMatrix(np.array(loadRotationMatrix()))
    [M,t] = loadSystemMatrix()
    M = np.array(M)
    t = np.array([t]).transpose()
    print(M)
    print(t)
    input()
    while(True):
        info = getControllerInfo(v,interval)
        #print(M.dot(np.array([info[3]]).transpose()))
        y = M.dot(np.array([info[3]]).transpose()) - t
        #print(y)
        rot_mat = rot.dot(np.array(info[:3]))
        yaw = 180 / math.pi * math.atan(rot_mat[1][0] /rot_mat[0][0])
        pitch = 180 / math.pi * math.atan(-1 * rot_mat[2][0] / math.sqrt(pow(rot_mat[2][1], 2) + math.pow(rot_mat[2][2], 2)))
        roll = 180 / math.pi * math.atan(rot_mat[2][1] /rot_mat[2][2])
        print(yaw,pitch,roll)
        print('##################',y[0,0],y[1,0],y[2,0])




elif mode == 5:
    #get position and angle in our system as string
    #one call
    rot = invMatrix(np.array(loadRotationMatrix()))
    [M,t] = loadSystemMatrix()
    M = np.array(M)
    t = np.array([t])
    
    info = getControllerInfo(v,interval)
    y = M.dot(np.array([info[3]])) - t
    rot_mat = rot.dot(np.array(info[:3]))
    yaw = 180 / math.pi * math.atan(rot_mat[1][0] /rot_mat[0][0])
    pitch = 180 / math.pi * math.atan(-1 * rot_mat[2][0] / math.sqrt(pow(rot_mat[2][1], 2) + math.pow(rot_mat[2][2], 2)))
    roll = 180 / math.pi * math.atan(rot_mat[2][1] /rot_mat[2][2])
    
    txt = str(y[0]) + ' ' + str(y[1]) + ' ' + str(y[2])
    txt += ' ' + yaw + ' ' + pitch + ' ' + roll
    print(txt)

