#pragma once

#include "common/entities.h"

#include <vector>

class MetersToImageProjector {
public:
    MetersToImageProjector(Coord min_corner, Coord max_corner,
                           size_t image_x_size, size_t image_y_size);

    PointI project(const Coord& coord) const;


private:
    Coord min_corner_, max_corner_;
    size_t image_x_size_, image_y_size_;
};

