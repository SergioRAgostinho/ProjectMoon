//
//  findnodevisitor.cpp
//  MoonBase
//
//  Created by Neglective on 14/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <MB/findnodevisitor.h>

using namespace mb;

FindNodeVisitor::FindNodeVisitor(){
    FindNodeVisitor("");
}

FindNodeVisitor::FindNodeVisitor(const std::string &searchName) : NodeVisitor(TRAVERSE_ALL_CHILDREN) {
    searchForName = searchName;
}

void FindNodeVisitor::apply(osg::Node &searchNode) {
    //Compare the nodes name with the search name and if it is valid
    if(searchNode.getName() == searchForName)
        foundNodeList.push_back(&searchNode);

    traverse(searchNode);
}


osg::Node* FindNodeVisitor::getFirst() {
    //test if the array is filled otherwise return null
    if (foundNodeList.empty())
        return nullptr;
    else
        return foundNodeList.front();
}

std::vector<osg::Node*>& FindNodeVisitor::getNodeList() { return foundNodeList; }