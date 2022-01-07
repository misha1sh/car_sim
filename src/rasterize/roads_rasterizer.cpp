#include "roads_rasterizer.h"

#include "common/entities.h"
#include "utils/verify.h"

#include <QElapsedTimer>
#include <fmt/core.h>
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

    VERIFY(result.size() == 1);

    return result[0];
}

PolygonF ExpandPoint(const PointF& point, const double radius) {
//    boost::geometry::strategy::buffer::point_circle point_strategy(4);
//    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(radius);
//
//    boost::geometry::strategy::buffer::join_round    join_strategy;
//    boost::geometry::strategy::buffer::end_round     end_strategy;
//    boost::geometry::strategy::buffer::side_straight side_strategy;
//
//
//    MultiPolygonF result;
//    boost::geometry::buffer(point, result,
//               distance_strategy, side_strategy,
//               join_strategy, end_strategy, point_strategy);
//
//    VERIFY(result.size() == 1);
//    return result[0];
    return PolygonF{{
//                            PointF{0, 0},
//                            PointF{0, 100},
//                            PointF{100, 100},
//                            PointF{100, 0},
//                            PointF{0, 0},
                            PointF{point.x() - 30, point.y() - 30},
                            PointF{point.x() - 30, point.y() + 30},
                            PointF{point.x() + 30, point.y() + 30},
                            PointF{point.x() + 30, point.y() - 30},
                            PointF{point.x() - 30, point.y() - 30}
                    }};
}


PolylineF BuildPolyline(const RoadsVectorMapPtr &vector_map, const Road& road) {
    PolylineF polyline;
    polyline.resize(road.nodes.size());
    for (size_t i = 0; i < road.nodes.size(); i++) {
        const auto& node_id = road.nodes[i];
        const auto& c = vector_map->nodes.at(node_id).c;;;
        polyline[i] = c.asPointF();
    }

    return polyline;
}


void RoadsRasterizer::RasterizeRoads(const RoadsVectorMapPtr &vector_map, RasterMap &map) {
    QElapsedTimer timer;
    timer.start();

    const double lane_width = 4.0;
    MetersToImageProjector imageProjector{vector_map->stats.min_xy, vector_map->stats.max_xy,
                                          map.road_dir.sizeX(), map.road_dir.sizeY()};

    {
        const auto metersSize = vector_map->stats.max_xy - vector_map->stats.min_xy;
        fmt::print("Rasterizing roads from {}x{} meters to {}x{} px", metersSize.x, metersSize.y, map.road_dir.sizeX(), map.road_dir.sizeY());
    }


    for (const auto& [road_id, road] : vector_map->roads) {
        const auto polyline = BuildPolyline(vector_map, road);
        for (int i = 0; i + 1 < polyline.size(); i++) {
            const PolylineF segment{polyline[i], polyline[i + 1]};
            const auto polygon = ExpandPolyline(polyline, road.lanes_count * lane_width);
            const PolygonI polygonImage = imageProjector.project(polygon);

            const Coord dir = (Coord{polyline[i + 1]} - Coord{polyline[i]}).Norm();
            map.road_dir.fill(polygonImage, dir);
        }
    }
    fmt::print("Build roadsDir took {} ms\n", timer.restart());

    int tooBigJunctionCount = 0;
    for (const auto& [node_id, roads] : vector_map->roads_for_node) {
        const auto outgoingRoads = roads | ranges::views::remove_if([](const auto& road) {
            return road.next_node == std::nullopt;
        }) | ranges::to_vector;
        if (outgoingRoads.size() <= 1) {
            continue;
        }

        const Node& node1 = vector_map->nodes.at(node_id);
        const auto centerPos = node1.c;
        const auto projectedCenterPos = imageProjector.project(centerPos.asPointF());
        const auto polygon = ExpandPoint(centerPos.asPointF(), lane_width * 40);
        const PolygonI polygonImage = imageProjector.project(polygon);

        for (const auto& [road_id, next_node_id] : outgoingRoads) {
            VERIFY(next_node_id);
            const Node& node2 = vector_map->nodes.at(*next_node_id);
            const Coord dir = (node2.c - node1.c).Norm();

            if (map.decision1.getOrDefault(projectedCenterPos.x(), projectedCenterPos.y(), {0, 0}) == Coord{0, 0}) {
                map.decision1.fill(polygonImage, dir);
            } else if (map.decision2.getOrDefault(projectedCenterPos.x(), projectedCenterPos.y(), {0, 0}) == Coord{0, 0}) {
                map.decision2.fill(polygonImage, dir);
            } else {
                tooBigJunctionCount++;
            }
        };
    }
    fmt::print("Got {} too big junctions \n", tooBigJunctionCount);
    fmt::print("Build decision took {} ms\n", timer.restart());
}
