//
//  keyboardeventhandler.hpp
//  MoonBase
//
//  Created by Neglective on 12/07/14.
//  Copyright (c) 2014 WeShallExplode. All rights reserved.
//

#ifndef MoonBase_keyboardeventhandler_hpp
#define MoonBase_keyboardeventhandler_hpp

#include <osgGA/GUIEventHandler>
#include <MB/body.h>
#include <MB/application.h>


namespace mb {

    class KeyboardEventHandler : public osgGA::GUIEventHandler {

        Application* app;

    public:

        KeyboardEventHandler(Application* a);

        virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

        virtual void accept(osgGA::GUIEventHandlerVisitor& v);
        
    };

}

#endif
