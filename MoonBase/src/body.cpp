//
//  moonbaseobject.cpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <time.h>
#include <iostream>
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

void Body::initCollision(dSpaceID space) {
    pSpace = space;
    triOGS2ODE();
}

void Body::initPhysics(dWorldID world, dSpaceID space) {
    pWorld = world;
    initCollision(space);

    dMass mass;
    initMass(&mass);
    pBody = dBodyCreate(pWorld);
    dBodySetMass(pBody, &mass);

    dGeomSetBody(pGeom, pBody);
}

void Body::initMass(dMass* mass) {
    dMassSetBoxTotal(mass, 10, 10, 10, 10);
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

//Get angular velocity
void Body::getAngularVelocity(double* av) {
    if (pBody) {
        const dReal* temp;
        temp = dBodyGetAngularVel(pBody);
        *(av) = (double) *temp;
        *(av + 1) = (double) *(temp + 1);
        *(av + 2) = (double) *(temp + 2);
    }
}

//Get linear velocity
void Body::getLinearVelocity(double* lv) {
    if (pBody) {
        const dReal* temp;
        temp = dBodyGetLinearVel(pBody);
        *(lv) = (double) *temp;
        *(lv + 1) = (double) *(temp + 1);
        *(lv + 2) = (double) *(temp + 2);
    }
}

//Set angular velocity
double Body::getAngularSpeed() {
    double av[3] = {};
    getAngularVelocity(&av[0]);
    return sqrt(av[0] * av[0] + av[1] * av[1] + av[2] * av[2]);
};

//Set linear velocity
double Body::getLinearSpeed() {
    double lv[3] = {};
    getLinearVelocity(&lv[0]);
    return sqrt(lv[0] * lv[0] + lv[1] * lv[1] + lv[2] * lv[2]);
};

void Body::setAngularVelocity(double x, double y, double z) {
    if(pBody)
        dBodySetAngularVel(pBody, x, y, z);
}

void Body::setLinearVelocity(double x, double y, double z) {
    if(pBody)
        dBodySetLinearVel(pBody, x, y, z);
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
    if(!(pBody && pGeom))
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


    time_t start = time(0);



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

    double seconds_since_start = difftime( time(0), start);
    std::cout << "[DEBUG] Took " << seconds_since_start
                << " seconds to parse a vertex array with " << nVerts
                << " elements" << std::endl;
    
    return true;
}