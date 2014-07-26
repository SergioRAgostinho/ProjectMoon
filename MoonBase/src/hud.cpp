//
//  hud.cpp
//  MoonBase
//
//  Created by Fabien FARIN on 23/07/14.
//  Copyright (c) 2014 ___FabienFarin___. All rights reserved.
//

#include <MB/hud.h>

using namespace mb;

osg::Projection* Hud::init(){
    
    // A geometry node for our HUD:
    osg::Geode* HUDGeode = new osg::Geode();
    // Projection node for defining view frustrum for HUD:
    osg::Projection* HUDProjectionMatrix = new osg::Projection;
    
    // Initialize the projection matrix for viewing everything we
    // will add as descendants of this node. Use screen coordinates
    // to define the horizontal and vertical extent of the projection
    // matrix. Positions described under this node will equate to
    // pixel coordinates.
    HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,screenWidth,0,screenHeight));
    
    // For the HUD model view matrix use an identity matrix:
    osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
    HUDModelViewMatrix->setMatrix(osg::Matrix::identity());
    
    // Make sure the model view matrix is not affected by any transforms
    // above it in the scene graph:
    HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    
    // Add the HUD projection matrix as a child of the root node
    // and the HUD model view matrix as a child of the projection matrix
    // Anything under this node will be viewed using this projection matrix
    // and positioned with this model view matrix.
    HUDProjectionMatrix->addChild(HUDModelViewMatrix);
    
    // Add the Geometry node to contain HUD geometry as a child of the
    // HUD model view matrix.
    HUDModelViewMatrix->addChild( HUDGeode );
    
    // Set up geometry for the HUD and add it to the HUD
    osg::Geometry* HUDBackgroundGeometry = new osg::Geometry();
    
    osg::Vec3Array* HUDBackgroundVertices = new osg::Vec3Array;
    HUDBackgroundVertices->push_back( osg::Vec3(screenWidth/2.0-10,screenHeight/2.0-10,-1) );
    HUDBackgroundVertices->push_back( osg::Vec3(screenWidth/2.0+10,screenHeight/2.0-10,-1) );
    HUDBackgroundVertices->push_back( osg::Vec3(screenWidth/2.0+10,screenHeight/2.0+10,-1) );
    HUDBackgroundVertices->push_back( osg::Vec3(screenWidth/2.0-10,screenHeight/2.0+10,-1) );
    
    osg::DrawElementsUInt* HUDBackgroundIndices =
    new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
    HUDBackgroundIndices->push_back(0);
    HUDBackgroundIndices->push_back(1);
    HUDBackgroundIndices->push_back(2);
    HUDBackgroundIndices->push_back(3);
    
    
    osg::Vec4Array* HUDcolors = new osg::Vec4Array;
    HUDcolors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
    
    
    osg::Vec2Array* texcoords = new osg::Vec2Array(4);
    (*texcoords)[0].set(0.0f,0.0f);
    (*texcoords)[1].set(1.0f,0.0f);
    (*texcoords)[2].set(1.0f,1.0f);
    (*texcoords)[3].set(0.0f,1.0f);
    
    HUDBackgroundGeometry->setTexCoordArray(0,texcoords);
    osg::Texture2D* HUDTexture = new osg::Texture2D;
    HUDTexture->setDataVariance(osg::Object::DYNAMIC);
    osg::Image* hudImage;
    hudImage = osgDB::readImageFile("../res/textures/crosshair_w.png");
    HUDTexture->setImage(hudImage);
    osg::Vec3Array* HUDnormals = new osg::Vec3Array;
    HUDnormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
    HUDBackgroundGeometry->setNormalArray(HUDnormals);
    HUDBackgroundGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
    HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);
    HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);
    HUDBackgroundGeometry->setColorArray(HUDcolors);
    HUDBackgroundGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    
    HUDGeode->addDrawable(HUDBackgroundGeometry);
    
    // Create and set up a state set using the texture from above:
    osg::StateSet* HUDStateSet = new osg::StateSet();
    HUDGeode->setStateSet(HUDStateSet);
    HUDStateSet->
    setTextureAttributeAndModes(0,HUDTexture,osg::StateAttribute::ON);
    
    // For this state set, turn blending on (so alpha texture looks right)
    HUDStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
    
    // Disable depth testing so geometry is draw regardless of depth values
    // of geometry already draw.
    HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    
    // Need to make sure this geometry is draw last. RenderBins are handled
    // in numerical order so set bin number to 11
    HUDStateSet->setRenderBinDetails( 11, "RenderBin");
    
    return HUDProjectionMatrix;
}

void Hud::setScreenDimensions(int height, int width){
    screenHeight = height;
    screenWidth = width;
}