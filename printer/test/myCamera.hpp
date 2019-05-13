//
//  myCamera.hpp
//  simulator
//
//  Created by Pol Surakitkoson on 2/18/19.
//  Copyright © 2019 Pol Surakitkoson. All rights reserved.
//

#ifndef myCamera_hpp
#define myCamera_hpp

#include <stdio.h>
#include "opencv2/opencv.hpp"
#include <iostream>
using namespace std;
using namespace cv;

class myCamera{ // 0.1 mm. per pixel
    
public:
    myCamera();
    int rowSize = 2000;
    int colSize = 2000;
    Mat getImage(double x, double y, double theta, Mat field);

};


#endif /* myCamera_hpp */
