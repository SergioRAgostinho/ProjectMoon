#pragma once
#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osgGA/TrackballManipulator>

#define dDOUBLE
#include <ode/ode.h>

class Application
{
	/////////////////////
	// Open Scene Graph
	/////////////////////

	//GUI Related stuff
	osgViewer::Viewer viewer;
	osg::ref_ptr<osgGA::CameraManipulator> camManip;

	//Graphical context and related traits
	osg::ref_ptr<osg::GraphicsContext> gc;
	osg::ref_ptr<osg::GraphicsContext::Traits> traits;

	//Optimizer
	osgUtil::Optimizer optimizer;

	//Scene population
	osg::ref_ptr<osg::Group> root;
	osg::ref_ptr<osg::Box> box;
	osg::ref_ptr<osg::Geode> boxGeode;
	osg::ref_ptr<osg::ShapeDrawable> boxShape;

	////////////////////////
	// ODE (physics)
	////////////////////////
	
	//World
	dWorldID pWorld;
	dSpaceID pSpace;

	//Joint groups
	dJointGroupID pCollisionJG;

	//Solver specifics
	dThreadingImplementationID pSolverThreading;
	dThreadingThreadPoolID pSolverThreadPool;
	const int nIterSteps = 20;
	const dReal stepSize = 0.05;

	////Model loading
	//osg::ref_ptr<osg::Node> loadedModel;

	/////////////////////////////////////
	// Methods
	/////////////////////////////////////

	//Configure and set the graphical context
	void setGraphicsContext();

	//Populate scene
	void populateScene();

	//Render Loop
	void renderLoop();

	//Set up physics
	void setPhysics();

	//Function called everytime two objects are potencially near
	void nearCallback(void *data, dGeomID o1, dGeomID o2);

public:
	Application();
	~Application();

	int run();

};

