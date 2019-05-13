//
//  myPrinter.hpp
//  simulator
//
//  Created by Pol Surakitkoson on 2/18/19.
//  Copyright © 2019 Pol Surakitkoson. All rights reserved.
//

#ifndef myPrinter_hpp
#define myPrinter_hpp

#include <stdio.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "myCamera.hpp"
#include "myRandom.hpp"
#include "constant.cpp"
using namespace cv;
using namespace std;

class myPrinter{
public:
    
    //var
    myCamera camera;
    double x;
    double y;
    double theta;
    double cameraX; //camera position compare to center of printer position
    double cameraY;
    double cameraZ;
    
    Mat cameraImage;
    Mat field;
    Mat printField;

    ////// for simulation
    bool noError = false;
    double movePositionError = 10; // *.1 mm.
    double moveRotationError = 5; // degree
    double readPositionError = 5; // *.1 mm.
    double readRotationError = 0.1; //degree
    myRandom randomGenerator;
    //////

    //func
    //constructor
    myPrinter();
    myPrinter(double cameraX, double cameraY, double cameraZ, Mat field);
    vector<double> getPosition();
    void move(double newX, double newY);
    void paint(int xlocal, int ylocal, Vec3b c, Mat field);
    void paint2(int xlocal, int ylocal, Vec3b c, Mat field);
    void paint3(int xlocal, int ylocal, Vec3b c, Mat field);
    void saveTifFile();
    void clearPrintField();
    void print();
    void getCameraImage(VideoCapture cap);
    void update();

    void moveSimulation(double newX, double newY);
    vector<double> getPositionSimulation();

    VideoCapture cap;
    
};

#endif /* myPrinter_hpp */
