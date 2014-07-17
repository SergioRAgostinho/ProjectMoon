//
//  loadedobject.cpp
//  MoonBase
//
//  Created by Neglective on 13/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <osgDB/ReadFile>
#include <MB/loader.h>
#include <MB/findnodevisitor.h>

using namespace mb;

Loader::Loader(const std::string& path) {

    //Load object
    gNode = osgDB::readNodeFile(path);

}

osg::Node* Loader::getNode(const std::string &name) {
    FindNodeVisitor nodeVisitor = FindNodeVisitor(name);
    gNode->accept(nodeVisitor);
    return nodeVisitor.getFirst();
}

bool Loader::setRoot(const std::string& name) {

    FindNodeVisitor nodeVisitor = FindNodeVisitor(name);
    gNode->accept(nodeVisitor);
    osg::Node* node = nodeVisitor.getFirst();
    if (node) {
        gPAT->removeChild(gPAT->getChild(0));
        gPAT->addChild(node);
        return true;
    }

    return false;
}
