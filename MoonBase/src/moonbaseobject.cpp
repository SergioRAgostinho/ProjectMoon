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

osg::Geode* MoonBaseObject::getGeode() { return gGeode; }

void MoonBaseObject::setPosition(double x, double y, double z) {
	gPAT->setPosition(osg::Vec3(x, y, z));
	dGeomSetPosition(pGeom, (dReal) x, (dReal) y, (dReal) z);
}