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


using namespace mb;

FirstPersonManipulator::FirstPersonManipulator(osg::Camera* cam, std::vector<Body*> *b) : camera(cam) ,selectableBodies(b) {

    translationFactor = 2;
    _mouvement.set(0,0,0);
    deltaTZ = 0;
    deltaRX = 0;
    deltaRY = 0;
    offsetScreen = 50.0;

    screenCenter.set(0, 0);

    selected = new bool[selectableBodies->size()]();
    memset(selected,0,selectableBodies->size()*sizeof(bool));
    active = new bool[selectableBodies->size()]();
    memset(active,0,selectableBodies->size()*sizeof(bool));
    inactiveCounter = new int[selectableBodies->size()]();
    memset(inactiveCounter,0,selectableBodies->size()*sizeof(int));
}

FirstPersonManipulator::~FirstPersonManipulator() {
    if (selected) {
            delete [] selected;
    }

    if (active) {
            delete [] active;
    }

    if (inactiveCounter) {
            delete [] inactiveCounter;
    }

}


bool FirstPersonManipulator::handle (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us){

//    static int counter = 0;
//    std::cout << " MANIP: " << counter++ << std::endl;

    static bool grabbed = false;

    bool frameEvent = false;
    switch (ea.getEventType()) {
        // at frame rendering, GUIEventAdapter::FRAME is thrown here, so we apply detected movements between two frames at this moment. When a frame is not being rendered, we save all movements detected in "_mouvement" et deltaTZ for translation,deltaRX and deltaRY for rotation of the camera.

        case osgGA::GUIEventAdapter::FRAME :{
            
            _eye+= _rotation * _mouvement;
            _eye += osg::Vec3d(0,0,deltaTZ);
            rotateYawPitch(_rotation, deltaRX * 0.001, deltaRY * 0.001, osg::Vec3(0,0,1));
            deltaTZ = 0;
            deltaRX = 0;
            deltaRY = 0;
            frameEvent = true;


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
                us.requestWarpPointer(newMouseXPosition, ea.getWindowHeight() - newMouseYPosition);
            }

            break;
        }
        case osgGA::GUIEventAdapter::SCROLL :
        {
            deltaTZ += ea.getScrollingDeltaY();
            break;
        }
        case osgGA::GUIEventAdapter::PUSH: {
            if (selectedBody) {
                grabbed = true;
                selectedBody->setLinearVelocity(0, 0, 0);
                selectedBody->setAngularVelocity(0, 0, 0);
                selectedBody->disablePBody();

                static osg::Quat corr = osg::Matrix::rotate(M_PI/2.0, 1, 0, 0).getRotate();
                _rotationGrab =  selectedBody->getOrientationQuat() * (corr * _rotation).conj();
//                _rotationGrab = osg::Quat(); 
            }
            break;
        }
        case osgGA::GUIEventAdapter::RELEASE:
            if (selectedBody && grabbed) {
                grabbed = false;
                selectedBody->enablePBody();
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
    if (view) {
        checkSelectables(view, &ea);
    }
    if (selectedBody && grabbed) {
        updateGrabbed();
    }


    return false;
}

void FirstPersonManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation ){

    std::cout << "mouse set A transform" << std::endl;
}

void FirstPersonManipulator::setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up ) {

    // set variables
    osg::Matrixd m( osg::Matrixd::lookAt( eye, center, up ) );
    _eye = eye;
    _rotation = m.getRotate().inverse();

    //Fix vertical something

}

void FirstPersonManipulator::getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const {
    std::cout << "mouse get A transform" << std::endl;
}


void FirstPersonManipulator::getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const {
    std::cout << "mouse get B transform" << std::endl;
}

//Get position
osg::Vec3 FirstPersonManipulator::getPosition() {
    return _eye;
}

//Get position
osg::Quat FirstPersonManipulator::getOrientation() {
    return _rotation;
}

/** set the position of the matrix manipulator using a 4x4 Matrix.*/
void FirstPersonManipulator::setByMatrix(const osg::Matrixd& matrix){

    // set variables
    _eye = matrix.getTrans();
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


                    for( int i = 0 ; i < selectableBodies->size(); ++i) {
                        if((*selectableBodies)[i]->getGeode() == geo) {
                            (*selectableBodies)[i]->activateBB();
                            selected[i] = true;
                            inactiveCounter[i] = 0;
                            active[i] = true;
                            mustBreak = true;
                            selectedBody = (*selectableBodies)[i];
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

    for(int i = 0; i < selectableBodies->size(); ++i) {
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
    selectedBody->setPosition(pos.x(), pos.y(), pos.z());

    static osg::Quat corr = osg::Matrix::rotate(M_PI/2.0, 1, 0, 0).getRotate();
    //osg::Matrix::rotate(M_PI/2.0, 1, 0, 0 ).getRotate();
//    osg::Matrix q;
//    _rotation.get(q);
//    osg::Matrix qBody = selectedBody->getOrientationMat();
//    q.set(qBody);

    osg::Quat q1 = _rotationGrab * corr * _rotation ;
    selectedBody->setOrientationQuat(q1.x(), q1.y(), q1.z(), q1.w());
}

