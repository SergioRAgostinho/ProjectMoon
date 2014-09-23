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
#include <MB/printnodevisitor.h>

using namespace mb;

//Some templates here. Check headers for inline definitions of some methods

Loader::Loader(const std::string& path) {

    //Load object
    osg::ref_ptr<osgDB::Options> options = new osgDB::Options();
    options->setDatabasePath("../res/textures/");
    gNode = osgDB::readNodeFile(path,options);

}

osg::Node* Loader::getNode() {
	return Object::getNode();
}

osg::Node* Loader::getNode(const std::string &name) {
    FindNodeVisitor<osg::Node> nodeVisitor = FindNodeVisitor<osg::Node>(name);
    gNode->accept(nodeVisitor);
    return nodeVisitor.getFirst();
}


bool Loader::setRoot(const std::string& name) {
	return setRoot<osg::Node>(name);
}

void Loader::printGraph() {
	printGraph<osg::Node>();
}

Loader::~Loader()
{
}