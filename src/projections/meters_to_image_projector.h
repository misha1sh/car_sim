#pragma once

#include "common/entities.h"

#include <vector>

class MetersToImageProjector {
public:
    MetersToImageProjector(Coord min_corner, Coord max_corner,
                           int image_x_size, int image_y_size);

    PointI project(const PointF& coord) const;
    PolygonI project(const PolygonF& polygonF) const;

private:
    Coord min_corner_, max_corner_;
    int image_x_size_, image_y_size_;
};

