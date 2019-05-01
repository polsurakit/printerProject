import triad_openvr
import time
import sys
import numpy as np
import math

RECTANGLE_W = 8410.0        #in 0.1 mm.
RECTANGLE_H = 3040.0        #in 0.1 mm.

ROT_MATRIX_FILENAME = "rotationMatrix.txt"
SYS_MATRIX_FILENAME = "systemMatrix.txt"

def rearrange(s,n):
    result = [[],[],[],[]]
    for i in range(12):
        if (i+1) %4 == 0:
            result[3].append(s[i]/n)
        else:
            result[i//4].append(s[i]/n)
    return result

def testRearrange():
    s = []
    for i in range(12):
        s.append(i)
    result = rearrange(s)
    print(result)

def getControllerInfo(v,interval,isprint=True):
    minx = 9999999999
    miny = 9999999999
    minz = 9999999999
    maxx = -9999999999
    maxy = -9999999999
    maxz = -9999999999
    cx = 0
    cy = 0
    cz = 0
    while(True):
        x = []
        n = 30
        s = [0]*12
        for i in range(n):
            start = time.time()
            txt = ""
            xi = v.devices["controller_1"].get_pose()
            x.append(xi)
            for j in range(3):
                for k in range(4):
                    if k==3:
                        xi[j][k]*=10000
                    s[j*4+k] += xi[j][k]
            sleep_time = interval-(time.time()-start)
            if sleep_time>0:
                time.sleep(sleep_time)
        #return rearrange(s)
        std = [0]*12
        isP = True
        return rearrange(s,n)
        for i in range(12):
            for j in range(n):
                std[i] += (float(s[i])/n-float(x[j][i//4][i%4]))**2
            std[i] = (std[i]/(n-1))**0.5
        stdpos = [std[3], std[7], std[11]]
        std[3] = 0
        std[7] = 0
        std[11] = 0
        m1 = max(std)
        # print(m1)
        if m1 < 0.0001:
            return rearrange(s,n)
        x = s[3]/n
        y = s[7]/n
        z = s[11]/n
        if x < minx or x > maxx:
            maxx = max(maxx, math.ceil(x))
            minx = min(minx, math.floor(x))
            cx = 0
        else:
            cx += 1
        if y < miny or y > maxy:
            maxy = max(maxy, math.ceil(y))
            miny = min(miny, math.floor(y))
            cy = 0
        else:
            cy += 1
        if z < minz or z > maxz:
            maxz = max(maxz, math.ceil(z))
            minz = min(minz, math.floor(z))
            cz = 0
        else :
            cz += 1
        m1 = max(std)
        print(m1)
        if m1 < 0.0001:

            if cx > 10 and cy > 10 and cz > 10:
                return rearrange(s,n)
            elif isprint:
                print(s[3]/n, s[7]/n, s[11]/n, cx, cy, cz)

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
    # print("Loaded rot from", ROT_MATRIX_FILENAME)
    return rot

def saveSystemMatrix(M,t):
    #save M, t to file
    txt = ""
    for i in range(3):
        for j in range(3):
            txt += str(M[i][j]) + ' '
        txt += "\n"
    for i in range(3):
        txt += str(t[0][i]) + ' '
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
    # print("Loaded M,t from", SYS_MATRIX_FILENAME)
    return [M,t]

def invMatrix(a):
    return np.linalg.inv(a)

v = triad_openvr.triad_openvr()
if len(sys.argv)==1:
    v.print_discovered_objects()
interval = 1/100

if len(sys.argv) > 1:
    #get position and angle in our system as string
    #one call
    rot = invMatrix(np.array(loadRotationMatrix()))
    [M,t] = loadSystemMatrix()
    M = np.array(M)
    t = np.array([t])
    
    info = getControllerInfo(v,interval,False)
    y = M.dot(np.array([info[3]]).transpose()) - t.transpose()
    rot_mat = np.array(info[:3]).dot(rot)
    yaw = 180 / math.pi * math.atan(rot_mat[1][0] /rot_mat[0][0])
    pitch = math.atan(-1 * rot_mat[2][0] / math.sqrt(pow(rot_mat[2][1], 2) + math.pow(rot_mat[2][2], 2)))
    roll = 180 / math.pi * math.atan(rot_mat[2][1] /rot_mat[2][2])
    a = -250
    b = 2000
    y[0][0] = y[0][0] - a + math.cos(pitch)*a - math.sin(pitch)*b
    y[2][0] = y[2][0] - b + math.sin(pitch)*a + math.cos(pitch)*b
    txt = str(y[0][0]) + ' ' + str(y[1][0]) + ' ' + str(y[2][0])
    txt += ' ' + str(yaw) + ' ' + str(pitch*180/math.pi) + ' ' + str(roll)
    print(txt)
    sys.exit()

print("Please select the mode")
print("0: print raw data")
print("1: full calibration (M,t,R)")
print("2: position calibration (M)")
print("3: rotation calibration R")
print("4: origin calibration t")
print("5: print data in our system")
print("6: get data")
print("--------------------------------")

mode = int(input())

if mode == 0:
    #review raw data
    while True:
        x = v.devices["controller_1"].get_pose()
        start = time.time()
        
        sleep_time = interval-(time.time()-start)
        print(x[0][3]*10000,x[1][3]*10000,x[2][3]*10000)
        if sleep_time>0:
            time.sleep(2)

elif mode == 1:
    #full calibration (rot+pos)
    #requires 1+4 moves (1 correct rotation + 4 angles of rec)
    while True:
        print("move printer to the top right corner")
        print("Press ENTER after finished moving")
        input()
        tr = getControllerInfo(v,interval)
        print(tr)
        tr = tr[3]
        print("Data collected")
        print("If you need to repeat this step type 'r' and ENTER")
        print("Else, just press ENTER")
        repeat = input()
        if repeat != 'r':
            break
    while True:
        print("move printer to the bottom right corner")
        print("Press ENTER after finished moving")
        input()
        br = getControllerInfo(v,interval)
        print(br)
        br = br[3]
        print("Data collected")
        print("If you need to repeat this step type 'r' and ENTER")
        print("Else, just press ENTER")
        repeat = input()
        if repeat != 'r':
            break

    while True:
        print("move printer to the bottom left corner")
        print("Press ENTER after finished moving")
        input()
        bl = getControllerInfo(v,interval)
        print(bl)
        bl = bl[3]
        print("Data collected")
        print("If you need to repeat this step type 'r' and ENTER")
        print("Else, just press ENTER")
        repeat = input()
        if repeat != 'r':
            break

    while True:
        print("move printer to the top left corner")
        print("Press ENTER after finished moving")
        input()
        tl = getControllerInfo(v,interval)
        print(tl)
        tl = tl[3]
        print("Data collected")
        print("If you need to repeat this step type 'r' and ENTER")
        print("Else, just press ENTER")
        repeat = input()
        if repeat != 'r':
            break

    #find M
    Ax = []
    Ax.append([tr[0]-tl[0], tr[1]-tl[1], tr[2]-tl[2]])
    Ax.append([br[0]-tl[0], br[1]-tl[1], br[2]-tl[2]])
    Ax.append([bl[0]-tl[0], bl[1]-tl[1], bl[2]-tl[2]])
    Ax.append([bl[0]-tr[0], bl[1]-tr[1], bl[2]-tr[2]])
    Ax.append([br[0]-tr[0], br[1]-tr[1], br[2]-tr[2]])
    Ax.append([br[0]-bl[0], br[1]-bl[1], br[2]-bl[2]])
    Ax = np.array(Ax)
    bx = np.array([RECTANGLE_W, RECTANGLE_W, 0, -RECTANGLE_W, 0, RECTANGLE_W])
    M0 = (np.linalg.inv(Ax.transpose().dot(Ax)).dot(Ax.transpose())).dot(bx)
    
    print(M0)

    M1 = np.array([0.,0.,0.])

    Az = Ax
    bz = np.array([0, RECTANGLE_H, RECTANGLE_H, RECTANGLE_H, RECTANGLE_H, 0])
    M2 = (np.linalg.inv(Az.transpose().dot(Az)).dot(Az.transpose())).dot(bz)
    print(M2)

    M = np.array([M0,M1,M2])
    print(M)

    while True:
        print("move printer to the origin(0,0) with right rotation")
        print("Press ENTER after finished moving")
        input()
        origin = getControllerInfo(v,interval)
        print(origin)
        print("Data collected")
        print("If you need to repeat this step type 'r' and ENTER")
        print("Else, just press ENTER")
        repeat = input()
        if repeat != 'r':
            break

    #save rot
    saveRotationMatrix(origin[:3])

    

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
    while True:
        print("move printer to the top right corner")
        print("Press ENTER after finished moving")
        input()
        tr = getControllerInfo(v,interval)
        print(tr)
        tr = tr[3]
        print("Data collected")
        print("If you need to repeat this step type 'r' and ENTER")
        print("Else, just press ENTER")
        repeat = input()
        if repeat != 'r':
            break
    while True:
        print("move printer to the bottom right corner")
        print("Press ENTER after finished moving")
        input()
        br = getControllerInfo(v,interval)
        print(br)
        br = br[3]
        print("Data collected")
        print("If you need to repeat this step type 'r' and ENTER")
        print("Else, just press ENTER")
        repeat = input()
        if repeat != 'r':
            break

    while True:
        print("move printer to the bottom left corner")
        print("Press ENTER after finished moving")
        input()
        bl = getControllerInfo(v,interval)
        print(bl)
        bl = bl[3]
        print("Data collected")
        print("If you need to repeat this step type 'r' and ENTER")
        print("Else, just press ENTER")
        repeat = input()
        if repeat != 'r':
            break

    while True:
        print("move printer to the top left corner")
        print("Press ENTER after finished moving")
        input()
        tl = getControllerInfo(v,interval)
        print(tl)
        tl = tl[3]
        print("Data collected")
        print("If you need to repeat this step type 'r' and ENTER")
        print("Else, just press ENTER")
        repeat = input()
        if repeat != 'r':
            break

    #find M
    Ax = []
    Ax.append([tr[0]-tl[0], tr[1]-tl[1], tr[2]-tl[2]])
    Ax.append([br[0]-tl[0], br[1]-tl[1], br[2]-tl[2]])
    Ax.append([bl[0]-tl[0], bl[1]-tl[1], bl[2]-tl[2]])
    Ax.append([bl[0]-tr[0], bl[1]-tr[1], bl[2]-tr[2]])
    Ax.append([br[0]-tr[0], br[1]-tr[1], br[2]-tr[2]])
    Ax.append([br[0]-bl[0], br[1]-bl[1], br[2]-bl[2]])
    Ax = np.array(Ax)
    bx = np.array([RECTANGLE_W, RECTANGLE_W, 0, -RECTANGLE_W, 0, RECTANGLE_W])
    M0 = (np.linalg.inv(Ax.transpose().dot(Ax)).dot(Ax.transpose())).dot(bx)
    
    print(M0)

    M1 = np.array([0.,0.,0.])

    Az = Ax
    bz = np.array([0, RECTANGLE_H, RECTANGLE_H, RECTANGLE_H, RECTANGLE_H, 0])
    M2 = (np.linalg.inv(Az.transpose().dot(Az)).dot(Az.transpose())).dot(bz)
    print(M2)

    M = np.array([M0,M1,M2])
    print(M)

    saveSystemMatrix(M,[[0,0,0]])
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
    #rot calibration and save
    #requires 1 move with correct rotation
    print("move printer to the origin")
    print("Press ENTER after finished moving")
    input()
    origin = getControllerInfo(v,interval)

    #save rot

    [M,t] = loadSystemMatrix()
    M = np.array(M)

    #find new t
    t = M.dot(np.array([origin[3]]).transpose()).transpose()
    print(t)
    #save M,t
    saveSystemMatrix(M,t)

    print("Finished calibration")

elif mode == 5:
    #review pos and angel in our system
    #compare to information of M,t,first rotation in our file
    #infinity call
    rot = invMatrix(np.array(loadRotationMatrix()))
    [M,t] = loadSystemMatrix()
    M = np.array(M)
    t = np.array([t]).transpose()
    print(M)
    print(t)
    
    while(True):
        # input()
        while(True):
            x = []
            n = 30
            s = [0]*12
            for i in range(n):
                start = time.time()
                txt = ""
                xi = v.devices["controller_1"].get_pose()
                x.append(xi)
                for j in range(3):
                    for k in range(4):
                        if k==3:
                            xi[j][k]*=10000
                        s[j*4+k] += xi[j][k]
                sleep_time = interval-(time.time()-start)
                if sleep_time>0:
                    time.sleep(sleep_time)
            #return rearrange(s)
            std = [0]*12
            isP = True
            for i in range(12):
                for j in range(n):
                    std[i] += (float(s[i])/n-float(x[j][i//4][i%4]))**2
                std[i] = (std[i]/(n-1))**0.5
            stdpos = [std[3], std[7], std[11]]
            std[3] = 0
            std[7] = 0
            std[11] = 0
            
            m1 = max(std)
            #print(m1)
            if m1 < 1:
                info = rearrange(s,n)
                break;

        #print(M.dot(np.array([info[3]]).transpose()))
        y = M.dot(np.array([info[3]]).transpose()) - t
        #print(y)
        rot_mat = np.array(info[:3]).dot(rot)
        yaw = 180 / math.pi * math.atan(rot_mat[1][0] /rot_mat[0][0])
        pitch = 180 / math.pi * math.atan(-1 * rot_mat[2][0] / math.sqrt(pow(rot_mat[2][1], 2) + math.pow(rot_mat[2][2], 2)))
        roll = 180 / math.pi * math.atan(rot_mat[2][1] /rot_mat[2][2])
        print('yaw,pitch,roll=',yaw,pitch,roll)
        print('################## x,y,z=',y[0,0],y[1,0],y[2,0])




elif mode == 6:
    #get position and angle in our system as string
    #one call
    rot = invMatrix(np.array(loadRotationMatrix()))
    [M,t] = loadSystemMatrix()
    M = np.array(M)
    t = np.array([t])
    
    info = getControllerInfo(v,interval,False)
    y = M.dot(np.array([info[3]])) - t
    rot_mat = np.array(info[:3]).dot(rot)
    yaw = 180 / math.pi * math.atan(rot_mat[1][0] /rot_mat[0][0])
    pitch = 180 / math.pi * math.atan(-1 * rot_mat[2][0] / math.sqrt(pow(rot_mat[2][1], 2) + math.pow(rot_mat[2][2], 2)))
    roll = 180 / math.pi * math.atan(rot_mat[2][1] /rot_mat[2][2])
    
    txt = str(y[0]) + ' ' + str(y[1]) + ' ' + str(y[2])
    txt += ' ' + yaw + ' ' + pitch + ' ' + roll
    print(txt)
    #we use x,z,pitch
