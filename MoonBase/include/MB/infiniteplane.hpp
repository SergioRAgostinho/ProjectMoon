//
//  infiniteplane.hpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_infiniteplane_hpp
#define MoonBase_infiniteplane_hpp

#include <ode/ode.h>
#include <osg/Shape>

#include <MB/moonbaseobject.hpp>


class InfinitePlane : public MoonBaseObject {


public:

    //osg Infinite plane specefic
	osg::ref_ptr<osg::InfinitePlane> gShape;

    //Constructor
    InfinitePlane(dWorldID pWorld, dSpaceID pSpace);

};


#endif
