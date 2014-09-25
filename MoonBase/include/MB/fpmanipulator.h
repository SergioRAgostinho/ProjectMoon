//
//  fpmanipulator.h
//  MoonBase
//
//  Created by Neglective on 20/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_fpmanipulator_h
#define MoonBase_fpmanipulator_h

#ifdef WIN32
//In windows we can properly compile ODE
#ifndef dDOUBLE
#define dDOUBLE
#endif
#endif

#include <osgGA/StandardManipulator>
#include <osgViewer/GraphicsWindow>
#include <osgManipulator/Dragger>
#include <MB/body.h>

namespace mb {
    class FirstPersonManipulator : public osgGA::StandardManipulator {

        osg::Vec3d _eye;
        osg::Quat _rotation;



        osg::Vec3 _eyeGrab;
        osg::Vec3 _eyeDesiredGrab;
        osg::Quat _rotationGrab;

        osg::Vec2d _mouse;
        osg::Vec3d _mouvement;
        
        double translationFactor;
        double deltaTZ;
        double deltaRX;
        double deltaRY;
        
        double offsetScreen;

        //Collision requirements
        //dSpaceID pSpace;
        //dGeomID pGeom;
		osg::ref_ptr<mb::Body> body;
        bool revert;
        osg::Vec3d _revertEye;


        //brought in from mouse event
        osgManipulator::PointerInfo pointerInfo;
        osg::Vec2 screenCenter;
        osg::Camera* camera;
        std::vector<Body*> *selectableBodies;
        Body* selectedBody = nullptr;
        Body* grabbedBody = nullptr;
        Body* alignRef = nullptr;
        bool align = false;
		bool grabComplete = false;
        bool *selected, *active;
        int *inactiveCounter;


        //invoked to reposition the camera and to updated any grabbed object maintained with it
        void checkSelectables(osgViewer::View* view, const osgGA::GUIEventAdapter *ea);

		//Output the name and type of node in front of us
		void displayInfo(osgViewer::View* view, const osgGA::GUIEventAdapter *ea);

        void updateGrabbed();

        //Shared constructor 
		void sharedConstructor();

    public:

		FirstPersonManipulator(osg::Camera* cam);
        FirstPersonManipulator(osg::Camera* cam, std::vector<Body*> *b);
        ~FirstPersonManipulator();

        void setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation );
        void setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up );
        void getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const;
        void getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const;

        //Get the geometry id
        dGeomID getGeomID();

        //Get the geometry id
        Body* getGrabbedBody();

		//Get grabbedComplete status
		bool getGrabbedComplete();

        //Get position
        osg::Vec3 getPosition();

        //Get position
        osg::Quat getOrientation();

        //Init collision functionalities
        void initCollision(dSpaceID s);
        void initCollision(dSpaceID s, float colRadius);

		//Initialize physics
		void initPhysics(dWorldID world, dSpaceID space, BodyPhysicsMode mode = DEFAULT_BODY);
		void initPhysics(dWorldID world, dSpaceID space, double massAmount, BodyPhysicsMode mode = DEFAULT_BODY, double size = 1.f);

        //Check the status on the revert flage
        void armRevert(double x, double y, double z);

        //Process an armed revert
        void processRevert();

		//Set angular acceleration
		void setAngularAcceleration(double x, double y, double z);
		void setAngularAcceleration(osg::Vec3 aa);
		void setAngularAcceleration(dVector3 aa);

        /** set the position of the matrix manipulator using a 4x4 Matrix.*/
        void setByMatrix(const osg::Matrixd& matrix);

        /** set the position of the matrix manipulator using a 4x4 Matrix.*/
        void setByInverseMatrix(const osg::Matrixd& matrix);

        /** get the position of the manipulator as 4x4 Matrix.*/
        osg::Matrixd getMatrix() const;

        /** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
        osg::Matrixd getInverseMatrix() const;

        //update the record of the object position
        void updateGrabbedPos(osg::Vec3 pos);

		//ensures consistency between the body in the physical engine and its rendering.
		//it should be called after world step from the physics engine
		void update();

    protected:
        
        bool handle (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);
    };
}


#endif
