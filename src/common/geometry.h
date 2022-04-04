#pragma once

#include "common/entities.h"
#include <iostream>


struct Line {
    PointF a;
    PointF b;

    PointF Dir() const {
        return (b - a).Norm();
    }

    static inline Line FromPoints(const PointF& p1, const PointF& p2) {
        return {p1, p2};
    }

    static inline Line FromPointAndDir(const PointF& p1, const PointF& dir) {
        return FromPoints(p1, p1 + dir);
    }
};

namespace geometry {

inline double Distance(const PointF& p1, const PointF& p2) {
    return (p1 - p2).Len();
}

std::optional<PointF> LineIntersection(PointF a1, PointF a2, PointF b1, PointF b2);

std::optional<PointF> LineIntersection(const Line& l1, const Line& l2);

// TODO: replace with a better algo
PointF ProjectOnLine(const Line& line, const PointF& point);

double DistancePointToSegment(const PointF& p1, const PointF& seg_1, const PointF& seg_2);

}  // namespace geometry



