#pragma once
#include "body.h"
class AssemblyBody :
	public mb::Body
{
protected:

	std::vector<osg::MatrixTransform*> joinList;

public:
	AssemblyBody();
	~AssemblyBody();
};

