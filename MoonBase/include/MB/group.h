#pragma once
#ifndef MoonBase_group_h
#define MoonBase_group_h

#ifdef WIN32
//In windows we can properly compile ODE
#ifndef dDOUBLE
#define dDOUBLE
#endif
#endif

#include <MB/object.h>
#include <osg/Referenced>
#include <ode/ode.h>



namespace mb {
	//class designed to handle complex subgraphs of geometries 
	class Group : public Object, public osg::Referenced
	{
		//Physic engine identities
		dBodyID pBody;
		dGeomID *pGeom;
		dWorldID pWorld;
		dSpaceID pSpace;

		int *pGeomClass;
		unsigned int n_pGeom;

		//Shared routines for all constructors
		void sharedConstructor();

	public:
		Group();
		Group(osg::Node *node);
		Group(osg::PositionAttitudeTransform *pat);

		////////////////////////////
		// OSG
		////////////////////////////

		//Set new PAT node
		void setPAT(osg::PositionAttitudeTransform *pat);

		////////////////////////////
		// ODE
		////////////////////////////

		//Set the collision space
		void setCollisionSpace(dSpaceID sid);

		//Set a "crude" collision bounding box
		void setCollisionBoundingBox(double xmin, double xmax, double ymin, double ymax, double zmin, float zmax);

		////////////////////////////
		// Both
		////////////////////////////

		//Set Common attitude
		void setAttitude(osg::Quat quat);
		void setAttitude(dQuaternion quat);
		void setAttitude(double x, double y, double z, double w);
		void setAttitude(osg::Matrix mat);
		//additional definition to go around the overload constraint
		void setAttitudeMatrixODE(dMatrix3 mat);

		//Set Common position
		void setPosition(osg::Vec3 pos);
		void setPosition(dVector3 pos);
		void setPosition(double x, double y, double z);

	protected:
		~Group();
	};
}
#endif
//End of MoonBase_group_h definition