//
//  moonbaseobject.cpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//


#include <MB/body.h>

using namespace mb;

Body::Body() : Object(new osg::Geode()){
    initialize();
    gGeode = gNode->asGeode();
}

Body::Body(osg::Geode* geode) : Object(geode) {
    initialize();
    gGeode = gNode->asGeode();
}

void Body::initialize() {
    pBody = nullptr;
    pGeom = nullptr;
    pWorld = nullptr;
    pSpace = nullptr;
    pVerts = nullptr;
    pIdx = nullptr;
    pMeshData = nullptr;
}

Body::Body(dWorldID w, dSpaceID s) {
    initialize();
    gNode = new osg::Geode();
    gGeode = gNode->asGeode();
    pWorld = w;
    pSpace = s;
}

Body::~Body() {
//    delete pBody;
//    delete pGeom;
    delete [] pVerts;
    delete [] pIdx;
}

void Body::initPhysics(dWorldID world, dSpaceID space) {
    pWorld = world;
    pSpace = space;

    triOGS2ODE();
    pBody = dBodyCreate(pWorld);
    dGeomSetBody(pGeom, pBody);
}

osg::Geode* Body::getGeode() { return gGeode.get(); }

void Body::setGeode(osg::Geode *geode) {
    gGeode = geode;
}

void Body::setWorld(dWorldID world) {
    pWorld = world;
}

void Body::setSpace(dSpaceID space) {
    pSpace = space;
}

void Body::setAngularVelocity(double x, double y, double z) {
    if(pBody)
        dBodySetAngularVel(pBody, x, y, z);
}

void Body::setOrientationQuat(double x, double y, double z, double w) {
	gPAT->setAttitude(osg::Quat(x, y, z, w));
    if (pGeom) {
        dQuaternion q = { (dReal) w, (dReal) x, (dReal) y, (dReal) z };
        dGeomSetQuaternion(pGeom, q);
    }
}

void Body::setPosition(double x, double y, double z) {
	gPAT->setPosition(osg::Vec3(x, y, z));
    if(pGeom)
        dGeomSetPosition(pGeom, (dReal) x, (dReal) y, (dReal) z);
}

void Body::update() {

    //if no geometry has been assigned just exit
    if(!pGeom)
        return;

	//Update position
	const dReal* pos = dGeomGetPosition(pGeom);
	gPAT->setPosition(osg::Vec3(*pos, *(pos + 1), *(pos + 2)));

	//Update orientation
	dQuaternion q;
	dGeomGetQuaternion(pGeom, q);
	gPAT->setAttitude(osg::Quat(q[1], q[2], q[3], q[0]));
}

bool Body::triOGS2ODE() {

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