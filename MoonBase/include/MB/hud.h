//
//  hud.h
//  MoonBase
//
//  Created by Fabien FARIN on 23/07/14.
//  Copyright (c) 2014 ___FabienFarin___. All rights reserved.
//

#ifndef __MoonBase__hud__
#define __MoonBase__hud__

#include <iostream>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Projection>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/MatrixTransform>
#include <osgViewer/Viewer>

namespace mb {
    class Hud {
        
        int screenHeight;
        int screenWidth;
        
    public:

        osg::Projection* init();
        void setScreenDimensions(int screenHeight, int screenWidth);
    protected:
        

    };
}

#endif /* defined(__MoonBase__hud__) */
