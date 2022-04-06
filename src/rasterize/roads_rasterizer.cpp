#include "roads_rasterizer.h"

#include "common/entities.h"
#include "common/geometry.h"
#include "roads_rasterizer_utils.h"

#include <QElapsedTimer>
#include <fmt/core.h>

#include <set>


RoadsRasterizer::RoadsRasterizer(RoadsVectorMapPtr vector_map):
        vector_map_(vector_map),
        nodes_(vector_map->nodes),
        roads_(vector_map->roads),
        crossroad_nodes_{},
        lane_width_(3.5),
//        lane_width_(1),
        image_projector_{}
{
}

void RoadsRasterizer::DFS(ID node, double depth, std::unordered_set<ID>& nodes_set, bool root) const {
    if (depth <= 0) {
        return;
    }

    // TODO: ignore crossroad_nodes??
    if (!root && (crossroad_nodes_.contains(node) || crossroad_nodes_merge_.contains(node))) {
        return;
    }

    const auto insertion_res = nodes_set.insert(node);
    if ( !insertion_res.second) {
        return;
    }

    const auto pos = nodes_.at(node).c;

    for(const auto& other_node : vector_map_->roads_for_node.at(node)) {
        if (other_node.prev_node) {
            const double dist = geometry::Distance(pos, nodes_.at(*other_node.prev_node).c);
            DFS(*other_node.prev_node, depth - dist, nodes_set, false);
        }
        if (other_node.next_node) {
            const double dist = geometry::Distance(pos, nodes_.at(*other_node.next_node).c);
            DFS(*other_node.next_node, depth - dist, nodes_set, false);
        }
    }
}


void RoadsRasterizer::FindCrossroads() {
    crossroad_nodes_.clear();
    for (const auto& [node_id, roads_for_node] : vector_map_->roads_for_node) {
        std::set<ID> nodess{};
        for (const auto& road : roads_for_node) {
            if (road.prev_node) {
                nodess.insert(*road.prev_node);
            }
            if (road.next_node) {
                nodess.insert(*road.next_node);
            }
        }
        if (nodess.size() <= 2) {
            continue;
        }

        crossroad_nodes_.insert(node_id);
    }
}



Lane RoadsRasterizer::BuildLane(const SegmentsGroup& road) {
    double right_move = 0.;
    double left_move = 0.;
    PointF right_perpendicular{};
    PointF left_perpendicular{};
    PointF node_start;
    PointF node_center;
    PointF dir;

    VERIFY(road.into_node || road.out_of_node);

    if (road.into_node) {
        VERIFY(nodes_.contains(road.into_node->n1.id));
        VERIFY(nodes_.contains(road.into_node->n2.id));
        right_perpendicular = road.into_node->dir.RightPerpendicular();
        left_perpendicular = road.into_node->dir.LeftPerpendicular();
        right_move = lane_width_ * road.straight.road.lanes_count;
        node_start = road.into_node->n1.c;
        node_center = road.into_node->n2.c;
        dir = road.into_node->dir;
    }

    if (road.out_of_node) {
        VERIFY(nodes_.contains(road.out_of_node->n1.id));
        VERIFY(nodes_.contains(road.out_of_node->n2.id));
        left_perpendicular = road.out_of_node->dir.RightPerpendicular();
        right_perpendicular = road.out_of_node->dir.LeftPerpendicular();
        left_move = lane_width_ * road.straight.road.lanes_count;
        node_start = road.out_of_node->n2.c;
        node_center = road.out_of_node->n1.c;
        dir = -road.out_of_node->dir;
    }

//    const PointF left_bottom = ingoing_segment.p1 + ingoing_perp * lane_width_ * (road.straight.road.lanes_count - 1);
//    const PointF right_bottom = ingoing_segment.p1 + ingoing_perp * lane_width_ * (road.straight.road.lanes_count);

    const Line line_right = Line::FromPointAndDir(node_start + right_perpendicular * right_move,
                                                  dir);
    const Line line_left = Line::FromPointAndDir(node_start + left_perpendicular * left_move,
                                                 dir);
    const Line line_center = Line::FromPoints(node_start, node_center);

//    double angle = std::min(
//            line_center.Dir()
//                    .AngleAbs(line_left.Dir()),
//            line_center.Dir()
//                    .AngleAbs(-line_left.Dir())
//    );
//
//    if (angle > 0.001) {
//        std::cerr << "Err";
//    }

    return Lane {
        line_left,
        line_right,
        line_center
    };
}


double CaluclateOffsetDistanceForLane(const Lane& lane,
                                      const Line& line) {
    double angle = std::min(
                lane.line_center.Dir()
                    .AngleAbs(line.Dir()),
                lane.line_center.Dir()
                    .AngleAbs(-line.Dir())
            );
    // TODO : make smarter condition
    if (angle < M_PI / 10) {
        return 0.;
    }

    const std::optional<PointF> intersection_left = geometry::LineIntersection(lane.line_left, line);
    const std::optional<PointF> intersection_right = geometry::LineIntersection(lane.line_right, line);
    VERIFY(intersection_left);
    VERIFY(intersection_right);
    /*if (!intersection_left || !intersection_right) {
        // means that lines are parallel;
        return 0.;
    }*/

    const PointF projection_left = geometry::ProjectOnLine(lane.line_center, *intersection_left);
    const PointF projection_right = geometry::ProjectOnLine(lane.line_center, *intersection_right);

    const PointF ddir = lane.line_center.Dir();
    const PointF seg_1 = lane.line_center.a - ddir * 50; // node_start
    const PointF seg_2 = lane.line_center.b; // node_center

    double res = 0;
    // TODO: think about this condition
    if (geometry::DistancePointToSegment(projection_left, seg_1, seg_2) < 0.001) {
        res = geometry::Distance(lane.line_center.b, projection_left);
    }
    if (geometry::DistancePointToSegment(projection_right, seg_1, seg_2) < 0.001) {
        res = std::max(res,
                       geometry::Distance(lane.line_center.b, projection_right));
    }

    return res;
}

double RoadsRasterizer::CalculateOffestDistanceForRoad(const SegmentsGroup& road,
                                                       const std::vector<SegmentsGroup>& segment_groups) {
    Lane road_lane = BuildLane(road);
    double max_distance = 0.;
//    std::cerr << "Calculating for " << road.straight.p1 << " " << road.straight.p2 << std::endl;
    for (const auto& other_road : segment_groups) {
        Lane other_lane = BuildLane(other_road);
//        std::cerr << "  with lane " << other_road.straight.p1 << " " << other_road.straight.p2 << std::endl;
//        std::cerr << "     left: " << CaluclateOffsetDistanceForLane(road_lane, other_lane.line_left) << std::endl;
//        std::cerr << "     right: " << CaluclateOffsetDistanceForLane(road_lane, other_lane.line_right) << std::endl;
        max_distance = std::max(max_distance,
                                CaluclateOffsetDistanceForLane(road_lane, other_lane.line_left));
        max_distance = std::max(max_distance,
                                CaluclateOffsetDistanceForLane(road_lane, other_lane.line_right));
    }

    return max_distance;
}

void RoadsRasterizer::RasterizeRoads(RasterMap &map) {
    raster_map_ = &map;

    lane_id_counter_ = 1;

    QElapsedTimer timer;
    timer.start();

    image_projector_ = &map.image_projector;
//    image_projector_ = std::make_unique<MetersToImageProjector>(vector_map_->stats.min_xy, vector_map_->stats.max_xy,
//                                                                map.sizeI.x, map.sizeI.y);

    {
        const auto metersSize = vector_map_->stats.max_xy - vector_map_->stats.min_xy;
        fmt::print("Rasterizing roads from {:.1f}x{:.1f} meters to {}x{} px\n", metersSize.x, metersSize.y, map.sizeI.x, map.sizeI.y);
    }

    int not_enough_layers_cnt = 0;

    FindCrossroads();
    for (const auto& node_id : crossroad_nodes_) {
        std::unordered_set<ID> nodes_to_merge;
        DFS(node_id, 40, nodes_to_merge);

        for (const auto& node_id_to_merge : nodes_to_merge) {
            crossroad_nodes_merge_[node_id_to_merge] = node_id;
        }
        crossroad_nodes_merge_backward_.insert({node_id, nodes_to_merge});
    }


    for (const auto& [node_id, roads_for_node] : vector_map_->roads_for_node) {
        const Node& center_node = nodes_.at(node_id);
        { // DRAW DEBUG
            const auto polygon = ExpandPoint(center_node.c, 1.);
            const PolygonI polygonImage = image_projector_->project(polygon);
            if (crossroad_nodes_merge_.contains(node_id) &&
                crossroad_nodes_merge_.at(node_id) != node_id) {
                map.debug.fill(polygonImage, 0xff0000);
            } else if (crossroad_nodes_merge_.contains(node_id) &&
                       crossroad_nodes_merge_.at(node_id) == node_id) {
                map.debug.fill(polygonImage, 0x00ff00);
            } else {
                map.debug.fill(polygonImage, 0x0f0fffu);
            }
        }

        // ignore removed nodes
        if (crossroad_nodes_merge_.contains(node_id) &&
            crossroad_nodes_merge_[node_id] != node_id) {
            continue;
        }

        // add removed nodes for center nodes
        std::vector<RoadsVectorMap::RoadForNode> new_roads_for_node;
        if (crossroad_nodes_.contains(node_id)) {
            for (const auto other_node : crossroad_nodes_merge_backward_.at(node_id)) {
                const auto& roads_for_other_node = vector_map_->roads_for_node.at(other_node);
                new_roads_for_node.insert(std::end(new_roads_for_node),
                                          std::begin(roads_for_other_node), std::end(roads_for_other_node));
            }
        } else {
            new_roads_for_node = roads_for_node;
        }


        auto [ingoing_segments, outgoing_segments, segment_groups] =
              BuildIngoingAndOutgoingSegments(center_node, new_roads_for_node);


        // TODO : probably better to build some sort of return path??
        if (ingoing_segments.empty()  || outgoing_segments.empty() || segment_groups.size() <= 1) {
            continue;
        }


        for (auto& segment_group : segment_groups) {
            VERIFY(nodes_.contains(segment_group.straight.n1.id));
            VERIFY(nodes_.contains(segment_group.straight.n2.id));
            double offset = CalculateOffestDistanceForRoad(segment_group, segment_groups);
//            std::cerr << "    offset " << offset << " for " <<
//                      segment_group.straight.n1.id << "  " << segment_group.straight.n2.id << std::endl;
            segment_group.offset = offset + 2.;
        }

        HandleSegments(center_node, ingoing_segments, outgoing_segments, segment_groups);
    }



    fmt::print("Got {} not enough decision layers \n", not_enough_layers_cnt);

    fmt::print("Rasterizing took {} ms\n", timer.restart());

    raster_map_ = nullptr;
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
//            const PointF dir = (PointF{polyline[i + 1]} - PointF{polyline[i]}).Norm();
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
//            const PointF dir = (node2.c - node1.c).Norm();
//
//            if (map.decision1.getOrDefault(projectedCenterPos.x(), projectedCenterPos.y(), {0, 0}) == PointF{0, 0}) {
//                map.decision1.fill(polygonImage, dir);
//            } else if (map.decision2.getOrDefault(projectedCenterPos.x(), projectedCenterPos.y(), {0, 0}) == PointF{0, 0}) {
//                map.decision2.fill(polygonImage, dir);
//            } else {
//                tooBigJunctionCount++;
//            }
//        };
//    }
