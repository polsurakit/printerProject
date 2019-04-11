//
//  myRandom.hpp
//  simulator
//
//  Created by Pol Surakitkoson on 2/18/19.
//  Copyright © 2019 Pol Surakitkoson. All rights reserved.
//

#ifndef myRandom_hpp
#define myRandom_hpp

#include <stdio.h>
#include "opencv2/opencv.hpp"
using namespace cv;

class myRandom{
    cv::RNG rng;
public:
    myRandom(){
        cv::RNG rng(cv::getCPUTickCount());
    }
    
    double normal(double sigma){
        return rng.gaussian(sigma);
    }
};

#endif /* myRandom_hpp */
