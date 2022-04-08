#pragma once

#include "common/entities.h"

#include <vector>

class MetersToImageProjector {
public:
    MetersToImageProjector(PointF min_corner, PointF max_corner,
                           int image_x_size, int image_y_size);

    PointI project(const PointF& coord) const;
    PointF projectF(const PointF& coord) const;
    PointF projectBackwards(const PointF& coord_on_image) const;
    PolygonI project(const PolygonF& polygonF) const;

private:
    PointF min_corner_, max_corner_;
    int image_x_size_, image_y_size_;
};

