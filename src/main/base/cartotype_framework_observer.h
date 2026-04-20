/*
cartotype_framework_observer.h
Copyright (C) 2021-2022 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_navigation.h>

namespace CartoTypeCore
{

/**
An observer interface which receives notifications
of changes to the framework state. It is intended for the use
of higher-level GUI objects which need to update their display
after framework state has been changed programmatically. For example,
if a route is created, dynamic map objects need to be redrawn.
*/
class MFrameworkObserver: public MNavigatorObserver
    {
    public:
    virtual ~MFrameworkObserver() { }

    /**
    This virtual function is called when the map view changes,
    which can be caused by panning, zooming, rotation, moving to a
    new location, or resizing the map.
    */
    virtual void OnViewChange() { }
    
    /** This virtual function is called when a map is loaded, unloaded, enabled, disabled, or changed by inserting or deleting an object. */
    virtual void OnMapChange() { }

    /** This virtual function is called when the style sheet, style sheet variables, or blend style is changed. */
    virtual void OnStyleChange() { }

    /** This virtual function is called when layers are enabled or disabled, or the drawing of 3D buildings is enabled or disabled. */
    virtual void OnLayerChange() { }

    /** This virtual function is called when the notices such as the legend, scale bar and copyright notice are changed, enabled or disabled. */
    virtual void OnNoticeChange() { }

    /** This virtual function is called when the framework is about to be destroyed. */
    virtual void OnFrameworkDestroyed() { }
    };

}

