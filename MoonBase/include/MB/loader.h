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

        osg::Node* getNode(const std::string& name);

        template<class osgNodeType>
        osgNodeType* getNode(const std::string& name);

		template<class osgNodeType>
		void getNodeList(std::vector<osgNodeType*> out);
        
        bool setRoot(const std::string& name);
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
