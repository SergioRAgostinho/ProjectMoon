
#include <osgGA/FirstPersonManipulator>
#include <osgViewer/View>
#include <MB/application.h>
#include <MB/keyboardeventhandler.h>
#include <MB/utils.hpp>
#include <MB/fpmanipulator.h>



Application::Application()
{
	dInitODE2(0);

	//model loader component
	loader = nullptr;

	//heads up display component
	hud = nullptr;

	//scene models
	marsSurface = nullptr;

	//Manipulators
	man = nullptr;
	human = nullptr;
}


Application::~Application()
{
	//FIXME
	SafeRelease(loader);
	SafeRelease(hud);
	SafeRelease(marsSurface);

	//FIXME - Cannot kill manipulators because of a shared point given to the osg viewer
	//SafeRelease(man);
	//SafeRelease(human);
	
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
        ////////////////////////////
        //Camera collision situation
        /////////////////////////////

		mb::FirstPersonManipulator *cam = app->man;
        dGeomID camID = cam->getGeomID();
        if (o1 == camID || o2 == camID) {

            //both static objects
            if (b1 == nullptr && b2 == nullptr) {

                //Find the normal more burried into the object
                double depth = 0;
                int iMaxDepth = -1;
                for (i = 0; i<n; ++i) {
                    double testDepth = (double) contact[i].geom.depth;
                    if (testDepth > depth) {
                        depth = testDepth;
                        iMaxDepth = i;
                    }
                }

                int sign;
                (camID == contact[iMaxDepth].geom.g2)? sign = -1 : sign = 1;
                cam->armRevert((double) sign * depth * contact[iMaxDepth].geom.normal[0],
                               (double) sign * depth * contact[iMaxDepth].geom.normal[1],
                               (double) sign * depth * contact[iMaxDepth].geom.normal[2]);

                return;
            }

            //FIXME: Remove when trying collision between static, grabbed object and camera
            return;
        }

        //////////////////////////////
        //// Cameras grabbed object
        ////////////////////////////
        mb::Body* grabbedBody = cam->getGrabbedBody();

        //if body exists
        if( grabbedBody ) {

            dGeomID grabbedGeom = grabbedBody->getGeomID();
            if(grabbedGeom == o1 || grabbedGeom == o2) {

                //find which one is the grabbed object
                dGeomID otherGeom;
                dBodyID otherBody;
                if(grabbedGeom == o1) {
                    otherGeom = o2;
                    otherBody = b2;
                } else {
                    otherGeom = o1;
                    otherBody = b1;
                }

                //check if the other object is static
                if(!otherBody) {

                    //Find the normal more burried into the object
#if 1
                    double depth = 0;
                    int iMaxDepth = -1;
                    for (i = 0; i<n; ++i) {
                        double testDepth = (double) contact[i].geom.depth;
                        if (testDepth > depth) {
                            depth = testDepth;
                            iMaxDepth = i;
                        }
                    }

                    int sign;
                    (grabbedGeom == contact[iMaxDepth].geom.g2)? sign = -1 : sign = 1;
                    grabbedBody->armRevert((double) sign * depth * contact[iMaxDepth].geom.normal[0],
                                           (double) sign * depth * contact[iMaxDepth].geom.normal[1],
                                           (double) sign * depth * contact[iMaxDepth].geom.normal[2]);
#else
                    double depth = 0;
                    int iMaxDepth = 0;
                    dVector3 normal = {0,0,0,0};
                    for (i = 0; i<n; ++i) {
                        if (contact[i].geom.depth > 0) {
                            depth = (double) contact[i].geom.depth;
                            iMaxDepth++;
                            normal[0] += depth*contact[i].geom.normal[0];
                            normal[1] += depth*contact[i].geom.normal[1];
                            normal[2] += depth*contact[i].geom.normal[2];
                        }
                    }

                    int sign;
                    (grabbedGeom == contact[iMaxDepth].geom.g2)? sign = -1 : sign = 1;
                    grabbedBody->armRevert((double) sign * normal[0] /iMaxDepth,
                                           (double) sign * normal[1] /iMaxDepth,
                                           (double) sign * normal[2] /iMaxDepth);
#endif
                }
            }
        }

        /////////////////////////
        /// Remaining stuff
        /////////////////////////

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
    //camManip->setByMatrix(osg::Matrix::rotate(M_PI/2.0, 1, 0, 0 ) * osg::Matrix::rotate(0, 1, 0, 0 ) * osg::Matrix::translate(0, -30, 30) );
	//camManip->setByMatrix(osg::Matrix::rotate(M_PI_2, 1, 0, 0) * osg::Matrix::rotate(-M_PI_2 * 0.28f, 1, 0, 0) 
	//	* osg::Matrix::rotate(M_PI * 1.14f, 0, 0, 1) * osg::Matrix::translate(0, -17.7, -0.4));
	//camManip->setByMatrix(osg::Matrix::rotate(M_PI_2, 1, 0, 0));
	
	man->setByMatrix(osg::Matrix::rotate(M_PI_2, 1, 0, 0));



    viewer.init();
    viewer.realize();
	while (!viewer.done())
	{
        //hide cursor for each frame (if you go out of the software the cursor will stay visible if you get back to the software)
        //hideCursor();

        viewer.advance();
        viewer.eventTraversal();

		//Physics update
		dSpaceCollide(pSpace, (void*) this, &nearCallback);
		dWorldQuickStep(pWorld, stepSize);
		dJointGroupEmpty(pCollisionJG);

        //Revert the camera movement if needed
        man->processRevert();
        mb::Body* grabbedBody = man->getGrabbedBody();
        if (grabbedBody && man->getGrabbedComplete())
            grabbedBody->processRevert();

		//Update our objects
        

		//Renders frame decomposing this into all the required events to prevent camera from jittering on collision
        //		viewer.frame();

        viewer.updateTraversal();
        viewer.renderingTraversals();
	}

}

void Application::populateScene() {

	//Create root node
	root = new osg::Group;

	//Add HUD
	root->addChild(hud->init());

    //Place the surface
	//loader = new mb::Loader("../res/models/MarsSurface.osgt");
 //   osg::ref_ptr<osg::Geode> surface = loader->getNode<osg::Geode>("planetSurface-GEODE");
 //   marsSurface = new mb::Body(surface.get());
 //   marsSurface->initCollision(pSpace);
	//root->addChild(marsSurface->getPAT());

	//Load the ISS
	loader = new mb::Loader("../res/models/iss_int5.ive");
	//loader->printGraph();
	loader->setRoot<osg::MatrixTransform>();
	loader->getPAT()->setAttitude(osg::Quat(M_PI, osg::Vec3(0, 0, 1)));
	loader->getPAT()->setPosition(osg::Vec3(0, -17.7, 0.4));

	//loader->setRoot("MSG");
	root->addChild(loader->getPAT());
	//root->addChild(loader->getNode());

	//osg::ref_ptr<osg::Geode> part = loader->getNode<osg::Geode>("p98"); //<- quads
	//osg::ref_ptr<osg::Geode> part2 = loader->getNode<osg::Geode>("Node2_f3922"); //<- triangles

	//osg::ref_ptr<osg::Geometry> geo = dynamic_cast<osg::Geometry*>(part2->getDrawable(0));
	//GLenum type = geo->getPrimitiveSet(0)->getMode();
	//root->addChild(part.get());

    //Add full tree to scene
    viewer.setSceneData(root.get());

	//Needed to be brought here because the manipulator needs to be initialized when the selected object list is already set
	//Camera manipulator
	man = new mb::FirstPersonManipulator(camera.get());
	man->initCollision(pSpace, 0.1f);

	//The new human camera manipulator
	human = new mb::HumanManipulator(&viewer, mb::HumanManipulatorMode::DEBUG_WINDOW);

	//Subscribe object
	//viewer.setCameraManipulator(man);
	viewer.setCameraManipulator(human);
	viewer.addEventHandler(new mb::KeyboardEventHandler(this));
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
	traits->useCursor = false;

	gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	camera = new osg::Camera;
	camera->setGraphicsContext(gc.get());
    camera->setViewport(new osg::Viewport(0, 0, traits->width , traits->height));
	GLenum bufferL = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(bufferL);
	camera->setReadBuffer(bufferL);

	// add this slave camera to the viewer, with a shift left of the projection matrix
    viewer.addSlave(camera.get());
	

    //HUD
    hud = new mb::Hud();
    hud->setScreenDimensions(traits->height, traits->width);

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