#include <MB/application.h>


Application::Application()
{
}


Application::~Application()
{
}

int Application::run()
{
	//Set the graphical context and related traits
	setGraphicsContext();

	//Place where all things to be drawed are registered.
	populateScene();

	//Start the render loop
	renderLoop();

	return 0;
}

void Application::renderLoop() {
	
	camManip = new osgGA::TrackballManipulator();
	viewer.setCameraManipulator(camManip);
	while (!viewer.done())
	{
		viewer.frame();
	}

}

void Application::populateScene() {

	box = new osg::Box(osg::Vec3(0, 0, 0), 1);
	boxShape = new osg::ShapeDrawable(box.get());
	boxGeode = new osg::Geode();
	boxGeode->addDrawable(boxShape.get());

	root = new osg::Group;
	root->addChild(boxGeode.get());
	viewer.setSceneData(root.get());
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
}