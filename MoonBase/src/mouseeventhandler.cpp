//
//  mouseeventhandler.cpp
//  MoonBase
//
//  Created by Neglective on 11/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <MB/mouseeventhandler.h>
#include <iostream>


using namespace mb;

bool MouseEventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object * object, osg::NodeVisitor * node) {



    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (view) {
        //switch (ea.getEventType()) {

        //    default:
        //        break;
        //}
    }
    return false;
}
