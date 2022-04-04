#include "ui/main_window.h"
#include "utils/verify.h"

#include "ui/map/map_painter.h"

#include <boost/geometry.hpp>

#include <QApplication>
#include <iostream>

#include "main/simulator.h"


int main(int argc, char *argv[]) {
//    std::unique_ptr<QThread> simulator_thread = std::make_unique<QThread>();
    QThread* simulator_thread = new QThread();
    simulator_thread->setObjectName("Simulator thread");

    SimulatorParams simulator_params{};
    simulator_params.enable_cars = true;

    Simulator simulator(simulator_params);
    simulator.Initialize();

    simulator.moveToThread(simulator_thread);

//    simulator.RunTick();

    MapPainterPtr map_painter = std::make_shared<MapPainter>();
    map_painter->setMap(simulator.GetMapHolder());

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.getUI()->openGLWidget->setPainter(map_painter);
    {
        RasterMapPtr map{};
        auto guard = simulator.GetMapHolder().Get(map);
        w.getUI()->openGLWidget->setMapSize(map->size);

    }
    w.SetRunTickHandler(simulator, &Simulator::RunTickSlot);

    map_painter->setDrawSettings(w.getDrawSettings());

    simulator_thread->start();

    return a.exec();

//
//
//    typedef double coordinate_type;
//    typedef boost::geometry::model::d2::point_xy<coordinate_type> point;
//    typedef boost::geometry::model::polygon<point> polygon;
//
//    // Declare strategies
//    const double buffer_distance = 1.0;
//    const int points_per_circle = 36;
//    boost::geometry::strategy::buffer::distance_symmetric<coordinate_type> distance_strategy(buffer_distance);
//    boost::geometry::strategy::buffer::join_round join_strategy(points_per_circle);
//    boost::geometry::strategy::buffer::end_flat end_strategy;
//    boost::geometry::strategy::buffer::point_circle circle_strategy(points_per_circle);
//    boost::geometry::strategy::buffer::side_straight side_strategy;
//
//    // Declare output
//    boost::geometry::model::multi_polygon<polygon> result;
//
//    // Declare/fill a linestring
//    boost::geometry::model::linestring<point> ls;
//    boost::geometry::read_wkt("LINESTRING(0 0,4 5,7 4,10 6)", ls);
//
//    // Create the buffer of a linestring
//    boost::geometry::buffer(ls, result,
//                            distance_strategy, side_strategy,
//                            join_strategy, end_strategy, circle_strategy);

    return 0;
}