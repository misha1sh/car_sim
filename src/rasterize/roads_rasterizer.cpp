#include "roads_rasterizer.h"

#include "common/entities.h"

#include <range/v3/all.hpp>

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

    if (result.size() != 1) {
        throw std::logic_error("Incorrect polygons count");
    }

    return result[0];
}

PolylineF BuildPolyline(const RoadsVectorMapPtr &vector_map, const Road& road) {
    PolylineF polyline;
    polyline.resize(road.nodes.size());
    for (size_t i = 0; i < road.nodes.size(); i++) {
        const auto& node_id = road.nodes[i];
        const auto& c = vector_map->nodes.at(node_id).c;
        polyline[i] = projector.project(c).p;
    }
    return polyline;
}



void RoadsRasterizer::RasterizeRoads(const RoadsVectorMapPtr &vector_map, RasterMap &map) {
    const double lane_width = 4.0;
    for (const auto& [road_id, road] : vector_map->roads) {
        const auto polyline = BuildPolyline(vector_map, road);
        const auto polygon = ExpandPolyline(polyline, road.lanes_count);
        map.road_dir.fill(polygon, {0.1, 0.2});
    }
}
