//
//  moonbaseobject.hpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_moonbaseobject_hpp
#define MoonBase_moonbaseobject_hpp

#include <ode/ode.h>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>

class MoonBaseObject {

protected:

    //Physic engine identities
	dBodyID pBody;
	dGeomID pGeom;
	dWorldID pWorld;
	dSpaceID pSpace;

    //Open Scene Graph
    osg::ref_ptr<osg::Geometry> gGeometry;
	osg::ref_ptr<osg::Geode> gGeode;
	osg::ref_ptr<osg::PositionAttitudeTransform> gPAT;

    enum ConstructorFlags {STANDARD, NO_BODY};

    //Members
//    MoonBaseObject(dWorldID w, dSpaceID s, ConstructorFlags flags);

public:

    MoonBaseObject(dWorldID w, dSpaceID s);

    osg::Geode* getGeode();


};


#endif
