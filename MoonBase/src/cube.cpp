#include <MB/cube.h>


Cube::Cube(dWorldID w, dSpaceID s, dReal size)
{
    pWorld = w;
    pSpace = s;

	pBody = dBodyCreate(pWorld);
	pGeom = dCreateBox(pSpace, (dReal)size, (dReal)size, (dReal)size);
	dGeomSetBody(pGeom, pBody);
	

	//Graphics
	gBox = new osg::Box(osg::Vec3(0, 0, 0), (float) size);
	gBoxShape = new osg::ShapeDrawable(gBox.get());
    gGeode->removeDrawable(gGeometry);
	gGeode->addDrawable(gBoxShape.get());
    
    //FIXME: OSG Manipulator
    gDragger = new osgManipulator::TrackballDragger();
    gDragger->setupDefaultGeometry();
    osg::Matrix mat = osg::Matrix::translate(gPAT->getBound().center());
    gDragger->setMatrix(mat);
    gGroup->addChild(gDragger.get());
    
    gSelection = new osgManipulator::Selection();
    gSelection->addChild(gPAT.get());
    gGroup->addChild(gSelection.get());
    
    gCommandManager = new osgManipulator::CommandManager();
    gCommandManager->connect(*(gDragger.get()),*(gSelection.get()));
}


Cube::~Cube()
{
}


osg::Group* Cube::osgGet() { return gGroup.get(); }

void Cube::setOrientationQuat(double x, double y, double z, double w) {
	gPAT->setAttitude(osg::Quat(x, y, z, w));
	dQuaternion q = { (dReal) w, (dReal) x, (dReal) y, (dReal) z };
	dGeomSetQuaternion(pGeom, q);
}

void Cube::setAngularVelocity(double x, double y, double z) { dBodySetAngularVel(pBody, x, y, z); }
