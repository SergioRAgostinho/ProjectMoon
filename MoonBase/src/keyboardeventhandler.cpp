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

	Body* body = nullptr;

    //    static int counter = 0;
    //    std::cout << " KEY: " << counter++ << std::endl;

    switch (ea.getEventType()) {
        case osgGA::GUIEventAdapter::KEYUP:
        {
            switch (ea.getKey()) {
                case 'f':
                case 'F':
					if (body) {
						body->setPosition(0, 0, 60);
						body->setLinearVelocity(uniRand(-10, 10), uniRand(-10, 10), uniRand(-10, 10));
						body->setAngularVelocity(uniRand(-1, 1), uniRand(-1, 1), uniRand(-1, 1));
					}
                    break;
				case 'm':
				case 'M':
					if (app->crates)
					{
						for (size_t i = 0; i < app->n_crates; i++)
						{
							app->crates[i]->setLinearVelocity(mb::uniRand(-.1, .1), mb::uniRand(-.1, .1), mb::uniRand(-.1, .1));
						}
					}
					break;
                case 'r':
                case 'R':
                    //align with other object
                    break;
                case 't':
                case 'T': {
					if (body) {
						osg::Vec3 axis = osg::Vec3(mb::uniRand(-1, 1), mb::uniRand(-1, 1), mb::uniRand(-1, 1));
						axis.normalize();
						body->setAttitude(osg::Quat(mb::uniRand(-M_PI, M_PI), axis));
					}
                    break;
                }
                case 'z':
                case 'Z': {
					if (body) {
						static bool enable = true;
						enable ? body->disablePBody() : body->enablePBody();
						enable = !enable;
					}
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

