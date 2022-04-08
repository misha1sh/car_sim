#pragma once

#include "utils/math_utils.h"

#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <osmium/geom/coordinates.hpp>
#include <osmium/osm/location.hpp>

#include <iomanip>
#include <cstdint>
#include <vector>

typedef int64_t ID;


typedef boost::geometry::model::d2::point_xy<double> BoostPointF;
typedef boost::geometry::model::polygon<BoostPointF> PolygonF;
typedef boost::geometry::model::linestring<BoostPointF> PolylineF;
typedef boost::geometry::model::multi_polygon<PolygonF> MultiPolygonF;


typedef int PointIType;
typedef boost::geometry::model::d2::point_xy<PointIType> BoostPointI;
typedef boost::geometry::model::polygon<BoostPointI> PolygonI;
typedef boost::geometry::model::linestring<BoostPointI> PolylineI;
typedef boost::geometry::model::multi_polygon<PolygonI> MultiPolygonI;

struct PointF;

struct PointI {
    PointIType x = 0;
    PointIType y = 0;

    template<typename TNum1, typename TNum2,
            std::enable_if_t<std::is_integral<TNum1>::value, bool> = true,
            std::enable_if_t<std::is_integral<TNum2>::value, bool> = true>
    PointI(TNum1 _x, TNum2 _y) : x(_x), y(_y) {}

    PointI(BoostPointI pp) :
        PointI(pp.x(), pp.y()) {}

    operator BoostPointI() const {
        return {x, y};
    }

    inline PointF asPointF() const;

    PointI() = default;

    PointI(const PointI &coord) = default;

    PointI(PointI &&coord) = default;

    PointI &operator=(const PointI &) = default;

    PointI &operator=(PointI &&) = default;

    bool operator==(const PointI& other) const = default;

};

struct PointF {
    double x = 0;
    double y = 0;

    template<typename TNum1, typename TNum2>
    PointF(TNum1 _x, TNum2 _y) : x(_x), y(_y) {}

    explicit PointF(const osmium::geom::Coordinates &coord) :
            PointF(coord.x, coord.y) {}

    explicit PointF(const osmium::Location &location) :
            PointF(location.lon_without_check(), location.lat_without_check()) {}

    explicit PointF(PointI pp) :
            PointF(pp.x, pp.y) {}

    explicit PointF(BoostPointI pp) :
            PointF(pp.x(), pp.y()) {}

    PointF(BoostPointF pp) :
            PointF(pp.x(), pp.y()) {}

    operator BoostPointF() const {
        return {x, y};
    }

    inline PointI asPointI() const {
        return {static_cast<int>(x), static_cast<int>(y)};
    }

    PointF() = default;

    PointF(const PointF &coord) = default;

    PointF(PointF &&coord) = default;

    PointF &operator=(const PointF &) = default;

    PointF &operator=(PointF &&) = default;

    inline PointF operator+(const PointF &other) const {
        return {x + other.x, y + other.y};
    }

    inline PointF operator-(const PointF &other) const {
        return {x - other.x, y - other.y};
    }


    inline PointF operator*(const PointF &other) const {
        return {x * other.x, y * other.y};
    }

    inline PointF operator/(const PointF &other) const {
        return {x / other.x, y / other.y};
    }


    template<typename T>
    inline PointF operator+(const T &num) const {
        return {x + num, y + num};
    }

    template<typename T>
    inline PointF operator-(const T &num) const {
        return {x - num, y - num};
    }

    template<typename T>
    inline PointF operator*(const T &num) const {
        return {x * num, y * num};
    }

    template<typename T>
    inline PointF operator/(const T &num) const {
        return {x / num, y / num};
    }

    inline double SqrLen() const {
        return x * x + y * y;
    }

    inline double Len() const {
        return sqrt(SqrLen());
    }

    inline PointF Norm() const {
        const double len = Len();
        if (len < 0.0000001) {
            return {1, 0};
        }
        return (*this) / len;
    }

    inline double AngleDenormalized(const PointF &other) const {
        const double dot = x * other.x + y * other.y;
        const double det = x * other.y - y * other.x;
        return atan2(det, dot);
    }

    // (-pi, pi]
    inline double Angle(const PointF &other) const {
        double angle = AngleDenormalized(other);
        if (angle > M_PI) {
            angle -= 2 * M_PI;
        } else if (angle <= -M_PI) {
            angle += 2 * M_PI;
        }
        return angle;
    }

    // [0, pi]
    inline double AngleAbs(const PointF &other) const {
        return std::abs(Angle(other));
    }

    inline PointF Rotate(const double angle) const {
        double s, c;
        s = sin(angle);
        c = cos(angle);
        return {x * c - y * s,
                x * s + y * c};
    }

    inline std::tuple<PointF, bool> RotateTowards(const PointF &other, const double angle_speed) {
        double angle = Angle(other);

        if (std::abs(angle) <= angle_speed) {
            return {other, true};
        }

        angle = std::copysign(angle_speed, angle);

        double c = cos(angle);
        double s = sin(angle);
        double xx = x * c + y * s;
        double yy = y * c + x * s;

        return {
                {xx, yy},
                false
        };
    }

    inline PointF RightPerpendicular() const {
        return PointF{y, -x}.Norm();
    }

    inline PointF LeftPerpendicular() const {
        return -RightPerpendicular();
    }

    inline PointF Crop(const PointF &min_val, const PointF &max_val) const {
        return {
                math_utils::Crop(x, min_val.x, max_val.x),
                math_utils::Crop(y, min_val.y, max_val.y),
        };
    }

    inline PointF Crop(const double min_val, const double max_val) const {
        return Crop({min_val, min_val},
                    {max_val, max_val});
    }

    inline bool operator==(const PointF &other) const {
        return x == other.x && y == other.y;
    }


    inline bool operator!=(const PointF &other) const {
        return x != other.x || y != other.y;
    }

    inline PointF &operator+=(const PointF &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    inline PointF &operator-=(const PointF &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    inline PointF &operator*=(const PointF &other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    inline PointF &operator/=(const PointF &other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    inline PointF operator-() const {
        return {-x, -y};
    }



    friend std::ostream &operator<<(std::ostream &stream, const PointF &coord) {
        stream << std::fixed << std::setprecision(6) << "(" << coord.x << " " << coord.y << ")";
        return stream;
    }
};


PointF PointI::asPointF() const {
    return {x, y};
}

struct Node {
    ID id{-1};
    PointF c{};
};

struct Road {
    ID id{-1};
    int lanes_count = 0;
    bool is_one_way = false;
    std::vector<ID> nodes{};
};