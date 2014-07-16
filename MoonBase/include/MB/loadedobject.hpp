//
//  loadedobject.hpp
//  MoonBase
//
//  Created by Neglective on 13/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_loadedobject_hpp
#define MoonBase_loadedobject_hpp

#include <Windows.h>
#include <string>
#include <MB/moonbaseobject.hpp>

class LoadedObject : public MoonBaseObject {

    //OSG
    osg::ref_ptr<osg::Node> gNode;

    //ODE Mesh Data
    float* pVerts;
    dTriIndex* pIdx;
    dTriMeshDataID pMeshData;

protected:

    bool triOGS2ODE();

public:

    LoadedObject(dWorldID w, dSpaceID s, const std::string& path);

    ~LoadedObject();

    osg::Node* getNode();
};


#endif
