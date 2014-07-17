//
//  mouseeventhandler.cpp
//  MoonBase
//
//  Created by Neglective on 11/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <MB/mouseeventhandler.h>

MouseEventHandler::MouseEventHandler(osg::Camera* cam) :  camera(cam) {}

//FIXME: Locking behavior. Conflitc between camera manipulator and the object manipulator
bool MouseEventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object * object, osg::NodeVisitor * node) {
    
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (view) {
        switch (ea.getEventType()) {
            case osgGA::GUIEventAdapter::PUSH:{
                osgUtil::LineSegmentIntersector::Intersections intersections;
                pointerInfo.reset();
                
                //Verify if there are intersections
                if (view->computeIntersections(ea.getX(), ea.getY(), intersections)) {
                    pointerInfo.setCamera(camera);
                    pointerInfo.setMousePosition(ea.getX(), ea.getY());
                    
                    
                    for (osgUtil::LineSegmentIntersector::Intersections::iterator interIter = intersections.begin(); interIter != intersections.end(); ++interIter) {
                        pointerInfo.addIntersection(interIter->nodePath, interIter->getLocalIntersectPoint());
                    }
                    
                    for (osg::NodePath::iterator npIter = pointerInfo._hitList.front().first.begin(); npIter != pointerInfo._hitList.front().first.end(); ++npIter) {
                        if (osgManipulator::Dragger* dragger = dynamic_cast<osgManipulator::Dragger*>(*npIter)) {
                            dragger->handle(pointerInfo, ea, aa);
                            activeDragger = dragger;
                            return true;
                        }
                    }
                    
                }
            }
                break;
            case osgGA::GUIEventAdapter::RELEASE:
            case osgGA::GUIEventAdapter::DRAG:
                if (activeDragger) {
                    pointerInfo._hitIter = pointerInfo._hitList.begin();
                    pointerInfo.setCamera(camera);
					pointerInfo.setMousePosition(ea.getX(), ea.getY());
					activeDragger->handle(pointerInfo, ea, aa);
					return true;
                }
            default:
                break;
        }
    }
    return false;
}
