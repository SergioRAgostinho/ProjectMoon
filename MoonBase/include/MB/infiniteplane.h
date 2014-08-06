//
//  infiniteplane.hpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_infiniteplane_hpp
#define MoonBase_infiniteplane_hpp

#ifdef WIN32
//In windows we can properly compile ODE
#ifndef dDOUBLE
#define dDOUBLE
#endif
#endif

#include <ode/ode.h>

#include <MB/body.h>

namespace mb {
    class InfinitePlane : public Body {


    public:

        //Constructor
        InfinitePlane(dSpaceID space);
        
    };

}

#endif
