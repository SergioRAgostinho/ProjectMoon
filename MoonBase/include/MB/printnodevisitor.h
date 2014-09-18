#pragma once

#ifndef MoonBase_printnodevisitor_h
#define MoonBase_printnodevisitor_h

#include <osg/NodeVisitor>

namespace mb {
	
	template <class osgNodeType>
	class PrintNodeVisitor :
		public osg::NodeVisitor
	{
	public:

		//Default constructor. The traversal mode is set to TRAVERSE_ALL_CHILDREN
		PrintNodeVisitor();

		// The 'apply' method for 'node' type instances.
		// searchNode provided as argument 
		virtual void apply(osgNodeType &searchNode);
	};

	////////////////////////////////////////
	////   INLINE DEFINITIONS BECAUSE OF TEMPLATES
	///////////////////////////////////////////

	template <class osgNodeType>
	PrintNodeVisitor<osgNodeType>::PrintNodeVisitor() : NodeVisitor(TRAVERSE_ALL_CHILDREN) {}

	template <class osgNodeType>
	void PrintNodeVisitor<osgNodeType>::apply(osgNodeType &searchNode) {

		//Infer path traversed
		const osg::NodePath path = getNodePath();

		//Print out name
		const std::string indent(path.size() - 1, '-');
		const std::string className = searchNode.className();

		std::cout << indent << (path.size() > 1 ? "> " : "")
			<< (searchNode.getName() != "" ? searchNode.getName() : "[no name]")
			<< " <" << className << ">" << std::endl;

		//Proceed to the next node
		traverse(searchNode);
	}
}
#endif