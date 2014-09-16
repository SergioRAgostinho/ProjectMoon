//
//  loadedobject.hpp
//  MoonBase
//
//  Created by Neglective on 13/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_loadedobject_hpp
#define MoonBase_loadedobject_hpp

#include <string>
#include <MB/object.h>
#include <MB/findnodevisitor.h>

namespace mb {


    class Loader : public Object {

    public:

        Loader(const std::string& path);

		//GetNode returns a pointer to the top node or to one specified by the type and name provided
		osg::Node* getNode();
        osg::Node* getNode(const std::string& name);
        template<class osgNodeType>
        osgNodeType* getNode(const std::string& name);

		template<class osgNodeType>
		void getNodeList(std::vector<osgNodeType*> out);
        
		//Change the current root node to the node with the provided name
        bool setRoot(const std::string& name);

		//Prints a list of node names and types properly indented
		void printGraph();
    };

    template<class osgNodeType>
    inline osgNodeType* Loader::getNode(const std::string &name) {
        FindNodeVisitor<osgNodeType> nodeVisitor = FindNodeVisitor<osgNodeType>(name);
        gNode->accept(nodeVisitor);
        return nodeVisitor.getFirst();
    }

	template<class osgNodeType>
	inline void Loader::getNodeList(std::vector<osgNodeType*> out) {
		FindNodeVisitor<osgNodeType> nodeVisitor = FindNodeVisitor<osgNodeType>();
		gNode->accept(nodeVisitor);
		out.push_back(nodeVisitor.getNodeList());
	}

}
#endif
