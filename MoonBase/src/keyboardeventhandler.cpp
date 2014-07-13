//
//  keyboardeventhandler.cpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <iostream>
#include <MB/keyboardeventhandler.hpp>

KeyboardEventHandler::KeyboardEventHandler(MoonBaseObject* o) : obj(o) {}

bool KeyboardEventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa) {

    switch (ea.getEventType()) {
        case osgGA::GUIEventAdapter::KEYUP:
        {
            switch (ea.getKey()) {
                case 'r':
                    case 'R':
                    std::cout << "pressed reset" << std::endl;
                    obj->setPosition(0, 0, 1);
                    break;
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

