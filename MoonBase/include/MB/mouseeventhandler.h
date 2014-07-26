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
    //To be used only when the mouse button has nothing to do with the camera
    class MouseEventHandler : public osgGA::GUIEventHandler {



        FirstPersonManipulator *camManip;

    public:

        //the event handler
        bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object * object, osg::NodeVisitor * node);
        
    };

}
#endif
