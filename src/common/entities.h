#pragma once

#include "utils/math_utils.h"

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

typedef int PointIType;
typedef boost::geometry::model::d2::point_xy<PointIType> PointI;
typedef boost::geometry::model::polygon<PointI> PolygonI;
typedef boost::geometry::model::linestring<PointI> PolylineI;
typedef boost::geometry::model::multi_polygon<PolygonI> MultiPolygonI;

struct Coord {
    double x = 0;
    double y = 0;

    template <typename TNum1, typename TNum2>
    Coord(TNum1 _x, TNum2 _y) : x(_x), y(_y) {}
//    Coord(int _x, int _y) : x(_x), y(_y) {}
//    Coord(double _x, double _y): x(_x), y(_y) {}
    explicit Coord(const osmium::geom::Coordinates& coord) :
            Coord(coord.x, coord.y) {}
    explicit Coord(const osmium::Location& location) :
            Coord(location.lon_without_check(), location.lat_without_check()) {}
    explicit Coord(PointF pp) :
            Coord(pp.x(), pp.y()) {}
    explicit Coord(PointI pp) :
            Coord(pp.x(), pp.y()) {}

    Coord() = default;
    Coord(const Coord& coord) = default;
    Coord(Coord&& coord) = default;
    Coord& operator=(const Coord&) = default;
    Coord& operator=(Coord&&) = default;

    inline Coord operator+(const Coord& other) const {
        return {x + other.x, y + other.y};
    }

    inline Coord operator-(const Coord& other) const {
        return {x - other.x, y - other.y};
    }


    inline Coord operator*(const Coord& other) const {
        return {x * other.x, y * other.y};
    }

    inline Coord operator/(const Coord& other) const {
        return {x / other.x, y / other.y};
    }



    template <typename T>
    inline Coord operator+(const T& num) const {
        return {x + num, y + num};
    }

    template <typename T>
    inline Coord operator-(const T& num) const {
        return {x - num, y - num};
    }

    template <typename T>
    inline Coord operator*(const T& num) const {
        return {x * num, y * num};
    }

    template <typename T>
    inline Coord operator/(const T& num) const {
        return {x / num, y / num};
    }

    inline double SqrLen() const {
        return x * x + y * y;
    }

    inline double Len() const {
        return sqrt(SqrLen());
    }

    inline Coord Norm() const {
        const double len = Len();
        if (len < 0.0000001) {
            return {1, 0};
        }
        return (*this) / len;
    }

    inline double AngleDenormalized(const Coord& other) const {
        const double dot = x*other.x + y*other.y;
        const double det = x*other.y - y*other.x;
        return atan2(det, dot);
    }

    // (-pi, pi]
    inline double Angle(const Coord& other) const {
        double angle = AngleDenormalized(other);
        if (angle > M_PI) {
            angle -= 2 * M_PI;
        } else if (angle <= -M_PI) {
            angle += 2 * M_PI;
        }
        return angle;
    }

    // [0, pi]
    inline double AngleAbs(const Coord& other) const {
        return std::abs(Angle(other));
    }

    inline Coord RightPerpendicular() const {
        return Coord{y, -x}.Norm();
    }

    inline Coord LeftPerpendicular() const {
        return -RightPerpendicular();
    }

    inline Coord Crop(const Coord& min_val, const Coord& max_val) const {
        return {
                math_utils::Crop(x, min_val.x, max_val.x),
                math_utils::Crop(y, min_val.y, max_val.y),
        };
    }

    inline Coord Crop(const double min_val, const double max_val) const {
        return Crop({min_val, min_val},
                    {max_val, max_val});
    }

    inline bool operator==(const Coord& other) const {
        return x == other.x && y == other.y;
    }


    inline bool operator!=(const Coord& other) const {
        return x != other.x || y != other.y;
    }

    inline Coord& operator+=(const Coord& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    inline Coord& operator-=(const Coord& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    inline Coord& operator*=(const Coord& other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    inline Coord& operator/=(const Coord& other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    inline Coord operator-() const {
        return {-x, -y};
    }

    inline PointI asPointI() const {
        return {static_cast<int>(x), static_cast<int>(y)};
    }


    inline PointF asPointF() const {
        return {x, y};
    }
};

struct Node {
    ID id{-1};
    Coord c{};
};

struct Road {
    ID id{-1};
    int lanes_count =0;
    bool is_one_way = false;
    std::vector<ID> nodes{};
};