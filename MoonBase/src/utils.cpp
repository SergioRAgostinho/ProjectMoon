//
//  utils.cpp
//  MoonBase
//
//  Created by Neglective on 17/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <random>
#include <MB/utils.hpp>
#include <iostream>

double mb::uniRand(double min, double max) {
    //        double mean = 0.5 * (max - min);
    //        int K = (int) 1/(max-min);
    //        return (rand() % K  - 0.5 * K)/K + mean;

    // Seed with a real random value, if available
    std::random_device rd;

    // Choose a random mean between 1 and 6
    std::default_random_engine e1(rd());
    std::uniform_real_distribution<double> uniform_dist(min, max);
    return uniform_dist(e1);

}

void mb::printDebugMatrix(osg::Matrix mat, std::string opt) {
    std::cout << opt << std::endl;
    std::cout << mat(0,0) << " " << mat(0,1) << " " << mat(0,2) << " " << mat(0,3) << " " << std::endl;
    std::cout << mat(1,0) << " " << mat(1,1) << " " << mat(1,2) << " " << mat(1,3) << " " << std::endl;
    std::cout << mat(2,0) << " " << mat(2,1) << " " << mat(2,2) << " " << mat(2,3) << " " << std::endl;
    std::cout << mat(3,0) << " " << mat(3,1) << " " << mat(3,2) << " " << mat(3,3) << " " << std::endl;
}

void mb::printDebugOrientation(osg::Matrix mat, osg::Quat q,  std::string opt) {
    std::cout << opt << std::endl;
    std::cout << mat(0,0) << " " << mat(0,1) << " " << mat(0,2) << std::endl;
    std::cout << mat(1,0) << " " << mat(1,1) << " " << mat(1,2) << std::endl;
    std::cout << mat(2,0) << " " << mat(2,1) << " " << mat(2,2) << std::endl;
    std::cout << q.x() << " " << q.y() << " " << q.z() << " " << q.w() << std::endl;
    std::cout << std::endl;
}
