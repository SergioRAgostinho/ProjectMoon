#pragma once

#ifdef WIN32 

#ifndef MoonBase_humanmanipulator_h
#define MoonBase_humanmanipulator_h


//In windows we can properly compile ODE
#ifndef dDOUBLE
#define dDOUBLE
#endif


#include <ode/ode.h>
#include <osgViewer/Viewer>
#include <osgGA/StandardManipulator>
#include <Kinect/sensor.h>
#include <MB/body.h>


//Kinect image specs
#define HEIGHT 480
#define WIDTH  640
#define CHANNELS 4

//Kinect rendered joints
#define JOINTS 9

namespace mb {

	//Initiliaziation mods
	enum HumanManipulatorMode
	{
		DEFAULT, DEBUG_WINDOW
	};

	enum HumanManipulatorBodyPart
	{
		LEFT_HAND, RIGHT_HAND
	};

	class HumanManipulator :
		public osgGA::StandardManipulator
	{
		

		//Camera pose
		osg::Vec3d _eye;
		osg::Quat _rotation;

		//Movement related
		osg::Vec3d _movement;

		//Kinect sensor object
		Kinect::Sensor kinect;
		osg::Matrix k_trans;
		const osg::Vec3 k_pos;
		const float k_pitch;
		const float k_yaw;

		//Collision related
		//body
		dSpaceID pSpace;
		dGeomID pGeom;
		bool revert;
		osg::Vec3d _revertEye;

		//Skeleton frame, tracking id, array index and the necessary pointers to update the 
		//skeletal rendering efficiently
		NUI_SKELETON_FRAME s_frame;
		unsigned long s_track_id;
		short int s_idx;
		osg::ref_ptr<osg::Geode> s_geode;
		osg::Vec2Array* vertex_ptr[JOINTS];

		//Body members (pun intended)
		//osg::ref_ptr<osg::PositionAttitudeTransform> l_hand, r_hand;
		osg::ref_ptr<mb::Body> l_hand, r_hand;
		osg::Vec3 l_hand_in_cam, r_hand_in_cam;
		osg::Quat l_hand_quat_in_cam, r_hand_quat_in_cam, c_quat_in_cam;

		//Hand representation sensitivity parameters
		const float front_offset, front_scale;
		const float lat_offset, lat_scale;
		const float vert_offset, vert_scale;

		//Kinnect video feed window related
		byte *frame;
		osg::ref_ptr<osg::Image> image;
		osg::ref_ptr<osg::Texture2D> texture;

		//Members required for creating and rendering on the skeleton window
		osgViewer::Viewer *viewer;
		osg::ref_ptr<osg::Camera> camera;

		//Status control
		HumanManipulatorMode _mode;


		//METHODS

		//Shared initialization method
		void sharedInitialization(HumanManipulatorMode mode = DEFAULT);

		//Sets up the window, camera, quad and texture to properly visualize the camera feed
		void setUpVideoWindow();

		//Process Skeleton, ensure proper tracking, and rejection of additional elements
		void processSkeleton();

		//Handles the drawing of the skeleton
		void createSkeletonDraw();
		void updateSkeletonDraw();
		void updateBoneDraw(Vector4 skeletonPoint_1, Vector4 skeletonPoint_2, short unsigned int idx);

	public:

		HumanManipulator();
		HumanManipulator(osgViewer::Viewer* v, HumanManipulatorMode mode);

		~HumanManipulator();

		//Provide reference to geom object
		dGeomID getGeomID();
		dGeomID getLeftHandGeomID();
		dGeomID getRightHandGeomID();
		void getHandsGeomIDs(dGeomID *l, dGeomID *r);

		//Populate the scene with the models derived 
		//osg::PositionAttitudeTransform* populateBodyModels(osg::Node *model, HumanManipulatorBodyPart bodyPart);
		osg::PositionAttitudeTransform* populateBodyModels(Body *body, HumanManipulatorBodyPart bodyPart);

		//Init collision functionalities
		void initCollision(dSpaceID s);
		void initCollision(dSpaceID s, float colRadius);
		void initHandCollision(HumanManipulatorBodyPart hand);

		//Check the status on the revert flage
		void armRevert(double x, double y, double z);

		//Process an armed revert
		void processRevert();

		////////////////////////////////////////////////////
		//Required declarations for successful inheritance
		////////////////////////////////////////////////////

		void setTransformation(const osg::Vec3d& eye, const osg::Quat& rotation);
		void setTransformation(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);
		void getTransformation(osg::Vec3d& eye, osg::Quat& rotation) const;
		void getTransformation(osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up) const;

		/** set the position of the matrix manipulator using a 4x4 Matrix.*/
		void setByMatrix(const osg::Matrixd& matrix);

		/** set the position of the matrix manipulator using a 4x4 Matrix.*/
		void setByInverseMatrix(const osg::Matrixd& matrix);

		/** get the position of the manipulator as 4x4 Matrix.*/
		osg::Matrixd getMatrix() const;

		/** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
		osg::Matrixd getInverseMatrix() const;

	protected:

		//The method invoked every time theres a new event to be processed, this being frame update
		// or input devices 
		bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);
	};



}

#endif
//finishes defining the human manipulator

#endif
//closes the protection against compilation in non win32 environments