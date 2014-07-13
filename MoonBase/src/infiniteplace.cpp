//
//  infiniteplace.cpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#include <MB/infiniteplane.hpp>

InfinitePlane::InfinitePlane(dWorldID pWorld, dSpaceID pSpace) :
MoonBaseObject(pWorld, pSpace)
{
    //ODE
	pGeom = dCreatePlane(pSpace, 0, 0, 1, 0);

    //OSG
    //Vertice array
    double const size = 2;
    osg::Vec3Array* verts = new osg::Vec3Array();
    verts->push_back( osg::Vec3( -size, -size, 0) );
    verts->push_back( osg::Vec3( -size, size, 0) );
    verts->push_back( osg::Vec3( size, size, 0) );
    verts->push_back( osg::Vec3( size, -size, 0) );
    gGeometry->setVertexArray(verts);

    //Draw primitive
    osg::DrawElementsUInt* primitive = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    primitive->push_back(3);
    primitive->push_back(2);
    primitive->push_back(1);
    primitive->push_back(0);
    gGeometry->addPrimitiveSet(primitive);

    //Set color
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 0 red
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 1 green
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); //index 2 blue
    colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 3 white
    gGeometry->setColorArray(colors);
    gGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

}



