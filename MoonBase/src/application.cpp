
#include <osgGA/FirstPersonManipulator>
#include <MB/application.h>
#include <MB/keyboardeventhandler.h>
#include <MB/utils.hpp>



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
//    delete moscatelClone;

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

    camManip = new osgGA::TrackballManipulator();
	viewer.setCameraManipulator(camManip);

    camManip->setByMatrix(osg::Matrix::rotate(M_PI/2.0, 1, 0, 0 ) * osg::Matrix::rotate(0, 1, 0, 0 ) * osg::Matrix::translate(0, -30, 10) );


	while (!viewer.done())
	{
		//Physics update
		dSpaceCollide(pSpace, (void*) this, &nearCallback); 
		dWorldQuickStep(pWorld, stepSize);
		dJointGroupEmpty(pCollisionJG);

		//Update our objects
        for (int i = 0; i < N_CUBES; ++i) {
            cubes[i].update();
            if (cubes[i].getLinearSpeed() < 0.01 && cubes[i].getAngularSpeed()< 0.01) {
                cubes[i].setPosition(mb::uniRand(-120, 120), mb::uniRand(-120, 120), mb::uniRand(180, 320));
                cubes[i].setLinearVelocity(mb::uniRand(-10, 10),mb::uniRand(-10, 10),mb::uniRand(-10, 10));
                cubes[i].setAngularVelocity(mb::uniRand(-1, 1),mb::uniRand(-1, 1),mb::uniRand(-1, 1));
            }
        }
        moscatel->update();
        if (moscatel->getLinearSpeed() < 0.01 && moscatel->getAngularSpeed() < 0.01) {
            moscatel->setPosition(mb::uniRand(-120, 120), mb::uniRand(-120, 120), mb::uniRand(180, 320));
            moscatel->setLinearVelocity(mb::uniRand(-10, 10),mb::uniRand(-10, 10),mb::uniRand(-10, 10));
            moscatel->setAngularVelocity(mb::uniRand(-1, 1),mb::uniRand(-1, 1),mb::uniRand(-1, 1));
        }

        //Update our objects
        for (int i = 0; i < N_BOTTLES; ++i) {
            moscatelClone[i].update();
            if (moscatelClone[i].getLinearSpeed() < 0.01 && moscatelClone[i].getAngularSpeed()< 0.01) {
                moscatelClone[i].setPosition(mb::uniRand(-120, 120), mb::uniRand(-120, 120), mb::uniRand(180, 320));
                moscatelClone[i].setLinearVelocity(mb::uniRand(-10, 10),mb::uniRand(-10, 10),mb::uniRand(-10, 10));
                moscatelClone[i].setAngularVelocity(mb::uniRand(-1, 1),mb::uniRand(-1, 1),mb::uniRand(-1, 1));
            }
        }


		//Renders frame
		viewer.frame();
	}

}

void Application::populateScene() {

    loader = new mb::Loader("../res/models/MarsSurface.osgt");
    loader2 = new mb::Loader("../res/models/moscatel.osgt");


	//Place and set the cube
    for (int i = 0; i < N_CUBES; ++i) {
        cubes[i] = mb::Cube(pWorld, pSpace, 10);
        cubes[i].setTotalMass(10);
        cubes[i].setPosition(mb::uniRand(-120, 120), mb::uniRand(-120, 120), mb::uniRand(180, 320));
        cubes[i].setLinearVelocity(mb::uniRand(-10, 10),mb::uniRand(-10, 10),mb::uniRand(-10, 10));
        cubes[i].setAngularVelocity(mb::uniRand(-1, 1),mb::uniRand(-1, 1),mb::uniRand(-1, 1));
    }


    //Place the hexagon
    marsSurface = new mb::Body(dynamic_cast<osg::Geode*>(loader->getNode("planetSurface-GEODE")));
    marsSurface->initCollision(pSpace);

    moscatel = new mb::Body(dynamic_cast<osg::Geode*>(loader2->getNode("pCylinder1-GEODE")));
    moscatel->initPhysics(pWorld, pSpace, 2);
    moscatel->setPosition(60, 0, 60);

    for (int i = 0; i < N_BOTTLES; ++i) {
        moscatelClone[i] = *(moscatel->clone());
        moscatelClone[i].setTotalMass(10);
        moscatelClone[i].setPosition(mb::uniRand(-120, 120), mb::uniRand(-120, 120), mb::uniRand(180, 320));
        moscatelClone[i].setLinearVelocity(mb::uniRand(-10, 10),mb::uniRand(-10, 10),mb::uniRand(-10, 10));
        moscatelClone[i].setAngularVelocity(mb::uniRand(-1, 1),mb::uniRand(-1, 1),mb::uniRand(-1, 1));
    }



    //Add to root
    root = new osg::Group;
    for (int i = 0; i < N_CUBES; ++i)
        root->addChild(cubes[i].getPAT());
    root->addChild(marsSurface->getPAT());
    root->addChild(moscatel->getPAT());
    for(int i = 0; i < N_BOTTLES; ++i)
        root->addChild(moscatelClone[i].getPAT());
    viewer.setSceneData(root.get());

    //Subscribe object
    //viewer.addEventHandler(new mb::KeyboardEventHandler(cube));
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

	osg::ref_ptr<osg::Camera> cameraL = new osg::Camera;
	cameraL->setGraphicsContext(gc.get());
//	cameraL->setViewport(new osg::Viewport(0, 0, traits->width / 2.0, traits->height));
    	cameraL->setViewport(new osg::Viewport(0, 0, traits->width , traits->height));
	GLenum bufferL = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	cameraL->setDrawBuffer(bufferL);
	cameraL->setReadBuffer(bufferL);

	// add this slave camera to the viewer, with a shift left of the projection matrix
//	viewer.addSlave(cameraL.get(), osg::Matrixd::translate(0.06, 0, 0), osg::Matrixd());
    	viewer.addSlave(cameraL.get());

//	osg::ref_ptr<osg::Camera> cameraR = new osg::Camera;
//	cameraR->setGraphicsContext(gc.get());
//	cameraR->setViewport(new osg::Viewport(traits->width / 2.0, 0, traits->width / 2.0, traits->height));
//	GLenum bufferR = traits->doubleBuffer ? GL_BACK : GL_FRONT;
//	cameraR->setDrawBuffer(bufferR);
//	cameraR->setReadBuffer(bufferR);
//
//	// add this slave camera to the viewer, with a shift left of the projection matrix
//	viewer.addSlave(cameraR.get(), osg::Matrixd::translate(-.06, 0, 0), osg::Matrixd());

    //Place the camera
    cameraL->setViewMatrixAsLookAt(osg::Vec3d(0,-2,1), osg::Vec3d(0,0,0), osg::Vec3d(0,1,0));

}