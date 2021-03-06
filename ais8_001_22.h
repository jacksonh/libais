#ifndef AIS8_001_22_H
#define AIS8_001_22_H

// -*- c++ -*-
// Way complicated
// Since 2011-Feb-03


/*
  Design notes:

  Matches IMO Circ 289, Page 39.  May or may not match RTCM working
  group final message.  That is a moving target and is in ais8_366_22

  For polygon and polyline, I am planning to keep the data structures
  matching exactly the VDL data structure.  From there, I am hoping to
  extend the message class to separately reassemble that polyline and polygon.
 */

const size_t AIS8_001_22_NUM_NAMES=128;
const size_t AIS8_001_22_SUBAREA_SIZE=87;
extern const char *ais8_001_22_notice_names[AIS8_001_22_NUM_NAMES];

enum Ais8_001_22_AreaShapeEnum {
    AIS8_001_22_SHAPE_ERROR = -1,
    AIS8_001_22_SHAPE_CIRCLE = 0, // OR Point
    AIS8_001_22_SHAPE_RECT = 1,
    AIS8_001_22_SHAPE_SECTOR = 2,
    AIS8_001_22_SHAPE_POLYLINE = 3,
    AIS8_001_22_SHAPE_POLYGON = 4,
    AIS8_001_22_SHAPE_TEXT = 5,
    AIS8_001_22_SHAPE_RESERVED_6 = 6,
    AIS8_001_22_SHAPE_RESERVED_7 = 7
};

extern const char *ais8_001_22_shape_names[8];


//////////////////////////////////////////////////////////////////////
// Sub-Areas for the Area Notice class
//////////////////////////////////////////////////////////////////////

class Ais8_001_22_SubArea {
 public:
    virtual Ais8_001_22_AreaShapeEnum getType() const = 0;
    virtual ~Ais8_001_22_SubArea() {}
    virtual void print()=0;
};

Ais8_001_22_SubArea* ais8_001_22_subarea_factory(const std::bitset<AIS8_MAX_BITS> &bs, const size_t offset);

// or Point if radius is 0
class Ais8_001_22_Circle : public Ais8_001_22_SubArea {
 public:
    float x,y; // longitude and latitude
    // Going to assume that the precision is load of crap
    int precision; // How many decimal places for x and y.  Track it, but it's useless in my opinion
    int radius_m;
    unsigned int spare; // 18 bits

    Ais8_001_22_Circle(const std::bitset<AIS8_MAX_BITS> &bs, const size_t offset);
    ~Ais8_001_22_Circle() {}
    Ais8_001_22_AreaShapeEnum getType() const {return AIS8_001_22_SHAPE_CIRCLE;}
    void print();
};

class Ais8_001_22_Rect : public Ais8_001_22_SubArea {
 public:
    float x,y; // longitude and latitude
    int precision; // How many decimal places for x and y.  Useless
    int e_dim_m; // East dimension in meters
    int n_dim_m;
    int orient_deg; // Orientation in degrees from true north
    unsigned int spare; // 5 bits

    Ais8_001_22_Rect(const std::bitset<AIS8_MAX_BITS> &bs, const size_t offset);
    ~Ais8_001_22_Rect() {}
    Ais8_001_22_AreaShapeEnum getType() const {return AIS8_001_22_SHAPE_RECT;}
    void print();

};

class Ais8_001_22_Sector : public Ais8_001_22_SubArea {
 public:
    float x,y; // longitude and latitude
    int precision; // How many decimal places for x and y  FIX: in IMO, but not RTCM
    int radius_m;
    int left_bound_deg;
    int right_bound_deg;

    Ais8_001_22_Sector(const std::bitset<AIS8_MAX_BITS> &bs, const size_t offset);
    ~Ais8_001_22_Sector() {}
    Ais8_001_22_AreaShapeEnum getType() const {return AIS8_001_22_SHAPE_SECTOR;}
    void print();
};



// FIX: do I bring in the prior point x,y, precision?
// And do we fold the sub area data
// into one polygon if there are more than one?
struct Ais8_001_22_Polybase : public Ais8_001_22_SubArea {
public:
    // TODO: int precision; // How many decimal places for x and y.  FIX: in IMO

    // Up to 4 points
    std::vector<float> angles;
    std::vector<float> dists_m;
    unsigned int spare; // 2 bit
    
    Ais8_001_22_Polybase(const std::bitset<AIS8_MAX_BITS> &bs, const size_t offset);
    void print();
};


// Or Waypoint
// Must have a point before on the VDL
// FIX: do I bring in the prior point x,y, precision?
class Ais8_001_22_Polyline : public Ais8_001_22_Polybase {
public:
    Ais8_001_22_Polyline(const std::bitset<AIS8_MAX_BITS> &bs, const size_t offset);
    ~Ais8_001_22_Polyline() {}
    Ais8_001_22_AreaShapeEnum getType() const {return AIS8_001_22_SHAPE_POLYLINE;}
    void print();
};

// FIX: brin in the prior point?  And do we fold the sub area data
// into one polygon if there are more than one?
class Ais8_001_22_Polygon : public Ais8_001_22_Polybase {
public:
    Ais8_001_22_Polygon(const std::bitset<AIS8_MAX_BITS> &bs, const size_t offset);
    ~Ais8_001_22_Polygon() {}
    Ais8_001_22_AreaShapeEnum getType() const {return AIS8_001_22_SHAPE_POLYGON;}
    void print();
};


class Ais8_001_22_Text : public Ais8_001_22_SubArea {
 public:
    std::string text;
    // TODO: spare?

    Ais8_001_22_Text(const std::bitset<AIS8_MAX_BITS> &bs, const size_t offset);
    ~Ais8_001_22_Text() {}
    Ais8_001_22_AreaShapeEnum getType() const {return AIS8_001_22_SHAPE_TEXT;}
    void print();
};


//////////////////////////////////////////////////////////////////////
// Area Notice class
//////////////////////////////////////////////////////////////////////

class Ais8_001_22 : public Ais8 {
 public:
    // Common block at the front
    int link_id; // 10 bit id to match up text blocks
    int notice_type; // area_type / Notice Description
    int month; // These are in UTC
    int day;   // UTC!
    int hour;  // UTC!
    int minute;
    int duration_minutes; // Time from the start until the notice expires

    // 1 or more sub messages

    std::vector<Ais8_001_22_SubArea *> sub_areas;

    /// @arg pad Padding bits, this is the last value before the checksum in the NMEA string.
    Ais8_001_22(const char *nmea_payload, const size_t pad);
    ~Ais8_001_22();
    void print();

    /*
       FIX: need some sort of higher level accessors and checks
       - return interpreted geometry and associated agreegated text
         - What formats to return?  GeoJSON, WKT, etc?
       - validate that the contents of sub_areas is sane
         - polylines and polygons start with a point
         - text has an associated geometry
    */

};
std::ostream& operator<< (std::ostream& o, Ais8_001_22 const& msg);

/// WGS84 position in degrees.
struct AisPosition
{
    float longitude;
    float latitude;
    
    AisPosition(float lon, float lat):longitude(lon),latitude(lat){}
    AisPosition(AisPosition const &p):longitude(p.longitude),latitude(p.latitude){}

    bool operator!=(AisPosition const&other) const {return longitude != other.longitude || latitude != other.latitude;}
    bool operator==(AisPosition const&other) const {return !((*this)!=other);}
    
    /// Constructs a position from an inital point along with a 
    /// range and bearing.
    /// Vincenty's formula is used to calculate the resulting position
    /// on the WGS84 ellipsoid.
    /// http://en.wikipedia.org/wiki/Vincenty%27s_formulae
    /// Vincenty, T. (April 1975a). "Direct and Inverse Solutions of Geodesics on the Ellipsoid with application of nested equations". Survey Review XXIII (misprinted as XXII) (176): 88–93.
    AisPosition(AisPosition const &prev, float angle, float range);
};

std::vector<AisPosition> convertToPositions(Ais8_001_22 const &msg);

#endif
