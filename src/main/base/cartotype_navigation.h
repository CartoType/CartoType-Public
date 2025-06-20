/*
cartotype_navigation.h
Copyright (C) 2013-2024 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_path.h>
#include <cartotype_stream.h>
#include <cartotype_feature_info.h>
#include <cartotype_map_object.h>
#include <cartotype_color.h>

namespace CartoTypeCore
{

class Framework;
class CMap;
class CMapDataBase;
class MapObject;
class RouteProfile;
class Projection;
class Route;
class Bitmap;

namespace Router
    {
    class TJunctionInfo;
    }

/** Types of router; used when selecting a router type. */
enum class RouterType
    {
    /** This router type causes the default router to be selected: the one for which serialized data is available in the map file, or, failing that, StandardAStar. */
    Default,
    /** A synonym for Default, used when returning an unknown or unavailable router type. */
    None = Default,
    /** The A* router, which gives fast performance but takes a lot of memory and cannot create a route going twice through the same junction. */
    StandardAStar,
    /** A version of the A* router with road nodes and turn arcs; slower than StandardAStar, and uses more memory, but can generate routes going twice through the same junction, for more flexible routing. */
    TurnExpandedAStar,
    /**
    The contraction hierarchy router is intended where less RAM is available: for example with large maps on mobile devices.
    It gives the same routes as StandardAStar, but is a little slower and does not support custom route profiles; the route profile is decided at the time of creating the CTM1 file.
    */
    StandardContractionHierarchy,
    /** Turn-expanded contraction hierarchy. */
    TECH,
    /** A router using the online Mapbox API. */
    Mapbox
    };

/**
Turn types on a route. Turns at junctions are classified
by dividing the full circle into 45-degree segments,
except for sharp turns left and right, which encompass everything
up to an actual U-turn, and left and right forks, for which there
are special rules to avoid classifying them as ahead.

The actual turn angle is also given in the Turn class.
*/
enum class TurnType
    {
    /** No turn exists or is needed. This turn type is used at the start of a route. */
    None,
    /**
    A turn of less than 15 degrees left or right, unless this is a fork with two choices, in which
    case the turn will be bear right or bear left.
    */
    Ahead,
    /**
    A turn between 15 degrees and 45 degrees right
    or a turn through a smaller angle which is the rightmost of a fork with two choices.
    */
    BearRight,
    /** A turn between 45 degrees and 120 degrees right. */
    Right,
    /** A turn between 120 degrees and 180 degrees right. */
    SharpRight,
    /** This turn type is use for U-turns: turns back along the same road. */
    Around,
    /** A turn between 120 degrees and 180 degrees left. */
    SharpLeft,
    /** A turn between 45 degrees and 120 degrees left. */
    Left,
    /**
    A turn between 15 degrees and 45 degrees left.
    or a turn through a smaller angle which is the leftmost of a fork with two choices.
    */
    BearLeft
    };

/**
Roundabout states; turns involving roundabouts are marked as such so that
exit numbers can be counted
*/
enum class RoundaboutState
    {
    /** This junction does not involve a roundabout. */
    None,
    /** This junction enters a roundabout. */
    Enter,
    /** This junction continues around a roundabout. */
    Continue,
    /** This junction exits a roundabout. */
    Exit
    };

/** A turn: a choice of route through a node. */
class Turn
    {
    public:
    /** Sets the Turn object to its just-constructed state. */
    void Clear()
        {
        *this = Turn();
        }

    /**
    Sets the Turn object using inward and outward direction in degrees, left and right alternative choices, and fork and turn-off flags.
    If aDirInRadiansAntiClockwise is true, aInDir and aOutDir are in radians (anticlockwise from map east), otherwise they are in degrees (clockwise from map north).
    */
    void SetTurn(double aInDir,double aOutDir,int32_t aChoices,int32_t aLeftAlternatives,int32_t aRightAlternatives,bool aIsFork,bool aTurnOff,bool aDirInRadiansAntiClockwise = false)
        {
        if (aDirInRadiansAntiClockwise)
            {
            aInDir *= KRadiansToDegreesDouble;
            aInDir = -(aInDir - 90);
            aOutDir *= KRadiansToDegreesDouble;
            aOutDir = -(aOutDir - 90);
            }
        aInDir = AngleInDegreesNormalized(aInDir);
        aOutDir = AngleInDegreesNormalized(aOutDir);
        
        double turn_angle = aOutDir - aInDir;
        if (turn_angle > 180)
            turn_angle -= 360;
        else if (turn_angle < -180)
            turn_angle += 360;
        SetTurn(turn_angle);
        InDirection = aInDir;
        OutDirection = aOutDir;
        Choices = aChoices;
        LeftAlternatives = aLeftAlternatives;
        RightAlternatives = aRightAlternatives;
        IsFork = aIsFork;
        IsTurnOff = aTurnOff;
        if (TurnType == CartoTypeCore::TurnType::Ahead && (IsTurnOff || (IsFork && Choices == 2)))
            TurnType = LeftAlternatives ? CartoTypeCore::TurnType::BearRight : CartoTypeCore::TurnType::BearLeft;
        }

    /** Sets the Turn object's turn angle and turn type from an angle in degrees. Leaves other date members unchanged. */
    void SetTurn(double aTurnAngle)
        {
        TurnAngle = aTurnAngle;
        if (TurnAngle > 120)
            TurnType = CartoTypeCore::TurnType::SharpRight;
        else if (TurnAngle > 45)
            TurnType = CartoTypeCore::TurnType::Right;
        else if (TurnAngle > 15)
            TurnType = CartoTypeCore::TurnType::BearRight;
        else if (TurnAngle > -15)
            TurnType = CartoTypeCore::TurnType::Ahead;
        else if (TurnAngle > -45)
            TurnType = CartoTypeCore::TurnType::BearLeft;
        else if (TurnAngle > -120)
            TurnType = CartoTypeCore::TurnType::Left;
        else
            TurnType = CartoTypeCore::TurnType::SharpLeft;
        }

    /** Writes the Turn object in XML format as a turn element. */
    void WriteAsXml(OutputStream& aOutput) const;

    /** The turn type: ahead, left, right, etc. */
    CartoTypeCore::TurnType TurnType = CartoTypeCore::TurnType::None;
    /** True if this turn is a continuation of the current road and no notification is needed. */
    bool IsContinue = true;
    /** The roundabout state. */
    CartoTypeCore::RoundaboutState RoundaboutState = CartoTypeCore::RoundaboutState::None;
    /** The turn angle in degrees: 0 = straight ahead; negative = left, positive = right. */
    double TurnAngle = 0;
    /** The incoming direction in degrees relative to map north, going clockwise. */
    double InDirection = 0;
    /** The outgoing direction in degrees relative to map north, going clockwise. */
    double OutDirection = 0;
    /** The junction to take, counting the current junction as 0, if this junction is part of a roundabout. */
    int32_t ExitNumber = 0;
    /** The number of choices at this turning if known; 0 if not known. */
    int32_t Choices = 0;
    /** The number of choices to the left of the turn actually taken; if Choices is zero, this data is not known. */
    int32_t LeftAlternatives = 0;
    /** The number of choices to the right of the turn actually taken; if Choices is zero, this data is not known. */
    int32_t RightAlternatives = 0;
    /** True if this turn is a fork: that is, there is an alternative within 22.5 degrees of the turn actually taken. */
    bool IsFork = false;
    /** True if this choice is a turn off, defined as a turn on to a lower-status road. */
    bool IsTurnOff = false;
    /** The name of the junction. */
    String JunctionName;
    /** The reference code of the junction. */
    String JunctionRef;
    };

/** Information about a route segment. This class is immutable and is most easily used via shared pointers. */
class RouteSegment
    {
    friend class RouteBuilder;
    friend class RouteHandler;

    public:
    RouteSegment() = default;
    /** Creates a route segment from data about the route. For internal use. */
    RouteSegment(CartoTypeCore::FeatureInfo aFeatureInfo,const MString& aName,const MString& aRef,const MString& aDestination,
                 double aDistance,double aTime,double aTurnTime,const ContourView& aPath,int32_t aSection,
                 const CartoTypeCore::Turn& aTurn,const MString& aInstructions);

    /**
    Writes a route segment in XML as a segment element.
    Uses aProjection to convert the points to latitude and longitude.
    */
    void WriteAsXml(OutputStream& aOutput,const Projection& aProjection) const;
    /** Returns true if this segment is not normally accessible. */
    bool IsPrivate() const { return m_feature_info.IsPrivate(); }
    /** The feature info (road type, level, bridge, tunnel, etc.). */
    CartoTypeCore::FeatureInfo FeatureInfo() const { return m_feature_info; }
    /** The road name. */
    const MString& Name() const { return m_name; }
    /** The road reference. */
    const MString& Ref() const { return m_ref;  }
    /** The destination. */
    const MString& Destination() const { return m_destination; }
    /** The distance in metres. */
    double Distance() const { return m_distance; }
    /** The estimated time in seconds, including TurnTime, taken to traverse the segment. */
    double Time() const { return m_time; }
    /** The estimated time in seconds taken to navigate the junction at the start of the segment. */
    double TurnTime() const { return m_turn_time;  }
    /** The path of this segment in map units. */
    ContourView Path() const { return m_path; }
    /**
    The section number.
    Routes are divided into sections between waypoints. A simple route has one section,
    a route with an intermediate waypoint has two sections, and so on. Sections
    are numbered from zero upwards.
    */
    int32_t Section() const { return m_section; }
    /** Information about how to navigate the junction at the start of this segment. */
    const CartoTypeCore::Turn& Turn() const { return m_turn; }
    /** Turn instructions provided by the routing system. If not empty they are used instead of instructions generated from the information in this m_route_segment object. */
    const MString& Instructions() const { return m_instructions; }

    /** Merges a series of route segments for the purpose of creating route instructions. Updates aStartSegment. */
    static RouteSegment MergeRouteSegments(const Route& aRoute,size_t& aStartSegment);

    private:
    static void Merge(RouteSegment& aDest,const RouteSegment& aOther);
    static RouteSegment MergeRouteSegmentsHelper(const Route& aRoute,size_t& aStartSegment);

    /** The feature info (road type, level, bridge, tunnel, etc.). */
    CartoTypeCore::FeatureInfo m_feature_info;
    /** The road name. */
    String m_name;
    /** The road reference. */
    String m_ref;
    /** The destination. */
    String m_destination;
    /** The distance in metres. */
    double m_distance = 0;
    /** The estimated time in seconds, including TurnTime, taken to traverse the segment. */
    double m_time = 0;
    /** The estimated time in seconds taken to navigate the junction at the start of the segment. */
    double m_turn_time = 0;
    /** The path of this segment in map units. */
    OnCurveContour m_path;
    /**
    The section number.
    Routes are divided into sections between waypoints. A simple route has one section,
    a route with an intermediate waypoint has two sections, and so on. Sections
    are numbered from zero upwards.
    */
    int32_t m_section = 0;
    /** Information about how to navigate the junction at the start of this segment. */
    CartoTypeCore::Turn m_turn;
    /** Turn instructions provided by the routing system. If not empty they are used instead of instructions generated from the information in this m_route_segment object. */
    String m_instructions;
    };

/** The side of the road: used in traffic information. */
enum class SideOfRoad
    {
    None,
    Right,
    Left,
    Both
    };

/** Information about the nearest road (in fact, the nearest routable map object) to a certain point. */
class NearestRoadInfo
    {
    public:

    /** The route info, including route type, max speed, whether it is one-way, etc. */
    CartoTypeCore::FeatureInfo FeatureInfo;
    /** The standard name of the road. */
    String Name;
    /** The road reference of the road. */
    String Ref;
    /** The nearest point on the road, in map coordinates, to the chosen point. */
    Point NearestPoint;
    /** The distance from the chosen point to NearestPoint in meters. */
    double Distance = 0;
    /** The heading of the nearest segment in degrees. */
    double HeadingInDegrees = 0;
    /** The heading of the nearest segment as a unit vector. */
    PointFP HeadingVector;
    /** The road's geometry in map coordinates. The road is guaranteed to be traversible in the direction of m_path. */
    Contour Path;
    /**
    The side of the road on which the selected point lies, or SideOfRoad::None if the vehicle heading is not supplied.
    It is Left if the heading vector is left of a vector from the chosen point to the nearest point on the road, otherwise Right.
    Another way to understand it is: if the road as seen by the driver of the vehicle is to the left, the side of the road is Right, and vice versa.
    */
    CartoTypeCore::SideOfRoad SideOfRoad = CartoTypeCore::SideOfRoad::None;
    };

/** Information about the nearest route segment to a point. */
class NearestSegmentInfo
    {
    public:
    /** The index of the segment in the Route object, or -1 if there were no segments. */
    int32_t SegmentIndex = -1;
    /**
    The index of the line within the segment's path:
    line N goes from point N to point N + 1.
    */
    int32_t LineIndex = 0;
    /** The point in the segment's path nearest to the other point, in map coordinates. */
    PointFP NearestPoint;
    /** The distance from the other point to NearestPoint in meters. */
    double DistanceToRoute = 0;
    /** The distance of the nearest point along the route in meters. */
    double DistanceAlongRoute = 0;
    /** The distance within the current segment in meters. */
    double DistanceAlongSegment = 0;
    /** The estimated time of the nearest point, along the route, in seconds. */
    double TimeAlongRoute = 0;
    /** The estimated time within the current segment, in seconds. */
    double TimeAlongSegment = 0;
    /** The heading of the nearest line as a map angle taken anti-clockwise from rightwards. */
    double Heading = 0;
    };

/**
Information about a path from the start or
end of the route to the nearest non-trivial junction.
Used when exporting traffic information.
*/
class PathToJunction
    {
    public:
    /** Sets the object to its just-contructed state. */
    void Clear()
        {
        *this = PathToJunction();
        }
    /** Reverses the path. */
    void Reverse()
        {
        Path.Reverse();
        std::swap(StartRouteInfo,EndRouteInfo);
        }

    /** The path between the junction and the start or end of the route. */
    Contour Path;
    /** The route info at the start of the path. */
    FeatureInfo StartRouteInfo;
    /** The route info at the start of the path. */
    FeatureInfo EndRouteInfo;
    /** The length of the path in meters. */
    double Distance = 0;
    };

/** Constants used to select frequently-used route profiles. */
enum class RouteProfileType
    {
    /** A profile type for driving in a private car (synonym of Car). */
    Drive,
    /** A profile type for driving in a private car (synonym of Drive). */
    Car = Drive,
    /** A profile type for walking. */
    Walk,
    /** A profile type for cycling (synonym of Bicycle). */
    Cycle,
    /** A profile type for cycling (synonym of Cycle). */
    Bicycle = Cycle,
    /** A profile type for walking, preferring off-road paths. */
    Hike,
    /** A profile type for downhill skiing. */
    Ski
    };

/** A vehicle type used in routing. */
class VehicleType
    {
    private:
    auto Tuple() const { return std::forward_as_tuple(AccessFlags,Height,AxleLoad,DoubleAxleLoad,TripleAxleLoad,Height,Width,Length,HazMat); }

    public:
    /** The equality operator. */
    bool operator==(const VehicleType& aOther) const { return Tuple() == aOther.Tuple(); }
    /** True if this vehicle type obeys one-way restrictions. Pedestrian types usually do not. */
    bool ObeysOneWay() const { return (AccessFlags & FeatureInfo::KRouteAccessWrongWayFlag) != 0; }
    /** True if walking (access to pedestrian routes) is allowed. */
    bool WalkingAllowed() const { return (AccessFlags & FeatureInfo::KRouteAccessPedestrianFlag) != 0; }
    /** True if cycling (access to cycle routes) is allowed. */
    bool CyclingAllowed() const { return (AccessFlags & FeatureInfo::KRouteAccessCycleFlag) != 0; }
    /** True if motor vehicles are allowed. */
    bool MotorVehiclesAllowed() const { return (AccessFlags & FeatureInfo::KRouteAccessMotorVehicleFlag) != 0; }

    /** Normalizes the vehicle type: ensures that the 'other acccess' flag is set if and only if vehicle details are given, and that weights and dimensions are non-negative. */
    void Normalize()
        {
        if (Weight > 0 ||
            AxleLoad > 0 ||
            DoubleAxleLoad > 0 ||
            TripleAxleLoad > 0 ||
            Height > 0 ||
            Width > 0 ||
            Length > 0 ||
            HazMat)
            AccessFlags |= FeatureInfo::KRouteAccessOtherFlag;
        else
            AccessFlags &= ~FeatureInfo::KRouteAccessOtherFlag;

        if (Weight < 0) Weight = 0;
        if (AxleLoad < 0) AxleLoad = 0;
        if (DoubleAxleLoad < 0) DoubleAxleLoad = 0;
        if (TripleAxleLoad < 0) TripleAxleLoad = 0;
        if (Height < 0) Height = 0;
        if (Width < 0) Width = 0;
        if (Length < 0) Length = 0;
        }

    /**
    Flags taken from FeatureInfo indicating the vehicle type.
    Arcs with restrictions matching any of these flags will not be taken.
    */
    uint32_t AccessFlags = FeatureInfo::KRouteAccessMotorVehicleFlag | FeatureInfo::KRouteAccessWrongWayFlag;

    /** The vehicle's weight in metric tons. Values of zero or less mean 'unknown'. */
    double Weight = 0;

    /** The vehicle's axle load in metric tons. Values of zero or less mean 'unknown'. */
    double AxleLoad = 0;

    /** The vehicle's double axle load in metric tons. Values of zero or less mean 'unknown'. */
    double DoubleAxleLoad = 0;

    /** The vehicle's triple axle weight in metric tons. Values of zero or less mean 'unknown'. */
    double TripleAxleLoad = 0;

    /** The vehicle's height in meters. Values of zero or less mean 'unknown'. */
    double Height = 0;

    /** The vehicle's width in meters. Values of zero or less mean 'unknown'. */
    double Width = 0;

    /** The vehicle's length in meters. Values of zero or less mean 'unknown'. */
    double Length = 0;

    /** True if the vehicle is carrying hazardous materials. */
    bool HazMat = false;
    };

/**
A routing profile: parameters determining the type of route,
including road speeds, bonuses and penalties, and vehicle type.
A road type will not normally be used if its speed plus bonus is equal to or less than zero.

However, if the speed is positive and the bonus is negative, and their sum is zero,
this type of road will be allowed at the beginning or end of a route, allowing
travel over farm tracks, for instance, to leave or to arrive at a destination.

Similarly, restricted road types such as private roads are allowed at the beginning
or end of a route.
*/
class RouteProfile
    {
    public:
    /** Creates a route profile. If the profile type is not supplied the default value is the car profile type. */
    RouteProfile(RouteProfileType aProfileType = RouteProfileType::Car);
    /** Creates a route profile from XML format by reading a CartoTypeRouteProfile element. Throws an exception on error. */
    explicit RouteProfile(InputStream& aInput);
    /** Reads a route profile from XML format by reading a CartoTypeRouteProfile element.. */
    Result ReadFromXml(InputStream& aInput);

    /** Writes the route profile in XML format as a CartoTypeRouteProfile element. */
    Result WriteAsXml(OutputStream& aOutput) const;

    /** The equality operator. */
    bool operator==(const RouteProfile& aOther) const;
    
    /** The inequality operator. */
    bool operator!=(const RouteProfile& aOther) const { return !(*this == aOther); }

    static constexpr size_t Motorway = 0;               ///< The index for motorways in the speed, bonus and restriction override arrays.
    static constexpr size_t MotorwayLink = 1;           ///< The index for motorway links in the speed, bonus and restriction override arrays.
    static constexpr size_t TrunkRoad = 2;              ///< The index for trunk roads in the speed, bonus and restriction override arrays.
    static constexpr size_t TrunkLink = 3;              ///< The index for trunk road links in the speed, bonus and restriction override arrays.
    static constexpr size_t PrimaryRoad = 4;            ///< The index for primary roads in the speed, bonus and restriction override arrays.
    static constexpr size_t PrimaryLink = 5;            ///< The index for primary road links in the speed, bonus and restriction override arrays.
    static constexpr size_t SecondaryRoad = 6;          ///< The index for secondary roads in the speed, bonus and restriction override arrays.
    static constexpr size_t SecondaryLink = 7;          ///< The index for secondary road links in the speed, bonus and restriction override arrays.
    static constexpr size_t TertiaryRoad = 8;           ///< The index for tertiary roads in the speed, bonus and restriction override arrays.
    static constexpr size_t UnclassifiedRoad = 9;       ///< The index for unclassified roads in the speed, bonus and restriction override arrays.
    static constexpr size_t ResidentialRoad = 10;       ///< The index for residential roads in the speed, bonus and restriction override arrays.
    static constexpr size_t Track = 11;                 ///< The index for tracks in the speed, bonus and restriction override arrays.
    static constexpr size_t ServiceRoad = 12;           ///< The index for service roads in the speed, bonus and restriction override arrays.
    static constexpr size_t PedestrianRoad = 13;        ///< The index for pedestrian roads in the speed, bonus and restriction override arrays.
    static constexpr size_t VehicularFerry = 14;        ///< The index for vehicular ferries in the speed, bonus and restriction override arrays.
    static constexpr size_t PassengerFerry = 15;        ///< The index for passenger ferries in the speed, bonus and restriction override arrays.
    static constexpr size_t LivingStreet = 16;          ///< The index for living streets in the speed, bonus and restriction override arrays.
    static constexpr size_t Cycleway = 17;              ///< The index for cycleways in the speed, bonus and restriction override arrays.
    static constexpr size_t Path = 18;                  ///< The index for paths in the speed, bonus and restriction override arrays.
    static constexpr size_t Footway = 19;               ///< The index for footways in the speed, bonus and restriction override arrays.
    static constexpr size_t Bridleway = 20;             ///< The index for bridleways in the speed, bonus and restriction override arrays.
    static constexpr size_t Steps = 21;                 ///< The index for steps in the speed, bonus and restriction override arrays.
    static constexpr size_t OtherRoad = 22;             ///< The index for other roads in the speed, bonus and restriction override arrays.
    static constexpr size_t UnpavedRoad = 23;           ///< The index for unpaved roads in the speed, bonus and restriction override arrays.
    static constexpr size_t Railway = 24;               ///< The index for railways in the speed, bonus and restriction override arrays.
    static constexpr size_t LightRail = 25;             ///< The index for light railways in the speed, bonus and restriction override arrays.
    static constexpr size_t Subway = 26;                ///< The index for subways in the speed, bonus and restriction override arrays.
    static constexpr size_t AerialWay = 27;             ///< The index for aerial ways in the speed, bonus and restriction override arrays.
    static constexpr size_t SkiDownhill = 28;           ///< The index for downhill ski trails in the speed, bonus and restriction override arrays.
    static constexpr size_t SkiNordic = 29;             ///< The index for nordic ski trails in the speed, bonus and restriction override arrays.
    static constexpr size_t Waterway = 30;              ///< The index for waterways in the speed, bonus and restriction override arrays.
    static constexpr size_t UnknownRoute = 31;          ///< The index for unknown routes in the speed, bonus and restriction override arrays.

    /** The size of the speed, bonus ands restriction override arrays, which are indexed by the route feature types from 0 to 31. */
    static constexpr size_t RouteTypeCount = 32;

    /** The optional name of the profile. */
    std::string Name;

    /** The vehicle type, defined using access flags, dimensions, weight, etc. */
    CartoTypeCore::VehicleType VehicleType;

    /** Speeds along roads in kilometres per hour. */
    std::array<double,RouteTypeCount> Speed = { };

    /**
    Bonuses or penalties in notional km per hour to be added to road types to make them more or less likely to be used.
    For example, a penalty of 1kph is applied to walking routes along major roads because it is pleasanter to walk along quieter minor roads.
    */
    std::array<double,RouteTypeCount> Bonus = { };

    /**
    This array of bit masks allows restrictions to be overridden for certain
    types of road. For example, to allow routing of motor vehicles along
    tracks that are normally forbidden to motor vehicles, set
    RestrictionOverride[size_t(FeatureType::Track)] to FeatureInfo::KRouteAccessMotorVehicle.
    */
    std::array<uint32_t,RouteTypeCount> RestrictionOverride = { };

    /**
    The estimated time in seconds taken for any turn at a junction that is not
    a slight turn or simply going straight ahead.
    */
    int32_t TurnTime = 4;

    /**
    The estimated time in seconds taken for a U-turn, defined as a turn
    very close to 180 degrees (within 1/32 of a circle = 11.75 degrees).
    */
    int32_t UTurnTime = 300;

    /**
    The estimated time in seconds taken for a turn across the traffic:
    that is, a left turn in drive-on-right countries,
    or a right turn in drive-on-left countries.
    */
    int32_t CrossTrafficTurnTime = 12;

    /**
    The estimated delay in seconds caused by traffic lights.
    */
    int32_t TrafficLightTime = 10;

    /** Set this flag to true to get the shortest route by distance or time, ignoring weightings. Whether distance or time is used depends on the setting of ShortestByTime. */
    bool Shortest = false;

    /** Set this flag to true if Shortest should produce the shortest route by time rather than distance. */
    bool ShortestByTime = false;

    /**
    The penalty applied to toll roads as a number between zero and one.
    The value 1 means that no toll roads will be used, 0.5 makes toll roads half as desirable as non-toll roads
    of the same road type, and so on. The value 0 means that no penalty is applied to toll roads.
    Values outside the range 0...1 are clamped to that range.
    */
    double TollPenalty = 0;

    /**
    Speeds added to roads with a particular gradient.
    These values are intended for cycling, walking, hiking and running profiles,
    where speeds are generally the same for different types of road, but
    can change according to the gradient.
    */
    std::array<double,FeatureInfo::KGradientCount> GradientSpeed = { };

    /** Bonuses applied to roads with a particular gradient. */
    std::array<double,FeatureInfo::KGradientCount> GradientBonus = { };

    /**
    Flags indicating which roads are affected by gradient speeds and bonuses; normally steps, etc., are excluded.
    Each bit is (1 << route type).
    */
    uint32_t GradientFlags = 0xFFFFFFFF & ~((1 << Steps) | (1 << PassengerFerry) | (1 << VehicularFerry));
    };

/** A route. This class is immutable and so is most easily used via shared pointers. */
class Route
    {
    friend class RouteBuilder;
    friend class RouteHandler;

    public:
    /**
    Creates a route by reading an XML CartoTypeRoute element.
    Uses aProjection to project the points from latitude and longitude.
    */
    Route(InputStream& aInput,std::shared_ptr<CartoTypeCore::Projection> aProjection);
    /** Creates an empty route with a given route profile. */
    Route(const CartoTypeCore::RouteProfile& aProfile,std::shared_ptr<CartoTypeCore::Projection> aProjection);
    /** Creates a Geometry object containing the points of a route in map coordinates. */
    std::unique_ptr<CartoTypeCore::Geometry> Geometry() const;
    /** Creates a Geometry object containing the points of a route in degrees. */
    std::unique_ptr<CartoTypeCore::Geometry> GeometryInDegrees(Result& aError) const;
    /** Returns true if this route has no route segments. */
    bool Empty() const { return m_route_segment.empty(); }
    /** Returns the number of route segments. */
    size_t RouteSegmentCount() const { return m_route_segment.size(); }
    /** Returns a route segment by index. */
    std::shared_ptr<CartoTypeCore::RouteSegment> RouteSegment(size_t aIndex) const { return m_route_segment[aIndex]; }
    /** The distance along the route in meters. */
    double Distance() const { return m_distance; }
    /** The estimated time taken to traverse the route in seconds. */
    double Time() const { return m_time; }
    /** The path along the entire route in map units. */
    const OnCurveContour& Path() const { return m_path; }
    /** The path to the first non-trivial junction before the route: used when creating OpenLR location data. */
    const PathToJunction& PathToJunctionBefore() const { return m_path_to_junction_before; }
    /** The path to the first non-trivial junction after the route: used when creating OpenLR location data. */
    const PathToJunction& PathToJunctionAfter() const { return m_path_to_junction_after; }
    /** The profile used to create the route. */
    const CartoTypeCore::RouteProfile& RouteProfile() const { return m_profile; }
    /** The projection used to convert points from latitude and longitude to the map coordinates stored in this route. */
    std::shared_ptr<CartoTypeCore::Projection> Projection() const { return m_projection; }
    /** Writes a route as an XML CartoTypeRoute element. */
    Result WriteAsXml(OutputStream& aOutput) const;
    /** Writes a route in GPX format. */
    Result WriteAsGpx(OutputStream& aOutput) const;
    /**
    Gets information about the nearest route segment to a point given in map coordinates.

    The parameter aSection gives the current route section (a part of the route between waypoints;
    simple routes have a single section). The returned segment will always be in the current
    section or a following section. It is theoretically possible for a section to be completely skipped if it is very short or
    of zero length, so there is no constraint that section 0 must be followed by section 1 and not by section 2, etc.
    If aSection is negative it is ignored and the returned segment may be in any section.

    The parameter aPreviousDistanceAlongRoute should be provided (set to a value greater than zero)
    for multi-section routes, so that the correct route segment can be chosen if the route doubles back on itself.
    Positions further along the route will be given preference over those earlier in the route.
    */
    NearestSegmentInfo NearestSegment(const Point& aPoint,int32_t aSection,double aPreviousDistanceAlongRoute) const;
    /** Gets information about the point a certain distance along a route. */
    NearestSegmentInfo PointAtDistance(double aDistanceInMeters) const;
    /** Gets information about the point a certain estimated time along a route. */
    NearestSegmentInfo PointAtTime(double aTimeInSeconds) const;
    /** Copies a route but does not copy restricted segments (e.g., private roads). */
    std::unique_ptr<Route> CopyWithoutRestrictedSegments() const;
    /** Returns textual instructions for a route. For internal use only. */
    String Instructions(CMap& aMap,const char* aLocale,bool aMetricUnits,bool aAbbreviate) const;
    /** Returns the total distance in metres of the parts of the route that are on toll roads. */
    double TollRoadDistance() const;
    /** Returns the waypoints of the route. The number of points is one more than the number of route sections. */
    std::vector<Point> RoutePoints() const;

    private:
    NearestSegmentInfo PointAlongRouteHelper(const Point* aPoint,double* aDistance,double* aTime,
                                             int32_t aSection,double aPreviousDistanceAlongRoute) const;

    /** An array of route segments representing the route. */
    std::vector<std::shared_ptr<CartoTypeCore::RouteSegment>> m_route_segment;
    /** The distance along the route in meters. */
    double m_distance = 0;
    /** The estimated time taken to traverse the route in seconds. */
    double m_time = 0;
    /** The path along the entire route in map units. */
    OnCurveContour m_path;
    /** The path to the first non-trivial junction before the route: used when creating OpenLR location data. */
    PathToJunction m_path_to_junction_before;
    /** The path to the first non-trivial junction after the route: used when creating OpenLR location data. */
    PathToJunction m_path_to_junction_after;
    /** The profile used to create the route. */
    CartoTypeCore::RouteProfile m_profile;
    /** The projection used to convert points from latitude and longitude to the map coordinates stored in this route. */
    std::shared_ptr<CartoTypeCore::Projection> m_projection;
    };

/** Data on the cost of creating a route. */
class RouteCreationData
    {
    public:
    /** The time take to calculate the route, in seconds. */
    double RouteCalculationTime = 0;
    /** The time taken to expand the raw route into a route object, in seconds. */
    double RouteExpansionTime = 0;
    /** The number of node cache queries, if relevant. */
    int32_t NodeCacheQueries = 0;
    /** The number of node cache misses, if relevant. */
    int32_t NodeCacheMisses = 0;
    /** The number of arc cache queries, if relevant. */
    int32_t ArcCacheQueries = 0;
    /** The number of arc cache misses, if relevant. */
    int32_t ArcCacheMisses = 0;
    };

/** States of the navigation system. */
enum class NavigationState
    {
    /** No route has been created, or navigation is disabled. */
    None,
    /** There is a route, and navigation is enabled, but no position has been supplied. */
    NoPosition,
    /** The current position is on the route and turn information is available. */
    Turn,
    /** The current position is off the route. */
    OffRoute,
    /** Route calculation is in progress. */
    Routing,
    /** A new route has been calculated. */
    NewRoute,
    /** The latest position is on the route but a U-turn is needed. */
    TurnRound,
    /** The latest position is on the route and very close to the destination. */
    Arrival,
    /** Navigation has started on a new route. */
    Departure
    };

/** Turn information for navigation: the base Turn class plus the distance to the turn, road names and turn instruction. */
class NavigatorTurn: public Turn
    {
    public:
    NavigatorTurn() = default;

    /**
    Creates a turn from aPrevSegment (which can be null at the start of a route) to aCurSegment.
    m_distance and/or time are used if positive, otherwise are taken from the previous segment if non-null.
    */
    NavigatorTurn(int32_t aIndex,const RouteSegment* aPrevSegment,const RouteSegment& aCurSegment,double aDistance = -1,double aTime = -1);

    /** Sets the turn to its just-constructed state. */
    void Clear()
        {
        *this = NavigatorTurn();
        }

    /** Determines if two turns can be merged. */
    bool CanMerge(const NavigatorTurn& aFollowingTurn,const RouteProfile& aProfile) const;
    /** Merges a turn with a following turn if possible. */
    bool MergeIfPossible(const NavigatorTurn& aFollowingTurn,const RouteProfile& aProfile);
    /** Merges a turn with a following turn. */
    void Merge(const NavigatorTurn& aFollowingTurn,const RouteProfile& aProfile);
    /** Creates instructions for this turn, sets them in the turn object, and optionally determines the turn phase. */
    void SetInstructions(std::shared_ptr<CEngine> aEngine,const RouteProfile& aProfile,const char* aLocale,bool aMetricUnits,bool aAbbreviate,int32_t* aTurnPhase = nullptr,NavigationState aState = NavigationState::Turn);
    /** Creates instructions for this turn and optionally determines the turn phase (0 = normal, 1 = advanced turn, 2 = immediate turn). */
    String CreateInstructions(std::shared_ptr<CEngine> aEngine,const RouteProfile& aProfile,const char* aLocale,bool aMetricUnits,bool aAbbreviate,
                              int32_t* aTurnPhase = nullptr,NavigationState aState = NavigationState::Turn,const NavigatorTurn* aNextTurn = nullptr,
                              double aDistanceLeft = 0,double aTimeLeft = 0) const;
    /**
    Creates a diagram of the turn: either a line with a bend in it,
    or two lines connected to a circle representing a roundabout.
    The parameter aEngine is needed to create a graphics context.
    The parameter aSizeInPixels is the width and height of a square enclosing the
    diagram. It is clamped to a minimum of 12.
    aColor is the color of the lines in the diagram.
    If the route profile obeys one-way restrictions a roundabout circle is drawn when the turn enters a roundabout.
    That rule prevents roundabouts from being drawn for walking routes, which may go the wrong way round them.
    */
    Bitmap Diagram(std::shared_ptr<CEngine> aEngine,const RouteProfile& aProfile,int32_t aSizeInPixels,Color aColor) const;

    /** The distance in meters from the current position to the turn or the arrival point. */
    double Distance = 0;
    /** The estimated time in seconds from the current position to the turn or arrival point. */
    double Time = 0;
    /** The name, if known, of the road before the turn. */
    String FromName;
    /** The reference code, if known, of the road before the turn. */
    String FromRef;
    /** The type of the road before the turn. */
    FeatureInfo FromRouteInfo = FeatureType::UnknownRoute;
    /** The name, if known, of the road after the turn. */
    String ToName;
    /** The reference code, if known, of the road after the turn. */
    String ToRef;
    /** The destination of the road after the turn. */
    String Destination;
    /** The type of the road after the turn. */
    FeatureInfo ToRouteInfo = FeatureType::UnknownRoute;
    /** The position of the turn in map coordinates. */
    Point Position;
    /** The localized turn instructions. */
    String Instructions;
    /**
    The index of the turn, which is unique within a route.
    It can be used to avoid duplication when creating custom voice instructions.
    The value -1 means that the index is unknown.
    */
    int32_t Index = -1;
    };

/**
Create an object of a class derived from MNavigatorObserver to handle
navigation events like turn instructions.
*/
class MNavigatorObserver
    {
    public:
    virtual ~MNavigatorObserver() { }

    /** This message updates the current route. */
    virtual void OnRoute(const Route* /*aRoute*/) {  }

    /**
    This message is sent asynchronously when a new route is created.
    It exists so that the Framework object can display a new route without
    making any fresh call to a Navigator function. The call to OnRouteAsync is made from a different
    thread to the one calling Navigator functions, so mutual exclusion may be necessary in the
    called function.
    */
    virtual void OnRouteAsync() {  }

    /**
    This message is sent whenever there is a location update and the location is on route and the direction along the route is correct.

    aFirstTurn is the first significant turn after the current position, ignoring 'ahead' and 'continue' turns.
    If its type is TurnType::None there are no remaining significant turns.
    aFirstTurn.m_distance is the distance from the current position to the first turn or the arrival point.

    aSecondTurn, if non-null, indicates that there is a significant turn 100 metres or less after aFirstTurn.
    aSecondTurn->m_distance is the distance from the first turn to the second turn.

    aContinuationTurn, if non-null, signals that there is an 'ahead' or 'continue' turn before aFirstTurn, so that
    the navigation system can show that no turn is required at the next junction or adjoining minor road.
    aContinuationTurn->m_distance is the distance from the current position to the continuation turn.

    aDistanceLeft is the distance to the destination (along the route) in metres.

    aTimeLeft is the time to the destination in seconds.
    */
    virtual void OnTurn(const NavigatorTurn& /*aFirstTurn*/,const NavigatorTurn* /*aSecondTurn*/,const NavigatorTurn* /*aContinuationTurn*/,
                        double /*aDistanceLeft*/,double /*aTimeLeft*/) { }

    /** This message updates the state. */
    virtual void OnState(NavigationState /*aState*/) { }
    };

/**
Basic data received from a navigation device such as a GPS receiver.
The only essential piece of data is the position. The time is set
automatically if not supplied, and the speed and course are calculated
from successive positions. The height (elevation in meters) may be supplied
but is used only when tracking (storing a log of positions) and is
not essential for that function.
*/
class NavigationData
    {
    public:
    /** A bit value for flags in Validity indicating that the time is valid. */
    static constexpr uint32_t KTimeValid = 1;
    /** A bit value for flags in Validity indicating that the position is valid. */
    static constexpr uint32_t KPositionValid = 2;
    /** A bit value for flags in Validity indicating that the speed is valid. */
    static constexpr uint32_t KSpeedValid = 4;
    /** A bit value for flags in Validity indicating that the course is valid. */
    static constexpr uint32_t KCourseValid = 8;
    /** A bit value for flags in Validity indicating that the time is valid. */
    static constexpr uint32_t KHeightValid = 16;
    
    /** Flags indicating validity or availability of data. */
    uint32_t Validity = 0;
    /** The time in seconds: the number of seconds since 00:00 on the 1st January 1970, UTC. */
    double Time = 0;
    /** Position in degrees latitude and longitude. */
    PointFP Position;
    /** Speed in kilometres per hour. */
    double Speed = 0;
    /** True course in degrees. */
    double Course = 0;
    /** GetHeight above sea level in meters. */
    double Height = 0;
    };

/** Parameters governing navigation behavior. */
class NavigatorParam
    {
    public:
    /** Minimum distance between location fixes in metres that is taken as an actual move. */
    int32_t MinimumFixDistance = 4;
    /** Maximum distance from the route in metres before the vehicle is deemed off-route. */
    int32_t RouteDistanceTolerance = 20;
    /** Maximum time off route in seconds before a new route needs to be calculated. */
    int32_t RouteTimeTolerance = 30;
    /**
    If true, and if there is a route, the position on route is updated and turn information is created, when a position update arrives.
    If false, the vehicle position and speed are updated but other behaviour is the same as if there is no route.
    */
    bool NavigationEnabled = true;
    };

/** Parameters used when matching a road or other feature to a location. */
class LocationMatchParam
    {
    public:
    /** Converts zeros to default values and clamps other values to legal ranges. */
    void Normalize();
    /** Returns a normalized version of this object. */
    LocationMatchParam Normalized() const { LocationMatchParam p = *this; p.Normalize(); return p; }

    /**
    The accuracy of a location fix given as a range error with 95% probability.
    It is clamped to the range 1 ... 1000.
    A value of zero indicates the default value: 8.
    */
    double LocationAccuracyInMeters = 0;
    /**
    The accuracy of a heading or course given as an angular error in degrees with 95% probability.
    It is clamped to the range 1 ... 90.
    A value of zero indicates the default value: 22.5.
    */
    double HeadingAccuracyInDegrees = 0;
    /**
    The expected maximum distance of a road from the current location.
    It is clamped to the range 5 ... 10,000.
    A value of zero indicates the default value: 100.
    */
    double MaxRoadDistanceInMeters = 0;
    };

/** A point on a route with heading and location match parameters. */
class RoutePoint
    {
    public:
    /** The point. */
    PointFP Point;
    /** The heading in degrees clockwise from north. */
    double Heading = 0;
    /** True if the heading is known. */
    bool HeadingKnown = false;
    /** Parameters used when matching the point to a road or other routable segment. */
    CartoTypeCore::LocationMatchParam LocationMatchParam;
    };

/** A point on a route in map coordinates, with its heading and location match parameters. */
class RoutePointInternal
    {
    public:
    /** Reads this point from an input stream. */
    void Read(InputStream& aInput);
    /** Writes this point to an output stream. */
    void Write(OutputStream& aOutput) const;

    /** The point in map coordinates. */
    CartoTypeCore::Point Point;
    /** The heading in degrees clockwise from true north. */
    double Heading = 0;
    /** The heading vector in map coordinates. */
    CartoTypeCore::Point HeadingVector;
    /** The location match parameters. */
    CartoTypeCore::LocationMatchParam LocationMatchParam;
    };

/**
A set of points for creating a route, with optional heading and accuracy information.
Headings are used where available, and where possible, to decide between
roads or choose the direction of travel on a road.
*/
class RouteCoordSet
    {
    public:
    /** Creates a RouteCoordSet with coordinates in degrees. */
    RouteCoordSet() { }
    /** Creates a RouteCoordSet with a specified coordinate type. */
    explicit RouteCoordSet(CartoTypeCore::CoordType aCoordType): CoordType(aCoordType) {  }
    /** Creates a RouteCoordSet by copying another one. */
    RouteCoordSet(const RouteCoordSet& aOther) = default;
    /** Creates a RouteCoordSet by moving data from another one. */
    RouteCoordSet(RouteCoordSet&& aOther) = default;
    /** Creates a RouteCoordSet from a CoordSet. */
    RouteCoordSet(const CoordSet& aCs,CoordType aCoordType,const LocationMatchParam& aParam);
    /** Creates a RouteCoordSet from a std::vector of points. */
    RouteCoordSet(const std::vector<PointFP>& aPointArray,CartoTypeCore::CoordType aCoordType,const LocationMatchParam& aParam);
    /** Creates a RouteCoordSet from an array of points. */
    RouteCoordSet(const PointFP* aPointArray,size_t aCount,CartoTypeCore::CoordType aCoordType,const LocationMatchParam& aParam);
    /** The assignment operator. */
    RouteCoordSet& operator=(const RouteCoordSet& aOther) = default;
    /** The move assignment operator. */
    RouteCoordSet& operator=(RouteCoordSet&& aOther) = default;

    /** The coordinate type of the route points. */
    CartoTypeCore::CoordType CoordType = CartoTypeCore::CoordType::Degree;
    /** The route points. */
    std::vector<RoutePoint> RoutePointArray;
    };

/** An iterator allowing a route to be traversed. */
class RouteIterator
    {
    public:
    /** Creates a route iterator referring to a certain route and sets it to the start of the route. */
    explicit RouteIterator(std::shared_ptr<Route> aRoute);
    /** Moves forward by aDistance. Returns false if the end of the path is reached, true if not. */
    bool Forward(double aDistance) { return iPathIter.Forward(aDistance); }
    /** Returns the current position in map coordinates. */
    const Point& Position() const { return iPathIter.Position(); }
    /** Returns the current direction in radians, clockwise from straight up, as an angle on the map, not a geodetic azimuth. */
    double Direction() const { return iPathIter.Direction(); }

    private:
    std::shared_ptr<Route> iRoute;
    PathIterator iPathIter;
    };

/**
Traffic information.
This information is normally used in combination with a location reference.
*/
class TrafficInfo
    {
    public:
    /** Writes this object in XML as a TrafficInfo element. */
    void WriteAsXml(OutputStream& aOutput) const;

    /** Speeds of this value or greater mean there is no speed limit. */
    static constexpr double KNoSpeedLimit = 255;

    /** Vehicle types affected by this information, taken from the FeatureInfo bit definitions; use FeatureInfo::KRouteAccessVehicle to select all vehicle types. */
    uint32_t VehicleTypes = FeatureInfo::KRouteAccessVehicle;
    /** Permitted or expected speed in kph: KNoSpeedLimit or greater means no speed limit; 0 or less means the route is forbidden. */
    double Speed = KNoSpeedLimit;
    };

/** The orientation of a path along a road: used in traffic information. */
enum class RoadOrientation
    {
    None,
    Forwards,
    Backwards,
    Both,
    };

/** The type of a location reference used for traffic information. */
enum class LocationRefType
    {
    /** A line in the route network. */
    Line,
    /** A point on the earth's surface. */
    GeoCoordinate,
    /** A point on a line in the route network. */
    PointAlongLine,
    /** A point on the route network providing access to a nearby POI: the first point is the point on the line, the second is the POI. */
    PointWithAccessPoint,
    /** A circle defined as a point and a radius. */
    Circle,
    /** A rectangle aligned to the grid of latitude and longitude: it is defined using two points at opposite corners. */
    Rectangle,
    /** A polygon defined using a set of points. */
    Polygon,
    /** A closed line in the route network. */
    ClosedLine
    };

/** Parameters used for a location reference when writing traffic information. */
class LocationRef
    {
    public:
    /** Creates a location reference with the specified reference type and coordinate type. */
    LocationRef(LocationRefType aType,CoordType aCoordType):
        Type(aType),
        Geometry(aCoordType),
        RadiusInMeters(0),
        SideOfRoad(CartoTypeCore::SideOfRoad::None),
        RoadOrientation(CartoTypeCore::RoadOrientation::None)
        {
        }

    void WriteAsXml(Framework& aFramework,OutputStream& aOutput) const;

    /** The type of this location reference. */
    LocationRefType Type;
    /** The arbitrary ID of the location reference. */
    String Id;
    /** The point or points. */
    CartoTypeCore::Geometry Geometry;
    /** The radius, if this is a circle. */
    double RadiusInMeters;
    /** The side of the road, if relevant. */
    CartoTypeCore::SideOfRoad SideOfRoad;
    /** The road orientation, if relevant. */
    CartoTypeCore::RoadOrientation RoadOrientation;
    };

/** A matrix of route times and distances between sets of points. */
class TimeAndDistanceMatrix
    {
    public:
    /** The default constructor. */
    TimeAndDistanceMatrix() = default;
    /** The copy constructor */
    TimeAndDistanceMatrix(const TimeAndDistanceMatrix&) = default;
    /** The move constructor. */
    TimeAndDistanceMatrix(TimeAndDistanceMatrix&&) = default;
    /** Constructs the object from from-count, to-count and raw matrix. */
    TimeAndDistanceMatrix(size_t aFromCount,size_t aToCount,std::vector<uint32_t>&& aMatrix):
        m_from_count(aFromCount),
        m_to_count(aToCount),
        m_matrix(std::move(aMatrix))
        {
        if (aFromCount * aToCount * 2 != m_matrix.size())
            throw KErrorInvalidArgument;
        }
    /** The copy assignment operator */
    TimeAndDistanceMatrix& operator=(const TimeAndDistanceMatrix&) = default;
    /** The move assignment operator. */
    TimeAndDistanceMatrix& operator=(TimeAndDistanceMatrix&&) = default;
    /** Returns the number of 'from' points. */
    size_t FromCount() const { return m_from_count; }
    /** Returns the number of 'to' points. */
    size_t ToCount() const { return m_to_count; }
    /** Returns the time in seconds taken to travel on the best route from one of the 'from' points to one of the 'to' points. */
    uint32_t Time(size_t aFromIndex,size_t aToIndex) const { return m_matrix[(aFromIndex * m_to_count + aToIndex) * 2]; }
    /** Returns the distance in metres along the best route from one of the 'from' points to one of the 'to' points. */
    uint32_t Distance(size_t aFromIndex,size_t aToIndex) const { return m_matrix[(aFromIndex * m_to_count + aToIndex) * 2 + 1]; }

    private:
    size_t m_from_count = 0;
    size_t m_to_count = 0;
    std::vector<uint32_t> m_matrix;
    };

/** A function called by the asynchronous routing function. It provides the result code, and the route if it could be created. */
using RouterAsyncCallBack = std::function<void(Result aError,std::unique_ptr<Route> aRoute)>;

/** Parameters for creating a route. */
class RouteParam
    {
    public:
    /**
    The version of the format in which this class is serialized. Bump this number when changing the class or any of its components.
    v2: added Heading to RoutePointInternal.
    */
    static constexpr uint32_t KSerializationVersion = 2;

    /** Reads this object from an input stream. */
    void Read(InputStream& aInput);
    /** Writes this object to an output stream. */
    void Write(OutputStream& aOutput) const;

    /** The profile index is used if there are built-in route profiles. */
    uint32_t ProfileIndex = 0;
    /** The route profile is used if the router allows profiles to be set. */
    RouteProfile Profile;
    /** The waypoints. */
    std::vector<RoutePointInternal> PointArray;
    /** The number of iterations to be used when calculating a best route. If zero, a default value is used. */
    uint32_t Iterations = 0;
    /** If true, calculate the best route through the points, rather than a route through the points in order. */
    bool Best = false;
    /** If true, and if Best is true, the route always starts at the first point in RoutePointArray. */
    bool StartFixed = true;
    /** If true, and if Best is true, the route always ends at the last point in RoutePointArray. */
    bool EndFixed = true;
    /** If true, snap points to nearest nodes (junctions) before calculating the route. */
    bool SnapToNodes = false;
    /** If true, infer the heading at intermediate points in a route with more than two points. */
    bool InferHeading = true;
    };

}
