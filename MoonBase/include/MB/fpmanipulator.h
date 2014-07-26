//
//  fpmanipulator.h
//  MoonBase
//
//  Created by Neglective on 20/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_fpmanipulator_h
#define MoonBase_fpmanipulator_h

#include <osgGA/StandardManipulator>
#include <osgViewer/GraphicsWindow>

namespace mb {
    class FirstPersonManipulator : public osgGA::StandardManipulator {

        osg::Vec3d _eye;
        osg::Quat _rotation;

        osg::Vec2d _mouse;
        osg::Vec3d _mouvement;
        
        double translationFactor;
        double deltaTZ;
        double deltaRX;
        double deltaRY;
        
        double offsetScreen;
        
    public:
        FirstPersonManipulator();

        void setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation );
        void setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up );
        void getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const;
        void getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const;

        //Get position
        osg::Vec3 getPosition();

        //Get position
        osg::Quat getOrientation();

        /** set the position of the matrix manipulator using a 4x4 Matrix.*/
        void setByMatrix(const osg::Matrixd& matrix);

        /** set the position of the matrix manipulator using a 4x4 Matrix.*/
        void setByInverseMatrix(const osg::Matrixd& matrix);

        /** get the position of the manipulator as 4x4 Matrix.*/
        osg::Matrixd getMatrix() const;

        /** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
        osg::Matrixd getInverseMatrix() const;

    protected:
        
        bool handle (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);
    };
}


#endif
