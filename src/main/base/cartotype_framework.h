/*
cartotype_framework.h
Copyright (C) 2012-2025 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_address.h>
#include <cartotype_bitmap.h>
#include <cartotype_find_param.h>
#include <cartotype_navigation.h>
#include <cartotype_stream.h>
#include <cartotype_string.h>
#include <cartotype_graphics_context.h>
#include <cartotype_legend.h>
#include <cartotype_style_sheet_data.h>
#include <cartotype_map_metadata.h>
#include <cartotype_framework_observer.h>
#include <cartotype_feature_info.h>
#include <cartotype_framework_base.h>

#include <memory>
#include <set>
#include <atomic>
#include <mutex>

namespace CartoTypeCore
{

/**
\mainpage CartoType

\section introduction Introduction

CartoType is a portable C++ library for drawing maps, calculating routes
and providing navigation instructions. It uses map files created using the
makemap tool supplied with the package.

Your application can get access to the CartoType API by including
the header cartotype.h. All classes and other declarations are in the
CartoType namespace.

\section highlevelapi The Framework API

The Framework class is the main API for CartoType and allows
you to build CartoType into your application.

You create a single Framework
object and keep it in existence while your program is running.
It provides access to everything you need, including
map drawing, adding your own data to the map, handling user
interaction, searching, route calculation and turn-by-turn navigation.
*/

class CEngine;
class CPerspectiveGraphicsContext;
class CMap;
class CMapDrawParam;
class CMapDataBase;
class CMapDataBaseArray;
class CMapStore;
class CMapStyle;
class CMemoryMapDataBase;
class CNavigator;
class GraphicsContext;
class C32BitColorBitmapGraphicsContext;
class CTileServer;
class MapTransform;
class CMapRendererImplementation;
class CAsyncFinder;
class CNavigatorFuture;
class CMapObjectEditor;
class MFrameworkObserver;
class FrameworkEngine;
class FrameworkGraphics;
class FrameworkMapDataSet;
class AutoMapLoader;

namespace Router
    {
    class TRoutePointInternal;
    }

/** A flag to make the center of the map follow the user's location. */
constexpr uint32_t KFollowFlagLocation = 1;
/** A flag to rotate the map to the user's heading. */
constexpr uint32_t KFollowFlagHeading = 2;
/** A flag to set the map to a suitable zoom level for the user's speed. */
constexpr uint32_t KFollowFlagZoom = 4;

/** Flags controlling the way the map follows the user location and heading and automatically zooms. */
enum class FollowMode
    {
    /** The map does not follow the user's location or heading. */
    None = 0,
    /** The map is centred on the user's location. */
    Location = KFollowFlagLocation,
    /** The map is centred on the user's location and rotated to the user's heading. */
    LocationHeading = KFollowFlagLocation | KFollowFlagHeading,
    /** The map is centred on the user's location and zoomed to a suitable level for the user's speed. */
    LocationZoom = KFollowFlagLocation | KFollowFlagZoom,
    /** The map is centred on the user's location, rotated to the user's heading, and zoomed to a suitable level for the user's speed. */
    LocationHeadingZoom = KFollowFlagLocation | KFollowFlagHeading | KFollowFlagZoom
    };

/** Parameters giving detailed control of the perspective view. */
class PerspectiveParam
    {
    public:
    PerspectiveParam() = default;
    explicit PerspectiveParam(const char* aText);
    Result ReadFromXml(const char* aText);
    std::string ToXml() const;

    /** The equality operator. */
    bool operator==(const PerspectiveParam& aP) const
        {
        return std::forward_as_tuple(PositionDegrees,AutoPosition,HeightMeters,AzimuthDegrees,AutoAzimuth,DeclinationDegrees,RotationDegrees,FieldOfViewDegrees) ==
               std::forward_as_tuple(aP.PositionDegrees,aP.AutoPosition,aP.HeightMeters,aP.AzimuthDegrees,aP.AutoAzimuth,aP.DeclinationDegrees,aP.RotationDegrees,aP.FieldOfViewDegrees);
        }
    /** The inequality operator. */
    bool operator!=(const PerspectiveParam& aP) const
        {
        return !(*this == aP);
        }
    /** The less-than operator. */
    bool operator<(const PerspectiveParam& aP) const
        {
        return std::forward_as_tuple(PositionDegrees,AutoPosition,HeightMeters,AzimuthDegrees,AutoAzimuth,DeclinationDegrees,RotationDegrees,FieldOfViewDegrees) <
               std::forward_as_tuple(aP.PositionDegrees,aP.AutoPosition,aP.HeightMeters,aP.AzimuthDegrees,aP.AutoAzimuth,aP.DeclinationDegrees,aP.RotationDegrees,aP.FieldOfViewDegrees);
        }

    /** The position of the point on the terrain below the camera, in degrees longitude (x) and latitude (y). */
    PointFP PositionDegrees;
    /** If true, ignore iPosition, and set the camera position so that the location in the center of the display is shown. */
    bool AutoPosition = true;
    /** The height of the camera above the terrain. The value 0 causes a default value to be used, which is the width of the display. */
    double HeightMeters = 0;
    /** The azimuth of the camera in degrees going clockwise, where 0 is N, 90 is E, etc. */
    double AzimuthDegrees = 0;
    /** If true, ignore iAzimuthDegrees and use the current map orientation. */
    bool AutoAzimuth = true;
    /**
    The declination of the camera downward from the horizontal plane.
    The value 90 is equivalent to looking straight downwards, and perspective is apparent only in 3D buildings.
    Values are clamped to the range 25...90 when creating the perspective transform.
    Smaller declinations cause performance problems and integer overflow in coordinate conversion.
    */
    double DeclinationDegrees = 30;
    /** The amount by which the camera is rotated about its axis, after applying the declination, in degrees going clockwise. */
    double RotationDegrees = 0;
    /** The camera's field of view in degrees. */
    double FieldOfViewDegrees = 22.5;
    };

/** The view state, which can be retrieved or set so that maps can be recreated. */
class ViewState
    {
    public:
    ViewState() = default;
    explicit ViewState(const char* aXmlText);
    Result ReadFromXml(const char* aXmlText);
    bool operator==(const ViewState& aOther) const;
    bool operator<(const ViewState& aOther) const;
    /** The inequality operator. */
    bool operator!=(const ViewState& aOther) const { return !(*this == aOther); };
    std::string ToXml() const;

    /** The display width in pixels. */
    int32_t WidthInPixels = 256;
    /** The display height in pixels. */
    int32_t HeightInPixels = 256;
    /** The view center in degrees longitude (x) and latitude (y). */
    PointFP ViewCenterDegrees;
    /** The denominator of the scale fraction; e.g., 50000 for 1:50000. */
    double ScaleDenominator = 0;
    /** The clockwise rotation of the view in degrees. */
    double RotationDegrees = 0;
    /** True if perspective mode is on. */
    bool Perspective = false;
    /** The parameters to be used for perspective mode. */
    CartoTypeCore::PerspectiveParam PerspectiveParam;

    private:
    auto Tuple() const { return std::forward_as_tuple(WidthInPixels,HeightInPixels,ViewCenterDegrees,ScaleDenominator,RotationDegrees,Perspective,PerspectiveParam); }
    };

/** A type for a sequence of track points. */
using TrackGeometry = GeneralGeometry<TrackPoint>;

/**
The Framework class provides a high-level CartoType API
to draw maps and perform routing, navigation and geocoding.
*/
class Framework: public MNavigatorObserver
    {
    public:
    static std::unique_ptr<Framework> New(Result& aError,
                                          const String& aMapFileName,
                                          const String& aStyleSheetFileName,
                                          const String& aFontFileName,
                                          int32_t aViewWidth,int32_t aViewHeight,
                                          const std::string* aKey = nullptr);

    ~Framework();

    /**
    Parameters for creating a framework.
    The only parameter that is mandatory is MapFileName.

    If both StyleSheetFileName and StyleSheetText are empty maps cannot be drawn
    but geocoding (finding addresses and creating addresses from points) can be performed.
    */
    class Param
        {
        public:
        /** The map. Must not be empty. It may be a filename or a URL. */
        String MapFileName;
        /** The style sheet file name. If this string is empty, the style sheet must be supplied in StyleSheetText. */
        String StyleSheetFileName;
        /** The style sheet text; used if StyleSheetFileName is empty. If no style sheet text is supplied an empty style sheet is used and the framework cannot be used to draw maps. */
        std::string StyleSheetText;
        /** The first font file. If this is empty, a small built-in font is loaded containing the Roman script only. */
        String FontFileName;
        /** The width of the initial map in pixels. Must be greater than zero. */
        int32_t ViewWidth = 256;
        /** The height of the initial map in pixels. Must be greater than zero. */
        int32_t ViewHeight = 256;
        /** If not empty, an API key or encryption key to be used when loading the map. */
        std::string Key;
        /** The file buffer size in bytes. If it is less than four the default value is used. */
        int32_t FileBufferSizeInBytes = 0;
        /** The maximum number of file buffers. If it is zero or less the default value is used. */
        int32_t MaxFileBufferCount = 0;
        /**
        The number of levels of the text index to load into memory.
        Use values from 2 to 5 to make text searches faster, at the cost of using more RAM.
        The value 0 causes the default number of levels (3) to be loaded, allowing fast text searching without using too much RAM.
        The value -1 disables text index loading.
        */
        int32_t TextIndexLevels = 0;
        };
    static std::unique_ptr<Framework> New(Result& aError,const Param& aParam);

    std::unique_ptr<Framework> Copy(Result& aError,bool aCopyTextIndex = true,bool aCopyFonts = true) const;

    // general
    Result License(const String& aKey);
    Result License(const uint8_t* aKey,size_t aKeyLength);
    Result SetProviderParam(const std::string& aKey,const std::string& aValue);
    std::string Licensee() const;
    std::string ExpiryDate() const;
    std::string AppBuildDate() const;
    uint8_t License() const;
    String Copyright() const;
    Result SetCopyrightNotice();
    Result SetCopyrightNotice(const String& aCopyright);
    Result SetLegend(const NoticePosition& aPosition,std::unique_ptr<Legend> aLegend = nullptr);
    Result SetLegend(NoticeAnchor aAnchor,double aWidth,const char* aUnit);
    void EnableLegend(bool aEnable);
    bool LegendEnabled() const;
    Result SetScaleBar(const NoticePosition& aPosition,std::unique_ptr<Legend> aLegend = nullptr);
    Result SetScaleBar(NoticeAnchor aAnchor,double aWidth,const char* aUnit);
    void EnableScaleBar(bool aEnable);
    bool ScaleBarEnabled() const;
    Result SetTurnInstructions(const NoticePosition& aPosition,std::unique_ptr<Legend> aLegend = nullptr,bool aAbbreviate = false);
    Result SetTurnInstructions(NoticeAnchor aAnchor,double aWidth,const char* aWidthUnit,double aTextSize = 0,const char* aTextSizeUnit = nullptr,bool aAbbreviate = false);
    void EnableTurnInstructions(bool aEnable);
    bool TurnInstructionsEnabled() const;
    void SetTurnInstructionText(const String& aText);
    String TurnInstructionText();
    void DrawNoticesAutomatically(bool aEnable);
    bool HasNotices() const;
    PositionedBitmap NoticeBitmap() const;
    Result Configure(const String& aFilename);
    Result LoadMap(const String& aMapFileName,const std::string* aKey = nullptr);
    Result LoadMapsAutomatically(const TileParam& aParam);
    void CancelAutomaticMapLoading();
    Result LoadMapAsync(LoadMapAsyncCallBack aCallback,const String& aMapFileName,const std::string& aKey);
    Result CreateWritableMap(WritableMapType aType,String aFileName = nullptr);
    Result SaveMap(uint32_t aHandle,const String& aFileName,FileType aFileType);
    Result ReadMap(uint32_t aHandle,const String& aFileName,FileType aFileType);
    Result SaveMap(uint32_t aHandle,std::vector<uint8_t>& aData,const FindParam& aFindParam);
    Result ReadMap(uint32_t aHandle,const std::vector<uint8_t>& aData);
    Result WriteMapImage(const String& aFileName,FileType aFileType,bool aCompress = false);
    bool MapIsEmpty(uint32_t aHandle);
    size_t MapCount() const;
    uint32_t MapHandle(size_t aIndex) const;
    bool MapIsWritable(size_t aIndex) const;
    std::unique_ptr<CartoTypeCore::MapMetaData> MapMetaData(size_t aIndex) const;
    Result UnloadMapByHandle(uint32_t aHandle);
    Result EnableMapByHandle(uint32_t aHandle,bool aEnable);
    Result EnableAllMaps();
    bool MapIsEnabled(uint32_t aHandle);
    uint32_t LastMapHandle() const;
    uint32_t MainMapHandle() const;
    uint32_t MemoryMapHandle() const;
    Result LoadFont(const String& aFontFileName);
    Result LoadFont(const uint8_t* aData,size_t aLength,bool aCopyData);
    Result SetStyleSheet(const String& aStyleSheetFileName,size_t aIndex = 0);
    Result SetStyleSheet(const char* aData,size_t aLength,size_t aIndex = 0);
    Result SetStyleSheet(const StyleSheetData& aStyleSheetData,size_t aIndex = 0);
    Result SetStyleSheet(const StyleSheetDataArray& aStyleSheetDataArray,const StringDictionary* aStyleSheetVariables = nullptr,const BlendStyleSet* aBlendStyleSet = nullptr);
    Result ReloadStyleSheet(size_t aIndex = 0);
    Result AppendStyleSheet(const String& aStyleSheetFileName);
    Result AppendStyleSheet(const char* aData,size_t aLength);
    Result DeleteStyleSheet(size_t aIndex);
    std::string StyleSheetText(size_t aIndex) const;
    CartoTypeCore::StyleSheetData StyleSheetData(size_t aIndex) const;
    const CartoTypeCore::StyleSheetDataArray& StyleSheetDataArray() const;
    const StringDictionary& StyleSheetVariables() const;
    bool SetNightMode(bool aSet);
    Color SetNightModeColor(Color aColor);
    bool NightMode() const;
    Color NightModeColor() const;
    CartoTypeCore::BlendStyleSet SetBlendStyle(const BlendStyleSet* aBlendStyleSet);
    CartoTypeCore::BlendStyleSet BlendStyleSet() const;
    FileLocation StyleSheetErrorLocation() const;

    Result Resize(int32_t aViewWidth,int32_t aViewHeight);
    void SetResolutionDpi(double aDpi);
    double ResolutionDpi() const;
    Result SetScaleDenominator(double aScale);
    double ScaleDenominator() const;
    Result SetScaleDenominatorInView(double aScale);
    double ScaleDenominatorInView() const;
    double DistanceInMeters(double aX1,double aY1,double aX2,double aY2,CoordType aCoordType) const;
    double ScaleDenominatorFromZoomLevel(double aZoomLevel,int32_t aImageSizeInPixels = 256) const;
    double ZoomLevelFromScaleDenominator(double aScaleDenominator,int32_t aImageSizeInPixels = 256) const;
    std::pair<double,double> DistortionAtPoint(const PointFP& aPoint,CoordType aCoordType) const;

    void SetPerspective(bool aSet);
    void SetPerspectiveParam(const PerspectiveParam& aParam);
    bool Perspective() const;
    CartoTypeCore::PerspectiveParam PerspectiveParam() const;
    Result Zoom(double aZoomFactor);
    Result ZoomAt(double aZoomFactor,double aX,double aY,CoordType aCoordType);
    Result Rotate(double aAngle);
    Result RotateAt(double aAngle,double aX,double aY,CoordType aCoordType);
    Result SetRotation(double aAngle);
    Result SetRotationAt(double aAngle,double aX,double aY,CoordType aCoordType);
    double Rotation() const;
    Result RotateAndZoom(double aAngle,double aZoomFactor,double aX,double aY,CoordType aCoordType);
    Result Pan(int32_t aDx,int32_t aDy);
    Result Pan(const Point& aFrom,const Point& aTo);
    Result Pan(double aFromX,double aFromY,CoordType aFromCoordType,double aToX,double aToY,CoordType aToCoordType);
    Result SetViewCenter(double aX,double aY,CoordType aCoordType);
    Result SetView(double aMinX,double aMinY,double aMaxX,double aMaxY,CoordType aCoordType,int32_t aMarginInPixels = 0,int32_t aMinScaleDenominator = 0);
    Result SetView(const MapObject* const* aMapObjectArray,size_t aMapObjectCount,int32_t aMarginInPixels,int32_t aMinScaleDenominator);
    Result SetView(const MapObjectArray& aMapObjectArray,int32_t aMarginInPixels,int32_t aMinScaleDenominator);
    Result SetView(const MapObject& aMapObject,int32_t aMarginInPixels,int32_t aMinScaleDenominator);
    Result SetView(const Geometry& aGeometry,int32_t aMarginInPixels,int32_t aMinScaleDenominator);
    Result SetView(const ViewState& aViewState);
    Result SetViewToRoute(int32_t aMarginInPixels,int32_t aMinScaleDenominator);
    Result SetViewToWholeMap();
    Result GetView(double& aMinX,double& aMinY,double& aMaxX,double& aMaxY,CoordType aCoordType) const;
    Result GetView(RectFP& aView,CoordType aCoordType) const;
    Result GetView(FixedSizeContourFP<4,true>& aView,CoordType aCoordType) const;
    Result GetMapExtent(double& aMinX,double& aMinY,double& aMaxX,double& aMaxY,CoordType aCoordType) const;
    Result GetMapExtent(RectFP& aMapExtent,CoordType aCoordType) const;
    String ProjectionAsProj4Param() const;
    CartoTypeCore::ViewState ViewState() const;
    Result SetViewLimits(double aMinScaleDenominator,double aMaxScaleDenominator,const Geometry& aGeometry);
    Result SetViewLimits(double aMinScaleDenominator = 0,double aMaxScaleDenominator = 0);

    Result InsertMapObject(uint32_t aMapHandle,const String& aLayerName,const Geometry& aGeometry,
                           const String& aStringAttributes,FeatureInfo aFeatureInfo,uint64_t& aId,bool aReplace);
    Result InsertPointMapObject(uint32_t aMapHandle,const String& aLayerName,double aX,double aY,
                                CoordType aCoordType,const String& aStringAttributes,FeatureInfo aFeatureInfo,uint64_t& aId,bool aReplace);
    Result InsertCircleMapObject(uint32_t aMapHandle,const String& aLayerName,
                                 double aCenterX,double aCenterY,CoordType aCenterCoordType,double aRadius,CoordType aRadiusCoordType,
                                 const String& aStringAttributes,FeatureInfo aFeatureInfo,uint64_t& aId,bool aReplace);
    Result InsertEllipseMapObject(uint32_t aMapHandle,const String& aLayerName,
                                  double aCenterX,double aCenterY,CoordType aCenterCoordType,double aRadiusX,double aRadiusY,CoordType aRadiusCoordType,double aRotationDegrees,
                                  const String& aStringAttributes,FeatureInfo aFeatureInfo,uint64_t& aId,bool aReplace);
    Result InsertEnvelopeMapObject(uint32_t aMapHandle,const String& aLayerName,const Geometry& aGeometry,
                                   double aRadius,CoordType aRadiusCoordType,
                                   const String& aStringAttributes,FeatureInfo aFeatureInfo,uint64_t& aId,bool aReplace);
    Result InsertPushPin(double aX,double aY,CoordType aCoordType,const String& aStringAttrib,const String& aColor,int32_t aIconCharacter,uint64_t& aId);
    Result InsertCopyOfMapObject(uint32_t aMapHandle,const String& aLayerName,const MapObject& aObject,double aEnvelopeRadius,CoordType aRadiusCoordType,uint64_t& aId,bool aReplace,
                                 const String* aStringAttributes = nullptr,const FeatureInfo* aFeatureInfo = nullptr);
    Result DeleteMapObjects(uint32_t aMapHandle,uint64_t aStartId,uint64_t aEndId,uint64_t& aDeletedCount,String aCondition = nullptr);
    std::unique_ptr<MapObject> LoadMapObject(Result& aError,uint32_t aMapHandle,uint64_t aId);
    Result ReadGpx(uint32_t aMapHandle,const String& aFileName);
    Geometry Range(Result& aError,const CartoTypeCore::RouteProfile* aProfile,double aX,double aY,CoordType aCoordType,double aTimeOrDistance,bool aIsTime);
    CartoTypeCore::TimeAndDistanceMatrix TimeAndDistanceMatrix(Result& aError,const std::vector<PointFP>& aFrom,const std::vector<PointFP>& aTo,CoordType aCoordType,
                                                               ProgressCallBack* aProgressCallBack = nullptr);
    bool MutuallyAccessible(Result& aError,const std::vector<PointFP>& aPointArray,CoordType aCoordType,std::vector<uint32_t>* aGroupArray = nullptr);

    void EnableLayer(const String& aLayerName,bool aEnable);
    bool LayerIsEnabled(const String& aLayerName) const;
    void SetDisabledLayersInternal(const std::set<String>& aLayerNames);
    std::vector<String> LayerNames();

    Result ConvertCoords(double* aCoordArray,size_t aCoordArraySize,CoordType aFromCoordType,CoordType aToCoordType) const;
    Result ConvertCoords(const WritableCoordSet& aCoordSet,CoordType aFromCoordType,CoordType aToCoordType) const;
    /** Converts the coordinates of a geometry object between any combination of lat/long, map coordinates and display pixels. */
    template<class T> Result ConvertCoords(GeneralGeometry<T>& aGeometry,CoordType aToCoordType)
        {
        if (aGeometry.CoordType() == aToCoordType)
            return KErrorNone;
        size_t contour_count = aGeometry.ContourCount();
        for (size_t i = 0; i < contour_count; i++)
            {
            WritableCoordSet cs{ aGeometry.CoordSet(i) };
            Result error = ConvertCoords(cs,aGeometry.CoordType(),aToCoordType);
            if (error)
                return error;
            }
        return KErrorNone;
        }
    Result ConvertPoint(double& aX,double& aY,CoordType aFromCoordType,CoordType aToCoordType) const;
    double PixelsToMeters(double aPixels) const;
    double MetersToPixels(double aMeters) const;
    String DataSetName() const;

    // interactive editing of map objects
    Result EditSetWritableMap(uint32_t aMapHandle);
    Result EditNewLineObject(const PointFP& aDisplayPoint);
    Result EditNewPolygonObject(const PointFP& aDisplayPoint);
    Result EditMoveCurrentPoint(const PointFP& aDisplayPoint);
    Result EditAddCurrentPoint();
    Result EditDeleteCurrentPoint();
    Result EditDeleteCurrentObject();
    Result EditSelectNearestPoint(const PointFP& aDisplayPoint,double aRadiusInMillimeters);
    Result EditInsertCurrentObject(const String& aLayer,uint64_t& aId,bool aReplace);
    Result EditSetCurrentObjectStringAttribute(const String& aKey,const String& aValue);
    Result EditSetCurrentObjectFeatureInfo(FeatureInfo aFeatureInfo);
    Result EditGetCurrentObjectAreaAndLength(double& aArea,double& aLength) const;

    // drawing the map
    const BitmapView* MapBitmap(Result& aError,bool* aRedrawWasNeeded = nullptr);
    void DrawNotices(GraphicsContext& aGc) const;
    void CancelDrawing();
    bool ClipBackgroundToMapBounds(bool aEnable);
    bool DrawBackground(bool aEnable);
    int32_t SetTileOverSizeZoomLevels(int32_t aLevels);
    Result DrawLabelsToLabelHandler(MLabelHandler& aLabelHandler);
    bool ObjectWouldBeDrawn(Result& aError,uint64_t aId,MapObjectType aType,const String& aLayer,FeatureInfo aFeatureInfo,const String& aStringAttrib);
    bool SetDraw3DBuildings(bool aEnable);
    bool Draw3DBuildings() const;
    bool SetAnimateTransitions(bool aEnable);
    bool AnimateTransitions() const;
    double SetLabelFPS(double aFPS);
    double LabelFPS() const;
    bool SetFixedLabels(bool aSet);
    bool FixedLabels();

    // adding and removing style sheet icons loaded from files
    Result LoadIcon(const String& aFileName,const String& aId,const Point& aHotSpot,const Point& aLabelPos);
    void UnloadIcon(const String& aId);

    // drawing tiles
    Bitmap TileBitmap(Result& aError,int32_t aTileSizeInPixels,int32_t aZoom,int32_t aX,int32_t aY,const TileBitmapParam* aParam = nullptr);
    Bitmap TileBitmap(Result& aError,int32_t aTileSizeInPixels,const String& aQuadKey,const TileBitmapParam* aParam = nullptr);
    Bitmap TileBitmap(Result& aError,int32_t aTileWidth,int32_t aTileHeight,const RectFP& aBounds,CoordType aCoordType,const TileBitmapParam* aParam = nullptr);

    // finding map objects
    Result Find(MapObjectArray& aObjectArray,const FindParam& aFindParam) const;
    Result Find(MapObjectGroupArray& aObjectGroupArray,const FindParam& aFindParam) const;
    Result FindInDisplay(MapObjectArray& aObjectArray,size_t aMaxObjectCount,double aX,double aY,double aRadius) const;
    Result FindInLayer(MapObjectArray& aObjectArray,size_t aMaxObjectCount,
                        const String& aLayer,double aMinX,double aMinY,double aMaxX,double aMaxY,CoordType aCoordType) const;
    Result FindText(MapObjectArray& aObjectArray,size_t aMaxObjectCount,const String& aText,
                     StringMatchMethod aMatchMethod,const String& aLayers,const String& aAttribs) const;
    Result FindAddress(MapObjectArray& aObjectArray,size_t aMaxObjectCount,const Address& aAddress,bool aFuzzy = false) const;
    Result FindPolygonsContainingPath(MapObjectArray& aObjectArray,const Geometry& aPath,const FindParam* aParam = nullptr) const;
    Result FindPointsInPath(MapObjectArray& aObjectArray,const Geometry& aPath,const FindParam* aParam = nullptr) const;
    Result FindAsync(const FindAsyncCallBack& aCallBack,const FindParam& aFindParam,bool aOverride = false);
    Result FindAsync(const FindAsyncGroupCallBack& aCallBack,const FindParam& aFindParam,bool aOverride = false);
    Result FindAddressAsync(const FindAsyncCallBack& aCallBack,size_t aMaxObjectCount,const Address& aAddress,bool aFuzzy = false,bool aOverride = false);

    // geocoding
    Result GetGeoCodeSummary(String& aSummary,const MapObject& aMapObject) const;
    Result GetGeoCodeSummary(String& aSummary,double aX,double aY,CoordType aCoordType) const;
    Result GetAddress(Address& aAddress,const MapObject& aMapObject) const;
    Result GetAddressFast(Address& aAddress,const MapObject& aMapObject) const;
    Result GetAddress(Address& aAddress,double aX,double aY,CoordType aCoordType,bool aFullAddress = true) const;

    // terrain heights
    std::vector<int32_t> Heights(Result& aError,const CoordSet& aCoordSet,CoordType aCoordType) const;

    // style sheet variables
    void SetStyleSheetVariable(const String& aVariableName,const String& aValue);
    void SetStyleSheetVariable(const String& aVariableName,int32_t aValue);

    // access to graphics
    std::unique_ptr<GraphicsContext> CreateGraphicsContext(int32_t aWidth,int32_t aHeight);
    CartoTypeCore::Font Font(const FontSpec& aFontSpec);
    std::shared_ptr<GraphicsContext> MapGraphicsContext();

    /** The default size of the cache used by the image server. */
    static constexpr size_t KDefaultImageCacheSize = 10 * 1024 * 1024;

    // navigation

    /** The maximum number of alternative routes that can be displayed simultaneously. */
    static constexpr size_t KMaxRoutesDisplayed = 16;    // allow a number of alternative routes well in excess of the expected maximum, which is probably 3

    void SetPreferredRouterType(RouterType aRouterType);
    RouterType PreferredRouterType() const;
    RouterType ActualRouterType() const;
    Result StartNavigation(double aStartX,double aStartY,CoordType aStartCoordType,
                            double aEndX,double aEndY,CoordType aEndCoordType);
    Result StartNavigation(const RouteCoordSet& aCoordSet);
    Result StartNavigationDeferred(const RouteCoordSet& aCoordSet);
    Result StartNavigation(const CoordSet& aCoordSet,CoordType aCoordType);
    void EndNavigation();
    bool EnableNavigation(bool aEnable);
    bool NavigationEnabled() const;
    Result LoadNavigationData();
    bool NavigationDataHasGradients() const;
    const CartoTypeCore::RouteProfile& RouteProfile() const;
    Result SetRouteProfile(const CartoTypeCore::RouteProfile& aProfile);
    Result SetRouteProfileType(CartoTypeCore::RouteProfileType aProfileType);
    uint32_t BuiltInRouteProfileCount();
    const CartoTypeCore::RouteProfile* BuiltInRouteProfile(uint32_t aIndex);
    Result SetBuiltInRouteProfile(uint32_t aIndex);
    uint32_t BuiltInRouteProfileIndex() const;
    bool Navigating() const;
    void AddObserver(std::weak_ptr<MFrameworkObserver> aObserver);
    void RemoveObserver(std::weak_ptr<MFrameworkObserver> aObserver);
    Point RouteStart() const;
    Point RouteEnd() const;
    Result DisplayRoute(bool aEnable = true);
    Result DisplayNewRoute();
    std::shared_ptr<CartoTypeCore::Route> Route() const;
    std::unique_ptr<CartoTypeCore::Route> CreateRoute(Result& aError,const CartoTypeCore::RouteProfile& aProfile,const RouteCoordSet& aCoordSet);
    std::unique_ptr<CartoTypeCore::Route> CreateRoute(Result& aError,const CartoTypeCore::RouteProfile& aProfile,const CoordSet& aCoordSet,CoordType aCoordType);
    std::unique_ptr<CartoTypeCore::Route> CreateBestRoute(Result& aError,const CartoTypeCore::RouteProfile& aProfile,const RouteCoordSet& aCoordSet,bool aStartFixed,bool aEndFixed,uint32_t aIterations);
    std::unique_ptr<CartoTypeCore::Route> CreateBestRoute(Result& aError,const CartoTypeCore::RouteProfile& aProfile,const CoordSet& aCoordSet,CoordType aCoordType,bool aStartFixed,bool aEndFixed,uint32_t aIterations);
    std::unique_ptr<CartoTypeCore::Route> CreateRouteFromXml(Result& aError,const CartoTypeCore::RouteProfile& aProfile,const String& aFileNameOrData);
    Result CreateRouteAsync(const RouterAsyncCallBack& aCallBack,const CartoTypeCore::RouteProfile& aProfile,const RouteCoordSet& aCoordSet,bool aOverride);
    Result CreateBestRouteAsync(const RouterAsyncCallBack& aCallBack,const CartoTypeCore::RouteProfile& aProfile,const RouteCoordSet& aCoordSet,bool aStartFixed,bool aEndFixed,uint32_t aIterations,bool aOverride);
    String RouteInstructions(const CartoTypeCore::Route& aRoute) const;
    Result UseRoute(std::shared_ptr<CartoTypeCore::Route> aRoute);
    Result ReadRouteFromXml(const String& aFileNameOrData);
    Result WriteRouteAsXml(const CartoTypeCore::Route& aRoute,const String& aFileName,FileType aFileType = FileType::CTROUTE) const;
    Result WriteRouteAsXmlString(const CartoTypeCore::Route& aRoute,std::string& aXmlString,FileType aFileType = FileType::CTROUTE) const;
    std::shared_ptr<RouteSegment> CurrentRouteSegment() const;
    std::shared_ptr<RouteSegment> NextRouteSegment() const;
    Result ReverseRoute();
    Result DeleteRoute();
    CartoTypeCore::RouteCreationData RouteCreationData() const;
    Result Navigate(const NavigationData& aNavData);
    const NavigatorTurn& FirstTurn() const;
    const NavigatorTurn& SecondTurn() const;
    const NavigatorTurn& ContinuationTurn() const;
    CartoTypeCore::NavigationState NavigationState() const;
    String VoiceInstruction() const;
    void SetNavigatorParam(const CartoTypeCore::NavigatorParam& aParam);
    CartoTypeCore::NavigatorParam NavigatorParam() const;
    void SetLocationMatchParam(const CartoTypeCore::LocationMatchParam& aParam);
    CartoTypeCore::LocationMatchParam LocationMatchParam() const;
    void SetNavigationMinimumFixDistance(int32_t aMeters);
    void SetNavigationTimeOffRouteTolerance(int32_t aSeconds);
    void SetNavigationDistanceOffRouteTolerance(int32_t aMeters);
    uint32_t SetNearbyObjectWarning(Result& aError,uint32_t aId,const String& aLayer,const String& aCondition,double aMaxDistance,size_t aMaxObjectCount);
    uint32_t SetVehicleTypeWarning(Result& aError,double aMaxDistance,size_t aMaxObjectCount);
    bool DeleteNearbyObjectWarning(uint32_t aId);
    bool ClearNearbyObjectWarnings();
    MapObjectArray CopyNearbyObjects();
    double DistanceToDestination();
    double EstimatedTimeToDestination();
    Result FindNearestRoad(NearestRoadInfo& aInfo,double aX,double aY,CoordType aCoordType,double aHeadingInDegrees,bool aDisplayPosition);
    Result DisplayPositionOnNearestRoad(const NavigationData& aNavData,NearestRoadInfo* aInfo = nullptr);
    void SetVehiclePosOffset(double aXOffset,double aYOffset);
    Result SetFollowMode(CartoTypeCore::FollowMode aFollowMode);
    CartoTypeCore::FollowMode FollowMode() const;
    Result SetFollowScale(double aMinScale,double aSlowScale,double aFastScale,double aMaxScale);
    Result GetNavigationPosition(PointFP& aPos,CoordType aCoordType) const;
    Result GetNavigationData(NavigationData& aData,double& aHeading) const;

    // locales
    void SetLocale(const char* aLocale);
    std::string Locale() const;
    void SetMetricUnits(bool aMetricUnits);
    bool MetricUnits() const;

    // locale-dependent and configuration-dependent string handling
    void AppendDistance(MString& aString,double aDistanceInMeters,bool aAbbreviate = false) const;
    String DistanceToString(double aDistanceInMeters,bool aAbbreviate = false) const;
    void AppendTime(MString& aString,double aTimeInSeconds) const;
    String TimeToString(double aTimeInSeconds) const;
    void SetCase(MString& aString,LetterCase aCase) const;
    void AbbreviatePlacename(MString& aString) const;

    // traffic information and general location referencing
    Result AddTrafficInfo(uint64_t& aId,const TrafficInfo& aTrafficInfo,LocationRef& aLocationRef);
    double TrafficInfoSpeed(double aX,double aY,CoordType aCoordType,uint32_t aVehicleTypes);
    Result AddPolygonSpeedLimit(uint64_t& aId,const Geometry& aPolygon,double aSpeed,uint32_t aVehicleTypes);
    Result AddLineSpeedLimit(uint64_t& aId,const Geometry& aLine,double aSpeed,uint32_t aVehicleTypes);
    Result AddClosedLineSpeedLimit(uint64_t& aId,const Geometry& aLine,double aSpeed,uint32_t aVehicleTypes);
    Result AddForbiddenArea(uint64_t& aId,const Geometry& aPolygon);
    Result DeleteTrafficInfo(uint64_t aId);
    void ClearTrafficInfo();
    Result WriteTrafficMessageAsXml(OutputStream& aOutput,const TrafficInfo& aTrafficInfo,LocationRef& aLocationRef);
    Result WriteLineTrafficMessageAsXml(OutputStream& aOutput,const TrafficInfo& aTrafficInfo,const String& aId,const CartoTypeCore::Route& aRoute);
    Result WriteClosedLineTrafficMessageAsXml(OutputStream& aOutput,const TrafficInfo& aTrafficInfo,const String& aId,const CartoTypeCore::Route& aRoute);
    bool EnableTrafficInfo(bool aEnable);
    bool TrafficInfoEnabled() const;

    // tracking
    void StartTracking();
    void EndTracking();
    void DeleteTrack();
    bool Tracking() const;
    Result DisplayTrack(bool aEnable);
    bool TrackIsDisplayed() const;
    TrackGeometry Track() const;
    double TrackLengthInMeters() const;
    Result WriteTrackAsXml(const String& aFileName) const;
    Result WriteTrackAsXmlString(std::string& aXmlString) const;

    // server
    std::string HandleQuery(const std::string& aQuery,const std::string& aData);

    // functions for internal use only
    Result LoadMaps(int32_t aZoom,const TileParam& aParam,const std::vector<TileFileInfo>& aMapFileArray);
    std::shared_ptr<CMapDataBaseArray> MapDataBaseArray();
    void HandleMapChange(const MapChange& aMapChange);
    void SyncMapData(const Framework& aFramework);
    uint32_t MapDataGeneration() const ;
    std::shared_ptr<CMapStyle> CreateStyleSheet(double aScale);
    std::unique_ptr<CMapStore> NewMapStore(std::shared_ptr<CMapStyle> aStyleSheet,const Rect& aBounds,bool aDynamic);
    /** Returns the main map database. For internal use only. */
    CMapDataBase& MainDb() const;
    Transform3D MapTransform() const;
    AffineTransform MapTransform2D() const;
    Transform3D PerspectiveTransform() const;
    /** Returns the FrameworkEngine object used by this framework. For internal use only. */
    std::shared_ptr<CartoTypeCore::FrameworkEngine> FrameworkEngine() const;
    /** Returns the CEngine object used by this framework. For internal use only. */
    std::shared_ptr<CEngine> Engine() const;
    /** Returns the CMap object owned by this framework. For internal use only.  */
    CMap& Map() const { return *iMap; }
    /** Returns the CMap object owned by this framework. For internal use only.  */
    std::shared_ptr<CMap> MapSharedPtr() const { return iMap; }
    Color OutlineColor() const;
    std::unique_ptr<Framework> CreateLegendFramework(Result& aError);
    std::unique_ptr<Bitmap> CreateBitmapFromSvg(InputStream& aInputStream,FileLocation& aErrorLocation,int32_t aForcedWidth = 0);
    /** Associates arbitrary data with this framework. Used by the Android SDK. */
    void SetUserData(std::shared_ptr<MUserData> aUserData) { iUserData = aUserData; }
    double SetLabelUpAngle(double aAngle);
    double LabelUpAngle() const;
    void SetLabelUpVector(PointFP aVector);
    PointFP LabelUpVector() const;
    size_t RouteCalculationCost() const;
    /** Returns the current map drawing parameters. For internal use only. */
    CMapDrawParam& MapDrawParam() const { return *iMapDrawParam; }
    double PolygonArea(const CoordSet& aCoordSet,CoordType aCoordType);
    double PolylineLength(const CoordSet& aCoordSet,CoordType aCoordType);
    Result GetAreaAndLength(const Geometry& aGeometry,double& aArea,double& aLength) const;
    Result GetContourAreaAndLength(const Geometry& aGeometry,size_t aContourIndex,double& aArea,double& aLength);
    double Pixels(double aSize,const char* aUnit) const;
    static std::string ReadFromUrl(const std::string& aUrl,const std::string& aPostData,int& aStatus);
    void RunAutoMapLoader();

    private:
    Framework();

    Framework(const Framework&) = delete;
    Framework(Framework&&) = delete;
    void operator=(const Framework&) = delete;
    void operator=(Framework&&) = delete;

    Result Construct(const Param& aParam,
                     std::shared_ptr<CartoTypeCore::FrameworkEngine> aSharedEngine = nullptr,
                     std::shared_ptr<FrameworkMapDataSet> aSharedMapDataSet = nullptr);
    void InvalidateMapBitmaps() { iMapBitmapType = TMapBitmapType::None; }
    void HandleChangedView();
    void HandleChangedLayer() { InvalidateMapBitmaps(); LayerChanged(); }
    void CreateTileServer(int32_t aTileWidthInPixels,int32_t aTileHeightInPixels);
    void SetRoutePositionAndVector(const Point& aPos,const Point& aVector);
    void CreateNavigator();
    void InsertTrackObject();
    void CreateMapObjectGroupArray(MapObjectGroupArray& aObjectGroupArray,MapObjectArray& aObjectArray,const FindParam& aFindParam) const;
    bool EnforcePanAndZoomLimits();
    void AddNearbyObjectsToMap();
    void ConvertCoordsInternal(double* aCoordArray,size_t aCoordArraySize,CoordType aFromCoordType,CoordType aToCoordType) const;
    void ConvertPointInternal(double& aX,double& aY,CoordType aFromCoordType,CoordType aToCoordType) const;
    std::vector<RoutePointInternal> CreateRoutePointArray(const RouteCoordSet& aRouteCoordSet);
    double ScaleForSpeed(double aSpeedInKph) const;
    void InsertRouteObjects(const Point* aPos);
    void DeleteRouteObjects();
    void CreateBuiltInRouteProfileArray();

    // Notifying observers.
    void NotifyObservers(std::function<void(MFrameworkObserver&)>);
    void DeleteNullObservers();
    void ViewChanged() { NotifyObservers([](MFrameworkObserver& aP) { aP.OnViewChange(); }); }
    void MapChanged() { NotifyObservers([&](MFrameworkObserver& aP) { aP.OnMapChange(); }); }
    void StyleChanged() { NotifyObservers([](MFrameworkObserver& aP) { aP.OnStyleChange(); }); }
    void LayerChanged() { NotifyObservers([](MFrameworkObserver& aP) { aP.OnLayerChange(); }); }
    void NoticeChanged() { NotifyObservers([](MFrameworkObserver& aP) { aP.OnNoticeChange(); }); }
    void FrameworkDestroyed() { NotifyObservers([](MFrameworkObserver& aP) { aP.OnFrameworkDestroyed(); }); }

    // Graphics.
    C32BitColorBitmapGraphicsContext* Gc() const;
    CPerspectiveGraphicsContext* PerspectiveGc() const;

    // virtual functions from MNavigatorObserver
    void OnRoute(const CartoTypeCore::Route* aRoute) override;
    void OnRouteAsync() override;
    void OnTurn(const NavigatorTurn& aFirstTurn,const NavigatorTurn* aSecondTurn,const NavigatorTurn* aContinuationTurn,
                double aDistanceLeft,double aTimeLeft) override;
    void OnState(CartoTypeCore::NavigationState aState) override;

    void ChangeStyleSheet(const CartoTypeCore::StyleSheetDataArray& aStyleSheetData,const StringDictionary* aStyleSheetVariables = nullptr,const CartoTypeCore::BlendStyleSet* aBlendStyle = nullptr);
    void ClearTurns();

    std::shared_ptr<CartoTypeCore::FrameworkEngine> iEngine;
    std::shared_ptr<FrameworkMapDataSet> iMapDataSet;
    std::shared_ptr<CMap> iMap;
    std::shared_ptr<CMapDrawParam> iMapDrawParam;
    std::unique_ptr<FrameworkGraphics> iFrameworkGraphics;

    enum class TMapBitmapType
        {
        None,   // the map bitmap is invalid
        Full,   // the map bitmap is valid
        Memory, // the map bitmap has memory map data only
        Label   // the map bitmap has labels only
        };

    TMapBitmapType iMapBitmapType = TMapBitmapType::None;
    RouterType iPreferredRouterType = RouterType::Default;
    std::unique_ptr<CNavigator> iNavigator;
    std::vector<std::weak_ptr<MFrameworkObserver>> iObservers;
    std::vector<CartoTypeCore::RouteProfile> iBuiltInRouteProfileArray;
    bool iBuiltInRouteProfileArrayKnown = false;
    uint64_t iTravelledRouteLineId = 0;
    uint64_t iTravelledRouteEnvelopeId = 0;
    uint64_t iUntravelledRouteLineId = 0;
    uint64_t iUntravelledRouteEnvelopeId = 0;
    std::vector<uint64_t> iNearbyObjects;
    uint64_t iRoutePositionObjectId = 0;
    uint64_t iRouteVectorObjectId = 0;
    std::atomic<bool> iNewRoute { false };
    bool iTracking = false;
    bool iDisplayTrack = false;
    uint64_t iTrackObjectId = 0;
    TrackGeometry iTrack = TrackGeometry(CoordType::Degree);
    std::weak_ptr<Legend> iTurnInstructionNotice;
    NavigatorTurn iFirstTurn;
    NavigatorTurn iSecondTurn;
    NavigatorTurn iContinuationTurn;
    double iDistanceLeft = 0;
    double iTimeLeft = 0;
    CartoTypeCore::NavigationState iNavigationState = NavigationState::None;
    CartoTypeCore::NavigatorParam iNavigatorParam;
    CartoTypeCore::LocationMatchParam iLocationMatchParam;
    CartoTypeCore::RouteProfile iRouteProfile;
    CartoTypeCore::RouteCreationData iRouteCreationData;
    PointFP iVehiclePosOffset;
    std::shared_ptr<CTileServer> iTileServer;
    int32_t iTileServerOverSizeZoomLevels = 1;
    std::string iLocale;
    bool iMetricUnits = true;
    CartoTypeCore::FollowMode iFollowMode = FollowMode::LocationHeadingZoom;
    std::array<double,4> iFollowScale = { 2000, 2000, 8000, 1000000 };
    std::unique_ptr<CAsyncFinder> iAsyncFinder;
    Geometry iPanArea;
    bool iPanAreaIsWholeMap = false;
    FileLocation iStyleSheetErrorLocation;
    std::unique_ptr<CMapObjectEditor> iMapObjectEditor;
    std::shared_ptr<MUserData> iUserData;
    std::recursive_mutex iMapDataMutex;
    std::mutex iAutoMapLoaderMutex;
    std::unique_ptr<AutoMapLoader> iAutoMapLoader;
    };

/** A map renderer using OpenGL ES graphics acceleration. */
class MapRenderer
    {
    public:
    /**
    Constructs a renderer object that can be used to draw a map into a display which supports OpenGL ES drawing.
    The MapRenderer::Draw function should be called to draw the map. You can use the framework object freely; any calls
    to functions which affect the view will automatically be reflected by the Draw() function.

    If aNativeWindow is non-null it is used to create an EGL context for that window, into which all drawing is done
    using a separate thread which calls Draw() 30 times per second. This feature is supported on Windows only.
    */
    MapRenderer(Framework& aFramework,const void* aNativeWindow = nullptr);
    /** Constructs a MapRenderer object and returns an error rather than throwing an exception. */
    static std::unique_ptr<MapRenderer> New(Result& aError,Framework& aFramework,const void* aNativeWindow = nullptr);

    ~MapRenderer();
    /** Draws the map using OpenGL ES. */
    void Draw();
    /** Returns true if this map renderer is valid. If it returns false, graphics acceleration is not enabled. */
    bool Valid() const;
    /**
    Enables or disables drawing by a separate thread. Returns the previous state.
    This function is intended for users who need the full capacity of the GPU
    for a period when drawing is unnecessary.
    When drawing is disabled, draw events can be handled by calls to Draw, but it is also necessary
    to create a timer to redraw occasionally (e.g., once per second) to allow missing tiles to be
    created and drawn after pans, zooms and other changes affecting the display.
    */
    bool Enable(bool aEnable);

    private:
    std::shared_ptr<CMapRendererImplementation> m_implementation;
    };

String UKGridReferenceFromMapPoint(const PointFP& aPointInMapMeters,int32_t aDigits);
String UKGridReferenceFromDegrees(const PointFP& aPointInDegrees,int32_t aDigits);
RectFP MapRectFromUKGridReference(const String& aGridReference);
PointFP MapPointFromUKGridReference(const String& aGridReference);
PointFP PointInDegreesFromUKGridReference(const String& aGridReference);

std::string ReadUrl(const char* aUrl);

}
