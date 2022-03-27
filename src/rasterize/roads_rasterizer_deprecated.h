#pragma once

//
//
//
//PolylineF RoadsRasterizer::BuildPolyline(const Road& road) {
//    PolylineF polyline;
//    polyline.resize(road.nodes.size());
//    for (size_t i = 0; i < road.nodes.size(); i++) {
//        const auto& node_id = road.nodes[i];
//        const auto& c = vector_map_->nodes.at(node_id).c;;;
//        polyline[i] = c.asPointF();
//    }
//
//    return polyline;
//}
//
//
//
//
//void RoadsRasterizer::ConnectRoadSegmentEachToEach(const RoadSegment& ingoing_segment,
//                                                   const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill) {
//    VERIFY(ingoing_segment.segment_group_id != outgoing_segment.segment_group_id);
////                std::cerr << ingoing_segment.p1.x << " -> " << ingoing_segment.p2.x << "   " << outgoing_segment.p1.x << " -> " << outgoing_segment.p2.x << "  " << ingoing_segment.dir.AngleAbs(outgoing_segment.dir) / M_PI * 180 << std::endl;
//
//
//
//    Coord median_dir = outgoing_segment.dir - ingoing_segment.dir;
//    if (median_dir == Coord{0, 0}) {
//        median_dir = outgoing_segment.dir.RightPerpendicular();
//    } else if (ingoing_segment.dir.Angle(outgoing_segment.dir) > 0) { // left turns
//        median_dir = -median_dir;
//    }
//    median_dir = median_dir.Norm();
//
//    const auto ingoing_perp = ingoing_segment.dir.RightPerpendicular();
//    const auto outgoing_perp = outgoing_segment.dir.RightPerpendicular();
//
//    for (int i = 1; i <= std::min(ingoing_segment.road.lanes_count, outgoing_segment.road.lanes_count); i++) {
//        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
//        const Coord pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;
//        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;
//
//        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pi.asPointF(), pm.asPointF()}, 0.5));
//        map_to_fill->fill(polygonImage, (pm - pi).Norm());
//    }
//
//    for (int i = 1; i <= std::min(ingoing_segment.road.lanes_count, outgoing_segment.road.lanes_count); i++) {
//        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
//        const Coord pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;
//        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;
//
//        remove
//        const Coord pm_overtake = pm - (po - pm).Norm() * 0.6; // TODO: figure coefficient
//
//        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pm_overtake.asPointF(), po.asPointF()}, 0.5));
//        map_to_fill->fill(polygonImage, (po - pm).Norm());
//    }
//}
//
//void RoadsRasterizer::ConnectRoadSegmentMainToIngoing(const RoadSegment& ingoing_segment,
//                                                      const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill) {
//    VERIFY(ingoing_segment.segment_group_id != outgoing_segment.segment_group_id);
//
//    Coord median_dir = outgoing_segment.dir.RightPerpendicular();
//
//    const auto ingoing_perp = ingoing_segment.dir.RightPerpendicular();
//    const auto outgoing_perp = outgoing_segment.dir.RightPerpendicular();
//
//    for (int i = 1; i <= ingoing_segment.road.lanes_count; i++) {
//        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
//        Coord pm;
//        {
//            double closest_distance = std::numeric_limits<double>::infinity();
//            for (int j = 1; j <= outgoing_segment.road.lanes_count; j++) {
//                const Coord pm_j = ingoing_segment.p2 + median_dir * lane_width_ * j;
//                const double distance_j = (pi - pm_j).SqrLen();
//                if (closest_distance > distance_j) {
//                    closest_distance = distance_j;
//                    pm = pm_j;
//                }
//            }
//        }
//        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;
//
//        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pi.asPointF(), pm.asPointF()}, 0.5));
//        map_to_fill->fill(polygonImage, (pm - pi).Norm());
//    }
//
//    for (int i = 1; i <= outgoing_segment.road.lanes_count; i++) {
//        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
//        const Coord pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;
//        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;
//
//        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pm.asPointF(), po.asPointF()}, 0.5));
//        map_to_fill->fill(polygonImage, (po - pm).Norm());
//    }
//}
//
//void RoadsRasterizer::ConnectRoadSegmentMainToOutgoing(const RoadSegment& ingoing_segment,
//                                                       const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill) {
//    VERIFY(ingoing_segment.segment_group_id != outgoing_segment.segment_group_id);
//
//    Coord median_dir = ingoing_segment.dir.RightPerpendicular();
//
//    const auto ingoing_perp = ingoing_segment.dir.RightPerpendicular();
//    const auto outgoing_perp = outgoing_segment.dir.RightPerpendicular();
//
//    for (int i = 1; i <= ingoing_segment.road.lanes_count; i++) {
//        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
//        const Coord pm = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;
//
//        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;
//
//        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pi.asPointF(), pm.asPointF()}, 0.5));
//        map_to_fill->fill(polygonImage, (pm - pi).Norm());
//    }
//
//    for (int i = 1; i <= outgoing_segment.road.lanes_count; i++) {
//        const Coord pi = ingoing_segment.p1 + ingoing_perp * lane_width_ * i;
//        Coord pm;
//        {
//            double closest_distance = std::numeric_limits<double>::infinity();
//            for (int j = 1; j <= outgoing_segment.road.lanes_count; j++) {
//                const Coord pm_j = ingoing_segment.p2 + median_dir * lane_width_ * j;
//                const double distance_j = (pi - pm_j).SqrLen();
//                if (closest_distance > distance_j) {
//                    closest_distance = distance_j;
//                    pm = pm_j;
//                }
//            }
//        }
//        const Coord po = outgoing_segment.p2 + outgoing_perp * lane_width_ * i;
//
//        const PolygonI polygonImage = image_projector_->project(ExpandPolyline({pm.asPointF(), po.asPointF()}, 0.5));
//        map_to_fill->fill(polygonImage, (po - pm).Norm());
//    }
//}








//if ((segment_groups.size() == 3 && (ingoing_segments.size() <= 2 || outgoing_segments.size() <= 2)) ||
//                    (ingoing_segments.size() == 1 || outgoing_segments.size() == 1)) {
//            // link: some roads merging, or some roads splitting
//            //   - find the main road, connect each to each
//            //   - others roads connect main to turn
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
//
//
//
//            for (const auto& ingoing_segment : ingoing_segments) {
//                for (const auto& outgoing_segment : outgoing_segments) {
//                    if (ingoing_segment.segment_group_id == outgoing_segment.segment_group_id) {
//                        continue;
//                    }
//
//                    if ((segment_group_id1 == ingoing_segment.segment_group_id &&
//                         segment_group_id2 == outgoing_segment.segment_group_id) ||
//                        (segment_group_id1 == outgoing_segment.segment_group_id &&
//                         segment_group_id2 == ingoing_segment.segment_group_id)) {
//                        // main road (straight or reverse way)
//                        ConnectRoadSegmentEachToEach(ingoing_segment, outgoing_segment, &map.decision1); // TODO IM#QIMRIMF replace ro road_dir
//                        continue;
//                    }
//
//                    if (segment_group_id1 == outgoing_segment.segment_group_id ||
//                        segment_group_id2 == outgoing_segment.segment_group_id) {
//                        ConnectRoadSegmentMainToIngoing(ingoing_segment, outgoing_segment, &map.decision2);
//                        continue;
//                    }
//                    if (segment_group_id1 == ingoing_segment.segment_group_id ||
//                        segment_group_id2 == ingoing_segment.segment_group_id) {
//                        ConnectRoadSegmentMainToOutgoing(ingoing_segment, outgoing_segment, &map.decision2);
//                    }
//                }
//            }
//
//        } else
// // probably some junction
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

/*
 * if (segment_groups.size() == 2) {
        // no junction, just road continuation
        //   - connect each to each
        for (const auto& ingoing_segment : ingoing_segments) {
            for (const auto &outgoing_segment: outgoing_segments) {
                if (ingoing_segment.segment_group_id == outgoing_segment.segment_group_id) {
                    continue;
                }
                ConnectRoadSegmentEachToEach(ingoing_segment, outgoing_segment, &map.road_id);
            }
        }
    } else {

        int outgoing_layer = 1;
        for (const auto &outgoing_segment: outgoing_segments) {
            if (outgoing_layer >= 3) {
                not_enough_layers_cnt++;
                continue;
            }

            RasterDataPoint *map_to_fill;
            if (outgoing_layer == 1) {
                map_to_fill = &map.decision1;
            } else if (outgoing_layer == 2) {
                map_to_fill = &map.decision2;
            }


            for (const auto &ingoing_segment: ingoing_segments) {
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
 */