#include "roads_rasterizer.h"
#include "common/geometry.h"

void RoadsRasterizer::CreateLane(const PointF& left_bottom_corner, const PointF& right_bottom_corner,
                                 const PointF& left_top_corner, const PointF& right_top_corner) {
    const PolygonI polygonImage = image_projector_->project(PolygonF{{
                                                                             left_bottom_corner,
                                                                             right_bottom_corner,
                                                                             right_top_corner,
                                                                             left_top_corner
                                                                     }});
    const PointF dir = (left_top_corner - left_bottom_corner).Norm();
    const ID lane_id = lane_id_counter_++;

    raster_map_->lane_id.fill(polygonImage, lane_id);
    raster_map_->lane_dir.insert({lane_id, dir});
}




void RoadsRasterizer::ConnectRoadSegmentEachToEach(const RoadSegment& ingoing_segment,
                                                   const RoadSegment& outgoing_segment) {
    VERIFY(ingoing_segment.segment_group_id != outgoing_segment.segment_group_id);
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

void RoadsRasterizer::ConnectToJunction(const RoadSegment& segment, const bool is_ingoing, double offset) {
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

    const auto perp = segment.dir.RightPerpendicular();
    for (int i = 1; i <= segment.road.lanes_count; i++) {
        PointF left_bottom = p1 + perp * lane_width_ * (i - 1);
        PointF right_bottom = p1 + perp * lane_width_ * i;

//        const PointF left_medium = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * (i - 1);
//        const PointF right_medium = ingoing_segment.p2 /* equals outgoing_segment.p1 */ + median_dir * lane_width_ * i;

        PointF left_top = p2 + perp * lane_width_ * (i - 1);
        PointF right_top = p2 + perp * lane_width_ * i;

        CreateLane(left_bottom, right_bottom, left_top, right_top);
    }
}

void RoadsRasterizer::HandleSegments(const std::vector<RoadSegment>& ingoing_segments,
                                     const std::vector<RoadSegment>& outgoing_segments,
                                     const std::vector<SegmentsGroup>& segment_groups) {
    if (segment_groups.size() == 2) {
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
        for (const auto& ingoing_segment : ingoing_segments) {
            ConnectToJunction(ingoing_segment, true, segment_groups[ingoing_segment.segment_group_id].offset);
        }
        for (const auto& outgoing_segment : outgoing_segments) {
            ConnectToJunction(outgoing_segment, false, segment_groups[outgoing_segment.segment_group_id].offset);
        }
    }
}


