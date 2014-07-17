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

namespace mb {


    class Loader : public Object {


    protected:

        bool triOGS2ODE();

    public:

        Loader(const std::string& path);

        osg::Node* getNode(const std::string& name);
        
        bool setRoot(const std::string& name);
    };

}
#endif
