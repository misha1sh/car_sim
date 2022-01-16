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
}


PolylineF RoadsRasterizer::BuildPolyline(const Road& road) {
    PolylineF polyline;
    polyline.resize(road.nodes.size());
    for (size_t i = 0; i < road.nodes.size(); i++) {
        const auto& node_id = road.nodes[i];
        const auto& c = vector_map_->nodes.at(node_id).c;;;
        polyline[i] = c.asPointF();
    }

    return polyline;
}

RoadSegment CreateIngoingSegment(const Node& node1, const Node& node2, const Road& road, const ID segment_group_id) {
    const auto p1 = (node1.c + node2.c) / 2.;
    const auto p2 = node2.c;
    const auto dir = (p2 - p1).Norm();
    return RoadSegment{
            .n1 = node1,
            .n2 = node2,
            .road = road,
            .p1 = p1,
            .p2 = p2,
            .dir = dir,
            .segment_group_id = segment_group_id
    };
}

RoadSegment CreateOutgoingSegment(const Node& node1, const Node& node2, const Road& road, const ID segment_group_id) {
    const auto p1 = node1.c;
    const auto p2 = (node1.c + node2.c) / 2.;
    const auto dir = (p2 - p1).Norm();
    return RoadSegment{
            .n1 = node1,
            .n2 = node2,
            .road = road,
            .p1 = p1,
            .p2 = p2,
            .dir = dir,
            .segment_group_id = segment_group_id
    };
}

std::tuple<std::vector<RoadSegment>, std::vector<RoadSegment>, std::vector<SegmentsGroup>>
    RoadsRasterizer::BuildIngoingAndOutgoingSegments(const auto& center_node,
                                                    const std::vector<RoadsVectorMap::RoadForNode>& roads_for_node) {
    std::vector<RoadSegment> ingoing_segments, outgoing_segments;
    std::vector<SegmentsGroup> segments_groups;

    ID segment_id_counter = 0;

    for (const auto& road_for_node : roads_for_node) {
        const auto& road = roads_.at(road_for_node.road_id);
        if (road_for_node.next_node) {
            const ID segment_group_id = ++segment_id_counter;
            outgoing_segments.push_back(
                    CreateOutgoingSegment(center_node, nodes_.at(*road_for_node.next_node), road, segment_group_id)
            );
            RoadSegment& outgoing_segment = outgoing_segments.back();


            RoadSegment* ingoing_segment = nullptr;
            if (!road.is_one_way) {
                ingoing_segments.push_back(
                        CreateIngoingSegment(nodes_.at(*road_for_node.next_node), center_node, road, segment_group_id)
                );
                ingoing_segment = &ingoing_segments.back();
            }

            segments_groups.push_back(SegmentsGroup{
                                              segment_group_id,
                                              outgoing_segment,
                                              ingoing_segment
                                      });
        }


        if (road_for_node.prev_node) {
            const ID segment_group_id = ++segment_id_counter;
            ingoing_segments.push_back(
                    CreateIngoingSegment(nodes_.at(*road_for_node.prev_node), center_node, road, segment_group_id)
            );
            RoadSegment& ingoing_segment = ingoing_segments.back();


            RoadSegment* outgoing_segment = nullptr;
            if (!road.is_one_way) {
                outgoing_segments.push_back(
                        CreateOutgoingSegment(center_node, nodes_.at(*road_for_node.prev_node), road, segment_group_id)
                );
                outgoing_segment = &outgoing_segments.back();
            }

            segments_groups.push_back(SegmentsGroup{
                    segment_group_id,
                    ingoing_segment,
                    outgoing_segment
            });
        }
    }

    return {ingoing_segments, outgoing_segments, segments_groups};
}



void RoadsRasterizer::ConnectRoadSegmentEachToEach(const RoadSegment& ingoing_segment,
                                                  const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill) {
    VERIFY(ingoing_segment.segment_group_id != outgoing_segment.segment_group_id);
//                std::cerr << ingoing_segment.p1.x << " -> " << ingoing_segment.p2.x << "   " << outgoing_segment.p1.x << " -> " << outgoing_segment.p2.x << "  " << ingoing_segment.dir.AngleAbs(outgoing_segment.dir) / M_PI * 180 << std::endl;



    Coord median_dir = outgoing_segment.dir - ingoing_segment.dir;
    if (median_dir == Coord{0, 0}) {
        median_dir = outgoing_segment.dir.RightPerpendicular();
    } else if (ingoing_segment.dir.Angle(outgoing_segment.dir) > 0) { // left turns
        median_dir = -median_dir;
    }
    median_dir = median_dir.Norm();

    const auto ingoing_perp = ingoing_segment.dir.RightPerpendicular();
    const auto outgoing_perp = outgoing_segment.dir.RightPerpendicular();

    for (int i = 1; i <= std::min(ingoing_segment.road.lanes_count, outgoing_segment.road.lanes_count); i++) {
        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
        const Coord pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;
        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;

        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pi.asPointF(), pm.asPointF()}, 0.5));
        map_to_fill->fill(polygonImage, (pm - pi).Norm());
    }

    for (int i = 1; i <= std::min(ingoing_segment.road.lanes_count, outgoing_segment.road.lanes_count); i++) {
        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
        const Coord pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;
        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;

        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pm.asPointF(), po.asPointF()}, 0.5));
        map_to_fill->fill(polygonImage, (po - pm).Norm());
    }
}

void RoadsRasterizer::ConnectRoadSegmentMainToIngoing(const RoadSegment& ingoing_segment,
                                                      const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill) {
    VERIFY(ingoing_segment.segment_group_id != outgoing_segment.segment_group_id);

    Coord median_dir = outgoing_segment.dir.RightPerpendicular();

    const auto ingoing_perp = ingoing_segment.dir.RightPerpendicular();
    const auto outgoing_perp = outgoing_segment.dir.RightPerpendicular();

    for (int i = 1; i <= ingoing_segment.road.lanes_count; i++) {
        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
        Coord pm;
        {
            double closest_distance = std::numeric_limits<double>::infinity();
            for (int j = 1; j <= outgoing_segment.road.lanes_count; j++) {
                const Coord pm_j = ingoing_segment.p2 + median_dir * lane_width_ * j;
                const double distance_j = (pi - pm_j).SqrLen();
                if (closest_distance > distance_j) {
                    closest_distance = distance_j;
                    pm = pm_j;
                }
            }
        }
        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;

        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pi.asPointF(), pm.asPointF()}, 0.5));
        map_to_fill->fill(polygonImage, (pm - pi).Norm());
    }

    for (int i = 1; i <= outgoing_segment.road.lanes_count; i++) {
        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
        const Coord pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;
        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;

        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pm.asPointF(), po.asPointF()}, 0.5));
        map_to_fill->fill(polygonImage, (po - pm).Norm());
    }
}

void RoadsRasterizer::ConnectRoadSegmentMainToOutgoing(const RoadSegment& ingoing_segment,
                                                      const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill) {
    VERIFY(ingoing_segment.segment_group_id != outgoing_segment.segment_group_id);

    Coord median_dir = ingoing_segment.dir.RightPerpendicular();

    const auto ingoing_perp = ingoing_segment.dir.RightPerpendicular();
    const auto outgoing_perp = outgoing_segment.dir.RightPerpendicular();

    for (int i = 1; i <= ingoing_segment.road.lanes_count; i++) {
        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
        const Coord pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;

        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;

        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pi.asPointF(), pm.asPointF()}, 0.5));
        map_to_fill->fill(polygonImage, (pm - pi).Norm());
    }

    for (int i = 1; i <= outgoing_segment.road.lanes_count; i++) {
        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
        Coord pm;
        {
            double closest_distance = std::numeric_limits<double>::infinity();
            for (int j = 1; j <= outgoing_segment.road.lanes_count; j++) {
                const Coord pm_j = ingoing_segment.p2 + median_dir * lane_width_ * j;
                const double distance_j = (pi - pm_j).SqrLen();
                if (closest_distance > distance_j) {
                    closest_distance = distance_j;
                    pm = pm_j;
                }
            }
        }
        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;

        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pm.asPointF(), po.asPointF()}, 0.5));
        map_to_fill->fill(polygonImage, (po - pm).Norm());
    }
}

RoadsRasterizer::RoadsRasterizer(const RoadsVectorMapPtr& vector_map):
        vector_map_(vector_map),
        nodes_(vector_map->nodes),
        roads_(vector_map->roads),
        lane_width_(3.),
        image_projector_{}
{
}

//bool IsMergeOrSplitOrThreeway(const auto& ingoing_segments, const auto& outgoing_segments, const auto& segment_groups) {
//    for (const auto main_road_start : ) {
//    }
//}

void RoadsRasterizer::RasterizeRoads(RasterMap &map) {
    QElapsedTimer timer;
    timer.start();

    image_projector_ = std::make_unique<MetersToImageProjector>(vector_map_->stats.min_xy, vector_map_->stats.max_xy,
                                                                map.road_dir.sizeX(), map.road_dir.sizeY());

    {
        const auto metersSize = vector_map_->stats.max_xy - vector_map_->stats.min_xy;
        fmt::print("Rasterizing roads from {:.1f}x{:.1f} meters to {}x{} px\n", metersSize.x, metersSize.y, map.road_dir.sizeX(), map.road_dir.sizeY());
    }

    int not_enough_layers_cnt = 0;

    for (const auto& [node_id, roads_for_node] : vector_map_->roads_for_node) {

        const Node& center_node = nodes_.at(node_id);

//        if (node_id != 60768222) {
//            continue;
//        }

        const auto [ingoing_segments, outgoing_segments, segment_groups] = BuildIngoingAndOutgoingSegments(center_node, roads_for_node);
        {
            const auto polygon = ExpandPoint(center_node.c.asPointF(), 1.);
            const PolygonI polygonImage = image_projector_->project(polygon);
            map.decision2.fill(polygonImage, {1, 0});
        }

        // TODO : probably better to build some sort of return path??
        if (ingoing_segments.empty()  || outgoing_segments.empty() || segment_groups.size() <= 1) {
            continue;
        }

        if (segment_groups.size() == 2) {
            // no junction, just road continuation
            //   - connect each to each
            for (const auto& ingoing_segment : ingoing_segments) {
                for (const auto &outgoing_segment: outgoing_segments) {
                    if (ingoing_segment.segment_group_id == outgoing_segment.segment_group_id) {
                        continue;
                    }
                    ConnectRoadSegmentEachToEach(ingoing_segment, outgoing_segment, &map.road_dir);
                }
            }
        } else /*if ((segment_groups.size() == 3 && (ingoing_segments.size() <= 2 || outgoing_segments.size() <= 2)) ||
                    (ingoing_segments.size() == 1 || outgoing_segments.size() == 1)) {
            // link: some roads merging, or some roads splitting
            //   - find the main road, connect each to each
            //   - others roads connect main to turn
            double most_straight_line_angle = M_PI; // should be close to 0
            // store segment id, so we can connect the reverse too
            ID segment_group_id1 = ingoing_segments[0].segment_group_id;
            ID segment_group_id2 = outgoing_segments[0].segment_group_id;
            for (const auto& ingoing_segment : ingoing_segments) {
                for (const auto& outgoing_segment : outgoing_segments) {
                    const double angle = ingoing_segment.dir.AngleAbs(outgoing_segment.dir);
                    if (angle < most_straight_line_angle) {
                        segment_group_id1 = ingoing_segment.segment_group_id;
                        segment_group_id2 = outgoing_segment.segment_group_id;
                        most_straight_line_angle = angle;
                    }
                }
            }



            for (const auto& ingoing_segment : ingoing_segments) {
                for (const auto& outgoing_segment : outgoing_segments) {
                    if (ingoing_segment.segment_group_id == outgoing_segment.segment_group_id) {
                        continue;
                    }

                    if ((segment_group_id1 == ingoing_segment.segment_group_id &&
                         segment_group_id2 == outgoing_segment.segment_group_id) ||
                        (segment_group_id1 == outgoing_segment.segment_group_id &&
                         segment_group_id2 == ingoing_segment.segment_group_id)) {
                        // main road (straight or reverse way)
                        ConnectRoadSegmentEachToEach(ingoing_segment, outgoing_segment, &map.decision1); // TODO IM#QIMRIMF replace ro road_dir
                        continue;
                    }

                    if (segment_group_id1 == outgoing_segment.segment_group_id ||
                        segment_group_id2 == outgoing_segment.segment_group_id) {
                        ConnectRoadSegmentMainToIngoing(ingoing_segment, outgoing_segment, &map.decision2);
                        continue;
                    }
                    if (segment_group_id1 == ingoing_segment.segment_group_id ||
                        segment_group_id2 == ingoing_segment.segment_group_id) {
                        ConnectRoadSegmentMainToOutgoing(ingoing_segment, outgoing_segment, &map.decision2);
                    }
                }
            }

        } else*/ {
            // probably some junction
//
//            double most_straight_line_angle = M_PI; // should be close to 0
//            // store segment id, so we can connect the reverse too
//            ID segment_group_id1 = ingoing_segments[0].segment_group_id;
//            ID segment_group_id2 = outgoing_segments[0].segment_group_id;
//            for (const auto& ingoing_segment : ingoing_segments) {
//                for (const auto& outgoing_segment : outgoing_segments) {
//                    const double angle = ingoing_segment.dir.AngleAbs(outgoing_segment.dir);
//                    if (angle < most_straight_line_angle) {
//                        segment_group_id1 = ingoing_segment.segment_group_id;
//                        segment_group_id2 = outgoing_segment.segment_group_id;
//                        most_straight_line_angle = angle;
//                    }
//                }
//            }

        int outgoing_layer = 1;
        for (const auto& outgoing_segment : outgoing_segments) {
            if (outgoing_layer >= 3) {
                not_enough_layers_cnt++;
                continue;
            }

            RasterDataPoint* map_to_fill;
            if (outgoing_layer == 1) {
                map_to_fill = &map.decision1;
            } else if (outgoing_layer == 2) {
                map_to_fill = &map.decision2;
            }


            for (const auto& ingoing_segment : ingoing_segments) {
                if (ingoing_segment.dir.AngleAbs(outgoing_segment.dir) > M_PI * 0.75) {
                    continue;
                }

                if (true || ingoing_segment.dir.AngleAbs(outgoing_segment.dir) < M_PI / 6) {
                    ConnectRoadSegmentEachToEach(ingoing_segment, outgoing_segment, map_to_fill);
                    continue;
                }
                if (ingoing_segment.road.lanes_count > outgoing_segment.road.lanes_count) {
                    ConnectRoadSegmentMainToOutgoing(ingoing_segment, outgoing_segment, map_to_fill);
                } else {
                    ConnectRoadSegmentMainToIngoing(ingoing_segment, outgoing_segment, map_to_fill);
                }
            }

            outgoing_layer++;
        }

        }







    }

//    for (const auto& [node_id, node] : nodes_) {
//        map.decision1(image_projector_->project(node.c.asPointF()).x(), image_projector_->project(node.c.asPointF()).y()) = {1, 1};
//    }

//    fmt::print("Got {} too big junctions \n", tooBigJunctionCount);
    fmt::print("Got {} not enough decision layers \n", not_enough_layers_cnt);

    fmt::print("Rasterizing took {} ms\n", timer.restart());
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
