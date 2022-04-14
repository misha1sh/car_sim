#include "main/simulator.h"
#include "ui/main_window.h"

#include <boost/geometry.hpp>
#include <yaml-cpp/yaml.h>
#include <QApplication>
#include <QMessageBox>
#include <osmium/io/error.hpp>

#include <optional>


void ShowErrorMessage(const std::string& message) {
    QMessageBox messageBox;
    messageBox.critical(nullptr, "Error", message.c_str());
    messageBox.setFixedSize(500,200);
}

std::optional<SimulatorParams> ReadFromYaml(const std::string& path) {
    try {
        const auto settings = YAML::LoadFile(path);
        return SimulatorParams {
                .pixels_per_meter = settings["pixels_per_meter"].as<double>(),
                .map_path = settings["map_path"].as<std::string>(),
                .enable_cars = settings["enable_cars"].as<bool>(),
                .cars_count = settings["cars_count"].as<int>(),
                .delta_time_per_simulation = settings["delta_time_per_simulation"].as<double>()
        };
    } catch (const YAML::BadFile& exception) {
        ShowErrorMessage("Could not read file settings.yaml");
    } catch (const YAML::InvalidNode& node) {
        ShowErrorMessage("Invalid data in settins.yaml: " + node.msg);
    } catch (...) {
        ShowErrorMessage("Unable to parse settings.yaml");
    }
    return std::nullopt;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    const auto simulator_params_opt = ReadFromYaml("settings.yaml");
    if (!simulator_params_opt) {
        return -1;
    }

    // TODO: memory leak
    QThread* simulator_thread = new QThread();
    simulator_thread->setObjectName("Simulator thread");

    Simulator simulator(*simulator_params_opt);
    try {
        simulator.Initialize();
    } catch (const osmium::io_error& error) {
        ShowErrorMessage("Failed to read map from " + simulator_params_opt->map_path);
        return -1;
    } catch (...) {
        ShowErrorMessage("Failed to init");
        return -1;
    }

    simulator.moveToThread(simulator_thread);

    MapPainterPtr map_painter = std::make_shared<MapPainter>();
    map_painter->setMap(simulator.GetMapHolder());


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