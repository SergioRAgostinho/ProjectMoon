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
#include <MB/moonbaseobject.hpp>

class LoadedObject : public MoonBaseObject {

    osg::ref_ptr<osg::Node> gNode;

protected:

    void triOGS2ODE();

public:

    LoadedObject(dWorldID w, dSpaceID s, const std::string& path);

    osg::Node* getNode();

    void setPosition(double x, double y, double z);
};


#endif
