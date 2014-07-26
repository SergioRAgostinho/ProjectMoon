//
//  fpmanipulator.cpp
//  MoonBase
//
//  Created by Neglective on 20/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <MB/fpmanipulator.h>
#include <iostream>

using namespace mb;

FirstPersonManipulator::FirstPersonManipulator(){
    //_mouse.set(450, 450);
    translationFactor = 2;
    _mouvement.set(0,0,0);
    deltaTZ = 0;
    deltaRX = 0;
    deltaRY = 0;
    offsetScreen = 50.0;
    blockUpdateForNextCall = false;
}

bool FirstPersonManipulator::handle (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us){
    
// at frame rendering, GUIEventAdapter::FRAME is thrown here, so we apply detected movements between two
// frames at this moment. When a frame is not being rendered, we save all movements detected in
// "_mouvement" et deltaTZ for translation,deltaRX and deltaRY for rotation of the camera.
    
    switch (ea.getEventType()) {
            
        case osgGA::GUIEventAdapter::FRAME :
            
            _eye+= _rotation * _mouvement;
            _eye += osg::Vec3d(0,0,deltaTZ);
            rotateYawPitch(_rotation, deltaRX * 0.001, deltaRY * 0.001, osg::Vec3(0,0,1));
            deltaTZ = 0;
            deltaRX = 0;
            deltaRY = 0;
            
            break;
            
        //handling keyboard events
        case osgGA::GUIEventAdapter::KEYDOWN :
        {
            switch (ea.getKey()) {
                case 'z':
                case 'Z':
                    _mouvement.set(_mouvement.x(), _mouvement.y(),-translationFactor);
                    return false;
                    break;
                case 'q':
                case 'Q':
                    _mouvement.set(-translationFactor, _mouvement.y(),_mouvement.z());
                    return false;
                    break;
                case 's':
                case 'S':
                    _mouvement.set(_mouvement.x(), _mouvement.y(),translationFactor);
                    return false;
                    break;
                case 'd':
                case 'D':
                    _mouvement.set(translationFactor, _mouvement.y(),_mouvement.z());
                    return false;
                    break;
            }
        }
            break;
        case osgGA::GUIEventAdapter::KEYUP :
        {
            switch (ea.getKey()) {
                case 'z':
                case 'Z':
                    if(_mouvement.z()==-translationFactor)_mouvement.set(_mouvement.x(), _mouvement.y(),0);
                    return false;
                    break;
                case 'q':
                case 'Q':
                    if(_mouvement.x()==-translationFactor)_mouvement.set(0, _mouvement.y(),_mouvement.z());
                    return false;
                    break;
                case 's':
                case 'S':
                    if(_mouvement.z()==translationFactor)_mouvement.set(_mouvement.x(), _mouvement.y(),0);
                    return false;
                    break;
                case 'd':
                case 'D':
                    if(_mouvement.x()==translationFactor)_mouvement.set(0, _mouvement.y(),_mouvement.z());
                    return false;
                    break;
            }
        }
            break;
        //handling mouse movement events
        case osgGA::GUIEventAdapter::MOVE:
        {
            if(!blockUpdateForNextCall){
                deltaRX += ea.getX() - _mouse.x();
                deltaRY += ea.getY() - _mouse.y();
                
                std::cout << "VarX: " << ea.getX() - _mouse.x() << std::endl;
            }
            else{
                blockUpdateForNextCall = false;
                
                std::cout << "BLOCK VarX: " << ea.getX() - _mouse.x() << std::endl;
            }

                
                double newMouseXPosition = ea.getX();
                double newMouseYPosition = ea.getWindowHeight() - ea.getY();
                
                bool doPositionChange = false;
                
                if(ea.getX()<=offsetScreen){
                    doPositionChange = true;
                    newMouseXPosition = ea.getWindowWidth()-2*offsetScreen;
                }
                if(ea.getX()>(ea.getWindowWidth()-offsetScreen)){
                    doPositionChange = true;
                    newMouseXPosition = 2*offsetScreen;
                }
                if(ea.getY()<=offsetScreen){
                    doPositionChange = true;
                    newMouseYPosition = 2*offsetScreen;
                }
                if(ea.getY()>(ea.getWindowHeight()-offsetScreen)){
                    doPositionChange = true;
                    newMouseYPosition = ea.getWindowHeight()-2*offsetScreen;
                }
                
                if(doPositionChange){
                    us.requestWarpPointer(newMouseXPosition, newMouseYPosition);
                    blockUpdateForNextCall = true;
                }
            
            _mouse.set(ea.getX(), ea.getY());

            return false;

        }
            break;
        case osgGA::GUIEventAdapter::SCROLL :
        {
            deltaTZ += ea.getScrollingDeltaY();
            return false;
        }
            break;
        default:
            break;
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

    std::cout << "mouse set B transform" << std::endl;
}

void FirstPersonManipulator::getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const {
    std::cout << "mouse get A transform" << std::endl;
}


void FirstPersonManipulator::getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const {
    std::cout << "mouse get B transform" << std::endl;
}

/** set the position of the matrix manipulator using a 4x4 Matrix.*/
void FirstPersonManipulator::setByMatrix(const osg::Matrixd& matrix){

    // set variables
    _eye = matrix.getTrans();
    _rotation = matrix.getRotate();

    //set the mouse to the center of screen
    _mouse = osg::Vec2d(_mouseCenterX,_mouseCenterY);

    //Fix vertical something

    std::cout << "mouse set by matrix" << std::endl;

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

//    static double deg = 0;
//    osg::Quat q = osg::Quat(0,0,1, deg);
//    deg += 0.001;
//
//    return osg::Matrixd::translate( -_eye ) * osg::Matrixd::rotate( q * _rotation.inverse() );

    return osg::Matrixd::translate( -_eye ) * osg::Matrixd::rotate(  _rotation.inverse() );
};