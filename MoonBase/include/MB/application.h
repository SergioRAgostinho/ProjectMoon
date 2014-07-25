#pragma once
#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osgGA/TrackballManipulator>

//#ifndef dDOUBLE
//#define dDOUBLE
//#endif
#include <ode/ode.h>
#include <MB/cube.h>
#include <MB/infiniteplane.h>
#include <MB/mouseeventhandler.h>
#include <MB/loader.h>

#define N_CUBES 1
#define N_BOTTLES 100

class Application
{
public:
	/////////////////////
	// Open Scene Graph
	/////////////////////

	//GUI Related stuff
	osgViewer::Viewer viewer;
	osg::ref_ptr<osgGA::CameraManipulator> camManip;

	//Graphical context and related traits
	osg::ref_ptr<osg::GraphicsContext> gc;
	osg::ref_ptr<osg::GraphicsContext::Traits> traits;
    osg::ref_ptr<osg::Camera> camera;

	//Optimizer
	osgUtil::Optimizer optimizer;

	//Scene population
	osg::ref_ptr<osg::Group> root;
    mb::Loader* loader, *loader2;
    mb::Body *marsSurface, *moscatel, *moscatelTBRot;
    std::vector<mb::Body*> selectableObjects;


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
	static void nearCallback(void *data, dGeomID o1, dGeomID o2);

    //To hide the cursor in every windows of the osgViewer
    void hideCursor();

    

	Application();
	~Application();

	int run();

};

