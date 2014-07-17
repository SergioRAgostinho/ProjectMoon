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

class MouseEventHandler : public osgGA::GUIEventHandler {
    
    osgManipulator::PointerInfo pointerInfo;
    osgManipulator::Dragger* activeDragger;
    osg::Camera* camera;

public:
    
    MouseEventHandler(osg::Camera* cam);
    
    //the event handler
    bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object * object, osg::NodeVisitor * node);
    
};

#endif
