#pragma once
#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osgGA/TrackballManipulator>

class Application
{
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

public:
	Application();
	~Application();

	int run();

};

