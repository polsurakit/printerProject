//
//  main.cpp
//  printer
//
//  Created by Pol Surakitkoson on 12/27/18.
//  Copyright © 2018 Pol Surakitkoson. All rights reserved.
//

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
#include <iostream>
#include <fstream>
// #include "HTTPRequest.hpp"
using namespace std;
#include "myCamera.hpp"
#include "myPrinter.hpp"
#include "myRandom.hpp"
#include <map>
#include "constant.cpp"

#include "opencv2/aruco.hpp" 

//var
std::string INPUT_NAME = "grid2.jpg";
std::string OUTPUT_NAME = "algoralgor1.jpg";
int TARGET_H_SIZE = 3204; //0.1 millimeter / pixel
int TARGET_W_SIZE = 1800; //0.1 millimeter / pixel

myRandom randomGenerator;

Mat field(fieldSize,fieldSize,CV_8UC3,Vec3b(0,0,0)); //0.1 millimeter / pixel

vector<vector<int> > zone;
vector<vector<int> > zone2;
vector<vector<vector<double> > > zone3;
myPrinter printer(0,0,0,field);

Mat image;
Mat image2;
Mat showedImage;
Mat expectedImage;
Mat expectedResult;


Mat polpolpol(Mat im);

cv::Ptr<cv::aruco::Dictionary> dictionary =   cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
VideoCapture cap;


map<int,double> markerPosXs;
map<int,double> markerPosYs;
map<int,double> markerThetas;

void loadMarkerData(){
    ifstream myReadFile;
    myReadFile.open("aruco.txt");
    string output;
    vector<double> v(3);
    int idx = -1;
    int id;
    if (myReadFile.is_open()) {
        while (!myReadFile.eof()) {
            myReadFile >> output;
            if (idx==-1){
                id = stoi(output);
                idx++;
            }else if (idx < 3){
                v[idx] = stod(output);
                idx++;
            }else{
                markerPosXs[id] = v[1];
                markerPosYs[id] = v[0];
                markerThetas[id] = v[2];
                id = stoi(output);
                idx = 0;
            }
        }
    }
    myReadFile.close();
}

void printMarkerData(){
    for(auto it = markerPosXs.begin() ; it != markerPosXs.end() ; it++){
        int id = it->first;
        cout << id << " " << markerPosXs[id] << " " << markerPosYs[id] << " " << markerThetas[id] << endl;
    }
}

Mat testundist(Mat in){

    Mat cam(cv::Size(3, 3), CV_64FC1);

    cam.at<double>(0,0) = 2.68566643e+03;
    cam.at<double>(0,1) = 0.00000000e+00;
    cam.at<double>(0,2) = 1.76025212e+03;
    cam.at<double>(1,0) = 0.00000000e+00;
    cam.at<double>(1,1) = 2.68566643e+03;
    cam.at<double>(1,2) = 1.32815535e+03;
    cam.at<double>(2,0) = 0.00000000e+00;
    cam.at<double>(2,1) = 0.00000000e+00;
    cam.at<double>(2,2) = 1.00000000e+00;

    Mat dist(cv::Size(1, 8), CV_64FC1);
    dist.at<double>(0,0) = 1.45252895e+01;
    dist.at<double>(0,1) = 4.36753815e+00;
    dist.at<double>(0,2) = 2.45529062e-03;
    dist.at<double>(0,3) = 8.76449085e-04;
    dist.at<double>(0,4) =-5.38246708e+01;
    dist.at<double>(0,5) = 1.44956654e+01;
    dist.at<double>(0,6) = 4.93164447e+00;
    dist.at<double>(0,7) =-5.45507391e+01;


    // Mat in,out,out2;
    // in = imread("../../cameracalibration/pic/1.jpg");
    // cout << in.cols << " " << in.rows << endl;
    // resize(in,out2,Size(800, 600),0,0);
    // imshow( "Display window", out2);
    // waitKey(0);
    Mat out;
    undistort(in, out, cam, dist);
    
    // resize(out,out2,Size(800, 600),0,0);
    // imshow( "Display window", out2);
    // waitKey(0);
    return out;
}

Mat getPicture(){
    Mat frame,frame3;
    while(true){
        cap >> frame;
        frame3 = testundist(frame);

        Mat frame2;
        frame2 = polpolpol(frame3);
        resize(frame,frame2,Size(800,600),0,0);
        int x = waitKey(30);
        imshow( "Display window", frame2);
        if (x==100) break;
    }
    cap >> frame;
    return testundist(frame);
}

double distancePoint2f(Point2f p1, Point2f p2){
    return sqrt((p1.y-p2.y)*(p1.y-p2.y)+(p1.x-p2.x)*(p1.x-p2.x));
}

double cameraW = 2400;
double cameraH = 1920; // in 0.1mm.
double cameraOffsetX = -100;
double cameraOffsetY = -1213;

vector<double> getposition(){
    vector<double> result;
    Mat pic5 = getPicture();
    Mat pic = polpolpol(pic5);
    int ox = 1037;
    int oy = 562;

    vector< int > markerIds; 
    vector< vector<Point2f> > markerCorners, rejectedCandidates; 
    cv::Ptr<cv::aruco::DetectorParameters> parameters;
    cv::aruco::detectMarkers(pic, dictionary, markerCorners, markerIds);
    cv::aruco::drawDetectedMarkers(pic, markerCorners, markerIds);
    Mat pic2;
    resize(pic,pic2,Size(800, 600),0,0);

    imshow( "Display window", pic2);
    waitKey(0);

    int r = pic.rows;
    int c = pic.cols;
    if (markerIds.size() == 0){
        cout << "No marker detected\n";
        imshow( "Display window", pic);
        waitKey(0);
        return result;
    }
    int id = markerIds[0];
    Point2f corner = markerCorners[0][0];
    Point2f lastcorner = markerCorners[0][3];
    double distanceToMid = distancePoint2f(Point2f(ox,oy), corner);
    for(int i = 1 ; i < markerIds.size() ; i++){
        double newdist = distancePoint2f(markerCorners[i][0],Point2f(c/2,r/2));
        if (newdist < distanceToMid){
            id = markerIds[i];
            corner = markerCorners[i][0];
            lastcorner = markerCorners[i][3];
            distanceToMid = newdist;
        }
    }

    double markerGlobalPosX = markerPosXs[id];
    double markerGlobalPosY = markerPosYs[id];
    double markerGlobalTheta = markerThetas[id];

    cout << id << " " << markerGlobalPosX << " " << markerGlobalPosY << " " << markerGlobalTheta << " " << markerGlobalTheta*180/M_PI << endl;

    double markerLocalPosX = corner.x;
    double markerLocalPosY = corner.y;
    cout << "corner\n" << corner << endl << lastcorner << endl;
    double markerLocalTheta = atan((corner.x-lastcorner.x)/(corner.y-lastcorner.y));

    cout << "r,c " << r << " " << c << endl;
    cout << "markerLocalPosX " << markerLocalPosX << "\nmarkerLocalPosY " << markerLocalPosY << endl;
    cout << "markerLocaltheta " << markerLocalTheta << endl;
    cout << "markerGlobaltheta " << markerGlobalTheta << endl;
    double diffTheta = markerLocalTheta - markerGlobalTheta;
    cout << "difftheta " << diffTheta << endl;

    double cameraToMarkerX = (ox - markerLocalPosX);
    double cameraToMarkerY = (oy - markerLocalPosY);

    cout << "cameraToMarkerX " << cameraToMarkerX << endl;
    cout << "cameraToMarkerY " << cameraToMarkerY << endl;

    double cameraToMarkerXGlobal = cos(diffTheta)*cameraToMarkerX - sin(diffTheta)*cameraToMarkerY;///////
    double cameraToMarkerYGlobal = sin(diffTheta)*cameraToMarkerX + cos(diffTheta)*cameraToMarkerY;

    double cameraGlobalPosX = markerGlobalPosX + cameraToMarkerXGlobal;
    double cameraGlobalPosY = markerGlobalPosY + cameraToMarkerYGlobal;

    double cameraGlobalTheta = diffTheta;

    cout << "cameraGlobalPosX " << cameraGlobalPosX << endl;
    cout << "cameraGlobalPosY " << cameraGlobalPosY << endl;
    cout << "cameraGlobalTheta " << cameraGlobalTheta << " " << cameraGlobalTheta*180/M_PI << endl;

    double printerToCameraXGlobal = cos(cameraGlobalTheta)*cameraOffsetX - sin(cameraGlobalTheta)*cameraOffsetY;
    double printerToCameraYGlobal = sin(cameraGlobalTheta)*cameraOffsetX + cos(cameraGlobalTheta)*cameraOffsetY;
    cout << "printerToCameraXGlobal " << printerToCameraXGlobal << endl;
    cout << "printerToCameraYGlobal " << printerToCameraYGlobal << endl;

    double cameraThetaOffset = atan(45.0/1495.0);//

    double printerGlobalPosX = cameraGlobalPosX + printerToCameraXGlobal;
    double printerGlobalPosY = cameraGlobalPosY + printerToCameraYGlobal;

    result.push_back(printerGlobalPosX);
    result.push_back(printerGlobalPosY);
    result.push_back(-cameraGlobalTheta + cameraThetaOffset - 1.22/180*M_PI);

    printer.x = result[0];
    printer.y = result[1];
    printer.theta = result[2];

    cout << "last position " << result[0] << " " << result[1] << " " << result[2] << " " << result[2]*180/M_PI << endl;
    return result;
}




void initialize(){
    if (!isSimulation && isAruco){
        cout << "aruco mode\n";
        loadMarkerData();
        printMarkerData();
        cap.open(0);
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 3264); // valueX = your wanted width 
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 2448); // valueY = your wanted heigth
    }
    
    image = imread(INPUT_NAME);
    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
    }
    for(int i = 0 ; i < image.rows ; i++){
        image.at<Vec3b>(i,0) = Vec3b(255,255,0);
        image.at<Vec3b>(i,1) = Vec3b(255,255,0);
        image.at<Vec3b>(i,2) = Vec3b(255,255,0);
        image.at<Vec3b>(i,3) = Vec3b(255,255,0);
        image.at<Vec3b>(i,image.cols-1) = Vec3b(255,255,0);
        image.at<Vec3b>(i,image.cols-2) = Vec3b(255,255,0);
        image.at<Vec3b>(i,image.cols-3) = Vec3b(255,255,0);
        image.at<Vec3b>(i,image.cols-4) = Vec3b(255,255,0);
    }
    for(int i = 0 ; i < image.cols ; i++){
        image.at<Vec3b>(0,i) = Vec3b(255,255,0);
        image.at<Vec3b>(1,i) = Vec3b(255,255,0);
        image.at<Vec3b>(2,i) = Vec3b(255,255,0);
        image.at<Vec3b>(3,i) = Vec3b(255,255,0);
        image.at<Vec3b>(image.rows-1,i) = Vec3b(255,255,0);
        image.at<Vec3b>(image.rows-2,i) = Vec3b(255,255,0);
        image.at<Vec3b>(image.rows-3,i) = Vec3b(255,255,0);
        image.at<Vec3b>(image.rows-4,i) = Vec3b(255,255,0);
    }
    imshow( "Display window", image);
    waitKey(0);

    cout << 1 ;
    resize(image,showedImage,Size(smallWindowSize, smallWindowSize),0,0);
    
    cout << 1 ;
    resize(image,image2,Size(TARGET_W_SIZE, TARGET_H_SIZE),0,0);
    //blur
    if(isBlur){
        blur(image2,image2,Size(50,50));
    }

    for(int i = TOPLEFTY ; i < fieldSize-TOPLEFTY ; i++){
        for(int j = TOPLEFTX ; j < fieldSize-TOPLEFTX ; j++){
            field.at<Vec3b>(i,j) = Vec3b(255,255,255);
        }
    }
    for(int i = TOPLEFTY+1 ; i <= TOPLEFTY+TARGET_H_SIZE ; i++){
        for(int j = TOPLEFTX+1 ; j <= TOPLEFTX+TARGET_W_SIZE ; j++){
            field.at<Vec3b>(i,j) = image2.at<Vec3b>(i-TOPLEFTY-1,j-TOPLEFTX-1);
        }
    }
    cout << 1 ;
    resize(field,expectedImage,Size(bigWindowSize,bigWindowSize),0,0);
    field.copyTo(expectedResult);
    for(int i = TOPLEFTY+1 ; i <= TOPLEFTY+TARGET_H_SIZE ; i++){
        for(int j = TOPLEFTX+1 ; j <= TOPLEFTX+TARGET_W_SIZE ; j++){
            field.at<Vec3b>(i,j) = Vec3b(255,255,255);
        }
    }
    cout << "end init" << endl;
}

void showResult(){
    Mat window;
    Mat window2;
    int bigRatio = fieldSize/bigWindowSize;
    resize(printer.cameraImage, window, cv::Size(smallWindowSize, smallWindowSize), 0, 0);
    resize(field, window2, cv::Size(bigWindowSize, bigWindowSize), 0, 0);

    //plot printer
    for(int i = -200 ; i < 200 ; i++){
        for(int j = -200 ; j < 200 ; j++){
            if(i*i+j*j > 100*100) continue;
            window2.at<Vec3b>((printer.y+i)/bigRatio, (printer.x+j)/bigRatio) = Vec3b(255,0,255);
        }
    }
    double theta = printer.theta;
    cout << "theta = " << theta << " " << theta*180/M_PI<< endl;
    //plot camera line =
    for(int i = -printer.camera.rowSize/2 ; i < printer.camera.rowSize/2 ; i++){
         int j = -printer.camera.colSize/2;
         int posX = round(cos(theta)*(j) - sin(theta)*(i));
         int posY = round(sin(theta)*(j) + cos(theta)*(i));
         posX = posX + printer.x;
         posY = -posY + printer.y;
         if(posX < 0 || posY < 0 || posX >= field.cols || posY >= field.rows){

         }else{
             window2.at<Vec3b>(posY/bigRatio, posX/bigRatio) = Vec3b(0,0,255);
         }
         j = printer.camera.colSize/2-1;
         posX = round(cos(theta)*(j) - sin(theta)*(i));
         posY = round(sin(theta)*(j) + cos(theta)*(i));
         posX = posX + printer.x;
         posY = -posY + printer.y;
         if(posX < 0 || posY < 0 || posX >= field.cols || posY >= field.rows){

         }else{
             window2.at<Vec3b>(posY/bigRatio, posX/bigRatio) = Vec3b(0,0,255);
         }
      }
    //plot camera line ||
    for(int j = -printer.camera.colSize/2 ; j < printer.camera.colSize/2 ; j++){
       int i = -printer.camera.rowSize/2;
       int posX = round(cos(theta)*(j) - sin(theta)*(i));
       int posY = round(sin(theta)*(j) + cos(theta)*(i));
       posX = posX + printer.x;
       posY = -posY + printer.y;
       if(posX < 0 || posY < 0 || posX >= field.cols || posY >= field.rows){

       }else{
           window2.at<Vec3b>(posY/bigRatio, posX/bigRatio) = Vec3b(0,0,255);
       }
       i = printer.camera.rowSize/2-1;
       posX = round(cos(theta)*(j) - sin(theta)*(i));
       posY = round(sin(theta)*(j) + cos(theta)*(i));
       posX = posX + printer.x;
       posY = -posY + printer.y;
       if(posX < 0 || posY < 0 || posX >= field.cols || posY >= field.rows){

       }else{
           window2.at<Vec3b>(posY/bigRatio, posX/bigRatio) = Vec3b(0,0,255);
       }
    }

    //plot print zone line =
    for(int i = -printFieldSize/2 ; i < printFieldSize/2 ; i++){
         int j = -printFieldSize/2;
         int posX = round(cos(theta)*(j) - sin(theta)*(i));
         int posY = round(sin(theta)*(j) + cos(theta)*(i));
         posX = posX + printer.x;
         posY = -posY + printer.y;
         if(posX < 0 || posY < 0 || posX >= field.cols || posY >= field.rows){

         }else{
             window2.at<Vec3b>(posY/bigRatio, posX/bigRatio) = Vec3b(0,255,0);
         }
         j = printFieldSize/2-1;
         posX = round(cos(theta)*(j) - sin(theta)*(i));
         posY = round(sin(theta)*(j) + cos(theta)*(i));
         posX = posX + printer.x;
         posY = -posY + printer.y;
         if(posX < 0 || posY < 0 || posX >= field.cols || posY >= field.rows){

         }else{
             window2.at<Vec3b>(posY/bigRatio, posX/bigRatio) = Vec3b(0,255,0);
         }
      }
    //plot zone line ||
    for(int j = -printFieldSize/2 ; j < printFieldSize/2 ; j++){
       int i = -printFieldSize/2;
       int posX = round(cos(theta)*(j) - sin(theta)*(i));
       int posY = round(sin(theta)*(j) + cos(theta)*(i));
       posX = posX + printer.x;
       posY = -posY + printer.y;
       if(posX < 0 || posY < 0 || posX >= field.cols || posY >= field.rows){

       }else{
           window2.at<Vec3b>(posY/bigRatio, posX/bigRatio) = Vec3b(0,255,0);
       }
       i = printFieldSize/2-1;
       posX = round(cos(theta)*(j) - sin(theta)*(i));
       posY = round(sin(theta)*(j) + cos(theta)*(i));
       posX = posX + printer.x;
       posY = -posY + printer.y;
       if(posX < 0 || posY < 0 || posX >= field.cols || posY >= field.rows){

       }else{
           window2.at<Vec3b>(posY/bigRatio, posX/bigRatio) = Vec3b(0,255,0);
       }
    }

    vconcat(window,showedImage,window);
    hconcat(window,window2,window);
    hconcat(window,expectedImage,window);
    imshow( "Display window", window);
    waitKey(0);
}

void fillzone3();

vector<pair<int,int> > drt;

void filldrt(){
    int t = 0;
    vector<pair<int,int> > drt2;
    // drt.push_back(make_pair(2050,2050));
    // drt.push_back(make_pair(2050,3050));
    // drt.push_back(make_pair(3050,2050));
    // drt.push_back(make_pair(3050,3050));
    // return;
    for(int i = firstPosition ; i < BOTTOMRIGHTY ; i+=moveStep){
        if(t%2==1){
            drt2.resize(0);
        }
        for(int j = firstPosition ; j < BOTTOMRIGHTX ; j+=moveStep){
            if(t%2==0)drt.push_back(make_pair(i,j));
            else drt2.push_back(make_pair(i,j));
            if(TARGET_W_SIZE+TOPLEFTX <= j + 2*printFieldSize/5) {break;}
        }
        if(t%2==1){
            for(int j = 0 ; j < drt2.size() ; j++) drt.push_back(drt2[drt2.size()-1-j]);
        }
        t+=1;
        if(TARGET_H_SIZE+TOPLEFTY <= i + 2*printFieldSize/5) {break;}
    }
    cout << "drt\n";
    for(int i = 0 ; i < drt.size() ; i++){
        cout << drt[i].first << " " << drt[i].second << endl;
    }
}

bool checkDirection(int i, int j, int direction){
    int movej[4] = {0,moveStep,0,moveStep};
    int movei[4] = {moveStep,0,-moveStep,0};
    if ( i+movei[direction] < 0 || j+movej[direction] < 0) {cout << "da\n";  return false;}
    if ( i+movei[direction] >= fieldSize || j+movej[direction] >= fieldSize) {cout << "db\n"; return false;}
    if ( zone[j+movej[direction]][i+movei[direction]] == 0) {cout << "dc\n"; return false;}
    return true;
}

void changeDirection(int &i, int &j, int &direction){
    for(int ii = 0 ; ii < drt.size() ; ii++){
        //cout << "changedrt " << ii << " " << drt[ii].first << " " << drt[ii].second << " " << i << " " << j << endl;
        if(drt[ii].first == j && drt[ii].second == i){
            int k = ii+1;
            if(k == drt.size()){
                direction = 4;
                return;
            }else{
                j = drt[k].first;
                i = drt[k].second;
                return;
            }
        }
    }
    // cout << zone[2050][2050] << " z\n";
    // if(direction == 1) direction = 2;
    // if(direction == 3) direction = 0;
    // int movej[4] = {0,moveStep,0,moveStep};
    // int movei[4] = {moveStep,0,-moveStep,0};
    // int cnt = 0;
    // while(cnt < 4){
    //     cout << i << " " << j << " " << direction << endl;
    //     if (checkDirection(i, j, direction)){
    //         break;
    //     }else{
    //         direction += 1;
    //         direction %= 4;
    //         cnt++;
    //     }
    // }
    //     cout << i << " " << j << " " << direction << endl;

    // if(cnt==4) direction = 4;
    // else{
    //     i += movei[direction];
    //     j += movej[direction];
    // }
}

int checkCase2(int printy, int printx, int expy, int expx){
    int c;
    if (printx < expx - (moveStep-printFieldSize/2) && printy < expy + (moveStep-printFieldSize/2)) c = 1;
    else if (printx >= expx + (moveStep-printFieldSize/2) && printy >= expy + (moveStep-printFieldSize/2)) c = 1;
    else if (printx < expx + (moveStep-printFieldSize/2) && printy < expy - (moveStep-printFieldSize/2))
        c = 1;
    else c = 0;
    if (zone3[printy][printx].size()>2){
        if (TOPLEFTY+TARGET_H_SIZE-printy < TOPLEFTX+TARGET_W_SIZE-printx){
            return abs(c-1);
        }
        return abs(c-1)+2;
    }
    return c;
}

void algorithm6();



Mat pertransform;
void polpol(){
    Mat pol2 = imread("q.jpg");
    Mat pol = testundist(pol2);
    cout << "yy" << endl;
    vector< int > markerIds; 
    vector< vector<Point2f> > markerCorners, rejectedCandidates; 
    cv::aruco::detectMarkers(pol, dictionary, markerCorners, markerIds);
    cout << "asdfsdf" << endl;
    
    cout <<"xx";
    int first = 3;
    int old = -1;
    map<int,int> m;
    for(int i = 0 ; i < markerIds.size() ; i++) m[markerIds[i]]=i;
    vector<int> sindex;
    Point2f src[4];
    int i = 0 ;
    for(auto it = m.begin() ; it != m.end() ; it++){
        int x = it->first;
        cout << x << endl;
        src[i] = markerCorners[it->second][0];
        i+=1;
    }
    Point2f dest[4];
    dest[0].y = 0; 
    dest[0].x = 0;
    dest[1].y = 0; 
    dest[1].x = 2105;
    dest[2].y = 1300; 
    dest[2].x = 2105;
    dest[3].y = 1300; 
    dest[3].x = 0; 

    pertransform = getPerspectiveTransform(src, dest);
}
//#562,1037
Mat polpolpol(Mat im){
    Mat dst;
    warpPerspective(im,dst,pertransform,Size(2105,1300));
    return dst;
}


int main(int argc, char** argv)
{
    cout << "main\n";
    polpol();
    // testundist();
    // return 0;
    // if(argc>1){
    //     cout << argc << endl;
    //     OUTPUT_NAME = argv[1];
    //     if (argc > 3){
    //         TARGET_H_SIZE = stoi(argv[2]);
    //         TARGET_W_SIZE = stoi(argv[3]);
    //     }
    // }
    filldrt();
    printer.update();
    cout << OUTPUT_NAME << " " <<  TARGET_H_SIZE << " " << TARGET_W_SIZE << endl;
    initialize();
    cout << "bbb1\n";
    printer.getCameraImage(cap);
    
    // showResult();
    algorithm6();
    cout << "Save file ..." << endl;
    //cv::Rect myROI(TOPLEFTY, TOPLEFTX, TOPLEFTY+TARGET_H_SIZE, TOPLEFTX+TARGET_W_SIZE);
    Mat cropedImage = field(Rect(TOPLEFTX,TOPLEFTY,TARGET_W_SIZE,TARGET_H_SIZE));
    imwrite(OUTPUT_NAME, cropedImage);
    //imwrite("ex_"+OUTPUT_NAME, expectedResult);
    return 0;
}

void algorithm6(){
    cout << "algor6 start\n";
    //showResult();
    fillzone3();


    // showResult();
    cout << "start" << endl;
    int direction = 0;
    int i = firstPosition;
    int j = firstPosition;
    string name = "slide.jpg";
    //map<pair<int,int>,int > mp;
    while(direction <= 3){
        cout << i << " " << j << endl;
        cout << "This step will move to x = " << i << " y = " << j << endl;
        if(!isSimulation) cout << "If you need to skip type 'skip' and Enter" << endl;
        string input;
        if(!isSimulation)  cin >> input;
        if (input == "skip"){
            cout << "Skip this step" << endl;
            printer.getCameraImage(cap);
            cout << "show result" << endl;
            cout << "go to type on simulation screen\n";
            showResult();
            zone[i][j] = 0;
            changeDirection(i, j, direction);
            continue;
        }
        cout << "issim " << isSimulation << endl;
        vector<double> pos(3);
        if(isSimulation) printer.moveSimulation(i,j);
        else if(!isAruco)printer.move(i, j);
        else{
            while(true){
                // printer.getCameraImage(cap);
                pos = getposition();
                bool isok = true;
                if (abs(pos[0]-i) > 100) isok = false;
                if (abs(pos[1]-j) > 100) isok = false;
                // if (abs(pos[2]) > ) isok = false;
                if(isok){
                    cout << "Position is OK" << endl;
                    break;
                }else{
                    cout << "Position is not close enough with x,y,theta= " << pos[0] << " " << pos[1] << " " << pos[2] << " " << pos[2]*180/M_PI << endl;
                    string tmp;
                    cout << "please move to " << i << " " << j << endl;
                    // cout << "your current position is " <<  << " " << y << endl;
                    cout << "after move the printer press ENTER" << endl;
                    cin >> tmp;
                }
            }
            
        }
        cout << "Move finish" << endl;

        printer.getCameraImage(cap);
        // vector<double> pos(3);
        if(isSimulation) pos = printer.getPositionSimulation(); // x, y, theta
        else if (!isAruco){
            pos[0] = printer.x;
            pos[1] = printer.y;
            pos[2] = printer.theta;
        } 

        double theta = pos[2];
        cout << "go to type on simulation screen\n";
        showResult();

        cout << "paint" << endl;
        for(double ii = -printSize/2; ii < printSize/2 ; ii+=1){
            for(double jj = -printSize/2; jj < printSize/2 ; jj+=1){
                //theta = 0;
                //cout << ii << " " << jj << endl;

                int posX = round(cos(theta)*(ii) + sin(theta)*(jj));
                int posY = round(-sin(theta)*(ii) + cos(theta)*(jj));
                if((int)pos[1] + posY < 0 || (int)pos[1] + posY >= fieldSize) continue;
                if((int)pos[0] + posX < 0 || (int)pos[0] + posX >= fieldSize) continue;
                
                int Y = (int)pos[1] + posY; //position in field
                int X = (int)pos[0]+posX;
                // bool chkx = (i-printFieldSize/2 <= X && X < j+printFieldSize/2);
                // bool chky = (j-printFieldSize/2 <= Y && Y < j+printFieldSize/2);
                // if(!chkx || !chky) continue;
                if(Y<j-printFieldSize/2 || Y >= j+printFieldSize/2) continue;
                if(X<i-printFieldSize/2 || X >= i+printFieldSize/2) continue;
                int z = zone[Y][X];
                if(z==0 || z==zone2[(int)pos[1] + posY][(int)pos[0]+posX]) continue;
                if(Y<TOPLEFTY || X<TOPLEFTX) continue;
                if(Y>=fieldSize-TOPLEFTY || X>=fieldSize-TOPLEFTX) continue;
                //if(z==0 || z==zone2[(int)pos[1] + posY][(int)pos[0]+posX]) continue;
                Vec3b _exColor = expectedResult.at<Vec3b>((int)pos[1] + posY, (int)pos[0]+posX);
                int exColor[3] = {_exColor.val[0]/1, _exColor.val[1]/1, _exColor.val[2]/1};
                Vec3b color;
                //cout << ii << " " << jj << " " << z << endl;
                if(z!=1) {
                    if(z==2){
                        //print outside wrong
                        int c = checkCase2(Y,X,j, i);
                        // if ()
                        int c0,c1,c2;
                        c0 = exColor[0]+(int)((255-exColor[0])*zone3[Y][X][c]);
                        c1 = exColor[1]+(int)((255-exColor[1])*zone3[Y][X][c]);
                        c2 = exColor[2]+(int)((255-exColor[2])*zone3[Y][X][c]);
                        // c0 = 0;
                        // c1 = 0;
                        // c2 = 0;
                        // if (c==1){
                        //     c0 = 128;;
                        // }else c1 = 128;
                        // c0 = min(255,(255+exColor[0])/2);
                        // c1 = min(255,(255+exColor[1])/2);
                        // // c2 = min(255,(255+exColor[2])/2);
                        // if(c%2==(Y+X)%2){
                        //     c0 = exColor[0];
                        //     c1 = exColor[1];
                        //     c2 = exColor[2];
                        // }else{
                        //     c0 = 255;
                        //     c1 = 255;
                        //     c2 = 255;
                        // }

                        color = Vec3b(c0,c1,c2);
                        
                    }else{

                        int c1,c2,c0;
                        if(Y > j && X > i){
                            c0 = min(255,exColor[0]+(int)((255-exColor[0])*zone3[Y][X][0]));
                            c1 = min(255,exColor[1]+(int)((255-exColor[1])*zone3[Y][X][0]));
                            c2 = min(255,exColor[2]+(int)((255-exColor[2])*zone3[Y][X][0]));
                        }else if(Y > j && X < i){
                            c0 = min(255,exColor[0]+(int)((255-exColor[0])*zone3[Y][X][1]));
                            c1 = min(255,exColor[1]+(int)((255-exColor[1])*zone3[Y][X][1]));
                            c2 = min(255,exColor[2]+(int)((255-exColor[2])*zone3[Y][X][1]));
                        }else if(Y < j && X > i){
                            c0 = min(255,exColor[0]+(int)((255-exColor[0])*zone3[Y][X][2]));
                            c1 = min(255,exColor[1]+(int)((255-exColor[1])*zone3[Y][X][2]));
                            c2 = min(255,exColor[2]+(int)((255-exColor[2])*zone3[Y][X][2]));
                        }else if(Y < j && X < i){
                            c0 = min(255,exColor[0]+(int)((255-exColor[0])*zone3[Y][X][3]));
                            c1 = min(255,exColor[1]+(int)((255-exColor[1])*zone3[Y][X][3]));
                            c2 = min(255,exColor[2]+(int)((255-exColor[2])*zone3[Y][X][3]));
                        }else{
                            c0 = 255;
                            c1 = 255;
                            c2 = 255;
                            //cout << posY << " " << posX << " " << Y << " " << X << " 4" << endl;
                        }
                        // c0 = min(255,(255*3+exColor[0])/4);
                        // c1 = min(255,(255*3+exColor[1])/4);
                        // c2 = min(255,(255*3+exColor[2])/4);

                        // if(Y > j && X > i && (X+Y)%4==0){
                        //     c0 = exColor[0];
                        //     c1 = exColor[1];
                        //     c2 = exColor[2];
                        // }else if(Y > j && X < i && (X+Y)%4==1){
                        //     c0 = exColor[0];
                        //     c1 = exColor[1];
                        //     c2 = exColor[2];
                        // }else if(Y < j && X > i && (X+Y)%4==2){
                        //     c0 = exColor[0];
                        //     c1 = exColor[1];
                        //     c2 = exColor[2];
                        // }else if(Y < j && X < i && (X+Y)%4==3){
                        //     c0 = exColor[0];
                        //     c1 = exColor[1];
                        //     c2 = exColor[2];
                        // }else{
                        //     c0 = 255;
                        //     c1 = 255;
                        //     c2 = 255;
                        //     //cout << posY << " " << posX << " " << Y << " " << X << " 4" << endl;
                        // }
                        color = Vec3b(c0,c1,c2);
                    }
                }
                else{
                        color = Vec3b(exColor[0],exColor[1],exColor[2]);
                }
                // zone2[Y][X]++;
                printer.paint3(ii,jj, color, field);
            }
        }
        cout << "end paint" << endl;
        if(!isSimulation){
        //save printer.printField to tif file
            printer.saveTifFile();
            cout << "aaa\n";
            printer.clearPrintField();
        //command printer to print that picture
            printer.print();
        }
        // Mat cropedImage = field(Rect(TOPLEFTX,TOPLEFTY,TARGET_W_SIZE,TARGET_H_SIZE));
        // imwrite(name, cropedImage);
        // name = "q"+name;
        //show result
        printer.getCameraImage(cap);
        cout << "show result" << endl;
        cout << "go to type on simulation screen\n";
        showResult();
        
        zone[j][i] = 0;
        changeDirection(i, j, direction);
        cout << "change direct to " << i << " " << j << " " << direction << endl;
    }
}

void fillzone3(){
    cout << "fillzone3" << endl;
    zone.resize(fieldSize,vector<int>(fieldSize));
    zone2.resize(fieldSize,vector<int>(fieldSize));
    zone3.resize(fieldSize,vector<vector<double> >(fieldSize));
    for(int i = firstPosition ; i < BOTTOMRIGHTY ; i+=moveStep){
        for(int j = firstPosition ; j < BOTTOMRIGHTX ; j+=moveStep){
            for(int k = -printFieldSize/2; k < printFieldSize/2 ; k++){
                for(int l = -printFieldSize/2 ; l < printFieldSize/2 ; l++){
                    zone[i+k][j+l] += 1;
                    
                }
            }
            if(TARGET_W_SIZE+TOPLEFTX <= j + 2*printFieldSize/5) {break;}
        }
        if(TARGET_H_SIZE+TOPLEFTY <= i + 2*printFieldSize/5) {break;}
    }
    int a = printFieldSize-moveStep;//what is this
    int b = firstPosition+printFieldSize/2-(printFieldSize-moveStep); //firstmerge
    int c = moveStep; //merge step
    cout << "fillcol" << endl;
    for(int j = b ; j < BOTTOMRIGHTX ; j+= c){
        for(int i = TOPLEFTY ; i < BOTTOMRIGHTY ; i++){
            if(zone[i][j]>2){ //skip 4 area
                i+=a-1;
                continue;
            }
            if(zone[i][j]==0) break;
            if(zone[i][j]==1) continue;
            for(int k = 0 ; k < a ; k++){
                double w1 = (k+1);
                double w2 = (a-k);
                // field.at<Vec3b>(i,j+k) = Vec3b(64,0,0);
                zone3[i][j+k].push_back(w1/(w1+w2));
                zone3[i][j+k].push_back(w2/(w1+w2));
            }
        }
    }
    cout << "fillrow" << endl;
    for(int i = b ; i < BOTTOMRIGHTY ; i+= c){
        for(int j = TOPLEFTX ; j < BOTTOMRIGHTX ; j++){
            if(zone[i][j]>2){
                j+=a-1;
                continue;
            }
            if(zone[i][j]==0) break;
            if(zone[i][j]==1) continue;

            for(int k = 0 ; k < a ; k++){
                double w1 = (k+1);
                double w2 = (a-k);
                // field.at<Vec3b>(i+k,j) = Vec3b(0,64,0);
                // if(zone3[i+k][j].size()==2) field.at<Vec3b>(i+k,j) = Vec3b(64,64,0);
                zone3[i+k][j].push_back(w1/(w1+w2));
                zone3[i+k][j].push_back(w2/(w1+w2));
                // if(zone3[i+k][j].size()==4) cout << "!";
            }
        }
    }
    cout << "fill4" << endl;
    for(int i = b ; i < BOTTOMRIGHTY ; i+=c){
        for(int j = b ; j < BOTTOMRIGHTX ; j+=c){
            for(int k = 0 ; k < a ; k++){
                for(int l = 0 ; l < a ; l++){
                    if(zone[i+k][j+l]==0) break;
                    double w1 = sqrt((k+1)*(k+1)+(l+1)*(l+1));
                    double w2 = sqrt((k+1)*(k+1)+(a-l)*(a-l));
                    double w3 = sqrt((a-k)*(a-k)+(l+1)*(l+1));
                    double w4 = sqrt((a-k)*(a-k)+(a-l)*(a-l));
                    double w = w1+w2+w3+w4;
                    zone3[i+k][j+l].push_back(3*w1/w);
                    zone3[i+k][j+l].push_back(3*w2/w);
                    zone3[i+k][j+l].push_back(3*w3/w);
                    zone3[i+k][j+l].push_back(3*w4/w);
//                  0 -> top left
//                  1 -> top right
//                  2 -> bot left
//                  3 -> bot right
                }
                if(zone[i+k][j]==0) break;
            }
        }
    }
    
    cout << "end fillzone3" << endl;
}
