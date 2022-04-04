//
// Created by misha-sh on 1/4/22.
//

#include "meters_to_image_projector.h"

MetersToImageProjector:: MetersToImageProjector(PointF min_corner, PointF max_corner,
                                                int image_x_size, int image_y_size) :
        min_corner_(min_corner),
        max_corner_(max_corner),
        image_x_size_(image_x_size),
        image_y_size_(image_y_size) {
}

inline PointIType projectValue(const double value,
                       const double min_val_before, const double max_val_before,
                       const int min_val_after, const int max_val_after) {
    const double norm_val = ((value - min_val_before) /
                             (max_val_before - min_val_before));
    return norm_val * (max_val_after - min_val_after) + min_val_after;
}

PointI MetersToImageProjector::project(const PointF& coord) const {
    return {
            projectValue(coord.x, min_corner_.x, max_corner_.x, 0, image_x_size_),
            projectValue(coord.y, min_corner_.y, max_corner_.y, 0, image_y_size_)
    };
}

PolygonI MetersToImageProjector::project(const PolygonF& polygonF) const {
    PolygonI polygonI;
    for (const auto& pointF : polygonF.outer()) {
        polygonI.outer().push_back(project(pointF));
    }
    for (const auto& innerF : polygonF.inners()) {
        PolygonI::ring_type innerI;
        for (const auto& pointF : innerF) {
            innerI.push_back(project(pointF));
        }
        polygonI.inners().push_back(std::move(innerI));
    }
    return polygonI;
}
