#include <MB/cube.h>

using namespace mb;

Cube::Cube()
{
	//Graphics
	osg::ref_ptr<osg::Box> gBox = new osg::Box(osg::Vec3(0, 0, 0), 1);
	osg::ref_ptr<osg::ShapeDrawable>  gBoxShape = new osg::ShapeDrawable(gBox.get());
	gGeode->addDrawable(gBoxShape);
}

Cube::Cube(dWorldID w, dSpaceID s, dReal size) : Body(w,s)
{
	pGeom = dCreateBox(pSpace, (dReal)size, (dReal)size, (dReal)size);
    initPhysics(w, s);

	//Graphics
	osg::ref_ptr<osg::Box> gBox = new osg::Box(osg::Vec3(0, 0, 0), (float) size);
	osg::ref_ptr<osg::ShapeDrawable>  gBoxShape = new osg::ShapeDrawable(gBox.get());
	gGeode->addDrawable(gBoxShape);
    
//    //FIXME: OSG Manipulator
//    gDragger = new osgManipulator::TrackballDragger();
//    gDragger->setupDefaultGeometry();
//    osg::Matrix mat = osg::Matrix::translate(gPAT->getBound().center());
//    gDragger->setMatrix(mat);
//    gGroup->addChild(gDragger.get());
//    
//    gSelection = new osgManipulator::Selection();
//    gSelection->addChild(gPAT.get());
//    gGroup->addChild(gSelection.get());
//    
//    gCommandManager = new osgManipulator::CommandManager();
//    gCommandManager->connect(*(gDragger.get()),*(gSelection.get()));
}



