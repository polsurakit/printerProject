import triad_openvr
import time
import sys

v = triad_openvr.triad_openvr()
v.print_discovered_objects()

if len(sys.argv) == 1:
    interval = 1/250
elif len(sys.argv) == 2:
    interval = 1/float(sys.argv[0])
else:
    print("Invalid number of arguments")
    interval = False
    
if interval:
    
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
                    s[j*3+k] += xi[j][k]
            sleep_time = interval-(time.time()-start)
            if sleep_time>0:
                time.sleep(sleep_time)
        std = [0]*12
        isP = True
        for i in range(12):
            for j in range(10):
                std[i] += (float(s[i])/10-float(x[j][i]))**2
            std[i] = (std[i]/9)**0.5
            if(std[i] > 0.001):
                isP = False
                break
        if isP:
            txt = ""
            for i in s:
                txt += str(i/10)+" "
            print(txt)

