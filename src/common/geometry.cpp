#include "geometry.h"


namespace geometry {


std::optional<PointF> LineIntersection(PointF a1, PointF a2, PointF b1, PointF b2) {
//    std::cerr << a1 << " " << a2 << " " << b1 << " " << b2 << " intersects at :";
    const auto x1 = a1.x;
    const auto y1 = a1.y;
    const auto x2 = a2.x;
    const auto y2 = a2.y;
    const auto x3 = b1.x;
    const auto y3 = b1.y;
    const auto x4 = b2.x;
    const auto y4 = b2.y;

    const double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (std::abs(d) < 0.001) {
//        std::cerr << "kek!!" << " " << d << " " << std::endl;
        return std::nullopt;
    }
    const auto f1 = (x1*y2 - y1*x2);
    const auto f2 = (x3*y4 - y3*x4);

    return PointF{
            (f1 * (x3 - x4) - (x1 - x2) * f2) / d,
            (f1 * (y3 - y4) - (y1 - y2) * f2) / d
    };
}

std::optional<PointF> LineIntersection(const Line& l1, const Line& l2) {
    return LineIntersection(l1.a, l1.b, l2.a, l2.b);
}

PointF ProjectOnLine(const Line& line, const PointF& point) {
    const auto line_dir = (line.b - line.a).Norm();
    const auto line_perp_dir = line_dir.RightPerpendicular();
    const auto perpendicular_line = Line::FromPointAndDir(
            point,
            line_perp_dir
    );

    const auto projection_opt = LineIntersection(line, perpendicular_line);
    if (!projection_opt) {
        std::cerr << "ERROR WHILE PROJECTING \n";
        return point;
    }
    return *projection_opt;
}

double Dot(const PointF& p1, const PointF& p2) {
    return p1.x * p2.x + p1.y * p2.y;
}

// https://stackoverflow.com/a/1501725
double DistancePointToSegment(const PointF& p, const PointF& v, const PointF& w) {
    // Return minimum distance between line segment vw and point p
    const double l2 = (v - w).SqrLen();  // i.e. |w-v|^2 -  avoid a sqrt
    if (l2 == 0.0) return Distance(p, v);   // v == w case
    // Consider the line extending the segment, parameterized as v + t (w - v).
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    // We clamp t from [0,1] to handle points outside the segment vw.
    const double t = std::max(0., std::min(1., Dot(p - v, w - v) / l2));
    const PointF projection = v + (w - v) * t;  // Projection falls on the segment
    return Distance(p, projection);
}

inline double pow3(const double x) {
    return x*x*x;
}

inline double pow2(const double x) {
    return x*x;
}

inline double Sign(const double x) {
    if (x > 0) {
        return 1;
    }
    if (x < 0) {
        return -1;
    }
    return 0;
}

double OrientedProjectionLength(const PointF& p1, const PointF& p2, const PointF& p) {
    const Line l{p1, p2};
    const PointF projection = ProjectOnLine(l, p);
    const PointF diff = projection - p1;

    const auto dir = l.Dir();
    double sign;
    if (dir.x > dir.y) {
        sign = Sign(diff.x / dir.x);
    } else {
        sign = Sign(diff.y / dir.y);
    }
    return sign * diff.Len();
}

PointF BeizerCurve(double t,
                   const PointF& p1, const PointF& p2,
                   const PointF& p3, const PointF& p4) {
    const double k1 = pow3(1 -t);
    const double k2 = 3 * pow2(1 - t) * t;
    const double k3 = 3 * (1 - t) * pow2(t);
    const double k4 = pow3(t);
    return p1*k1 + p2*k2 + p3*k3 + p4*k4;
}

std::tuple<double, PointF> FindBeizerProjectionBinarySearch(double min_t, double max_t, const PointF p,
                                                           const PointF& p1, const PointF& p2,
                                                           const PointF& p3, const PointF& p4) {
    const auto calc_dist = [&](double t) {
        const auto b = BeizerCurve(t, p1, p2, p3, p4);
        return (b - p).SqrLen();
    };


    const double precision = 0.001;
    double left = min_t;
    double right = max_t;
    while (right - left > precision) {
        double m1 = left + (right - left) / 3.;
        double m2 = right - (right - left) / 3;

        if (calc_dist(m1) > calc_dist(m2)) {
            left = m1;
        } else {
            right = m2;
        }
    }

    return {
        right,
        BeizerCurve(right, p1, p2, p3, p4)
    };
}

std::tuple<double, PointF> FindBeizerProjection(double min_t, double max_t, const PointF& p,
                                               const PointF& p1, const PointF& p2,
                                               const PointF& p3, const PointF& p4) {
//    return FindBeizerProjectionBinarySearch(min_t, max_t,
//                                                    p, p1, p2, p3, p4);


    double step = 0.02;

    const auto calc_dist = [&](double t) {
        const auto b1 = BeizerCurve(t, p1, p2, p3, p4);
        const auto b2 = BeizerCurve(std::min(1., t + step), p1, p2, p3, p4);
        return DistancePointToSegment(p, b1, b2);
//        return (b - p).SqrLen();
    };

    double last_dist = calc_dist(min_t);
    double last_t = min_t;
    for (double t = min_t; t + step <= max_t; t += step) {
        double dist = calc_dist(t);
        if (dist > last_dist) {
            return FindBeizerProjectionBinarySearch(last_t, std::min(last_t + step, 1.),
                                                    p, p1, p2, p3, p4);
        }
    }

    return {
        1.,
        p4
    };
}

}