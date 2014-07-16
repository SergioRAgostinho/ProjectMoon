//
//  findnodevisitor.hpp
//  MoonBase
//
//  Created by Neglective on 14/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_findnodevisitor_hpp
#define MoonBase_findnodevisitor_hpp

#include <Windows.h>
#include <osg/NodeVisitor>
#include <osg/Node>

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
    virtual void apply(osg::Node &searchNode);

    // Set the searchForName to user-defined string
    void setNameToFind(const std::string &searchName);

    // Return a pointer to the first node in the list
    // with a matching name
    osg::Node* getFirst();

    // typedef a vector of node pointers for convenience
//    typedef std::vector<osg::Node*> nodeListType;

    // return a reference to the list of nodes we found
    std::vector<osg::Node*>& getNodeList();

private:

    // the name we are looking for
    std::string searchForName;

    // List of nodes with names that match the searchForName string
    std::vector<osg::Node*> foundNodeList;

};

#endif
