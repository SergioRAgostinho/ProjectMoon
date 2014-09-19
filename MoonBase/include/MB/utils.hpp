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
#include <exception>
#include <iostream>

#define SafeRelease(x) if(x) {delete x; x = nullptr;}
#define SafeReleaseArray(x) if(x) {delete[] x; x = nullptr;}

#define LOG(x) std::clog << "[LOG] " << x << std::endl
#define ERROR(x) std::cerr << "[ERROR] " << x << std::endl

namespace mb {

    double uniRand(double min, double max);

    template <typename T>
    int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    void printDebugMatrix(osg::Matrix mat, std::string opt);

    void printDebugOrientation(osg::Matrix mat, osg::Quat q,  std::string opt);

	class NotImplementedException : public std::exception{
			
		virtual const char* what() const throw() {
			return "Code not implemented";
		}
	};
}
#endif
