//
//  loadedobject.cpp
//  MoonBase
//
//  Created by Neglective on 13/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <osgDB/ReadFile>
#include <MB/loadedobject.hpp>
#include <MB/findnodevisitor.hpp>

LoadedObject::LoadedObject(dWorldID w, dSpaceID s, const std::string& path) {

    //Load object
    osg::ref_ptr<osg::Node> loadedObject = osgDB::readNodeFile(path);
    FindNodeVisitor nodeVisitor = FindNodeVisitor("pCube1-GEODE");
    loadedObject->accept(nodeVisitor);
    gPAT->removeChild(gGeode);
    gGeode = dynamic_cast<osg::Geode*>(nodeVisitor.getFirst());
    gPAT->addChild(gGeode);


    //Create pGeom from gGeometry

    //Physics
    pWorld = w;
    pSpace = s;
    
    pBody = dBodyCreate(pWorld);
}

osg::Node* LoadedObject::getNode() {
    return gNode.get();
}

//FIXME: remove the override
void LoadedObject::setPosition(double x, double y, double z) {
    gPAT->setPosition(osg::Vec3(x, y, z));
}


void triOGS2ODE() {

    /*
     GL_TRIANGLES
     
     1 - count the number of unique vertexs
     2 - allocate the vertex array with repeated vertex
     3 - allocate the index array num(vertexdata)
     
     4 - for(i = 0; i < index of current testing candidate & i< index current vector fill; ++i)
            if(vx != cx && vy != cy && vz != cz)
                add to vector
     
            register index

     5 - resize for later use
     */
}