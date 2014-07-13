#pragma once
//#ifndef dDOUBLE
//#define dDOUBLE
//#endif
#include <ode/ode.h>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>
#include <osgManipulator/Selection>
#include <osgManipulator/Dragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/CommandManager>

#include <MB/moonbaseobject.hpp>

class Cube : public MoonBaseObject
{
	//Open Scene Graph
	osg::ref_ptr<osg::Box> gBox;
	osg::ref_ptr<osg::ShapeDrawable> gBoxShape;

    //FIXME: OSG Manipulator
    osg::ref_ptr<osgManipulator::CommandManager> gCommandManager;
    osg::ref_ptr<osgManipulator::Selection> gSelection;
    osg::ref_ptr<osgManipulator::Dragger> gDragger;
    osg::ref_ptr<osg::Group> gGroup = new osg::Group();

public:
	Cube(dWorldID w, dSpaceID s, dReal size);
	~Cube();

	//Return pointer to geode
	osg::Geode* getGeode();
    osg::Group* osgGet();

	//Return pointer to PAT
	osg::PositionAttitudeTransform* getPAT();

	//Set the object position
	void setOrientationQuat(double x, double y, double z, double w);

	//Update position and orientation
	void update();

	//Set angular velocity
	void setAngularVelocity(double x, double y, double z);
};
