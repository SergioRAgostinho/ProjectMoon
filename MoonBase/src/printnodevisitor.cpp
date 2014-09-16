#include <MB/printnodevisitor.h>
#include <iostream>

using namespace mb;

PrintNodeVisitor::PrintNodeVisitor() : NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
}

PrintNodeVisitor::~PrintNodeVisitor()
{
}

void PrintNodeVisitor::apply(osg::Node &searchNode) {
	
	//Infer path traversed
	const osg::NodePath path = getNodePath();

	//Print out name
	const std::string indent(path.size() - 1, '-');
	const std::string className = searchNode.className();

	std::cout << indent << (path.size() > 1? "> " : "") 
		<< (searchNode.getName() != "" ? searchNode.getName() : "[no name]")
		<< " <" << className << ">" << std::endl;

	//Proceed to the next node
	traverse(searchNode);
}