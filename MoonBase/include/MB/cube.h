#pragma once
#ifndef dDOUBLE
#define dDOUBLE
#endif
#include <ode/ode.h>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>

class Cube
{
	//Physic engine identities
	dBodyID pBody;
	dGeomID pGeom;
	dWorldID pWorld;
	dSpaceID pSpace;
	
	//Open Scene Graph
	osg::ref_ptr<osg::Box> gBox;
	osg::ref_ptr<osg::ShapeDrawable> gBoxShape;
	osg::ref_ptr<osg::Geode> gGeode;
	osg::ref_ptr<osg::PositionAttitudeTransform> gT;

public:
	Cube(dWorldID w, dSpaceID s, dReal size);
	~Cube();

	//Return pointer to geode
	osg::Geode* getGeode();

	//Return pointer to PAT
	osg::PositionAttitudeTransform* getPAT();

	//Set the object position
	void setPosition(double x, double y, double z);

	//Set the object position
	void setOrientationQuat(double x, double y, double z, double w);

	//Update position and orientation
	void update();

	//Set angular velocity
	void setAngularVelocity(double x, double y, double z);
};

