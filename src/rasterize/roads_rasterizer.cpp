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
    boost::geometry::strategy::buffer::point_circle point_strategy(8);
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(radius);

    boost::geometry::strategy::buffer::join_round    join_strategy;
    boost::geometry::strategy::buffer::end_round     end_strategy;
    boost::geometry::strategy::buffer::side_straight side_strategy;


    MultiPolygonF result;
    boost::geometry::buffer(point, result,
               distance_strategy, side_strategy,
               join_strategy, end_strategy, point_strategy);

    VERIFY(result.size() == 1);
    return result[0];
//    return PolygonF{{
////                            PointF{0, 0},
////                            PointF{0, 100},
////                            PointF{100, 100},
////                            PointF{100, 0},
////                            PointF{0, 0},
////                            PointF{point.x() - 30, point.y() - 30},
////                            PointF{point.x() - 30, point.y() + 30},
////                            PointF{point.x() + 30, point.y() + 30},
////                            PointF{point.x() + 30, point.y() - 30},
////                            PointF{point.x() - 30, point.y() - 30}
//                    }};
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
        fmt::print("Rasterizing roads from {}x{} meters to {}x{} px\n", metersSize.x, metersSize.y, map.road_dir.sizeX(), map.road_dir.sizeY());
    }
//
//
//    for (const auto& [road_id, road] : vector_map->roads) {
//        const auto polyline = BuildPolyline(vector_map, road);
//        for (int i = 0; i + 1 < polyline.size(); i++) {
//            const PolylineF segment{polyline[i], polyline[i + 1]};
//            const auto polygon = ExpandPolyline(polyline, road.lanes_count * lane_width);
//            const PolygonI polygonImage = imageProjector.project(polygon);
//
//            const Coord dir = (Coord{polyline[i + 1]} - Coord{polyline[i]}).Norm();
//            map.road_dir.fill(polygonImage, dir);
//        }
//    }
//    fmt::print("Build roadsDir took {} ms\n", timer.restart());
//
//    int tooBigJunctionCount = 0;
//    for (const auto& [node_id, roads] : vector_map->roads_for_node) {
//        const auto outgoingRoads = roads | ranges::views::remove_if([](const auto& road) {
//            return road.next_node == std::nullopt;
//        }) | ranges::to_vector;
//        if (outgoingRoads.size() <= 1) {
//            continue;
//        }
//
//        const Node& node1 = vector_map->nodes.at(node_id);
//        const auto centerPos = node1.c;
//        const auto projectedCenterPos = imageProjector.project(centerPos.asPointF());
//        const auto polygon = ExpandPoint(centerPos.asPointF(), lane_width * 4);
//        const PolygonI polygonImage = imageProjector.project(polygon);
//
//        for (const auto& [road_id, next_node_id] : outgoingRoads) {
//            VERIFY(next_node_id);
//            const Node& node2 = vector_map->nodes.at(*next_node_id);
//            const Coord dir = (node2.c - node1.c).Norm();
//
//            if (map.decision1.getOrDefault(projectedCenterPos.x(), projectedCenterPos.y(), {0, 0}) == Coord{0, 0}) {
//                map.decision1.fill(polygonImage, dir);
//            } else if (map.decision2.getOrDefault(projectedCenterPos.x(), projectedCenterPos.y(), {0, 0}) == Coord{0, 0}) {
//                map.decision2.fill(polygonImage, dir);
//            } else {
//                tooBigJunctionCount++;
//            }
//        };
//    }

    struct RoadSegment {
        const Node& n1, n2;
        const Road& road;
        Coord p1, p2;
        Coord dir;
    };

    const auto& nodes = vector_map->nodes;
    const auto& roads = vector_map->roads;

    int not_enough_layers_cnt = 0;

    for (const auto& [node_id, roads_for_node] : vector_map->roads_for_node) {

        const Node& center_node = nodes.at(node_id);

//        {
//            const auto polygon = ExpandPoint(center_node.c.asPointF(), 1.);
//            const PolygonI polygonImage = imageProjector.project(polygon);
//            map.decision1.fill(polygonImage, {1, 0});
//        }


//        if (node_id != 3) {
//            continue;
//        }




        std::vector<RoadSegment> ingoing_segments, outgoing_segments;
        for (const auto& road_for_node : roads_for_node) {
//            if (road_for_node.road_id != 25093359) {
//                continue;
//            }
            const auto& road = roads.at(road_for_node.road_id);
            if (road_for_node.next_node) {
                {
                    const Node& n2 = nodes.at(*road_for_node.next_node);
                    const auto p1 = center_node.c;
                    const auto p2 = (n2.c + center_node.c) / 2.;
                    const auto dir = (p2 - p1).Norm();
                    outgoing_segments.push_back(RoadSegment{
                            .n1 = center_node,
                            .n2 = n2,
                            .road = road,
                            .p1 = p1,
                            .p2 = p2,
                            .dir = dir
                    });
                }
                if (!road.is_one_way) {
                    const Node& n1 = nodes.at(*road_for_node.next_node);
                    const auto p1 = (n1.c + center_node.c) / 2.;
                    const auto p2 = center_node.c;
                    const auto dir = (p2 - p1).Norm();
                    ingoing_segments.push_back(RoadSegment{
                            .n1 = n1,
                            .n2 = center_node,
                            .road = road,
                            .p1 = p1,
                            .p2 = p2,
                            .dir = dir
                    });
                }
            }

            if (road_for_node.prev_node) {
                {
                    const Node& n1 = nodes.at(*road_for_node.prev_node);
                    const auto p1 = (n1.c + center_node.c) / 2.;
                    const auto p2 = center_node.c;
                    const auto dir = (p2 - p1).Norm();
                    ingoing_segments.push_back(RoadSegment{
                            .n1 = n1,
                            .n2 = center_node,
                            .road = road,
                            .p1 = p1,
                            .p2 = p2,
                            .dir = dir
                    });
                }
                if (!road.is_one_way) {
                    const Node& n2 = nodes.at(*road_for_node.prev_node);
                    const auto p1 = center_node.c;
                    const auto p2 = (n2.c + center_node.c) / 2.;
                    const auto dir = (p2 - p1).Norm();
                    outgoing_segments.push_back(RoadSegment{
                            .n1 = center_node,
                            .n2 = n2,
                            .road = road,
                            .p1 = p1,
                            .p2 = p2,
                            .dir = dir
                    });
                }
            }
        }


        int outgoing_layer = 0;
        for (const auto& outgoing_segment : outgoing_segments) {
            if (outgoing_layer >= 3) {
                not_enough_layers_cnt++;
                continue;
            }

            RasterDataPoint* map_to_fill;
            if (outgoing_layer == 0) {
                map_to_fill = &map.road_dir;
            } else if (outgoing_layer == 1) {
                map_to_fill = &map.decision1;
            } else if (outgoing_layer == 2) {
                map_to_fill = &map.decision2;
            }


            for (const auto& ingoing_segment : ingoing_segments) {
                if (ingoing_segment.dir.AngleAbs(outgoing_segment.dir) > M_PI / 2) {
                    continue;
                }

//                std::cerr << ingoing_segment.p1.x << " -> " << ingoing_segment.p2.x << "   " << outgoing_segment.p1.x << " -> " << outgoing_segment.p2.x << "  " << ingoing_segment.dir.AngleAbs(outgoing_segment.dir) / M_PI * 180 << std::endl;


//                {
//                    const auto polygon = ExpandPoint(center_node.c.asPointF(), 1.);
//                    const PolygonI polygonImage = imageProjector.project(polygon);
//                    map.decision2.fill(polygonImage, {1, 0});
//                }

                Coord median_dir = outgoing_segment.dir - ingoing_segment.dir;
                if (median_dir == Coord{0, 0}) {
                    median_dir = outgoing_segment.dir.LeftPerpendicular();
                } else if (ingoing_segment.dir.Angle(outgoing_segment.dir) < 0) { // left turns
                    median_dir = -median_dir;
                }
                median_dir = median_dir.Norm();

                const auto ingoing_perp = ingoing_segment.dir.LeftPerpendicular();
                const auto outgoing_perp = outgoing_segment.dir.LeftPerpendicular();

                for (int i = 1; i <= ingoing_segment.road.lanes_count; i++) {
                    const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width * i;
                    const Coord pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width * i;
                    const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width * i;

                    const PolygonI polygonImage = imageProjector.project(ExpandPolyline({pi.asPointF(), pm.asPointF()}, 0.5));
                    map.road_dir.fill(polygonImage, (pm - pi).Norm());
                }

                for (int i = 1; i <= outgoing_segment.road.lanes_count; i++) {
                    const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width * i;
                    const Coord pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width * i;
                    const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width * i;

                    const PolygonI polygonImage = imageProjector.project(ExpandPolyline({pm.asPointF(), po.asPointF()}, 0.5));
                    map.road_dir.fill(polygonImage, (po - pm).Norm());
                }
            }

            outgoing_layer++;
        }

//        const auto centerPos = node1.c;
//        const auto projectedCenterPos = imageProjector.project(centerPos.asPointF());
//        const auto polygon = ExpandPoint(centerPos.asPointF(), lane_width * 4);
//        const PolygonI polygonImage = imageProjector.project(polygon);

//        for (const auto& [road_id, next_node_id] : outgoingRoads) {
//            VERIFY(next_node_id);
//            const Node& node2 = vector_map->nodes.at(*next_node_id);
//            const Coord dir = (node2.c - node1.c).Norm();
//
//            if (map.decision1.getOrDefault(projectedCenterPos.x(), projectedCenterPos.y(), {0, 0}) == Coord{0, 0}) {
//                map.decision1.fill(polygonImage, dir);
//            } else if (map.decision2.getOrDefault(projectedCenterPos.x(), projectedCenterPos.y(), {0, 0}) == Coord{0, 0}) {
//                map.decision2.fill(polygonImage, dir);
//            } else {
//                tooBigJunctionCount++;
//            }
//        };
    }

//    fmt::print("Got {} too big junctions \n", tooBigJunctionCount);
    fmt::print("Build decision took {} ms\n", timer.restart());
}
