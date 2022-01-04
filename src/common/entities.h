#pragma once

#include <osmium/geom/coordinates.hpp>

#include <cstdint>
#include <vector>

typedef int64_t ID;

union Coord {
    struct { double x, y; } xy;
    struct { double lon, lat;} ll;

    Coord(double x, double y): xy{x, y} {}
    explicit Coord(const osmium::geom::Coordinates& coord) :
            xy{coord.x, coord.y} {}

    Coord() = default;
    Coord(const Coord& coord) = default;
    Coord(Coord&& coord) = default;
    Coord& operator=(const Coord&) = default;
    Coord& operator=(Coord&&) = default;
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