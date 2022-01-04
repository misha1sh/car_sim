#pragma once

#include <boost/geometry.hpp>
#include <osmium/geom/coordinates.hpp>
#include <osmium/osm/location.hpp>

#include <cstdint>
#include <vector>

typedef int64_t ID;

typedef boost::geometry::model::d2::point_xy<double> PointF;
typedef boost::geometry::model::polygon<PointF> PolygonF;
typedef boost::geometry::model::linestring<PointF> PolylineF;
typedef boost::geometry::model::multi_polygon<PolygonF> MultiPolygonF;

typedef boost::geometry::model::d2::point_xy<long long> PointI;
typedef boost::geometry::model::polygon<PointI> PolygonI;
typedef boost::geometry::model::linestring<PointI> PolylineI;
typedef boost::geometry::model::multi_polygon<PolygonI> MultiPolygonI;

union Coord : PointF {
    struct { double x, y; } xy;
    struct { double lon, lat;} ll;
    PointF p;

    Coord(double x, double y): xy{x, y} {}
    explicit Coord(const osmium::geom::Coordinates& coord) :
            xy{coord.x, coord.y} {}
    explicit Coord(const osmium::Location& location) :
            ll{.lon=location.lon_without_check(), .lat=location.lat_without_check()} {}
    explicit Coord(PointF pp) :
            p{std::move(pp)} {}

    Coord() = default;
    Coord(const Coord& coord) = default;
    Coord(Coord&& coord) = default;
    Coord& operator=(const Coord&) = default;
    Coord& operator=(Coord&&) = default;

    Coord operator+(const Coord& other) const {
        return {xy.x + other.xy.x, xy.y + other.xy.y};
    }
};

struct Node {
    ID id;
    Coord c;
};

struct Road {
    ID id;
    int lanes_count;
    bool is_one_way;
    std::vector<ID> nodes;
};