//
//  utils.hpp
//  MoonBase
//
//  Created by Neglective on 17/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_utils_hpp
#define MoonBase_utils_hpp

#include <osg/Matrix>

namespace mb {

    double uniRand(double min, double max);

    template <typename T>
    int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    void printDebugMatrix(osg::Matrix mat, std::string opt);

    void printDebugOrientation(osg::Matrix mat, osg::Quat q,  std::string opt);

}
#endif
