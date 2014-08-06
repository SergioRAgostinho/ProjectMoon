#pragma once


#ifdef WIN32
//In windows we can properly compile ODE
#ifndef dDOUBLE
#define dDOUBLE
#endif
#endif

#include <ode/ode.h>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>
#include <osgManipulator/Selection>
#include <osgManipulator/Dragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/CommandManager>

#include <MB/body.h>

namespace mb {
    class Cube : public Body
    {


        //    //FIXME: OSG Manipulator
        //    osg::ref_ptr<osgManipulator::CommandManager> gCommandManager;
        //    osg::ref_ptr<osgManipulator::Selection> gSelection;
        //    osg::ref_ptr<osgManipulator::Dragger> gDragger;
        //    osg::ref_ptr<osg::Group> gGroup = new osg::Group();

    public:

        Cube();
        Cube(dWorldID w, dSpaceID s, dReal size);
        
    };

}