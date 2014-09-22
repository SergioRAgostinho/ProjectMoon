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

	pGeomClass = nullptr;

	n_pGeom = 0;
}

Group::~Group()
{
	SafeReleaseArray(pGeom);
	SafeReleaseArray(pGeomClass);
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
		for (unsigned int i = 0; i < n_pGeom; i++)
		{
			dSpaceRemove(pSpace, pGeom[i]);
			dSpaceAdd(sid, pGeom[i]);
		}
	}

	pSpace = sid;
}

//Set a "crude" collision bounding box
void Group::setCollisionBoundingBox(double xmin, double xmax, double ymin, double ymax, double zmin, float zmax)
{
	if (pSpace)
	{
		//This is not the way. We need to set up a group of four plane with normals pointing inwards
		
		//Back up the old and allocate the new vector
		unsigned int old_n_pGeom = n_pGeom;
		n_pGeom = old_n_pGeom + 6;
		
		dGeomID *old_pGeom = pGeom;
		pGeom = new dGeomID[n_pGeom];

		int *old_pGeomClass = pGeomClass;
		pGeomClass = new int[n_pGeom];

		//Copy the old 
		memcpy(pGeom, old_pGeom, old_n_pGeom*sizeof(dGeomID));
		memcpy(pGeomClass, old_pGeomClass, old_n_pGeom*sizeof(int));

		//Delete the old
		SafeReleaseArray(old_pGeom);
		SafeReleaseArray(old_pGeomClass);

		//Set the new geometries
		//Lower x
		pGeom[old_n_pGeom] = dCreatePlane(pSpace, 1, 0, 0, xmin);
		pGeomClass[old_n_pGeom] = dPlaneClass;

		//Upper x
		pGeom[old_n_pGeom + 1] = dCreatePlane(pSpace, -1, 0, 0, -xmax);
		pGeomClass[old_n_pGeom + 1] = dPlaneClass;

		//Lower y
		pGeom[old_n_pGeom + 2] = dCreatePlane(pSpace, 0, 1, 0, ymin);
		pGeomClass[old_n_pGeom + 2] = dPlaneClass;

		//Upper y
		pGeom[old_n_pGeom + 3] = dCreatePlane(pSpace, 0, -1, 0, -ymax);
		pGeomClass[old_n_pGeom + 3] = dPlaneClass;

		//Lower z
		pGeom[old_n_pGeom + 4] = dCreatePlane(pSpace, 0, 0, 1, zmin);
		pGeomClass[old_n_pGeom + 4] = dPlaneClass;

		//Upper z
		pGeom[old_n_pGeom + 5] = dCreatePlane(pSpace, 0, 0, -1, -zmax);
		pGeomClass[old_n_pGeom + 5] = dPlaneClass;
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
		for (unsigned int i = 0; i < n_pGeom; i++)
		{
			//We need to ensure the geom is placeable, aka different than infinite plane
			if (pGeomClass[i] != dPlaneClass)
			{
				dQuaternion q = { (dReal)quat.w(), (dReal)quat.x(), (dReal)quat.y(), (dReal)quat.z() };
				dGeomSetQuaternion(pGeom[i], q);
			}
			else
			{
				//If it is a plane we need to rotate the normal unit vector accordingly
				dReal params[4];
				dGeomPlaneGetParams(pGeom[i], params);
				osg::Vec3 rotated = quat * osg::Vec3(params[0], params[1], params[2]);
				dGeomPlaneSetParams(pGeom[i], rotated.x(), rotated.y(), rotated.z(), params[3]);
			}
		}
	}
}

void Group::setAttitude(dQuaternion quat)
{
	setAttitude(osg::Quat(quat[1], quat[2], quat[3], quat[0]));
}

void Group::setAttitude(double x, double y, double z, double w)
{
	setAttitude(osg::Quat(x, y, z, w));
}

void Group::setAttitude(osg::Matrix mat)
{
	setAttitude(mat.getRotate());
}

void Group::setAttitudeMatrixODE(dMatrix3 mat)
{
	DEBUG_WARNING("setAttitudeMatrixODE has not been tested yet");
	setAttitude(osg::Matrix(mat[0], mat[1], mat[2], 0,
		mat[4], mat[5], mat[6], 0,
		mat[7], mat[8], mat[9], 0,
		0, 0, 0, 0).getRotate());
}