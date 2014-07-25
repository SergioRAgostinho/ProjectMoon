//
//  utils.hpp
//  MoonBase
//
//  Created by Neglective on 17/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_utils_hpp
#define MoonBase_utils_hpp

namespace mb {

    double uniRand(double min, double max);

    template <typename T>
    int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

}
#endif
