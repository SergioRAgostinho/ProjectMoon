#include <MB/cube.h>


Cube::Cube(dWorldID w, dSpaceID s, dReal size)
{
	//Physics
	pWorld = w;
	pSpace = s;

	pBody = dBodyCreate(pWorld);
	pGeom = dCreateBox(pSpace, (dReal)size, (dReal)size, (dReal)size);
	dGeomSetBody(pGeom, pBody);
	

	//Graphics
	gBox = new osg::Box(osg::Vec3(0, 0, 0), (float) size);
	gBoxShape = new osg::ShapeDrawable(gBox.get());
	gGeode = new osg::Geode();
	gGeode->addDrawable(gBoxShape.get());
	gT = new osg::PositionAttitudeTransform();
	gT->addChild(gGeode);
}


Cube::~Cube()
{
}

osg::Geode* Cube::getGeode() { return gGeode.get(); }

osg::PositionAttitudeTransform* Cube::getPAT() { return gT.get(); }

void Cube::setPosition(double x, double y, double z) {
	gT->setPosition(osg::Vec3(x, y, z));
	dGeomSetPosition(pGeom, (dReal) x, (dReal) y, (dReal) z);
}

void Cube::setOrientationQuat(double x, double y, double z, double w) {
	gT->setAttitude(osg::Quat(x, y, z, w));
	dQuaternion q = { (dReal) w, (dReal) x, (dReal) y, (dReal) z };
	dGeomSetQuaternion(pGeom, q);
}

void Cube::update() {
	//Update position
	const dReal* pos = dGeomGetPosition(pGeom);
	gT->setPosition(osg::Vec3(*pos, *(pos + 1), *(pos + 2)));

	//Update orientation
	dQuaternion q;
	dGeomGetQuaternion(pGeom, q);
	gT->setAttitude(osg::Quat(q[1], q[2], q[3], q[0]));
}

void Cube::setAngularVelocity(double x, double y, double z) { dBodySetAngularVel(pBody, x, y, z); }
