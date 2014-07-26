//
//  mouseeventhandler.hpp
//  MoonBase
//
//  Created by Neglective on 11/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_mouseeventhandler_h
#define MoonBase_mouseeventhandler_h

#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include <osgManipulator/Dragger>
#include <MB/body.h>
#include <MB/fpmanipulator.h>

namespace mb {

    class MouseEventHandler : public osgGA::GUIEventHandler {

        osgManipulator::PointerInfo pointerInfo;
        osg::Camera* camera;
        std::vector<Body*> *selectableBodies;
        bool *selected, *active;
        int *inactiveCounter;

        FirstPersonManipulator *camManip;

    public:

        MouseEventHandler(osg::Camera* cam);
        MouseEventHandler(osg::Camera* cam, std::vector<Body*> *b);
        MouseEventHandler(osg::Camera* cam, std::vector<Body*> *b, FirstPersonManipulator* cM);

        ~MouseEventHandler();

        //the event handler
        bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object * object, osg::NodeVisitor * node);
        
    };

}
#endif
