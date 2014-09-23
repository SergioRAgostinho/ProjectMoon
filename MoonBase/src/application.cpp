
#include <osgGA/FirstPersonManipulator>
#include <osgViewer/View>
#include <MB/application.h>
#include <MB/keyboardeventhandler.h>
#include <MB/utils.hpp>
#include <MB/fpmanipulator.h>
#include <MB/findnodevisitor.h>


Application::Application(int argc, char* argv[]) :
n_bottles(20)
{
	//Parse console arguments if there any modifier
	modes = APP_MODE_STANDARD;
	if (argc > 1)
		parseConsoleArgument(argc, argv);

	//Initialize physics library as soon as possible
	dInitODE2(0);

	//model loader component
	loader = nullptr;

	//heads up display component
	hud = nullptr;

	//scene models
	root = nullptr;
	iss = nullptr;
	left_glove = nullptr;
	right_glove = nullptr;

	//to delete
	bottles = nullptr;

	//Manipulators
	man = nullptr;
	human = nullptr;
}


Application::~Application()
{
	//FIXME
	SafeRelease(hud);
	SafeReleaseArray(bottles);

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

void Application::parseConsoleArgument(int argc, char* argv[]) {



	//Control for invalid arguments
	bool invalid_argument = false;

	//Sweep argument list. 
	for (int i = 1; i < argc; i++)
	{
		std::string option(argv[i]);
		
		//Discard improperly constructed switches
		if (!option.compare("-debug")) {
			modes = APP_MODE_DEBUG;
			DEBUG_LOG("Debug window set up");
		}
		else if (!option.compare("-tv"))
		{
			modes |= APP_MODE_TV;
			DEBUG_LOG("TV mode set up");
		}
		else if (!option.compare("-stereo"))
		{
			modes |= APP_MODE_STEREO;
			DEBUG_LOG("Stereo mode set up");
		}
		else if (!option.compare("-mouse"))
		{
			modes |= APP_MODE_MOUSE;
			DEBUG_LOG("Mouse mode set up");
			if (modes & APP_MODE_DEBUG)
			{
				modes &= ~APP_MODE_DEBUG;
				DEBUG_WARNING("Disabling Kinect debug window");
			}
		}
		else
		{
			invalid_argument = true;
		}
	}

	//Output warning in case there were arguments passed incorrectly
	if (invalid_argument)
		DEBUG_WARNING("Some arguments could not be parsed");
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

	dGeomID camID;
	dGeomID l_hand = nullptr;
	dGeomID r_hand = nullptr;

	if (app->modes & APP_MODE_MOUSE)
	{
		camID = app->man->getGeomID();
	}
	else
	{
		camID = app->human->getGeomID();
		app->human->getHandsGeomIDs(&l_hand, &r_hand);
	}

	//Quick flags 
	const bool f_camera = (o1 == camID || o2 == camID);
	const bool f_hands = (o1 == l_hand || o2 == l_hand || o1 == r_hand || o2 == r_hand);

	//We need to exit here if the objects are both static and not cameras or hands
	if (!(b1 || b2 || f_camera || f_hands))
	{
		//Both geometries are static and none is a camera
		return;
	}

	const int N = 64;
	dContact contact[N];
	n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
	if (n > 0) {
        ////////////////////////////
        //Camera collision situation
        /////////////////////////////


        if (f_camera) {
			//Ignore collision between hands and camera
			if (f_hands)
			{
				return;
			}

			//Debug Code
			static unsigned long nr = 0;
			int class1 = dGeomGetClass(o1);
			int class2 = dGeomGetClass(o2);
			int isSpace1 = dGeomIsSpace(o1);
			int isSpace2 = dGeomIsSpace(o2);
			DEBUG_LOG("crashing against something " << nr++);

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
				depth *= 1.1;
				if (app->modes & APP_MODE_MOUSE)
				{
					app->man->armRevert((double)sign * depth * contact[iMaxDepth].geom.normal[0],
						(double)sign * depth * contact[iMaxDepth].geom.normal[1],
						(double)sign * depth * contact[iMaxDepth].geom.normal[2]);
				}
				else
				{
					app->human->armRevert((double)sign * depth * contact[iMaxDepth].geom.normal[0],
						(double)sign * depth * contact[iMaxDepth].geom.normal[1],
						(double)sign * depth * contact[iMaxDepth].geom.normal[2]);
				}

                return;
            }

		} 
		else if ((o1 == l_hand && o2 == r_hand ) || (o1 == r_hand && o2 == l_hand))
		{
			// Ignore collision between both hands
			return;
		}

#if 0
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
#endif

		

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
	dWorldSetGravity(pWorld, 0, 0, 0);
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
	if (modes & APP_MODE_MOUSE)
		man->setByMatrix(osg::Matrix::rotate(M_PI_2, 1, 0, 0));



    viewer.init();
    viewer.realize();
	while (!viewer.done())
	{
        //hide cursor for each frame (if you go out of the software the cursor will stay visible if you get back to the software)
        hideCursor();

        viewer.advance();
        viewer.eventTraversal();

		//Physics update
		dSpaceCollide(pSpace, (void*) this, &nearCallback);
		dWorldQuickStep(pWorld, stepSize);
		dJointGroupEmpty(pCollisionJG);

        //Revert the camera movement if needed
		if (modes & APP_MODE_MOUSE)
		{
			man->processRevert();
			//mb::Body* grabbedBody = man->getGrabbedBody();
			//if (grabbedBody && man->getGrabbedComplete())
			//	grabbedBody->processRevert();
		}
		else
		{
			human->processRevert();
		}
        


		//Update our objects
		for (size_t i = 0; i < n_bottles; i++)
		{
			bottles[i]->update();
		}

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

	//Load the ISS (tests)
	loader = new mb::Loader("../res/models/iss_int5.ive");
	osg::ref_ptr<osg::Node> iss_trans = dynamic_cast<osg::Node*>(loader->getNode<osg::MatrixTransform>());
	iss = new mb::Group(iss_trans.get());
	iss->getPAT()->setPivotPoint(osg::Vec3(0, -19.4, -0.4));
	iss->setCollisionSpace(pSpace);
	iss->setCollisionBoundingBox(-1, 1, -3.75, 2.6, -1.4, .6);
	iss->setAttitude(osg::Quat(M_PI, osg::Vec3(0, 0, 1)));
	root->addChild(iss->getPAT());

	//Disable astro man
	loader->getNode<osg::Group>("o1")->getParent(0)->setNodeMask(0x00);

	//Dump bottles inside the ISS
	loader = new mb::Loader("../res/models/muscatel.osgt");
	bottles = new osg::ref_ptr<mb::Body>[n_bottles];
	for (unsigned int i = 0; i < n_bottles; i++)
	{
		if (!i)
		{
			bottles[0] = new mb::Body(loader->getNode<osg::Geode>("pCylinder1-GEODE"));
			bottles[0]->getPAT()->setScale(osg::Vec3(0.025, 0.025, 0.025));
			bottles[0]->initPhysics(pWorld, pSpace, 2, mb::BOUNDING_BOX);
		}
		else
		{
			bottles[i] = bottles[0]->clone();
		}

		//Random position
		bottles[i]->setPosition(mb::uniRand(-0.9, 0.9), mb::uniRand(-2.6, 3.65), mb::uniRand(-1.3, .5));
		//random linear velocity
		bottles[i]->setLinearVelocity(mb::uniRand(-1, 1), mb::uniRand(-1, 1), mb::uniRand(-1, 1));
		root->addChild(bottles[i]->getPAT());
	}

	//Needed to be brought here because the manipulator needs to be initialized when the selected object list is already set
	//Camera manipulator
	if (modes & APP_MODE_MOUSE)
	{
		man = new mb::FirstPersonManipulator(camera.get());
		man->initCollision(pSpace, 0.1f);
	}
	else if (modes & APP_MODE_DEBUG)
	{
		//The new human camera manipulator
		human = new mb::HumanManipulator(&viewer, mb::HumanManipulatorMode::DEBUG_WINDOW);
		human->initCollision(pSpace, 0.1f);
	}
	else
	{
		//The new human camera manipulator
		human = new mb::HumanManipulator(&viewer, mb::HumanManipulatorMode::DEFAULT);
		human->initCollision(pSpace, 0.1f);
	}
	
	loader = new mb::Loader("../res/models/astronautgloveleft.osgt");
	mb::FindNodeVisitor<osg::Geode> nv;
	loader->getNode()->accept(nv);
	left_glove = new mb::Body(nv.getFirst());
	left_glove->setScale(osg::Vec3(.01, .01, .01));
	left_glove->initCollision(pSpace, mb::BOUNDING_BOX);
	if (human) {
		root->addChild(human->populateBodyModels(left_glove.get(), mb::HumanManipulatorBodyPart::LEFT_HAND));
	}
	else
	{
		loader->getPAT()->setPosition(osg::Vec3(-0.06, 0.4, -0.1));
		root->addChild(loader->getPAT());
	}


	loader = new mb::Loader("../res/models/astronautgloveright.osgt");
	nv = mb::FindNodeVisitor<osg::Geode>();
	loader->getNode()->accept(nv);
	right_glove = new mb::Body(nv.getFirst());
	right_glove->setScale(osg::Vec3(.01, .01, .01));
	right_glove->initCollision(pSpace, mb::BOUNDING_BOX);
	if (human) {
		root->addChild(human->populateBodyModels(right_glove.get(), mb::HumanManipulatorBodyPart::RIGHT_HAND));
	}
	else
	{
		right_glove->setPosition(osg::Vec3(0.06, 0.4, -0.1));
		root->addChild(right_glove->getPAT());
	}

	//Add full tree to scene
	viewer.setSceneData(root.get());
	
	//Subscribe object
	if (modes & APP_MODE_MOUSE)
	{
		viewer.setCameraManipulator(man);
	}
	else
	{
		viewer.setCameraManipulator(human);
	}
	viewer.addEventHandler(new mb::KeyboardEventHandler(this));
}

void Application::setGraphicsContext() {

	//Get information on the screen set-up
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	osg::GraphicsContext::ScreenIdentifier si;
	si.readDISPLAY();
	osg::GraphicsContext::ScreenSettings settings;
	int screen_id = -1;


	if (modes & APP_MODE_TV) {
		int n_screens = wsi->getNumScreens(si);

		//The tv has a vertical resolution of 1080
		for (int i = 0; i < n_screens; i++)
		{
			wsi->getScreenSettings(osg::GraphicsContext::ScreenIdentifier(i), settings);
			if (settings.height == 1080) {
				//Most likely our active TV
				screen_id = i;
				break;
			}
		}
	}
	else
	{
		screen_id = 0;
		wsi->getScreenSettings(si, settings);
	}

	traits = new osg::GraphicsContext::Traits;
	traits->x = 0;
	traits->y = 0;
	traits->width = settings.width;
	traits->height = settings.height;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->useCursor = false;
	traits->overrideRedirect = true;

	gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	camera = viewer.getCamera();
	camera->setGraphicsContext(gc.get());
	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(buffer);
	camera->setReadBuffer(buffer);

	viewer.setUpViewOnSingleScreen(screen_id);

	//Activate stereo
	if (modes & APP_MODE_STEREO) {
		osg::ref_ptr<osg::DisplaySettings> disp_settings = osg::DisplaySettings::instance();
		disp_settings->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
		disp_settings->setEyeSeparation(0.003);
		disp_settings->setStereo(true);
	}

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