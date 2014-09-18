


#ifdef WIN32

#include <MB/humanmanipulator.h>
#include <MB/utils.hpp>
#include <osg/Texture2D>
#include <osg/MatrixTransform>
#include <osg/LineWidth>
#include <iostream>


using namespace mb;

HumanManipulator::HumanManipulator()
{
	_mode = DEFAULT;
	sharedInitialization();
}

HumanManipulator::HumanManipulator(osgViewer::Viewer* v, HumanManipulatorMode mode) {
	
	viewer = v;
	_mode = mode;
	sharedInitialization(mode);
}

void HumanManipulator::sharedInitialization(HumanManipulatorMode mode) {
	
	//Set all heap allocated member to nulls for proper clean-up control
	frame = nullptr;

	//Start Kinect sensor and object
	HRESULT hr = kinect.Initialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON);
	if (FAILED(hr))
		std::cerr << "Sensor was not properly initialized" << std::endl;

	//Activate seated mode, which is deactivated by default
	hr = kinect.ToggleSeatedMode();
	if (SUCCEEDED(hr))
		std::clog << "Activated seated mode" << std::endl;


	//Check if we really need to create the second viewer otherwise exit
	if (mode == DEFAULT || FAILED(hr))
		return;

	//Sets up the window, camera, quad and texture to properly visualize the camera feed
	setUpVideoWindow();

	//Set movement to nulls
	_movement.set(0, 0, 0);
}


HumanManipulator::~HumanManipulator()
{
	SafeReleaseArray(frame);
}

//Sets up the window, camera, quad and texture to properly visualize the camera feed
void HumanManipulator::setUpVideoWindow() {

	//Set frame data
	const unsigned int size = WIDTH*HEIGHT*CHANNELS;
	frame = new byte[size];
	memset(frame, 0xFF, size);

	image = new osg::Image;
	image->setDataVariance(osg::Object::DYNAMIC);
	image->setImage(WIDTH, HEIGHT, 1, GL_RGB8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, frame, osg::Image::NO_DELETE);


	texture = new osg::Texture2D;
	texture->setResizeNonPowerOfTwoHint(false);
	texture->setDataVariance(osg::Object::DYNAMIC);
	texture->setTextureSize(WIDTH, HEIGHT);
	texture->setImage(image);

	//Apply it to a quad
	osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry(osg::Vec3(0.0f, 0.0f, -0.1f),
		osg::Vec3(WIDTH, 0.0f, 0.0f), osg::Vec3(0.0f, HEIGHT, 0.0f), 0.0f, 1.0f, 1.0f, 0.0f);
	osg::StateSet *state = quad->getOrCreateStateSet();
	state->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

	//Add the quad to the geode
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	state = geode->getOrCreateStateSet();
	state->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->addDrawable(quad.get());

	//Set new window
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 1920 + 600;
	traits->y = 100;
	traits->width = WIDTH;
	traits->height = HEIGHT;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	//Create new graphics context (window)
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	//Set up camera to deploy hud
	camera = new osg::Camera;
	camera->setGraphicsContext(gc.get());
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, WIDTH, 0, HEIGHT));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setAllowEventFocus(false);
	camera->addChild(geode);

	// add this slave camera to the viewer, with a shift right of the projection matrix
	viewer->addSlave(camera.get(), false);

	//Create the skeleton figure
	createSkeletonDraw();
}

//Process Skeleton, ensure proper tracking, and rejection of additional elements
void HumanManipulator::processSkeleton() {

	bool activeSkeletonPresent = false;
	for (int  i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		NUI_SKELETON_TRACKING_STATE trackingState = s_frame.SkeletonData[i].eTrackingState;
		if (trackingState == NUI_SKELETON_TRACKED)
		{
			//store id if there's none
			if (!s_track_id) {
				s_track_id = s_frame.SkeletonData[i].dwTrackingID;
				s_idx = i;

				//Reset mask so it is visible again
				s_geode->setNodeMask(0xff);
			}

			//The original skeleton is being tracked
			if (s_track_id == s_frame.SkeletonData[i].dwTrackingID) {
				activeSkeletonPresent = true;

				Vector4 centre = s_frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER];
				_movement.set(centre.x, centre.y, centre.z);

			}
		}
	}

	//if the previous active skeleton is not there reset its id
	if (!activeSkeletonPresent) {
		s_track_id = 0;
		s_idx = -1;

		//Apply mask so it doesn't get rendered
		s_geode->setNodeMask(0x00);
	}
}

void HumanManipulator::createSkeletonDraw() {

	s_geode = new osg::Geode;
	s_geode->setDataVariance(osg::Object::DYNAMIC);
	s_geode->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(4.0f), osg::StateAttribute::ON);
	s_geode->setNodeMask(0x00);

	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;
	color->push_back(osg::Vec4(1.0, 0.0, 0.0, 1.0));

	for (short int i = 0; i < JOINTS; i++)
	{
		const osg::Vec2 pt = osg::Vec2(1, 1);
		const osg::Vec2 pt2 = osg::Vec2(10, 10);

		osg::ref_ptr<osg::Vec2Array> points = new osg::Vec2Array;
		vertex_ptr[i] = points;
		points->push_back(pt);
		points->push_back(pt2);

		osg::ref_ptr<osg::Geometry> line(new osg::Geometry);
		line->setVertexArray(points.get());
		line->setColorArray(color.get());
		line->setColorBinding(osg::Geometry::BIND_OVERALL);
		line->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, points->size()));
		line->setUseDisplayList(false);

		s_geode->addDrawable(line.get());
	}
	

	camera->addChild(s_geode.get());
}

//Handles the drawing of the skeleton
void HumanManipulator::updateSkeletonDraw() {
	
	//Draw Skeletal joints
	updateBoneDraw(s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_HEAD], s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER], 0);

	updateBoneDraw(s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER], s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT], 1);
	updateBoneDraw(s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT], s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT], 2);
	updateBoneDraw(s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT], s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_WRIST_LEFT], 3);
	updateBoneDraw(s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT], s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_WRIST_LEFT], 4);

	updateBoneDraw(s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER], s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT], 5);
	updateBoneDraw(s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT], s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT], 6);
	updateBoneDraw(s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT], s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT], 7);
	updateBoneDraw(s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT], s_frame.SkeletonData[s_idx].SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT], 8);
}

void HumanManipulator::updateBoneDraw(Vector4 skeletonPoint_1, Vector4 skeletonPoint_2, short unsigned int idx) {

	long x, y;
	unsigned short depth;

	// Calculate the skeleton's position on the screen
	// NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
	NuiTransformSkeletonToDepthImage(skeletonPoint_1, &x, &y, &depth);
	(*vertex_ptr[idx])[0].set((x * WIDTH) / 320.0, HEIGHT - (y * HEIGHT) / 240.0);  //update directly the vertex

	NuiTransformSkeletonToDepthImage(skeletonPoint_2, &x, &y, &depth);
	(*vertex_ptr[idx])[1].set((x * WIDTH) / 320.0, HEIGHT - (y * HEIGHT) / 240.0);

	//Flag the need for redraw
	s_geode->getDrawable(idx)->dirtyDisplayList();
}

////////////////////////////////////////////////////
//Required Implementations for successful inheritance
////////////////////////////////////////////////////

void HumanManipulator::setTransformation(const osg::Vec3d& eye, const osg::Quat& rotation){
	throw NotImplementedException();
}

void HumanManipulator::setTransformation(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up) {

	// set variables
	osg::Matrixd m(osg::Matrixd::lookAt(eye, center, up));
	_eye = eye;
	_rotation = m.getRotate().inverse();

}

void HumanManipulator::getTransformation(osg::Vec3d& eye, osg::Quat& rotation) const {
	throw NotImplementedException();
}


void HumanManipulator::getTransformation(osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up) const {
	throw NotImplementedException();
}

/** set the position of the matrix manipulator using a 4x4 Matrix.*/
void HumanManipulator::setByMatrix(const osg::Matrixd& matrix){

	// set variables
	_eye = matrix.getTrans();
	_rotation = matrix.getRotate();
};

/** set the position of the matrix manipulator using a 4x4 Matrix.*/
void HumanManipulator::setByInverseMatrix(const osg::Matrixd& matrix){
	throw NotImplementedException();
};

/** get the position of the manipulator as 4x4 Matrix.*/
osg::Matrixd HumanManipulator::getMatrix() const {
	throw NotImplementedException();
	return osg::Matrixd::identity();
};

/** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
osg::Matrixd HumanManipulator::getInverseMatrix() const{
	return osg::Matrixd::translate(-_eye) * osg::Matrixd::rotate(_rotation.inverse());
};


//The method invoked every time theres a new event to be processed, this being frame update
// or input devices 
bool HumanManipulator::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us){

	//Used ONLY to control when to move the camera around
	//static bool s_updated = false;

	//Try to get the skeleton
	const HRESULT s_hr = kinect.GetSkeletonFrame(&s_frame);
	if (SUCCEEDED(s_hr)) {
		processSkeleton();
		//s_updated = true;
	}

	//Only copy frames if there's an actual window
	if (_mode == DEBUG_WINDOW) {
		
		//Try to get new depth frame it there's one available
		const HRESULT d_hr = kinect.GetDepthFrame(frame);
		if (FAILED(d_hr) && d_hr != E_NUI_FRAME_NO_DATA)
			std::cerr << "Problems extracking the depth frame" << std::endl;

		//Only update on new image
		if (SUCCEEDED(d_hr)) 
			image->dirty();


		//Draw the skeleton joints on top of the image in case it is updated
		if (SUCCEEDED(s_hr) && s_track_id)
			updateSkeletonDraw();
	}

	
	//Traditional manipulator events
	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::FRAME:

		//if (s_updated) {
			_eye = _rotation * _movement;
			//s_updated = false;
		//}
		break;
	default:
		break;
	}

	return false;
}

#endif
//closes the protection against compilation in non win32 environments