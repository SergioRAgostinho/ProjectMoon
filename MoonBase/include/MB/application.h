#ifndef MoonBase_application_h
#define MoonBase_application_h

#ifdef WIN32
//In windows we can properly compile ODE
#ifndef dDOUBLE
#define dDOUBLE
#endif
#endif


#pragma once
#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osgGA/TrackballManipulator>

#include <ode/ode.h>
#include <MB/cube.h>
#include <MB/infiniteplane.h>
#include <MB/mouseeventhandler.h>
#include <MB/loader.h>
#include <MB/hud.h>
#include <MB/humanmanipulator.h>
#include <MB/group.h>

#define N_CUBES 1
#define N_BOTTLES 100

#define APP_MODE_STANDARD 0x00
#define APP_MODE_DEBUG 0x01
#define APP_MODE_TV 0x02
#define APP_MODE_STEREO 0x04
#define APP_MODE_MOUSE 0x08

class Application
{
private:

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

	//To hide the cursor in every windows of the osgViewer
	void hideCursor();

	//Console argument parse
	void parseConsoleArgument(int argc, char* argv[]);

	//Parabolic gravity shift mimicking a parabolic flight
	void parabolicFlightGravityModifier();

public:
	/////////////////////
	// Application Options
	/////////////////////

	unsigned long int modes;

	/////////////////////
	// Open Scene Graph
	/////////////////////

	//GUI Related stuff
	osgViewer::Viewer viewer;
    mb::FirstPersonManipulator *man;
	mb::HumanManipulator *human;

	//Graphical context and related traits
	osg::ref_ptr<osg::GraphicsContext> gc;
	osg::ref_ptr<osg::GraphicsContext::Traits> traits;
    osg::ref_ptr<osg::Camera> camera;


	//Optimizer
	osgUtil::Optimizer optimizer;

	//Scene population
	osg::ref_ptr<osg::Group> root;
	osg::ref_ptr<mb::Group> iss;
	osg::ref_ptr<mb::Body> left_glove, right_glove;

	//to delete
	osg::ref_ptr<mb::Body> *bottles;
	const unsigned int n_bottles;

	//Loaders
	osg::ref_ptr<mb::Loader> loader;

    //HUD
    mb::Hud* hud;

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


	//Function called everytime two objects are potencially near
	static void nearCallback(void *data, dGeomID o1, dGeomID o2);

	Application(int argc, char* argv[]);
	~Application();

	int run();

};

#endif

