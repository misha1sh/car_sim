#include "roads_rasterizer.h"

#include "common/entities.h"
#include "utils/verify.h"

#include <QElapsedTimer>
#include <fmt/core.h>
#include <range/v3/all.hpp>


void RoadsRasterizer::CreateLane(const Coord& left_bottom_corner, const Coord& right_bottom_corner,
                       const Coord& left_top_corner, const Coord& right_top_corner) {
    const PolygonI polygonImage = image_projector_->project(PolygonF{{
            left_bottom_corner.asPointF(),
            right_bottom_corner.asPointF(),
            right_top_corner.asPointF(),
            left_top_corner.asPointF()
    }});
    const Coord dir = (left_top_corner - left_bottom_corner).Norm();
    const ID lane_id = lane_id_counter_++;

    raster_map_->lane_id.fill(polygonImage, lane_id);
    raster_map_->lane_dir.insert({lane_id, dir});
}



RoadsRasterizer::RoadsRasterizer(const RoadsVectorMapPtr& vector_map):
        vector_map_(vector_map),
        nodes_(vector_map->nodes),
        roads_(vector_map->roads),
        lane_width_(3.),
        image_projector_{}
{
}


void RoadsRasterizer::RasterizeRoads(RasterMap &map) {
    raster_map_ = &map;

    lane_id_counter_ = 1;

    QElapsedTimer timer;
    timer.start();

    image_projector_ = std::make_unique<MetersToImageProjector>(vector_map_->stats.min_xy, vector_map_->stats.max_xy,
                                                                map.sizeI.x(), map.sizeI.y());

    {
        const auto metersSize = vector_map_->stats.max_xy - vector_map_->stats.min_xy;
        fmt::print("Rasterizing roads from {:.1f}x{:.1f} meters to {}x{} px\n", metersSize.x, metersSize.y, map.sizeI.x(), map.sizeI.y());
    }

    int not_enough_layers_cnt = 0;

    for (const auto& [node_id, roads_for_node] : vector_map_->roads_for_node) {

        const Node& center_node = nodes_.at(node_id);

//        if (node_id != 60768222) {
//            continue;
//        }

        const auto [ingoing_segments, outgoing_segments, segment_groups] = BuildIngoingAndOutgoingSegments(center_node, roads_for_node);
//        { DRAW DEBUG
//            const auto polygon = ExpandPoint(center_node.c.asPointF(), 1.);
//            const PolygonI polygonImage = image_projector_->project(polygon);
//            map.decision2.fill(polygonImage, {1, 0});
//        }

        // TODO : probably better to build some sort of return path??
        if (ingoing_segments.empty()  || outgoing_segments.empty() || segment_groups.size() <= 1) {
            continue;
        }

        HandleSegments(ingoing_segments, outgoing_segments, segment_groups);


    }

//    for (const auto& [node_id, node] : nodes_) {
//        map.decision1(image_projector_->project(node.c.asPointF()).x(), image_projector_->project(node.c.asPointF()).y()) = {1, 1};
//    }

//    fmt::print("Got {} too big junctions \n", tooBigJunctionCount);
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
