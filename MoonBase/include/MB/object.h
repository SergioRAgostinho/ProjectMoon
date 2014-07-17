//
//  object.h
//  MoonBase
//
//  Created by Neglective on 17/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef __MoonBase__object__
#define __MoonBase__object__

#include <osg/Node>
#include <osg/PositionAttitudeTransform>

namespace mb {
    class Object {

    protected:
        //Open Scene Graph
        osg::ref_ptr<osg::Node> gNode;
        osg::ref_ptr<osg::PositionAttitudeTransform> gPAT;

    public:

        Object();
        Object(osg::Node* node);

        //Return pointer to node
        osg::Node* getNode();
        
        //Return pointer to PAT
        osg::PositionAttitudeTransform* getPAT();
    };
}

#endif /* defined(__MoonBase__object__) */
