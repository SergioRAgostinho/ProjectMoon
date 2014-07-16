//
//  moonbaseobject.cpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//


#include <MB/moonbaseobject.hpp>

//FIXME: Retarded constructor with no body
//MoonBaseObject::MoonBaseObject(dWorldID w, dSpaceID s, ConstructorFlags flags) {
//
//    pGeom = dCreatePlane(pSpace, 0, 0, 1, 0);
//	dGeomSetBody(pGeom, pBody);
//}

MoonBaseObject::MoonBaseObject() {

    gGeometry = new osg::Geometry();
	gGeode = new osg::Geode();
    gGeode->addDrawable(gGeometry);
	gPAT = new osg::PositionAttitudeTransform();
	gPAT->addChild(gGeode);
}

osg::Geode* MoonBaseObject::getGeode() { return gGeode.get(); }

osg::PositionAttitudeTransform* MoonBaseObject::getPAT() { return gPAT.get(); }

void MoonBaseObject::setPosition(double x, double y, double z) {
	gPAT->setPosition(osg::Vec3(x, y, z));
	dGeomSetPosition(pGeom, (dReal) x, (dReal) y, (dReal) z);
}

void MoonBaseObject::update() {
	//Update position
	const dReal* pos = dGeomGetPosition(pGeom);
	gPAT->setPosition(osg::Vec3(*pos, *(pos + 1), *(pos + 2)));

	//Update orientation
	dQuaternion q;
	dGeomGetQuaternion(pGeom, q);
	gPAT->setAttitude(osg::Quat(q[1], q[2], q[3], q[0]));
}