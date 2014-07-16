//
//  keyboardeventhandler.hpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_keyboardeventhandler_hpp
#define MoonBase_keyboardeventhandler_hpp

#include <Windows.h>
#include <osgGA/GUIEventHandler>
#include <MB/moonbaseobject.hpp>

class KeyboardEventHandler : public osgGA::GUIEventHandler {

    MoonBaseObject* obj;

public:

    KeyboardEventHandler(MoonBaseObject* o);

    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

    virtual void accept(osgGA::GUIEventHandlerVisitor& v);

};

#endif
