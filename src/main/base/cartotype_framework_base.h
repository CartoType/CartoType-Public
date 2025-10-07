/*
cartotype_framework_base.h
Copyright (C) 2025 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_base.h>
#include <cartotype_graphics_context.h>
#include <cartotype_map_object.h>

namespace CartoTypeCore
{

/** A function called by the asynchronous Find function. It provides the map objects found, if any. */
using FindAsyncCallBack = std::function<void(std::unique_ptr<MapObjectArray> aMapObjectArray)>;
/** A function called by the asynchronous Find function for map object group arrays. It provides the map object groups found, if any. */
using FindAsyncGroupCallBack = std::function<void(std::unique_ptr<MapObjectGroupArray> aMapObjectGroupArray)>;
/** A function called by the asynchronous LoadMap function. It provides the result of an attempted load, and the filename used. */
using LoadMapAsyncCallBack = std::function<void(Result aError,const String& aFileName,const std::string& aKey)>;

/**
A range of map files (CTM1 files) representing tiles.
Used in TileParam.
*/
class TileSet
    {
    public:
    /**
    A std::format template used to create the filename, using integer arguments zoom, x, y in that order.
    The arguments may be reordered using argument indexes: see the std::format documentation for details.
    */
    std::string FileNameTemplate;
    /** The inclusive range of files that are available. */
    Rect TileRange;
    /** The zoom level of these tiles. */
    int FileZoom = 0;
    /** The minimum zoom level at which these tiles are to be drawn. */
    int MinDisplayZoom = KMinTileZoomLevel;
    /** The maximum zoom level at which these tiles are to be drawn. */
    int MaxDisplayZoom = KMaxTileZoomLevel;
    };

/** Parameters specifying map files (CTM1 files) representing tiles to be loaded and unloaded automatically. */
class TileParam
    {
    public:
    /** The file sets. */
    std::vector<TileSet> TileSetArray;
    };

/**
Information about a change to one or more of the map databases used by a Framework, which may have been loaded, unloaded, enabled or disabled, or,
in the case of writable maps, the map data may have been changed by insertions or deletions.
*/
class MapChange
    {
    public:
    /** True if the map has been loaded or unloaded. */
    bool LoadedOrUnloaded = false;
    /** True if the combined extent of all maps has changed. */
    bool CombinedExtentChanged = false;
    };

}
