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

    //Physics
    pWorld = w;
    pSpace = s;

    pBody = dBodyCreate(pWorld);


    //Load object
    gNode = osgDB::readNodeFile(path);
    FindNodeVisitor nodeVisitor = FindNodeVisitor("pCube1-GEODE");
    gNode->accept(nodeVisitor);
    gPAT->removeChild(gGeode);
    osg::Node* node = nodeVisitor.getFirst();
    if (node) {
        gGeode = dynamic_cast<osg::Geode*>(nodeVisitor.getFirst());
        gPAT->addChild(gGeode);
    } else {
        gPAT->addChild(gNode);
    }

    //Create pGeom from gGeometry
    if(triOGS2ODE())
        dGeomSetBody(pGeom, pBody);
}

LoadedObject::~LoadedObject() {
    delete [] pVerts;
    delete [] pIdx;
}


osg::Node* LoadedObject::getNode() {
    return gNode.get();
}


bool LoadedObject::triOGS2ODE() {

    osg::Geometry* geo = gGeode->getDrawableList().front()->asGeometry();
    if (!geo || geo->getPrimitiveSet(0)->getMode() != GL_TRIANGLES)
        return false;

    osg::Vec3Array* array = dynamic_cast<osg::Vec3Array*>(geo->getVertexArray());
    unsigned int nVerts = array->getNumElements();

    pVerts = new float[nVerts*3]();
    pIdx = new dTriIndex[nVerts]();

    int uniqueVerts = 0;

    //Worst case - n(n-1) runs
    for (int i = 0; i < nVerts; ++i) {
        osg::Vec3 vert = (*array)[i];
        bool repeated = false;

        //sweep odeVerts to check if it is a repeated vertex
        for (int j = 0; j < uniqueVerts; ++j) {
            if ((pVerts[3*j] == vert.x()) && (pVerts[3*j + 1] == vert.y()) && (pVerts[3*j + 2] == vert.z())) {
                repeated = true;
                pIdx[i] = j;
                break;
            }
        }

        //if it is repeated just skip it
        if (repeated) {
            continue;
        }

        pIdx[i] = uniqueVerts;
        pVerts[3*uniqueVerts] = vert.x();
        pVerts[3*uniqueVerts + 1] = vert.y();
        pVerts[3*(uniqueVerts++) + 2] = vert.z();
    }

    //delete unnecessary space
    float temp[3*uniqueVerts];
    memcpy(temp,pVerts,sizeof(temp));
    delete pVerts;
    pVerts = new float[3*uniqueVerts]();
    memcpy(pVerts, temp, sizeof(temp));


//    dTriMeshDataID new_tmdata = dGeomTriMeshDataCreate();
    pMeshData = dGeomTriMeshDataCreate();

    //FIXME: Use dGeomTriMeshDataBuildSingle1 and provide also the normals for improved behavior in trimesh-trimesh collision
    dGeomTriMeshDataBuildSingle(pMeshData, pVerts, 3 * sizeof(float), uniqueVerts, (dTriIndex*)pIdx, nVerts, 3 * sizeof(dTriIndex));

    pGeom = dCreateTriMesh(pSpace, pMeshData, 0, 0, 0);
    dGeomSetData(pGeom, pMeshData);

    return true;
}