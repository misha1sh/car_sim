#include "ui/main_window.h"

#include <boost/geometry.hpp>

#include <QApplication>
#include <iostream>


int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();



    typedef double coordinate_type;
    typedef boost::geometry::model::d2::point_xy<coordinate_type> point;
    typedef boost::geometry::model::polygon<point> polygon;

    // Declare strategies
    const double buffer_distance = 1.0;
    const int points_per_circle = 36;
    boost::geometry::strategy::buffer::distance_symmetric<coordinate_type> distance_strategy(buffer_distance);
    boost::geometry::strategy::buffer::join_round join_strategy(points_per_circle);
    boost::geometry::strategy::buffer::end_flat end_strategy;
    boost::geometry::strategy::buffer::point_circle circle_strategy(points_per_circle);
    boost::geometry::strategy::buffer::side_straight side_strategy;

    // Declare output
    boost::geometry::model::multi_polygon<polygon> result;

    // Declare/fill a linestring
    boost::geometry::model::linestring<point> ls;
    boost::geometry::read_wkt("LINESTRING(0 0,4 5,7 4,10 6)", ls);

    // Create the buffer of a linestring
    boost::geometry::buffer(ls, result,
                            distance_strategy, side_strategy,
                            join_strategy, end_strategy, circle_strategy);

    return 0;
}