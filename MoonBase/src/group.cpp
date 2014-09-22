#include <MB/group.h>
#include <MB/utils.hpp>

using namespace mb;

Group::Group()
{
}

Group::Group(osg::Node *node) : Object(node)
{	
	sharedConstructor();
}

Group::Group(osg::PositionAttitudeTransform *pat)
{
	setPAT(pat);
	sharedConstructor();
}

//Shared routines for all constructors
void Group::sharedConstructor()
{
	pBody = nullptr;
	pGeom = nullptr;
	pWorld = nullptr;
	pSpace = nullptr;
}

Group::~Group()
{
}

//Set new PAT node
void Group::setPAT(osg::PositionAttitudeTransform *pat)
{
	const unsigned int n_children = pat->getNumChildren();
	switch (n_children)
	{
	case 0:
		//No children. Replace PAT and attach default gNode bellow it
		gPAT = pat;
		gPAT->addChild(gNode.get());
	case 1:
		//One child only. Replace gPat and gNode
		gPAT = pat;
		gNode = dynamic_cast<osg::Node*>(pat->getChild(0));
	default:
		//More than one child beneath the PAT node
		DEBUG_WARNING("More than one child node under the PAT node. The first one will be picked by default.");
		gPAT = pat;
		gNode = dynamic_cast<osg::Node*>(pat->getChild(0));
		break;
	}
}

//Set the collision space
void Group::setCollisionSpace(dSpaceID sid)
{
	//If the collision space is not set, just set the new value.
	//If the collision space is set and theres a geometry already in it we need to update it to the new space
	if (pGeom)
	{
		dSpaceRemove(pSpace, pGeom);
		dSpaceAdd(sid, pGeom);
	}

	pSpace = sid;
}

//Set a "crude" collision bounding box
void Group::setCollisionBoundingBox(double xmin, double xmax, double ymin, double ymax, double zmin, float zmax)
{
	if (pSpace)
	{
		pGeom = dCreateBox(pSpace, xmax - xmin, ymax - ymin, zmax - zmin);
		dGeomSetPosition(pGeom, (xmax + xmin)*0.5, (ymax + ymin)*0.5, (zmax + zmin)*0.5);
	}
	else
	{
		//warn the user about the lack of information on the collision space
		DEBUG_EXCEPTION("LogicException - No collision space provided");
		throw std::logic_error("No collision space provided");
	}
}

//Set Common attitude
void Group::setAttitude(osg::Quat quat)
{
	gPAT->setAttitude(quat);
	if (pGeom)
	{
		dQuaternion q = { (dReal)quat.w(), (dReal)quat.x(), (dReal)quat.y(), (dReal)quat.z() };
		dGeomSetQuaternion(pGeom, q);
	}
}

void Group::setAttitude(dQuaternion quat)
{
	gPAT->setAttitude(osg::Quat(quat[1], quat[2], quat[3], quat[0]));
	if (pGeom)
	{
		dGeomSetQuaternion(pGeom, quat);
	}
}

void Group::setAttitude(double x, double y, double z, double w)
{
	gPAT->setAttitude(osg::Quat(x, y, z, w));
	if (pGeom)
	{
		dQuaternion q = { (dReal) w, (dReal)x, (dReal)y, (dReal)z };
		dGeomSetQuaternion(pGeom, q);
	}
}

void Group::setAttitude(osg::Matrix mat)
{
	setAttitude(mat.getRotate());
}

void Group::setAttitudeMatrixODE(dMatrix3 mat)
{
	DEBUG_WARNING("setAttitudeMatrixODE has not been tested yet");
	gPAT->setAttitude(osg::Matrix(mat[0], mat[1], mat[2], 0,
		mat[4], mat[5], mat[6], 0,
		mat[7], mat[8], mat[9], 0,
		0, 0, 0, 0).getRotate());
	if (pGeom)
	{
		dGeomSetRotation(pGeom, mat);
	}
}