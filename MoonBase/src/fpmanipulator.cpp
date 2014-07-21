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



bool FirstPersonManipulator::handleMouseMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us ) {

    double deltaX = ea.getX() - _mouse.x();
    double deltaY = ea.getY() - _mouse.y();

    rotateYawPitch(_rotation, deltaX * 0.001, deltaY * 0.001, osg::Vec3(0,0,1));
    _mouse.set(ea.getX(), ea.getY());
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