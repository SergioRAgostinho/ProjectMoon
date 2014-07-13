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

public:

    LoadedObject(dWorldID w, dSpaceID s, std::string path);
};


#endif
