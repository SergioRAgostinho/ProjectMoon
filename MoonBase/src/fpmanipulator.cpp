//
//  fpmanipulator.cpp
//  MoonBase
//
//  Created by Neglective on 20/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <MB/fpmanipulator.h>
#include <iostream>
#include <osgUtil/LineSegmentIntersector>
#include <osgViewer/View>
#include <MB/utils.hpp>


using namespace mb;

FirstPersonManipulator::FirstPersonManipulator(osg::Camera* cam) : camera(cam) {

	translationFactor = 0.04;
	_mouvement.set(0, 0, 0);
	deltaTZ = 0;
	deltaRX = 0;
	deltaRY = 0;
#ifdef WIN32 
	offsetScreen = 150.0;
#else
	offsetScreen = 50.0;
#endif

	screenCenter.set(0, 0);


	selectableBodies = nullptr;
	selected = nullptr;
	active = nullptr;
	inactiveCounter = nullptr;

	revert = false;
}

FirstPersonManipulator::FirstPersonManipulator(osg::Camera* cam, std::vector<Body*> *b) : camera(cam) ,selectableBodies(b) {

    translationFactor = 0.04;
    _mouvement.set(0,0,0);
    deltaTZ = 0;
    deltaRX = 0;
    deltaRY = 0;
#ifdef WIN32 
	offsetScreen = 150.0;
#else
    offsetScreen = 50.0;
#endif

    screenCenter.set(0, 0);

    selected = new bool[selectableBodies->size()]();
    memset(selected,0,selectableBodies->size()*sizeof(bool));
    active = new bool[selectableBodies->size()]();
    memset(active,0,selectableBodies->size()*sizeof(bool));
    inactiveCounter = new int[selectableBodies->size()]();
    memset(inactiveCounter,0,selectableBodies->size()*sizeof(int));

    revert = false;
}

FirstPersonManipulator::~FirstPersonManipulator() {

	//*selectableBodies is a variable pre-allocade outside the scope of this class and therefore 
	// we should not release its memory here

    if (selected) {
            delete [] selected;
			selected = nullptr;
    }

    if (active) {
            delete [] active;
			active = nullptr;
    }

    if (inactiveCounter) {
            delete [] inactiveCounter;
			inactiveCounter = nullptr;
    }

}


bool FirstPersonManipulator::handle (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us){

    static bool grabbed = false;
    static osg::Quat corr = osg::Matrix::rotate(M_PI/2.0, 1, 0, 0).getRotate();

    bool frameEvent = false;
    switch (ea.getEventType()) {
        // at frame rendering, GUIEventAdapter::FRAME is thrown here, so we apply detected movements between two frames at this moment. When a frame is not being rendered, we save all movements detected in "_mouvement" et deltaTZ for translation,deltaRX and deltaRY for rotation of the camera.

        case osgGA::GUIEventAdapter::FRAME :{

            _eye+= _rotation * _mouvement;
            _eye += osg::Vec3d(0,0,deltaTZ);
			if (pGeom)
			{
				dGeomSetPosition(pGeom, (dReal)_eye.x(), (dReal)_eye.y(), (dReal)_eye.z());
			}
            rotateYawPitch(_rotation, deltaRX * 0.001, deltaRY * 0.001, osg::Vec3(0,0,1));
            deltaTZ = 0;
            deltaRX = 0;
            deltaRY = 0;
            frameEvent = true;

			//DEBUG_LOG("x: " << _eye.x() << " y: " << _eye.y() << " z: " << _eye.z());
            break;
        }
            
        //handling keyboard events
        case osgGA::GUIEventAdapter::KEYDOWN :
        {
            switch (ea.getKey()) {
                case 'w':
                case 'W':
                    _mouvement.set(_mouvement.x(), _mouvement.y(),-translationFactor);
                    break;
                case 'a':
                case 'A':
                    _mouvement.set(-translationFactor, _mouvement.y(),_mouvement.z());
                    break;
                case 's':
                case 'S':
                    _mouvement.set(_mouvement.x(), _mouvement.y(),translationFactor);
                    break;
                case 'd':
                case 'D':
                    _mouvement.set(translationFactor, _mouvement.y(),_mouvement.z());
                    break;
            }
			//std::cout << "X_eye: " << _eye.x() 
			//	<< "Y_eye: " << _eye.y() 
			//	<< "Z_eye: " << _eye.z() << std::endl;
            break;
        }
        case osgGA::GUIEventAdapter::KEYUP :
        {
            switch (ea.getKey()) {
                case 'w':
                case 'W':
                    if(_mouvement.z()==-translationFactor)
                        _mouvement.set(_mouvement.x(), _mouvement.y(),0);
                    break;
                case 'a':
                case 'A':
                    if(_mouvement.x()==-translationFactor)
                        _mouvement.set(0, _mouvement.y(),_mouvement.z());
                    break;
                case 's':
                case 'S':
                    if(_mouvement.z()==translationFactor)
                        _mouvement.set(_mouvement.x(), _mouvement.y(),0);
                    break;
                case 'd':
                case 'D':
                    if(_mouvement.x()==translationFactor)
                        _mouvement.set(0, _mouvement.y(),_mouvement.z());
                    break;
            }
        }
            break;
        //handling mouse movement events
        case osgGA::GUIEventAdapter::DRAG:
        case osgGA::GUIEventAdapter::MOVE:
        {

            static bool doXPositionChange = false;
            static bool doYPositionChange = false;
            static double newReqXPosition = 0;
            static double newReqYPosition = 0;
            static double varX = 0;
            static double varY = 0;

            if (!(doXPositionChange && (std::abs(ea.getX() - _mouse.x()) >=  std::abs(ea.getX() - newReqXPosition)))) {
                    varX = ea.getX() - _mouse.x();
            } else {
                //it warped
                doXPositionChange = false;
            }

            if (!(doYPositionChange && (std::abs(ea.getY() - _mouse.y()) >=  std::abs(ea.getY() - newReqYPosition)))) {
                varY = ea.getY() - _mouse.y();
            } else {
                //it warped
                doYPositionChange = false;
            }

            deltaRX += varX;
            deltaRY += varY;
            _mouse.set(ea.getX(), ea.getY());


            double newMouseXPosition = ea.getX();
            double newMouseYPosition = ea.getY();
                
            if(ea.getX()<=offsetScreen){
                doXPositionChange = true;
                newMouseXPosition = ea.getWindowWidth()- 2*offsetScreen + ea.getX();
            } else if(ea.getX()>(ea.getWindowWidth()-offsetScreen)){
                doXPositionChange = true;
                newMouseXPosition = ea.getX() - ea.getWindowWidth() + 2*offsetScreen;
            }

            if(ea.getY()<=offsetScreen){
                doYPositionChange = true;
                newMouseYPosition = ea.getWindowHeight()- 2*offsetScreen + ea.getY();
            } else if(ea.getY()>(ea.getWindowHeight()-offsetScreen)){
                doYPositionChange = true;
                newMouseYPosition = ea.getY() - ea.getWindowHeight() + 2*offsetScreen;
            }

            if(doXPositionChange || doYPositionChange){

                newReqXPosition = newMouseXPosition;
                newReqYPosition = newMouseYPosition;
#ifdef WIN32 
				us.requestWarpPointer(newMouseXPosition, newMouseYPosition);
#else
                us.requestWarpPointer(newMouseXPosition, ea.getWindowHeight() - newMouseYPosition);
#endif
            }

            break;
        }
        case osgGA::GUIEventAdapter::SCROLL :
        {
			if (ea.getScrollingDeltaY())
				deltaTZ += 0.4 * translationFactor * ea.getScrollingDeltaY();
			else {
				switch (ea.getScrollingMotion())
				{
				case osgGA::GUIEventAdapter::SCROLL_UP:
					deltaTZ += 1.6 * translationFactor;
					break;
				case osgGA::GUIEventAdapter::SCROLL_DOWN:
					deltaTZ -= 1.6 * translationFactor;
					break;
				}
			}
            break;
        }
        case osgGA::GUIEventAdapter::PUSH: {
            if (selectedBody) {
                grabbed = true;
                grabbedBody = selectedBody;
                grabbedBody->setAngularVelocity(0, 0, 0);

                _rotationGrab =  grabbedBody->getOrientationQuat() * (corr * _rotation).conj();
            }
            break;
        }
        case osgGA::GUIEventAdapter::RELEASE:
            if (grabbedBody && grabbed) {
                grabbed = false;
                align = false;
				grabComplete = false;

                double lv[3];
                grabbedBody->getLinearVelocity(&lv[0]);
                grabbedBody->setLinearVelocity(lv[0]*0.1, lv[1]*0.1, lv[2]*0.1);
                grabbedBody->enablePBody();
                grabbedBody = nullptr;
                alignRef = nullptr;
            }
            break;
        default:
            break;
    }

    //update the screen center
    if (!frameEvent) {
        screenCenter.set(ea.getWindowWidth()>>1,ea.getWindowHeight()>>1);
    }


    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&us);
    if (view && selectableBodies && selectableBodies->size() > 0) {
        checkSelectables(view, &ea);
    }

    if (grabbedBody && grabbed) {
        updateGrabbed();
        static osg::Quat q_final;
        static osg::Quat q_init;
        static osg::Quat q;

        if(alignRef) {

            double dist = (grabbedBody->getPosition() - alignRef->getPosition()).length();
            if (dist > 50) {
                alignRef = nullptr;
                align = false;
                q = q_final = q_init = osg::Quat();
            } else if (dist > 48 && dist <= 50) {
                q.slerp(0, q_init, q_final);
                grabbedBody->setAttitude(q);
            } else if (dist > 46 && dist <= 48) {
                q.slerp(0.2, q_init, q_final);
				grabbedBody->setAttitude(q);
            } else if (dist > 44 && dist <= 46) {
                q.slerp(0.4, q_init, q_final);
				grabbedBody->setAttitude(q);
            } else if (dist > 42 && dist <= 44) {
                q.slerp(0.6, q_init, q_final);
				grabbedBody->setAttitude(q);
            } else if (dist > 40 && dist <= 42) {
                q.slerp(0.8, q_init, q_final);
				grabbedBody->setAttitude(q);
            } else {
                grabbedBody->setAttitude(q);
            }


        } else {
            for (auto bd : *selectableBodies) {
                if (bd != grabbedBody && (grabbedBody->getPosition() - bd->getPosition()).length() <= 50) {

                    q_init = grabbedBody->getOrientationQuat();
                    q_final = grabbedBody->align(bd);
                    alignRef = bd;
                    align = true;
                    break;
                }
            }
        }
    }
//    else {
//        alignRef = nullptr;
//        align = false;
//    }

    return false;
}

void FirstPersonManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation ){

    std::cout << "mouse set A transform" << std::endl;
}

void FirstPersonManipulator::setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up ) {

    // set variables
    osg::Matrixd m( osg::Matrixd::lookAt( eye, center, up ) );
    _eye = eye;
	if (pGeom)
		dGeomSetPosition(pGeom, (dReal) _eye.x(), (dReal) _eye.y(), (dReal) _eye.z());
    _rotation = m.getRotate().inverse();

    //Fix vertical something

}

void FirstPersonManipulator::getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const {
    std::cout << "mouse get A transform" << std::endl;
}


void FirstPersonManipulator::getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const {
    std::cout << "mouse get B transform" << std::endl;
}

//Get the geometry id
dGeomID FirstPersonManipulator::getGeomID() {
    return pGeom;
}

//Get the geometry id
Body* FirstPersonManipulator::getGrabbedBody() {
    return grabbedBody;
}

//Get grabbedComplete status
bool FirstPersonManipulator::getGrabbedComplete() {
	return grabComplete;
}

//Get position
osg::Vec3 FirstPersonManipulator::getPosition() {
    return _eye;
}

//Get position
osg::Quat FirstPersonManipulator::getOrientation() {
    return _rotation;
}

//Init collision functionalities
void FirstPersonManipulator::initCollision(dSpaceID s) {
    initCollision(s, 20);
}
void FirstPersonManipulator::initCollision(dSpaceID s, float colRadius) {
    pSpace = s;
    pGeom = dCreateSphere(pSpace, (dReal) colRadius);
}

//Check the status on the revert flag
void FirstPersonManipulator::armRevert(double x, double y, double z) {
    _revertEye += osg::Vec3(x,y,z);
    revert = true;
}

//Process an armed revert
void FirstPersonManipulator::processRevert() {
    if (!revert) {
        return;
    }

    _eye += _revertEye;
    dGeomSetPosition(pGeom, (dReal) _eye.x(), (dReal) _eye.y(), (dReal) _eye.z());
    _revertEye.set(0,0,0);
    revert = false;

}

/** set the position of the matrix manipulator using a 4x4 Matrix.*/
void FirstPersonManipulator::setByMatrix(const osg::Matrixd& matrix){

    // set variables
    _eye = matrix.getTrans();
	if (pGeom)
		dGeomSetPosition(pGeom, (dReal) _eye.x(), (dReal) _eye.y(), (dReal) _eye.z());
    _rotation = matrix.getRotate();

    //set the mouse to the center of screen
    _mouse = osg::Vec2d(_mouseCenterX,_mouseCenterY);

    //Fix vertical something
};

/** set the position of the matrix manipulator using a 4x4 Matrix.*/
void FirstPersonManipulator::setByInverseMatrix(const osg::Matrixd& matrix){
    std::cout << "mouse set by inverse matrix" << std::endl;
};

/** get the position of the manipulator as 4x4 Matrix.*/
osg::Matrixd FirstPersonManipulator::getMatrix() const {
    std::cout << "mouse get matrix" << std::endl;
    return osg::Matrixd::identity();
};

/** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
osg::Matrixd FirstPersonManipulator::getInverseMatrix() const{
    return osg::Matrixd::translate( -_eye ) * osg::Matrixd::rotate(  _rotation.inverse() );
};

//invoked to reposition the camera and to updated any grabbed object maintained with it
void FirstPersonManipulator::checkSelectables(osgViewer::View* view,const osgGA::GUIEventAdapter *ea) {

    osgUtil::LineSegmentIntersector::Intersections intersections;
    pointerInfo.reset();

    //Verify if there are intersections
    memset(selected,0,selectableBodies->size()*sizeof(bool));

    if (view->computeIntersections(screenCenter.x(), screenCenter.y(), intersections)) {
        pointerInfo.setCamera(camera);
        pointerInfo.setMousePosition(ea->getWindowWidth()>>1, ea->getWindowHeight()>>1);


        for (osgUtil::LineSegmentIntersector::Intersections::iterator interIter = intersections.begin(); interIter != intersections.end(); ++interIter) {
            pointerInfo.addIntersection(interIter->nodePath, interIter->getLocalIntersectPoint());
        }



        bool mustBreak = false;

        for (auto &it : pointerInfo._hitList) {
            for (osg::NodePath::iterator npIter = it.first.begin(); npIter != it.first.end(); ++npIter) {

                if (osg::Geode* geo = dynamic_cast<osg::Geode*>(*npIter)) {


                    for(unsigned int i = 0 ; i < selectableBodies->size(); ++i) {
                        if((*selectableBodies)[i]->getGeode() == geo) {

//                            if(((*selectableBodies)[i]->getPosition() - _eye).length() <= 200) {
                                (*selectableBodies)[i]->activateBB();
                                selected[i] = true;
                                inactiveCounter[i] = 0;
                                active[i] = true;
                                mustBreak = true;
                                selectedBody = (*selectableBodies)[i];
//                            }
                            break;
                        }
                    }

                    if (mustBreak)
                        break;

                }
            }

            if (mustBreak)
                break;
        }

        //no break means no selectable body found
        if(!mustBreak)
            selectedBody = nullptr;

    }

    for(unsigned int i = 0; i < selectableBodies->size(); ++i) {
        if(!selected[i] && active[i]) {
            if(++inactiveCounter[i] > 0) {
                (*selectableBodies)[i]->removeBB();
                active[i] = false;
            }
        }
    }
}

void FirstPersonManipulator::updateGrabbed() {
    //Compute the relative distance
    osg::Vec3 pos = _eye + _rotation * osg::Vec3(0,0,-30);

    _eyeGrab.set(grabbedBody->getPosition());

    //Fix it once it's close
	bool isClose = (pos - _eyeGrab).length() < 2 ;
	if (isClose) {
		grabComplete = true;
	}

    if(!grabbedBody->getRevert() && (!grabbedBody->isPBodyEnabled() || isClose )) {
        grabbedBody->disablePBody();
        grabbedBody->setPosition(pos.x(), pos.y(), pos.z());
        grabbedBody->setLinearVelocity(0, 0, 0);

		
    } else {
        //Gravity gun style
        float powerfactor = 4;
        float maxVel = 800;

        if(grabbedBody->getRevert() && grabComplete) {
            grabbedBody->enablePBody();
            grabbedBody->setRevert(false);
			powerfactor = 0.01;
			maxVel = 1;
        }

        osg::Vec3 v = pos - _eyeGrab;
        v *= powerfactor;

        if ( v.length() > maxVel )
        {
            // if the correction-velocity is bigger than maximum
            v.normalize();
            v *= maxVel;
        }
        grabbedBody->setLinearVelocity(v.x(), v.y(), v.z());
    }
    grabbedBody->setAngularVelocity(0, 0, 0);

    if(!align) {
		static osg::Quat corr = osg::Quat(M_PI / 2.0, osg::Vec3(1, 0, 0));
		grabbedBody->setAttitude(_rotationGrab * corr * _rotation);
    }
}

//update the record of the object position
void FirstPersonManipulator::updateGrabbedPos(osg::Vec3 pos) {
    _eyeGrab.set(pos);
}

