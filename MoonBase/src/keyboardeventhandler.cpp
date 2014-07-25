//
//  keyboardeventhandler.cpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <iostream>
#include <MB/keyboardeventhandler.h>
#include <MB/utils.hpp>

using namespace mb;

KeyboardEventHandler::KeyboardEventHandler(Application* a) : app(a) {}

bool KeyboardEventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa) {

    Body* body = app->moscatelTBRot;

    switch (ea.getEventType()) {
        case osgGA::GUIEventAdapter::KEYUP:
        {
            switch (ea.getKey()) {
//                case 'r':
//                case 'R':
//                    body->setPosition(0, 0, 60);
//                    body->setLinearVelocity(uniRand(-10, 10),uniRand(-10, 10),uniRand(-10, 10));
//                    body->setAngularVelocity(uniRand(-1, 1),uniRand(-1, 1),uniRand(-1, 1));
//                    break;
                case 'r':
                case 'R':
                    //align with other object
                    body->align(app->moscatel);
                    break;
                case 't':
                case 'T': {
                    osg::Vec3 axis = osg::Vec3(mb::uniRand(-1, 1),mb::uniRand(-1, 1),mb::uniRand(-1, 1));
                    axis.normalize();
                    osg::Matrix rot = osg::Matrix::rotate(mb::uniRand(-M_PI, M_PI), axis);
                    osg::Quat q = rot.getRotate();
                    body->setOrientationQuat(q.x(),q.y(),q.z(),q.w());
                    break;
                }
                default:
                    break;
            }
        }
            break;
        default:
            break;
    }

    return false;
}

void KeyboardEventHandler::accept(osgGA::GUIEventHandlerVisitor& v) { v.visit(*this); }

