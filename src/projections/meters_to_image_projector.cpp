//
// Created by misha-sh on 1/4/22.
//

#include "meters_to_image_projector.h"

MetersToImageProjector:: MetersToImageProjector(Coord min_corner, Coord max_corner,
                                                size_t image_x_size, size_t image_y_size) :
        min_corner_(min_corner),
        max_corner_(max_corner),
        image_x_size_(image_x_size),
        image_y_size_(image_y_size) {
}

inline size_t projectValue(const double value,
                       const double min_val_before, const double max_val_before,
                       const size_t min_val_after, const size_t max_val_after) {
    const double norm_val = ((value - min_val_before) /
                             (max_val_before - min_val_before));
    return norm_val * (max_val_after - min_val_after) + max_val_after;
}

PointI MetersToImageProjector::project(const Coord& coord) const {
    return {
            projectValue(coord.xy.x, min_corner_.xy.x, max_corner_.xy.x, image_x_size_, image_y_size_),
            projectValue(coord.xy.x, min_corner_.xy.x, max_corner_.xy.x, image_x_size_, image_y_size_)
    };
}
