#include "roads_rasterizer.h"
#include "common/geometry.h"

#include <map>

ID RoadsRasterizer::CreateLane(const PointF& left_bottom_corner, const PointF& right_bottom_corner,
                                 const PointF& left_top_corner, const PointF& right_top_corner) {
    const PolygonI polygonImage = image_projector_->project(PolygonF{{
                                                                             left_bottom_corner,
                                                                             right_bottom_corner,
                                                                             right_top_corner,
                                                                             left_top_corner
                                                                     }});
    const PointF dir = (left_top_corner - left_bottom_corner).Norm();
    const ID lane_id = lane_id_counter_++;

    raster_map_->lane_id.fill(polygonImage, (int)lane_id);
    raster_map_->lane_dir.insert({lane_id, dir});

    return lane_id;
}




void RoadsRasterizer::ConnectRoadSegmentEachToEach(const RoadSegment& ingoing_segment,
                                                   const RoadSegment& outgoing_segment) {
    VERIFY(ingoing_segment.segment_group_id != outgoing_segment.segment_group_id);
    VERIFY(ingoing_segment.n2.id == outgoing_segment.n1.id);
    VERIFY(!crossroad_nodes_.contains(ingoing_segment.n2.id));
//                std::cerr << ingoing_segment.p1.x << " -> " << ingoing_segment.p2.x << "   " << outgoing_segment.p1.x << " -> " << outgoing_segment.p2.x << "  " << ingoing_segment.dir.AngleAbs(outgoing_segment.dir) / M_PI * 180 << std::endl;



    PointF median_dir = outgoing_segment.dir - ingoing_segment.dir;
    if (median_dir == PointF{0, 0}) {
        median_dir = outgoing_segment.dir.RightPerpendicular();
    } else if (ingoing_segment.dir.Angle(outgoing_segment.dir) > 0) { // left turns
        median_dir = -median_dir;
    }
    median_dir = median_dir.Norm();

    const auto ingoing_perp = ingoing_segment.dir.RightPerpendicular();
    const auto outgoing_perp = outgoing_segment.dir.RightPerpendicular();

//    median_dir = ingoing_perp + outgoing_perp;

    for (int i = 1; i <= std::min(ingoing_segment.road.lanes_count, outgoing_segment.road.lanes_count); i++) {
        const PointF left_bottom = ingoing_segment.p1 + ingoing_perp * lane_width_ * (i - 1);
        const PointF right_bottom = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;

//        const PointF left_medium = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * (i - 1);
//        const PointF right_medium = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;

        const PointF left_top = outgoing_segment.p2 + outgoing_perp * lane_width_ * (i - 1);
        const PointF right_top = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;

        const auto left_medium_opt = geometry::LineIntersection(left_bottom, left_bottom + ingoing_segment.dir,
                                                       left_top, left_top + outgoing_segment.dir);
        const PointF left_medium = left_medium_opt
                                    ? *left_medium_opt
                                    // means parallel
                                    : ingoing_segment.p2 + median_dir * lane_width_ * (i - 1);

        const auto right_medium_opt = geometry::LineIntersection(right_bottom, right_bottom + ingoing_segment.dir,
                                                 right_top, right_top + outgoing_segment.dir);
        const PointF right_medium = right_medium_opt
                ? *right_medium_opt
                // means parallel
                : ingoing_segment.p2 + median_dir * lane_width_ * i;



        CreateLane(left_bottom, right_bottom, left_medium, right_medium);
        CreateLane(left_medium, right_medium, left_top, right_top);
    }

//    for (int i = 1; i <= std::min(ingoing_segment.road.lanes_count, outgoing_segment.road.lanes_count); i++) {
//        const PointF pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
//        const PointF pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;
//        const PointF po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;
//
//        removec
//        const PointF pm_overtake = pm - (po - pm).Norm() * 0.6; // TODO: figure coefficient
//
//        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pm_overtake.asPointF(), po.asPointF()}, 0.5));
//        map_to_fill->fill(polygonImage, (po - pm).Norm());
//    }
}

std::vector<int> RoadsRasterizer::ConnectToJunction(const RoadSegment& segment, const bool is_ingoing, double offset) {
//    if (offset > geometry::Distance(segment.p1, segment.p2)) {
//        std::cerr << offset << " offset is too big" << std::endl;
//        std::cerr << segment.n1.id << " " << segment.n2.id << std::endl;
//        std::cerr <<  GetMergedNodeID(segment.n1.id) << " " << GetMergedNodeID(segment.n2.id) << std::endl;
//    }
    VERIFY(segment.n1.id != segment.n2.id);
    VERIFY(GetMergedNodeID(segment.n1.id) == segment.n1.id);
    VERIFY(GetMergedNodeID(segment.n2.id) == segment.n2.id);

//    std::cerr << offset << ": ";
    offset = std::min(offset,
                      geometry::Distance(segment.p1, segment.p2) - 0.5);
    offset = std::max(0., offset);

    PointF p1 = segment.p1;
    PointF p2 = segment.p2;
//    std::cerr << p1 << " " << p2 << std::endl;

    if (is_ingoing) {
        p2 -= segment.dir * offset;
    } else {
        p1 += segment.dir * offset;
    }

    std::vector<int> lane_ids;

    const auto perp = segment.dir.RightPerpendicular();
    for (int i = 1; i <= segment.road.lanes_count; i++) {
        PointF left_bottom = p1 + perp * lane_width_ * (i - 1);
        PointF right_bottom = p1 + perp * lane_width_ * i;

//        const PointF left_medium = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * (i - 1);
//        const PointF right_medium = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;

        PointF left_top = p2 + perp * lane_width_ * (i - 1);
        PointF right_top = p2 + perp * lane_width_ * i;

        const auto lane_id = CreateLane(left_bottom, right_bottom, left_top, right_top);
        lane_ids.push_back(lane_id);
        raster_map_->crossroad_lanes.insert({lane_id,
            CrossroadLane {
                .goes_into_crossroad = is_ingoing,
                .start_point = (left_bottom + right_bottom) / 2.,
                .end_point = (left_top + right_top) / 2.,
                .lane_num = i,
                .end_lanes = {} // we'll fill it later
            }
        });
    }

    return lane_ids;
}

void RoadsRasterizer::HandleSegments(const Node& center_node,
                                     const std::vector<RoadSegment>& ingoing_segments,
                                     const std::vector<RoadSegment>& outgoing_segments,
                                     const std::vector<SegmentsGroup>& segment_groups) {
    //if (segment_groups.size() == 2) {
    if (!crossroad_nodes_.contains(center_node.id)) {
        // no junction, just road continuation
        //   - connect each to each
        for (const auto& ingoing_segment : ingoing_segments) {
            for (const auto &outgoing_segment: outgoing_segments) {
                if (ingoing_segment.segment_group_id == outgoing_segment.segment_group_id) {
                    continue;
                }
                ConnectRoadSegmentEachToEach(ingoing_segment, outgoing_segment);
            }
        }
    } else {
        std::map<int, std::vector<int>> lane_ids_for_segment;
        for (const auto& ingoing_segment : ingoing_segments) {
            const auto lane_ids = ConnectToJunction(ingoing_segment, true,
                                                    segment_groups[ingoing_segment.segment_group_id].offset);

            lane_ids_for_segment.insert({ingoing_segment.id, lane_ids});
        }
        for (const auto& outgoing_segment : outgoing_segments) {
            const auto lane_ids = ConnectToJunction(outgoing_segment, false,
                                                    segment_groups[outgoing_segment.segment_group_id].offset);
            lane_ids_for_segment.insert({outgoing_segment.id, lane_ids});
        }



        for (const auto& ingoing_segment : ingoing_segments) {
            for (const auto& outgoing_segment : outgoing_segments) {
                if (ingoing_segment.segment_group_id == outgoing_segment.segment_group_id) {
                    continue;
                }
                bool is_straight = false;
                if (ingoing_segment.road.id == outgoing_segment.road.id) {
                    is_straight = true;
                }
                if (ingoing_segment.dir.AngleAbs(outgoing_segment.dir) < M_PI / 5) {
                    is_straight = true;
                }

                bool is_left = false;
                bool is_right = false;
                double angle = ingoing_segment.dir.AngleAbs(outgoing_segment.dir);
                if (std::abs(angle) > 3 * M_PI / 4) {
                    continue;
                }

                if (!is_straight) {
                    if (angle < 0) {
                        is_left = true;
                    } else {
                        is_right = true;
                    }
                }



                for (const auto& ingoing_lane_id : lane_ids_for_segment.at(ingoing_segment.id)) {
                    auto& ingoing_lane = raster_map_->crossroad_lanes.at(ingoing_lane_id);
                    for (const auto& outgoing_lane_id : lane_ids_for_segment.at(outgoing_segment.id)) {
                        const auto& outgoing_lane = raster_map_->crossroad_lanes.at(outgoing_lane_id);
                        if (is_straight && (
                                ingoing_lane.lane_num != outgoing_lane.lane_num &&
                                        !(ingoing_lane.lane_num > outgoing_segment.road.lanes_count &&
                                          outgoing_lane.lane_num == outgoing_segment.road.lanes_count))) {
                            continue;
                        }

                        if (is_right && !(
                                ingoing_lane.lane_num == ingoing_segment.road.lanes_count &&
                                outgoing_lane.lane_num == outgoing_segment.road.lanes_count
                                )) {
                            continue;
                        }

                        if (is_left && !(
                                ingoing_lane.lane_num == 1)
                            ) {
                            continue;
                        }


                        ingoing_lane.end_lanes.push_back(
                                outgoing_lane
                        );
//                        ingoing_lane.
//
//                        if (is_straight && !in_traffic_lights.contains(ingoing_lane.lane_num)) {
//                            in_traffic_lights.insert({ingoing_lane.lane_num});
//                            in_traffic_lights.insert({outgoing_lane.lane_num});
//                            traffic_lights.lanes.push_back({ingoing_lane.lane_num, outgoing_lane.lane_num});
//                        }
                    }
                }




            }
        }


        TrafficLights traffic_lights{
                .center = center_node.c,
                .lanes = {}
        };
        int traffic_lights_id = traffic_lights_counter_++;
        for (const auto& ingoing_segment : ingoing_segments) {
            for (const auto &ingoing_lane_id: lane_ids_for_segment.at(ingoing_segment.id)) {
                const auto lane = raster_map_->crossroad_lanes.at(ingoing_lane_id);
                const auto lane_dir = (lane.end_point - lane.start_point).Norm();
                bool found = false;
                for (auto& lanes_group : traffic_lights.lanes) {
                    const auto other_lane = raster_map_->crossroad_lanes.at(*lanes_group.begin());
                    const auto other_lane_dir = (other_lane.end_point - other_lane.start_point).Norm();
                    const double angle = lane_dir.AngleAbs(other_lane_dir);
                    if (angle < M_PI / 5 || angle > 4 * M_PI / 5.) {
                        lanes_group.insert(ingoing_lane_id);
                    }
                }

                if (!found) {
                    traffic_lights.lanes.push_back({ingoing_lane_id});
                }

            }
        }
        raster_map_->traffic_lights.insert({traffic_lights_id, traffic_lights});

    }
}



