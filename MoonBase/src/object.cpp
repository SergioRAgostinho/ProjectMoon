//
//  object.cpp
//  MoonBase
//
//  Created by Neglective on 17/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <MB/object.h>

using namespace mb;

Object::Object() {
    gNode = new osg::Node();
    gPAT = new osg::PositionAttitudeTransform();
    gPAT->addChild(gNode.get());
}

Object::Object(osg::Node* node) {
    gNode = node;
    gPAT = new osg::PositionAttitudeTransform();
    gPAT->addChild(gNode.get());
}

osg::Node* Object::getNode() { return gNode.get(); }

osg::PositionAttitudeTransform* Object::getPAT() { return gPAT.get(); }