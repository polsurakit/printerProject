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


//var

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

void initialize(){
    image = imread(INPUT_NAME);
    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
    }
    cout << 1 ;
    resize(image,showedImage,Size(smallWindowSize, smallWindowSize),0,0);
    
    cout << 1 ;
    resize(image,image2,Size(TARGET_H_SIZE, TARGET_W_SIZE),0,0);
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
    cout << "theta = " << theta << endl;
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

bool checkDirection(int i, int j, int direction){
    int movei[4] = {0,moveStep,0,-moveStep};
    int movej[4] = {moveStep,0,-moveStep,0};
    if ( i+movei[direction] < 0 || j+movej[direction] < 0) return false;
    if ( i+movei[direction] >= fieldSize || j+movej[direction] >= fieldSize) return false;
    if ( zone[i+movei[direction]][j+movej[direction]] == 0) return false;
    return true;
}

void changeDirection(int &i, int &j, int &direction){
    int movei[4] = {0,moveStep,0,-moveStep};
    int movej[4] = {moveStep,0,-moveStep,0};
    int cnt = 0;
    while(cnt < 4){
        if (checkDirection(i, j, direction)){
            break;
        }else{
            direction += 1;
            direction %= 4;
            cnt++;
        }
    }
    if(cnt==4) direction = 4;
    else{
        i += movei[direction];
        j += movej[direction];
    }
}

int checkCase2(int y, int x, int py, int px){
    for(int i = firstPosition+printFieldSize/2-(printFieldSize-moveStep) ; i < fieldSize-TOPLEFTY ; i+=moveStep){
        if(i <= y && y < i+printFieldSize-moveStep){
            if(py > y){
                return 1;
            }else{
                return 0;
            }
        }
        if(i <= x && x < i+printFieldSize-moveStep){
            if(px > x){
                return 1;
            }else{
                return 0;
            }
        }
    }
    return 0;
}

void algorithm6();

string exec1(const char* cmd) {
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

int main(int argc, char** argv)
{
    printer.update();
    cout << TARGET_H_SIZE << " " << TARGET_W_SIZE << endl;
    initialize();
    printer.getCameraImage();
    // showResult();
    algorithm6();
    cout << "Save file ..." << endl;
    imwrite(OUTPUT_NAME, field);
    //imwrite("ex_"+OUTPUT_NAME, expectedResult);
    return 0;
}

void algorithm6(){
    //showResult();
    fillzone3();
    // showResult();
    cout << "start" << endl;
    int direction = 0;
    int i = firstPosition;
    int j = firstPosition;
    //map<pair<int,int>,int > mp;
    while(direction <= 3){
        cout << i << " " << j << endl;
        cout << "This step will move to x = " << i << " y = " << j << endl;
        cout << "If you need to skip type 'skip' and Enter" << endl;
        string input;
        cin >> input;
        if (input == "skip"){
            cout << "Skip this step" << endl;
            printer.getCameraImage();
            cout << "show result" << endl;
            cout << "go to type on simulation screen\n";
            showResult();
            zone[i][j] = 0;
            changeDirection(i, j, direction);
            continue;
        }
        cout << "issim " << isSimulation << endl;
        if(isSimulation) printer.moveSimulation(i,j);
        else printer.move(i, j);
        

        printer.getCameraImage();
        vector<double> pos(3);
        if(isSimulation) pos = printer.getPositionSimulation(); // x, y, theta
        else{
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
                        int c = checkCase2(Y,X,(int)pos[1], (int)pos[0]);
                        int c0,c1,c2;
                        c0 = exColor[0]+(int)((255-exColor[0])*zone3[Y][X][c]);
                        c1 = exColor[1]+(int)((255-exColor[1])*zone3[Y][X][c]);
                        c2 = exColor[2]+(int)((255-exColor[2])*zone3[Y][X][c]);
                        color = Vec3b(c0,c1,c2);
                        
                    }else{

                        int c1,c2,c0;
                        if(Y > j && X > j){
                            c0 = min(255,exColor[0]+(int)((255-exColor[0])*zone3[Y][X][0]));
                            c1 = min(255,exColor[1]+(int)((255-exColor[1])*zone3[Y][X][0]));
                            c2 = min(255,exColor[2]+(int)((255-exColor[2])*zone3[Y][X][0]));
                        }else if(Y > j && X < j){
                            c0 = min(255,exColor[0]+(int)((255-exColor[0])*zone3[Y][X][1]));
                            c1 = min(255,exColor[1]+(int)((255-exColor[1])*zone3[Y][X][1]));
                            c2 = min(255,exColor[2]+(int)((255-exColor[2])*zone3[Y][X][1]));
                        }else if(Y < j && X > j){
                            c0 = min(255,exColor[0]+(int)((255-exColor[0])*zone3[Y][X][2]));
                            c1 = min(255,exColor[1]+(int)((255-exColor[1])*zone3[Y][X][2]));
                            c2 = min(255,exColor[2]+(int)((255-exColor[2])*zone3[Y][X][2]));
                        }else if(Y < j && X < j){
                            c0 = min(255,exColor[0]+(int)((255-exColor[0])*zone3[Y][X][3]));
                            c1 = min(255,exColor[1]+(int)((255-exColor[1])*zone3[Y][X][3]));
                            c2 = min(255,exColor[2]+(int)((255-exColor[2])*zone3[Y][X][3]));
                        }else{
                            c0 = 255;
                            c1 = 255;
                            c2 = 255;
                            //cout << posY << " " << posX << " " << Y << " " << X << " 4" << endl;
                        }
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
        //show result
        printer.getCameraImage();
        cout << "show result" << endl;
        cout << "go to type on simulation screen\n";
        showResult();
        
        zone[i][j] = 0;
        changeDirection(i, j, direction);
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
            for(int k = 0 ; k < a ; k++){
                double w1 = (k+1);
                double w2 = (a-k);
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
            for(int k = 0 ; k < a ; k++){
                double w1 = (k+1);
                double w2 = (a-k);
                zone3[i+k][j].push_back(w1/(w1+w2));
                zone3[i+k][j].push_back(w2/(w1+w2));
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
                }
                if(zone[i+k][j]==0) break;
            }
        }
    }
    
    cout << "end fillzone3" << endl;
}
