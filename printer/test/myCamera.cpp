//
//  myCamera.cpp
//  simulator
//
//  Created by Pol Surakitkoson on 2/18/19.
//  Copyright © 2019 Pol Surakitkoson. All rights reserved.
//

#include "myCamera.hpp"

myCamera::myCamera() {};

Mat myCamera::getImage(double x, double y, double theta, Mat field){
    Mat result(rowSize,colSize,CV_8UC3,Vec3b(0,0,0));
    for(int i = -colSize/2 ; i < colSize/2 ; i++ ){
        for(int j = rowSize/2-1 ; j >= -rowSize/2 ; j--){
            
            int posX = round(cos(theta)*(i) - sin(theta)*(j));
            int posY = round(sin(theta)*(i) + cos(theta)*(j));
            posX = posX + x;
            posY = -posY + y;
            if(posX < 0 || posY < 0 || posX >= field.cols || posY >= field.rows){
                result.at<Vec3b>(abs(j-rowSize/2+1),i+1500) = Vec3b(0,255,255);
            }else{
                //cout << posY << " " << posX << endl;
                result.at<Vec3b>(abs(j-rowSize/2+1),i+colSize/2) = field.at<Vec3b>(posY,posX);
            }
        }
    }
    return result;
}
