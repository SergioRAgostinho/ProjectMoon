//
//  memory.h
//  MoonBase
//
//  Created by Neglective on 20/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_memory_h
#define MoonBase_memory_h

namespace mb {
    template< typename T >
    struct array_deleter
    {
        void operator ()( T const * p)
        {
            delete[] p;
        }
    };
}

#endif
