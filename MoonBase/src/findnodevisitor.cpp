//
//  findnodevisitor.cpp
//  MoonBase
//
//  Created by Neglective on 14/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <MB/findnodevisitor.h>

using namespace mb;

//template <class osgNodeType>
//FindNodeVisitor<osgNodeType>::FindNodeVisitor() : NodeVisitor(TRAVERSE_ALL_CHILDREN){
//    searchForName = "";
//}
//
//template <class osgNodeType>
//FindNodeVisitor<osgNodeType>::FindNodeVisitor(const std::string &searchName) : NodeVisitor(TRAVERSE_ALL_CHILDREN) {
//    searchForName = searchName;
//}
//
//template <class osgNodeType>
//void FindNodeVisitor<osgNodeType>::apply(osgNodeType &searchNode) {
//    //Compare the nodes name with the search name and if it is valid
//    if(searchNode.getName() == searchForName)
//        foundNodeList.push_back(&searchNode);
//
//    traverse(searchNode);
//}
//
//template <class osgNodeType>
//osgNodeType* FindNodeVisitor<osgNodeType>::getFirst() {
//    //test if the array is filled otherwise return null
//    if (foundNodeList.empty())
//        return nullptr;
//    else
//        return foundNodeList.front();
//}
//
//template <class osgNodeType>
//std::vector<osgNodeType*>& FindNodeVisitor<osgNodeType>::getNodeList() { return foundNodeList; }