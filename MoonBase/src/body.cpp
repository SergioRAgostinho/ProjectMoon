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
#include <MB/memory.h>
#include <osg/Shape>
#include <osg/BlendFunc>
#include <osg/StateSet>
#include <osg/PolygonMode>
#include <mb/utils.hpp>

using namespace mb;

Body::Body() : Object(new osg::Geode()){
    initialize();
}

Body::Body(osg::Geode* geode) : Object(geode) {
    initialize();
}

void Body::initialize() {
    pBody = nullptr;
    pGeom = nullptr;
    pWorld = nullptr;
    pSpace = nullptr;
    pVerts = nullptr;
    pIdx = nullptr;
    pMeshData = nullptr;

    gGeode = gNode->asGeode();
    gBB = new osg::Geode();
    gBBState = false;

    //bounding box stuff
    osg::BoundingBox bb = gGeode->getBoundingBox();
    osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(new osg::Box(bb.center(),bb.xMax() - bb.xMin(),bb.yMax() - bb.yMin(),bb.zMax() - bb.zMin()));
    drawable->setColor(osg::Vec4(0,1,0,1));
    gBB->addDrawable(drawable.get());


    //Activate the transparency
    osg::StateSet* set = gBB->getOrCreateStateSet();
    osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
    polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
    set->setAttributeAndModes( polygonMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
}

Body::Body(dWorldID w, dSpaceID s) : Object(new osg::Geode()) {
    initialize();
    pWorld = w;
    pSpace = s;
}

Body::~Body() {}

void Body::align(mb::Body *ref) {

    osg::Matrix R_new;
    osg::Matrix R = ref->getOrientationMat();
    osg::Matrix A = this->getOrientationMat();

    int max_idx[3] = {-1,-1,-1};
    int max_sign[3] = {};

    bool ignore = false;
    
    for (int i = 0; i < 3; ++i) {
        double inner_prod[3]= {};
        int inner_sign[3]= {};

        for (int j = 0; j < 3; ++j) {

            for (int k = 0; k <= i; ++k){
                if (max_idx[k] == j) {
                    ignore = true;
                    break;
                }
            }

            if (ignore) {
                ignore = false;
                continue;
            }

            double innerProd = R(0,j)*A(0,i) + R(1,j)*A(1,i) + R(2,j)*A(2,i);
            inner_sign[j] = mb::sgn(innerProd);
            inner_prod[j] = std::abs(innerProd);
        }

        //find the max and the index
        double max_inner_prod = 0;
        for (int j = 0; j < 3; ++j) {
            if (inner_prod[j] > max_inner_prod) {
                max_inner_prod = inner_prod[j];
                max_idx[i] = j;
                max_sign[i] = inner_sign[j];
            }
        }

        R_new(0,i) = max_sign[i] * R(0,max_idx[i]);
        R_new(1,i) = max_sign[i] * R(1,max_idx[i]);
        R_new(2,i) = max_sign[i] * R(2,max_idx[i]);
    }

    setOrientationMat(R_new);
}

//FIXME: we're gonna need smart pointers for this because if the original mb::Body gets destroyed, everything will fall through
Body* Body::clone() {

    //Clone OSG stuff
    Body* out = new Body();
    out->gGeode->addDrawable(gGeode->getDrawable(0));
    out->gBB->removeDrawable(out->gBB->getDrawable(0));
    out->gBB->addDrawable(gBB->getDrawable(0));
    (out->gPAT)->setPosition(gPAT->getPosition());
    (out->gPAT)->setAttitude(gPAT->getAttitude());
    (out->gPAT)->setScale(gPAT->getScale());
    (out->gPAT)->setPivotPoint(gPAT->getPivotPoint());

    //Create geometry and body in the same collidable space and same world physics
    out->pWorld = pWorld;
    out->pSpace = pSpace;

    //Clone geometry
    if (pGeom) {
        int gClass = dGeomGetClass(pGeom);
        switch (gClass) {
            case dBoxClass:
                dReal size[3];
                dGeomBoxGetLengths(pGeom, size);
                out->pGeom = dCreateBox(pSpace, size[0], size[1], size[2]);
                break;
            case dTriMeshClass:
                out->pGeom = dCreateTriMesh(pSpace, pMeshData, 0, 0, 0);
                dGeomSetData(out->pGeom, pMeshData);
                out->pMeshData = pMeshData;
                out->pVerts = pVerts;
                out->pIdx = pIdx;
                break;
            default:
                break;
        }
    }

    //Create body
    if (pBody) {
        out->pBody = dBodyCreate(pWorld);
        dMass mass;
        dBodyGetMass(pBody, &mass);
        dBodySetMass(out->pBody, &mass);
        dGeomSetBody(out->pGeom, out->pBody);
    }

    return out;
}

void Body::initCollision(dSpaceID space) {
    pSpace = space;
    if(!pGeom)
        triOGS2ODE();
}

void Body::initPhysics(dWorldID world, dSpaceID space) {
    initPhysics(world, space, 1);
}

void Body::initPhysics(dWorldID world, dSpaceID space, double massAmount) {
    pWorld = world;
    initCollision(space);

    dMass mass;
    initMass(&mass, massAmount);
    pBody = dBodyCreate(pWorld);
    dBodySetMass(pBody, &mass);

    dGeomSetBody(pGeom, pBody);
}

void Body::initMass(dMass *mass) {
    initMass(mass, 1);
}
void Body::initMass(dMass* mass, double amount) {

    if (!pGeom)
        return;

    //Test for geometry class
    int geomClass = dGeomGetClass(pGeom);
    switch (geomClass) {
        case dBoxClass:
            dReal size[3];
            dGeomBoxGetLengths(pGeom, size);
            dMassSetBoxTotal(mass, amount, size[0], size[1], size[2]);
            break;
        case dTriMeshClass:
            dMassSetTrimeshTotal(mass, amount, pGeom);
            dMassTranslate( mass, -mass->c[0], -mass->c[1], -mass->c[2] );
            //Not sure if i need to translate more stuff
            break;
        default:
            break;
    }

}

void Body::setTotalMass(double amount) {
    if (!pBody)
        return;

    dMass mass;
    dBodyGetMass(pBody, &mass);
    dMassAdjust(&mass, amount);
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

//Get Orientation Mat
osg::Matrix Body::getOrientationMat() {
    osg::Matrix out;
    (gPAT->getAttitude()).get(out);
    return out;
}

//Get Orientation Mat
osg::Quat Body::getOrientationQuat() {
    return gPAT->getAttitude();
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

//Set Orientation Mat
void Body::setOrientationMat(osg::Matrix mat) {
    osg::Quat q = mat.getRotate();
    setOrientationQuat(q.x(), q.y(), q.z(), q.w());
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

    float* tempPVerts = new float[nVerts*3]();
    pIdx.reset(new dTriIndex[nVerts](),mb::array_deleter<dTriIndex>());

    int uniqueVerts = 0;


    time_t start = time(0);



    //Worst case - n(n-1) runs
    for (int i = 0; i < nVerts; ++i) {
        osg::Vec3 vert = (*array)[i];
        bool repeated = false;

        //sweep odeVerts to check if it is a repeated vertex
        for (int j = 0; j < uniqueVerts; ++j) {
            if ((tempPVerts[3*j] == vert.x()) && (tempPVerts[3*j + 1] == vert.y()) && (tempPVerts[3*j + 2] == vert.z())) {
                repeated = true;
                *(pIdx.get() + i) = j;
                break;
            }
        }

        //if it is repeated just skip it
        if (repeated) {
            continue;
        }

        *(pIdx.get() + i) = uniqueVerts;
        tempPVerts[3*uniqueVerts] = vert.x();
        tempPVerts[3*uniqueVerts + 1] = vert.y();
        tempPVerts[3*(uniqueVerts++) + 2] = vert.z();
    }

    //delete unnecessary space
    pVerts.reset(new float[3*uniqueVerts](),mb::array_deleter<float>());
    memcpy(pVerts.get(),tempPVerts,3*uniqueVerts * sizeof(float));
    delete tempPVerts;


    //    dTriMeshDataID new_tmdata = dGeomTriMeshDataCreate();
    pMeshData = dGeomTriMeshDataCreate();

    //FIXME: Use dGeomTriMeshDataBuildSingle1 and provide also the normals for improved behavior in trimesh-trimesh collision
    dGeomTriMeshDataBuildSingle(pMeshData, pVerts.get(), 3 * sizeof(float), uniqueVerts, (dTriIndex*)pIdx.get(), nVerts, 3 * sizeof(dTriIndex));

    pGeom = dCreateTriMesh(pSpace, pMeshData, 0, 0, 0);
    dGeomSetData(pGeom, pMeshData);

    double seconds_since_start = difftime( time(0), start);
    std::cout << "[DEBUG] Took " << seconds_since_start
                << " seconds to parse a vertex array with " << nVerts
                << " elements" << std::endl;

    
    return true;
}

void Body::activateBB() {
    if (gBBState)
        return;

    gPAT->addChild(gBB.get());
    gBBState = true;
}

void Body::removeBB() {
    if (!gBBState)
        return;

    gPAT->removeChild(gBB.get());
    gBBState = false;
}

void Body::toggleBB() {
    if (gBBState)
        gPAT->removeChild(gBB.get());
    else
        gPAT->addChild(gBB.get());

    gBBState = !gBBState;
}