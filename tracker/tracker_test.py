import triad_openvr
import time
import sys
from flask import Flask
app = Flask(__name__)
v = triad_openvr.triad_openvr()


@app.route("/")
def hello():
    #return '1.12345678 2.23456789 3.34567891'
    global v
    v.print_discovered_objects()
    
    if len(sys.argv) == 1:
        interval = 1/250
    elif len(sys.argv) == 2:
        interval = 1/float(sys.argv[0])
    else:
        print("Invalid number of arguments")
        interval = False
    if interval:
        start = time.time()
        txt = ""
        for each in v.devices["tracker_1"].get_pose_euler():
            txt += "%.4f" % each
            txt += " "
        print("\r" + txt, end="")
        sleep_time = interval-(time.time()-start)
        if sleep_time>0:
            time.sleep(sleep_time)
        return "\r" + txt
    else:
        return 'not ready'
if __name__ == "__main__":
    app.run(host="0.0.0.0")
