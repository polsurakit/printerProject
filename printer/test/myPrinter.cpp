//
//  myPrinter.cpp
//  simulator
//
//  Created by Pol Surakitkoson on 2/18/19.
//  Copyright © 2019 Pol Surakitkoson. All rights reserved.
//

#include "opencv2/opencv.hpp"
#include "myPrinter.hpp"
#include "../include/HTTPRequest.hpp"
#include <iostream>

using namespace std;

myPrinter::myPrinter(double cameraX, double cameraY, double cameraZ, Mat field) :
cameraX(cameraX), cameraY(cameraY), cameraZ(cameraZ), field(field) {
    printField = Mat(printFieldSize,printFieldSize,CV_8UC3,Vec3b(255,255,255));
    x = 5000;
    y = 5000;
    theta = 0;
};

vector<double> myPrinter::getPosition(){
    vector<double> result;
    cin >> x >> y >> theta;
    theta = theta/180*M_PI;
    result.push_back(x);
    result.push_back(y);
    result.push_back(theta);
    return result;
    try
    {
        string url = "http://0.0.0.0:5000/";
        std::string method = "GET";
        std::string arguments;
        std::string output;

        http::Request request(url);

        http::Response response = request.send(method, arguments, {
            "Content-Type: application/x-www-form-urlencoded",
            "User-Agent: runscope/0.1"
        });

        if (!output.empty())
        {
            std::ofstream outfile(output, std::ofstream::binary);
            outfile.write(reinterpret_cast<const char*>(response.body.data()),
                          static_cast<std::streamsize>(response.body.size()));
        }
        else{
            vector<string> vs(3);
            int idx = 0;
            for(unsigned int i = 0 ; i < response.body.size() ; i++){
                if(response.body[i]==' '){
                    idx++;
                    continue;
                }
                vs[idx] += response.body[i];
            }
            //have to change basis in the section
            //and assign to printer's x,y,theta
            double retx = stod(vs[0]);
            double rety = stod(vs[1]);
            double rettheta = stod(vs[2]);
            x = retx;
            y = rety;
            theta = rettheta;
            result.push_back(retx);
            result.push_back(rety);
            result.push_back(rettheta);
        }

        std::cout << response.body.data() << std::endl;
        return result;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << std::endl;
        //return EXIT_FAILURE;
    }


    return result;
};

void myPrinter::move(double newX, double newY){
    //try to move printer
    //show picture and show is it in correct position.
    //if the position and rotation is not good enough
    //it will not be able to pass
    Mat window;
    bool isOK = false;
    while(!isOK){
        resize(field, window, cv::Size(500, 500), 0, 0);
        vector<double> pos = getPosition();

        //plot printer
        int ratio = 15000/500;
        for(int i = -200 ; i < 200 ; i++){
            for(int j = -200 ; j < 200 ; j++){
                if(i*i+j*j > 100*100) continue;
                window.at<Vec3b>((y+i)/ratio, (x+j)/ratio) = Vec3b(255,0,255);
            }
        }

        //plot Marker
        for(int i = -200 ; i < 200 ; i++){
            for(int j = -200 ; j < 200 ; j++){
                if(i*i+j*j > 100*100) continue;
                window.at<Vec3b>((newY+i)/ratio, (newX+j)/ratio) = Vec3b(255,0,255);
            }
        }

        if(abs(newY-y)>15) isOK = false;
        else if(abs(newX-x)>15) isOK = false;
        else if(abs(theta)>10) isOK=false;
        else isOK = true;
        if(isOK){
            cout << "Position is OK" << endl;
        }else{
            cout << "Position is not close enough with x,y,theta= " << x << " " << y << " " << theta << endl;
        }
        imshow( "Display window", window);
        waitKey(0);
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

    if(xlocal < -printFieldSize/2 || xlocal > printFieldSize/2-1) return;
    if(ylocal < -printFieldSize/2 || ylocal > printFieldSize/2-1) return;
    int posX = round(cos(theta)*(xlocal) + sin(theta)*(ylocal));
    int posY = round(-sin(theta)*(xlocal) + cos(theta)*(ylocal));
    if(y+posY < 1000 || y+posY >= 14000) return;
    if(x+posX < 1000 || x+posX >= 14000) return;
    Vec3b old = field.at<Vec3b>(posY+y,posX+x);
    int b = old.val[0];
    int g = old.val[1];
    int r = old.val[2];
    b = min(255,max(0,b+((int)(c.val[0]))-255));
    g = min(255,max(0,g+((int)(c.val[1]))-255));
    r = min(255,max(0,r+((int)(c.val[2]))-255));
    field.at<Vec3b>(posY+y,posX+x) = Vec3b(b,g,r);
    printField.at<Vec3b>(ylocal+1500, xlocal+1500) = c;
};

void myPrinter::saveTifFile(){
    imwrite("tiffile/tiffile.tif",printField);
    system("python3 tiffile/Tiff.py tiffile.tif");
    cout << "saveFile\n";
}

void myPrinter::clearPrintField(){
    for(int i = 0 ; i < printFieldSize ; i++){
        for(int j = 0 ; j < printFieldSize ; j++){
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
