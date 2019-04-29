//
//  myPrinter.cpp
//  simulator
//
//  Created by Pol Surakitkoson on 2/18/19.
//  Copyright © 2019 Pol Surakitkoson. All rights reserved.
//

#include "opencv2/opencv.hpp"
#include "myPrinter.hpp"
// #include "../include/HTTPRequest.hpp"
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include "constant.cpp"

using namespace std;

myPrinter::myPrinter(double cameraX, double cameraY, double cameraZ, Mat field) :
cameraX(cameraX), cameraY(cameraY), cameraZ(cameraZ), field(field) {
    update();
    cout << isSimulation << endl;
    printField = Mat(printSize,printSize,CV_8UC3,Vec3b(255,255,255));
    vector<double> result;
    if (!isSimulation) result = getPosition();
    else{
        result.push_back(1000);
        result.push_back(1000);
        result.push_back(0);
    }
    x = result[0];
    y = result[1];
    theta = result[2]/180*M_PI;
};

void myPrinter::update(){
    cout <<"update 5 " << endl;
}
string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

vector<double> myPrinter::getPosition(){
    cout << "get position" << endl;
    vector<double> result;
    // vector<double> result;
    // cin >> x >> y >> theta;
    // cout << "get " << x << " " << y << " " << theta << endl;
    // theta = theta/180*M_PI;
    // result.push_back(x);
    // result.push_back(y);
    // result.push_back(theta);
    // return result;

    char* cmd = "python3 controller_test.py 1";
    string r = exec(cmd);
    cout << "result " << r << endl;
    vector<string> info(6);
    int idx = 0;
    for(unsigned int i = 0 ; i < r.size() ; i++){
        if(r[i]==' '){
            idx++;
            continue;
        }
        info[idx] += r[i];
    }
    result.push_back(stod(info[0]));
    result.push_back(stod(info[2]));
    result.push_back(stod(info[4]));
    x = result[0];
    y = result[1];
    theta = result[2]/180*M_PI;
    cout << "x = " << x << " y = " << y << " theta = " << theta << endl;  
    return result;
};

void myPrinter::move(double newX, double newY){
    //try to move printer
    //show picture and show is it in correct position.
    //if the position and rotation is not good enough
    //it will not be able to pass
    //Mat window;
    bool isOK = false;
    while(!isOK){
        //resize(field, window, cv::Size(500, 500), 0, 0);
        

        // //plot printer
        // int ratio = 15000/500;
        // for(int i = -200 ; i < 200 ; i++){
        //     for(int j = -200 ; j < 200 ; j++){
        //         if(i*i+j*j > 100*100) continue;
        //         window.at<Vec3b>((y+i)/ratio, (x+j)/ratio) = Vec3b(255,0,255);
        //     }
        // }

        // //plot Marker
        // for(int i = -200 ; i < 200 ; i++){
        //     for(int j = -200 ; j < 200 ; j++){
        //         if(i*i+j*j > 100*100) continue;
        //         window.at<Vec3b>((newY+i)/ratio, (newX+j)/ratio) = Vec3b(255,0,255);
        //     }
        // }

        if(abs(newY-y)>200) isOK = false;
        else if(abs(newX-x)>200) isOK = false;
        else if(abs(theta)>10) isOK=false;
        else isOK = true;
        if(isOK){
            cout << "Position is OK" << endl;
        }else{
            cout << "Position is not close enough with x,y,theta= " << x << " " << y << " " << theta << endl;
            string tmp;
            cout << "please move to " << newX << " " << newY << endl;
            cout << "your current position is " << x << " " << y << endl;
            cout << "after move the printer press ENTER" << endl;
            cin >> tmp;
            vector<double> pos = getPosition();
        }

        //imshow( "Display window", window);
        //waitKey(0);
    }
    cout << "exit move" << endl;

};

void myPrinter::paint(int xlocal, int ylocal, Vec3b c, Mat field){
    if(xlocal < -1500 || xlocal > 1499 || ylocal < -1500 || ylocal > 1499) return;
    int posX = round(cos(theta)*(xlocal) + sin(theta)*(ylocal));
    int posY = round(-sin(theta)*(xlocal) + cos(theta)*(ylocal));
    if(y+posY < 1000 || y+posY >= 14000) return;
    if(x+posX < 1000 || x+posX >= 14000) return;
    if(field.at<Vec3b>(posY+y,posX+x) != Vec3b(255,255,255) ) return;
    field.at<Vec3b>(posY+y,posX+x) = c;
    //paint at posY+y, posX+x with color c
};
    //use for show 
void myPrinter::paint2(int xlocal, int ylocal, Vec3b c, Mat field){

    if(xlocal < -1500 || xlocal > 1499 || ylocal < -1500 || ylocal > 1499) return;
    int posX = round(cos(theta)*(xlocal) + sin(theta)*(ylocal));
    int posY = round(-sin(theta)*(xlocal) + cos(theta)*(ylocal));
    if(y+posY < 1000 || y+posY >= 14000) return;
    if(x+posX < 1000 || x+posX >= 14000) return;
    //if(field.at<Vec3b>(posY+y,posX+x) != Vec3b(255,255,255) ) return;
    Vec3b old = field.at<Vec3b>(posY+y,posX+x);
    int b = old.val[0];
    int g = old.val[1];
    int r = old.val[2];
    b = min(255,max(0,b+((int)(c.val[0]))-255));
    g = min(255,max(0,g+((int)(c.val[1]))-255));
    r = min(255,max(0,r+((int)(c.val[2]))-255));
    
};

void myPrinter::paint3(int xlocal, int ylocal, Vec3b c, Mat field){

    if(xlocal < -printSize/2 || xlocal > printSize/2-1) return;
    if(ylocal < -printSize/2 || ylocal > printSize/2-1) return;
    int posX = round(cos(theta)*(xlocal) + sin(theta)*(ylocal));
    int posY = round(-sin(theta)*(xlocal) + cos(theta)*(ylocal));
    if(y+posY < TOPLEFTY || y+posY >= BOTTOMRIGHTY) return;
    if(x+posX < TOPLEFTX || x+posX >= BOTTOMRIGHTX) return;
    Vec3b old = field.at<Vec3b>(posY+y,posX+x);
    int b = old.val[0];
    int g = old.val[1];
    int r = old.val[2];
    b = min(255,max(0,b+((int)(c.val[0]))-255));
    g = min(255,max(0,g+((int)(c.val[1]))-255));
    r = min(255,max(0,r+((int)(c.val[2]))-255));
    field.at<Vec3b>(posY+y,posX+x) = Vec3b(b,g,r);
    printField.at<Vec3b>(-ylocal+printSize/2, xlocal+printSize/2) = c;
};

void myPrinter::saveTifFile(){
    Mat picField;
    int sz = round(printSize/2.54); //100 is dpi
    resize(printField,picField,Size(sz, sz),0,0);
    imwrite("tiffile.png",picField);
    system("python3 tiffile/Tiff.py tiffile.png");
    cout << "saveFile\n";
}

void myPrinter::clearPrintField(){
    for(int i = 0 ; i < printSize ; i++){
        for(int j = 0 ; j < printSize; j++){
            printField.at<Vec3b>(i,j) = Vec3b(255,255,255);
        }
    }
    cout << "clearPrintField\n";
}

void myPrinter::print(){
    //command to printer
    cout << "Go to print" << endl;
    cout << "After printing is finished, press ENTER" << endl;
    string s;
    cin  >> s;
}

void myPrinter::getCameraImage(){
    cameraImage = camera.getImage(x+cameraX, y+cameraY, theta, field);
};

void myPrinter::moveSimulation(double newX, double newY){
    double xerror = randomGenerator.normal(movePositionError);
    while (xerror > 2*movePositionError) xerror = randomGenerator.normal(movePositionError);
    double yerror = randomGenerator.normal(movePositionError);
    while (yerror > 2*movePositionError) yerror = randomGenerator.normal(movePositionError);
    double thetaerror = randomGenerator.normal(moveRotationError/180*M_PI);
    while (thetaerror > 2*moveRotationError/180*M_PI) thetaerror = randomGenerator.normal(moveRotationError/180*M_PI);
    x = newX+xerror;
    y = newY+yerror;
    theta = thetaerror;
}

vector<double> myPrinter::getPositionSimulation(){
    double xerror = randomGenerator.normal(readPositionError);
    while (xerror > 2*readPositionError) xerror = randomGenerator.normal(readPositionError);
    double yerror = randomGenerator.normal(readPositionError);
    while (yerror > 2*readPositionError) yerror = randomGenerator.normal(readPositionError);
    double thetaerror = randomGenerator.normal(readRotationError/180*M_PI);
    while (thetaerror > 2*readRotationError/180*M_PI) thetaerror = randomGenerator.normal(readRotationError/180*M_PI);
    double retx = x+xerror;
    double rety = y+yerror;
    double rettheta = theta+thetaerror;
    vector<double> result;
    result.push_back(retx);
    result.push_back(rety);
    result.push_back(rettheta);
    return result;
}
