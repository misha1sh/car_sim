#include "roads_rasterizer.h"

#include "utils/verify.h"

#include <boost/geometry.hpp>


PolygonF ExpandPolyline(const PolylineF& polyline, double width) {
    const int points_per_circle = 4;
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(width / 2.);
    boost::geometry::strategy::buffer::join_round join_strategy(points_per_circle);
    boost::geometry::strategy::buffer::end_flat end_strategy;
    boost::geometry::strategy::buffer::point_circle circle_strategy(points_per_circle);
    boost::geometry::strategy::buffer::side_straight side_strategy;

    MultiPolygonF result;
    boost::geometry::buffer(polyline, result,
                            distance_strategy, side_strategy,
                            join_strategy, end_strategy, circle_strategy);

    VERIFY(result.size() == 1);

    return result[0];
}



PolygonF ExpandPoint(const PointF& point, const double radius) {
    boost::geometry::strategy::buffer::point_circle point_strategy(8);
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(radius);

    boost::geometry::strategy::buffer::join_round    join_strategy;
    boost::geometry::strategy::buffer::end_round     end_strategy;
    boost::geometry::strategy::buffer::side_straight side_strategy;


    MultiPolygonF result;
    boost::geometry::buffer((BoostPointF)point, result,
                            distance_strategy, side_strategy,
                            join_strategy, end_strategy, point_strategy);

    VERIFY(result.size() == 1);
    return result[0];
}