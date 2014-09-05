//
//  findnodevisitor.hpp
//  MoonBase
//
//  Created by Neglective on 14/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_findnodevisitor_h
#define MoonBase_findnodevisitor_h

#include <osg/NodeVisitor>
#include <osg/Node>

#include <iostream>

//Modify to template different types of nodes
namespace mb {
    template <class osgNodeType>
    class FindNodeVisitor : public osg::NodeVisitor
    {
    public:

        // Default constructor - initialize searchForName to "" and
        // set the traversal mode to TRAVERSE_ALL_CHILDREN
        FindNodeVisitor();

        // Constructor that accepts string argument
        // Initializes searchForName to user string
        // set the traversal mode to TRAVERSE_ALL_CHILDREN
        FindNodeVisitor(const std::string &searchName);

        // The 'apply' method for 'node' type instances.
        // Compare the 'searchForName' data member against the node's name.
        // If the strings match, add this node to our list
        virtual void apply(osgNodeType &searchNode);

        // Set the searchForName to user-defined string
        void setNameToFind(const std::string &searchName);

        // Return a pointer to the first node in the list
        // with a matching name
        osgNodeType* getFirst();

        // typedef a vector of node pointers for convenience
        //    typedef std::vector<osg::Node*> nodeListType;

        // return a reference to the list of nodes we found
        std::vector<osgNodeType*>& getNodeList();

    private:

        // the name we are looking for
        std::string searchForName;
        
        // List of nodes with names that match the searchForName string
        std::vector<osgNodeType*> foundNodeList;
        
    };


    ////////////////////////////////////////
    ////   INLINE DEFINITIONS BECAUSE OF TEMPLATES
    ///////////////////////////////////////////

    template <class osgNodeType>
    inline FindNodeVisitor<osgNodeType>::FindNodeVisitor() : NodeVisitor(TRAVERSE_ALL_CHILDREN){
        searchForName = "";
    }

    template <class osgNodeType>
    inline FindNodeVisitor<osgNodeType>::FindNodeVisitor(const std::string &searchName) : NodeVisitor(TRAVERSE_ALL_CHILDREN) {
        searchForName = searchName;
    }

    template <class osgNodeType>
    inline void FindNodeVisitor<osgNodeType>::apply(osgNodeType &searchNode) {
        //Compare the nodes name with the search name and if it is valid
        if(searchNode.getName() == searchForName)
            foundNodeList.push_back(&searchNode);
		else if (searchForName == "")
			foundNodeList.push_back(&searchNode);


//        std::cout << searchNode.getName() << std::endl;

        traverse(searchNode);
    }

    template <class osgNodeType>
    inline osgNodeType* FindNodeVisitor<osgNodeType>::getFirst() {
        //test if the array is filled otherwise return null
        if (foundNodeList.empty())
            return nullptr;
        else
            return foundNodeList.front();
    }

    template <class osgNodeType>
    inline std::vector<osgNodeType*>& FindNodeVisitor<osgNodeType>::getNodeList() { return foundNodeList; }
}
#endif
