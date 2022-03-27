#pragma once

#include "roads_rasterizer.h"


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
                ConnectRoadSegmentEachToEach(ingoing_segment, outgoing_segment, &map.road_id);
            }
        }
    } else {

    }
}