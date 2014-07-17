//
//  moonbaseobject.hpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_body_h
#define MoonBase_body_h

#include <ode/ode.h>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <MB/object.h>

namespace mb {
    
    class Body : public Object {

        //Converter between OSG and ODE
        bool triOGS2ODE();

    protected:

        //Physic engine identities
        dBodyID pBody;
        dGeomID pGeom;
        dWorldID pWorld;
        dSpaceID pSpace;
        float* pVerts;
        dTriIndex* pIdx;
        dTriMeshDataID pMeshData;

        //Open Scene Graph
        osg::ref_ptr<osg::Geode> gGeode;

        //Members
        void initialize();

    public:

        //Constructor party
        Body();
        Body(osg::Geode* geode);
        Body(dWorldID w, dSpaceID s);

        //Deconstructor afterparty
        ~Body();

        //Initialize physics
        void initPhysics(dWorldID world, dSpaceID space);

        //return the Geode pointer
        osg::Geode* getGeode();

        //Set the geode
        void setGeode(osg::Geode* geode);

        //Set world
        void setWorld(dWorldID world);

        //Set space
        void setSpace(dSpaceID space);

        //Set angular velocity
        void setAngularVelocity(double x, double y, double z);

        //Set Orientation
        void setOrientationQuat(double x, double y, double z, double w);
        
        //Set the object position
        void setPosition(double x, double y, double z);
        
        //Update position and orientation
        void update();
    };

}

#endif
