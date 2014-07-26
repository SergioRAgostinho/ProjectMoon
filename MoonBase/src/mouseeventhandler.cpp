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

MouseEventHandler::MouseEventHandler(osg::Camera* cam) :  camera(cam) {}

MouseEventHandler::MouseEventHandler(osg::Camera* cam, std::vector<Body*> *b) : camera(cam) ,selectableBodies(b) {


    selected = new bool[selectableBodies->size()]();
    memset(selected,0,selectableBodies->size()*sizeof(bool));
    active = new bool[selectableBodies->size()]();
    memset(active,0,selectableBodies->size()*sizeof(bool));
    inactiveCounter = new int[selectableBodies->size()]();
    memset(inactiveCounter,0,selectableBodies->size()*sizeof(int));
}

MouseEventHandler::MouseEventHandler(osg::Camera* cam, std::vector<Body*> *b, mb::FirstPersonManipulator* cM) : camera(cam) ,selectableBodies(b), camManip(cM) {

    selected = new bool[selectableBodies->size()]();
    memset(selected,0,selectableBodies->size()*sizeof(bool));
    active = new bool[selectableBodies->size()]();
    memset(active,0,selectableBodies->size()*sizeof(bool));
    inactiveCounter = new int[selectableBodies->size()]();
    memset(inactiveCounter,0,selectableBodies->size()*sizeof(int));

}

MouseEventHandler::~MouseEventHandler() {
    delete [] selected;
    delete [] active;
    delete [] inactiveCounter;
}

//FIXME: Locking behavior. Conflitc between camera manipulator and the object manipulator
bool MouseEventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object * object, osg::NodeVisitor * node) {

    static Body* selectedBody = nullptr;
    static bool grabbed = false;

    static int counter = 0;
    std::cout << " MOUSE: " << counter++ << std::endl;

    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (view) {
        switch (ea.getEventType()) {
            case osgGA::GUIEventAdapter::MOVE:{
                osgUtil::LineSegmentIntersector::Intersections intersections;
                pointerInfo.reset();
                
                //Verify if there are intersections
                memset(selected,0,selectableBodies->size()*sizeof(bool));

//                std::cout << ++counter <<std::endl;

                if (view->computeIntersections(ea.getWindowWidth()>>1, ea.getWindowHeight()>>1, intersections)) {
                    pointerInfo.setCamera(camera);
                    pointerInfo.setMousePosition(ea.getX(), ea.getY());
                    
                    
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

//                std::cout << "MOVE" << " Grabbed: " << grabbed << std::endl;
                break;
            }
            case osgGA::GUIEventAdapter::PUSH: {
                if (!selectedBody) {
                    //abort if not selecting nothing
                    break;
                }



                grabbed = true;
//                std::cout << "PUSH" << " Grabbed: " << grabbed << std::endl;

//                //Compute the relative distance
//                osg::Vec3 pos = camManip->getPosition() + camManip->getOrientation() * osg::Vec3(0,0,-30);
//                selectedBody->setPosition(pos.x(), pos.y(), pos.z());

//                osg::Quat q = camManip->getOrientation() * selectedBody->getOrientationQuat();
//                selectedBody->setOrientationQuat(q.x(), q.y(), q.y(), q.w());
                break;
            }
            case osgGA::GUIEventAdapter::FRAME: {
                if (!(selectedBody && grabbed)) {
                    break;
                }

//                std::cout << "FRAME" << " Grabbed: " << grabbed <<std::endl;

                //Compute the relative distance
                osg::Vec3 pos = camManip->getPosition() + camManip->getOrientation() * osg::Vec3(0,0,-30);
                selectedBody->setPosition(pos.x(), pos.y(), pos.z());

//                osg::Quat q = camManip->getOrientation() * selectedBody->getOrientationQuat();
//                selectedBody->setOrientationQuat(q.x(), q.y(), q.y(), q.w());
                break;
            }
            case osgGA::GUIEventAdapter::RELEASE:
                if (!(selectedBody && grabbed)) {
                    break;
                }
                grabbed = false;
//                std::cout << "RELEASE" << " Grabbed: " << grabbed << std::endl;
                break;
            default:
                break;
        }
    }
    return false;
}
