//
//  loadedobject.cpp
//  MoonBase
//
//  Created by Neglective on 13/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <osgDB/ReadFile>
#include <MB/loadedobject.hpp>

LoadedObject::LoadedObject(dWorldID w, dSpaceID s, std::string path) {

    //Load object
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("hex.osgt");
    osg::ref_ptr<osg::Node> loadedModel2 = osgDB::readNodeFile("../res/hex.osgt");
    osg::ref_ptr<osg::Node> loadedModel3 = osgDB::readNodeFile(path);


    //Create pGeom from gGeometry


    //Physics
    pWorld = w;
    pSpace = s;
    
    pBody = dBodyCreate(pWorld);
}