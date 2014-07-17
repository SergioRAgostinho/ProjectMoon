
#include <MB/application.h>
#include <MB/keyboardeventhandler.h>


Application::Application()
{
	dInitODE2(0);
}


Application::~Application()
{
	//FIXME
	delete cube;
    delete plane;
    delete loader;
    delete hex;

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

	const int N = 4;
	dContact contact[N];
	n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
	if (n > 0) {
		for (i = 0; i<n; i++) {
			contact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
			if (dGeomGetClass(o1) == dSphereClass || dGeomGetClass(o2) == dSphereClass)
				contact[i].surface.mu = 20;
			else
				contact[i].surface.mu = 0.5;
			contact[i].surface.slip1 = 0.0;
			contact[i].surface.slip2 = 0.0;
			contact[i].surface.soft_erp = 0.8;
			contact[i].surface.soft_cfm = 0.01;
			dJointID c = dJointCreateContact(app->pWorld, app->pCollisionJG, contact + i);
			dJointAttach(c, dGeomGetBody(o1), dGeomGetBody(o2));
		}
	}
}

void Application::setPhysics() {
	// recreate world
	pWorld = dWorldCreate();
	dWorldSetGravity(pWorld, 0, 0, -0.01);
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

    camManip = new osgGA::TrackballManipulator();
	viewer.setCameraManipulator(camManip);

    camManip->setByMatrix(osg::Matrix::rotate(M_PI/2.0, 1, 0, 0 ) * osg::Matrix::rotate(-M_PI/6.0, 1, 0, 0 ) * osg::Matrix::translate(0, -10, 6) );


	while (!viewer.done())
	{
		//Physics update
		dSpaceCollide(pSpace, (void*) this, &nearCallback); 
		dWorldQuickStep(pWorld, stepSize);
		dJointGroupEmpty(pCollisionJG);

		//Update our cube position
		cube->update();
        hex->update();

		//Renders frame
		viewer.frame();
	}

}

void Application::populateScene() {

	cube = new mb::Cube(pWorld, pSpace, 1);
    plane = new mb::InfinitePlane(pSpace);
    loader = new mb::Loader("../res/models/hex.osgt");
//    loadedObject = new LoadedObject(pWorld, pSpace, "../res/models/MarsSurface.osgt");


	//Place and set the cube
	cube->setAngularVelocity(0, 0, 0.1);
    cube->setPosition(0, 0, 1);


    //Place the hexagon
    hex = new mb::Body(dynamic_cast<osg::Geode*>(loader->getNode("pCube1-GEODE")));
    hex->initPhysics(pWorld, pSpace);
    hex->setPosition(0, 10, 2);

    //Add to root
    root = new osg::Group;
	root->addChild(cube->getPAT());
    root->addChild(plane->getGeode());
    root->addChild(hex->getPAT());
    viewer.setSceneData(root.get());

    //Subscribe object
    viewer.addEventHandler(new mb::KeyboardEventHandler(cube));
}

void Application::setGraphicsContext() {

	traits = new osg::GraphicsContext::Traits;
	traits->x = 100;
	traits->y = 100;
	traits->width = 1280;
	traits->height = 480;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	osg::ref_ptr<osg::Camera> cameraL = new osg::Camera;
	cameraL->setGraphicsContext(gc.get());
	cameraL->setViewport(new osg::Viewport(0, 0, traits->width / 2.0, traits->height));
	GLenum bufferL = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	cameraL->setDrawBuffer(bufferL);
	cameraL->setReadBuffer(bufferL);

	// add this slave camera to the viewer, with a shift left of the projection matrix
	viewer.addSlave(cameraL.get(), osg::Matrixd::translate(0.06, 0, 0), osg::Matrixd());

	osg::ref_ptr<osg::Camera> cameraR = new osg::Camera;
	cameraR->setGraphicsContext(gc.get());
	cameraR->setViewport(new osg::Viewport(traits->width / 2.0, 0, traits->width / 2.0, traits->height));
	GLenum bufferR = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	cameraR->setDrawBuffer(bufferR);
	cameraR->setReadBuffer(bufferR);

	// add this slave camera to the viewer, with a shift left of the projection matrix
	viewer.addSlave(cameraR.get(), osg::Matrixd::translate(-.06, 0, 0), osg::Matrixd());

    //Place the camera
    cameraL->setViewMatrixAsLookAt(osg::Vec3d(0,-2,1), osg::Vec3d(0,0,0), osg::Vec3d(0,1,0));

}