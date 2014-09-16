#pragma once

#ifndef MoonBase_printnodevisitor_h
#define MoonBase_printnodevisitor_h

#include <osg/NodeVisitor>

namespace mb {
	
	class PrintNodeVisitor :
		public osg::NodeVisitor
	{
	public:

		//Default constructor. The traversal mode is set to TRAVERSE_ALL_CHILDREN
		PrintNodeVisitor();

		~PrintNodeVisitor();

		// The 'apply' method for 'node' type instances.
		// searchNode provided as argument 
		virtual void apply(osg::Node &searchNode);
	};

}
#endif