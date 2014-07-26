
#include <osgGA/FirstPersonManipulator>
#include <MB/application.h>
#include <MB/keyboardeventhandler.h>
#include <MB/utils.hpp>
#include <MB/fpmanipulator.h>



Application::Application()
{
	dInitODE2(0);
}


Application::~Application()
{
	//FIXME
//	delete [] cubes;
    delete loader;
    delete loader2;
    delete marsSurface;
    delete moscatel;


	// Shutdown threal pool
	dThreadingImplementationShutdownProcessing(pSolverThreading);
	dThreadingFreeThreadPool(pSolverThreadPool);
	dWorldSetStepThreadingImplementation(pWorld, NULL, NULL); 
	dThreadingFreeImplementation(pSolverThreading);

	//Destroy collision group
	dJointGroupDestroy(pCollisionJG);

	//Destroy colision space and sim world
	dSpaceDestroy(pSpace);
	dWorldDestroy(pWorld); 

	//Close library
	dCloseODE();
}

int Application::run()
{
	//Set physics
	setPhysics();

	//Set the graphical context and related traits
	setGraphicsContext();

	//Place where all things to be drawed are registered.
	populateScene();

	//Start the render loop
	renderLoop();

	return 0;
}

void Application::nearCallback(void *data, dGeomID o1, dGeomID o2) {
	
	int i, n;
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);
	Application* app = (Application*)data;

	if (b1 && b2 && dAreConnected(b1, b2))
		return;

	const int N = 64;
	dContact contact[N];
	n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
	if (n > 0) {
		for (i = 0; i<n; i++) {
            contact[i].surface.mode = dContactBounce | dContactSoftCFM;
            contact[i].surface.mu = dInfinity;
            contact[i].surface.mu2 = 0;
            contact[i].surface.bounce = 0.1;
            contact[i].surface.bounce_vel = 0.01;
            contact[i].surface.soft_cfm = 0.001;
			dJointID c = dJointCreateContact(app->pWorld, app->pCollisionJG, contact + i);
			dJointAttach(c, dGeomGetBody(o1), dGeomGetBody(o2));
		}
	}
}

void Application::setPhysics() {
	// recreate world
	pWorld = dWorldCreate();
	dWorldSetGravity(pWorld, 0, 0, -9.81);
	dWorldSetCFM(pWorld, 1e-10);
	dWorldSetERP(pWorld, 0.8);
	dWorldSetQuickStepNumIterations(pWorld, nIterSteps);

	//Create space for establising collision domain
	pSpace = dSweepAndPruneSpaceCreate(0, dSAP_AXES_XYZ);

	//Initialize the collision joint group
	pCollisionJG = dJointGroupCreate(0);

	//Setting multithreaded support for the physical solver
	pSolverThreading = dThreadingAllocateMultiThreadedImplementation();
	pSolverThreadPool = dThreadingAllocateThreadPool(4, 0, dAllocateFlagBasicData, NULL);
	dThreadingThreadPoolServeMultiThreadedImplementation(pSolverThreadPool, pSolverThreading);
	dWorldSetStepThreadingImplementation(pWorld, dThreadingImplementationGetFunctions(pSolverThreading), pSolverThreading);

	dAllocateODEDataForThread(dAllocateMaskAll);
}

void Application::renderLoop() {

    //needs to be invoked here!
    camManip->setByMatrix(osg::Matrix::rotate(M_PI/2.0, 1, 0, 0 ) * osg::Matrix::rotate(0, 1, 0, 0 ) * osg::Matrix::translate(0, -30, 10) );

	while (!viewer.done())
	{
        //hide cursor for each frame (if you go out of the software the cursor will stay visible if you get back to the software)
        hideCursor();
		//Physics update
		dSpaceCollide(pSpace, (void*) this, &nearCallback); 
		dWorldQuickStep(pWorld, stepSize);
		dJointGroupEmpty(pCollisionJG);

		//Update our objects
//        moscatel->update();
//        if (moscatel->getLinearSpeed() < 0.01 && moscatel->getAngularSpeed() < 0.01) {
//            moscatel->setPosition(mb::uniRand(-120, 120), mb::uniRand(-120, 120), mb::uniRand(180, 320));
//            moscatel->setLinearVelocity(mb::uniRand(-10, 10),mb::uniRand(-10, 10),mb::uniRand(-10, 10));
//            moscatel->setAngularVelocity(mb::uniRand(-1, 1),mb::uniRand(-1, 1),mb::uniRand(-1, 1));
//        }


		//Renders frame
		viewer.frame();

	}

}

void Application::populateScene() {

    loader = new mb::Loader("../res/models/MarsSurface.osgt");
    loader2 = new mb::Loader("../res/models/moscatel.osgt");

    //Place the hexagon
    osg::ref_ptr<osg::Geode> surface = loader->getNode<osg::Geode>("planetSurface-GEODE");
    marsSurface = new mb::Body(surface.get());
    marsSurface->initCollision(pSpace);


    moscatel = new mb::Body(loader2->getNode<osg::Geode>("pCylinder1-GEODE"));
//    moscatel->initPhysics(pWorld, pSpace, 2);
    moscatel->setPosition(60, 0, 60);
    selectableObjects.push_back(moscatel);

    moscatelTBRot = moscatel->clone();
    moscatelTBRot->setPosition(50, 0, 60);
    osg::Vec3 axis = osg::Vec3(mb::uniRand(-1, 1),mb::uniRand(-1, 1),mb::uniRand(-1, 1));
    axis.normalize();
    osg::Matrix rot = osg::Matrix::rotate(mb::uniRand(-M_PI, M_PI), axis);
    osg::Quat q = rot.getRotate();
    moscatelTBRot->setOrientationQuat(q.x(),q.y(),q.z(),q.w());
    selectableObjects.push_back(moscatelTBRot);

    //Add to root
    root = new osg::Group;
    root->addChild(marsSurface->getPAT());
    root->addChild(moscatel->getPAT());
    root->addChild(moscatelTBRot->getPAT());
    viewer.setSceneData(root.get());


}

void Application::setGraphicsContext() {

	traits = new osg::GraphicsContext::Traits;
	traits->x = 100;
	traits->y = 100;
	traits->width = 1280;
	traits->height = 720;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	camera = new osg::Camera;
	camera->setGraphicsContext(gc.get());
    camera->setViewport(new osg::Viewport(0, 0, traits->width , traits->height));
	GLenum bufferL = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(bufferL);
	camera->setReadBuffer(bufferL);

	// add this slave camera to the viewer, with a shift left of the projection matrix
    viewer.addSlave(camera.get());

    //Camera manipulator
    camManip = new mb::FirstPersonManipulator();

    
    //Subscribe object
    viewer.setCameraManipulator(camManip);
    viewer.addEventHandler(new mb::KeyboardEventHandler(this));
    viewer.addEventHandler(new mb::MouseEventHandler(camera.get(), &selectableObjects, dynamic_cast<mb::FirstPersonManipulator*>(camManip.get())));

}

void Application::hideCursor(){
    // switch off the cursor
    osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
    for(osgViewer::Viewer::Windows::iterator itr = windows.begin();
        itr != windows.end();
        ++itr)
    {
        (*itr)->useCursor(false);
    }
}