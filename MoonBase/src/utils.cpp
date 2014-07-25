//
//  utils.cpp
//  MoonBase
//
//  Created by Neglective on 17/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <random>
#include <MB/utils.hpp>

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
