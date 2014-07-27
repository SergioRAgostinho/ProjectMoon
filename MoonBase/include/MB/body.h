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
#include <memory>

namespace mb {
    
    class Body : public Object {

        //Converter between OSG and ODE
        bool triOGS2ODE();

        //Initialize mass
        void initMass(dMass* mass);
        void initMass(dMass* mass, double kg);



    protected:

        //Physic engine identities
        dBodyID pBody;
        dGeomID pGeom;
        dWorldID pWorld;
        dSpaceID pSpace;
        std::shared_ptr<float> pVerts;
        std::shared_ptr<dTriIndex> pIdx;
        dTriMeshDataID pMeshData;

        //Open Scene Graph
        osg::ref_ptr<osg::Geode> gGeode;
        osg::ref_ptr<osg::Geode> gBB;

        bool gBBState;
        bool gPermBB;

        //Members
        void initialize();

    public:

        //Constructor party
        Body();
        Body(osg::Geode* geode);
        Body(dWorldID w, dSpaceID s);

        //Deconstructor afterparty
        ~Body();

        //Align the current body with the provided one
        osg::Quat align(Body* ref);

        //return a clone/clones
        Body* clone();
        Body* clone(int nClones);

        //enable dinamical properties of the body
        void enablePBody();

        //disables dinamical properties of the body
        void disablePBody();

        //checks if pBody is enabled
        bool isPBodyEnabled();

        //Initialize only the collision geometry
        void initCollision(dSpaceID space);

        //Initialize physics
        void initPhysics(dWorldID world, dSpaceID space);
        void initPhysics(dWorldID world, dSpaceID space, double massAmount);

        //return the Geode pointer
        osg::Geode* getGeode();

        //Set angular velocity
        void getAngularVelocity(double* av);

        //Set linear velocity
        void getLinearVelocity(double* lv);

        //Set angular velocity
        double getAngularSpeed();

        //Set linear velocity
        double getLinearSpeed();

        //Get Orientation Mat
        osg::Matrix getOrientationMat();

        //Get Orientation Quaternion
        osg::Quat getOrientationQuat();

        //Get Position
        osg::Vec3 getPosition();

        //Set the geode
        void setGeode(osg::Geode* geode);

        //Set world
        void setWorld(dWorldID world);

        //Set space
        void setSpace(dSpaceID space);

        //Set angular velocity
        void setAngularVelocity(double x, double y, double z);

        //Set linear velocity
        void setLinearVelocity(double x, double y, double z);

        //Set linear acceleration
        void setLinearAcceleration(double x, double y, double z);

        //Set Orientation
        void setOrientationQuat(double x, double y, double z, double w);

        //Set Orientation Mat
        void setOrientationMat(osg::Matrix mat);

        //Set the object position
        void setPosition(double x, double y, double z);

        //Adjust object mass
        void setTotalMass(double amount);
        
        //Update position and orientation
        void update();

        //Show bounding box drawing
        void activateBB();

        //Remove bounding box drawing
        void removeBB();

        //Toggle bounding box drawing routines
        void toggleBB();

        //Toggle permanent BB state
        void togglePermBB();

    };

}

#endif
