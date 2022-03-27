#pragma once

#include "common/roads_vector_map.h"
#include "common/raster_map.h"

#include <optional>

struct RoadSegment {
    const Node& n1, n2;
    const Road& road;
    Coord p1, p2;
    Coord dir;
    ID segment_group_id; // IMPORTANT: parallel segemnts from one road will have the same id, but diffrent is_reverse
};

struct SegmentsGroup {
    ID segment_group_id; // IMPORTANT: parallel segemnts from one road will have the same id, but diffrent is_reverse
    RoadSegment& straight;
    RoadSegment* reverse;
};


class RoadsRasterizer {
public:
    explicit RoadsRasterizer(const RoadsVectorMapPtr& vector_map);
    void RasterizeRoads(RasterMap& map);
private:
//    PolylineF BuildPolyline(const Road& road);
    std::tuple<std::vector<RoadSegment>, std::vector<RoadSegment>, std::vector<SegmentsGroup>>
    BuildIngoingAndOutgoingSegments(const auto& center_node,
                                    const std::vector<RoadsVectorMap::RoadForNode>& roads_for_node);

//    void ConnectRoadSegmentEachToEach(const RoadSegment& ingoing_segment,
//                                      const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill);
//
//
//    void ConnectRoadSegmentMainToIngoing(const RoadSegment& ingoing_segment,
//                                         const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill);
//
//    void ConnectRoadSegmentMainToOutgoing(const RoadSegment& ingoing_segment,
//                                         const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill);

    void CreateLane(const Coord& left_bottom_corner, const Coord& right_bottom_corner,
                           const Coord& left_top_corner, const Coord& right_top_corner);

    void HandleSegments(const std::vector<RoadSegment>& ingoing_segments,
                        const std::vector<RoadSegment>& outgoing_segments,
                        const std::vector<SegmentsGroup>& segment_groups);


    const RoadsVectorMapPtr &vector_map_;
    const std::unordered_map<ID, Node>& nodes_;
    const std::unordered_map<ID, Road>& roads_;
    const double lane_width_;

    RasterMap* raster_map_{nullptr};

    std::unique_ptr<MetersToImageProjector> image_projector_;

    ID lane_id_counter_{1};
};


