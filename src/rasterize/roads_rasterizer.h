#pragma once

#include "common/roads_vector_map.h"
#include "common/raster_map.h"
#include "common/geometry.h"

#include <unordered_set>
#include <optional>

struct RoadSegment {
    // TODO : Switch toeferences. crutch because of UB in roads_rasterizer_segmenter
    const Node n1;
    const Node n2;
    const Road road;
    PointF p1, p2;
    PointF dir;
    ID segment_group_id; // IMPORTANT: parallel segments from one road will have the same id, but different is_reverse
    int id;
};

struct SegmentsGroup {
    ID segment_group_id; // IMPORTANT: parallel segments from one road will have the same id, but different is_reverse
    RoadSegment straight;
    std::optional<RoadSegment> reverse;

    std::optional<RoadSegment> into_node;
    std::optional<RoadSegment> out_of_node;

    double offset;
};


struct Lane {
    Line line_left;
    Line line_right;
    Line line_center; // guaranteed that line_center.a = node_start, line_center.b = center_node
};

class RoadsRasterizer {
public:
    explicit RoadsRasterizer(RoadsVectorMapPtr vector_map);
    void RasterizeRoads(RasterMap& map);
private:
//    PolylineF BuildPolyline(const Road& road);


    RoadSegment CreateIngoingSegment(const Node& node1, const Node& node2, const Road& road, const ID segment_group_id);
    RoadSegment CreateOutgoingSegment(const Node& node1, const Node& node2, const Road& road, const ID segment_group_id);

    std::tuple<std::vector<RoadSegment>, std::vector<RoadSegment>, std::vector<SegmentsGroup>>
    BuildIngoingAndOutgoingSegments(const Node& center_node,
                                    const std::vector<RoadsVectorMap::RoadForNode>& roads_for_node);
    ID GetMergedNodeID(ID node_id);
    const Node& GetMergedNode(ID node_id);


    void ConnectRoadSegmentEachToEach(const RoadSegment& ingoing_segment,
                                      const RoadSegment& outgoing_segment);
    std::vector<int> ConnectToJunction(const RoadSegment& segment, bool is_ingoing, double offset);
//    void ConnectRoadSegmentMainToIngoing(const RoadSegment& ingoing_segment,
//                                         const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill);
//
//    void ConnectRoadSegmentMainToOutgoing(const RoadSegment& ingoing_segment,
//                                         const RoadSegment& outgoing_segment, RasterDataPoint* map_to_fill);

    ID CreateLane(const PointF& left_bottom_corner, const PointF& right_bottom_corner,
                           const PointF& left_top_corner, const PointF& right_top_corner);

    void HandleSegments(const Node& center_node,
                        const std::vector<RoadSegment>& ingoing_segments,
                        const std::vector<RoadSegment>& outgoing_segments,
                        const std::vector<SegmentsGroup>& segment_groups);


    void DFS(ID node, double depth, std::unordered_set<ID>& nodes_set, bool root=true) const;

    void FindCrossroads();

    double CalculateOffestDistanceForRoad(const SegmentsGroup& road,
                                          const std::vector<SegmentsGroup>& segment_groups);

    Lane BuildLane(const SegmentsGroup& road);

    RoadsVectorMapPtr vector_map_;
    const std::unordered_map<ID, Node>& nodes_;
    const std::unordered_map<ID, Road>& roads_;
    std::unordered_set<ID> crossroad_nodes_{};


    std::unordered_map<ID, ID> crossroad_nodes_merge_{}; // node -> crossroad center
    std::unordered_map<ID, std::unordered_set<ID>> crossroad_nodes_merge_backward_; // crossroad_center -> {nodes}

    const double lane_width_;

    RasterMap* raster_map_{nullptr};

    MetersToImageProjector* image_projector_{nullptr};

    ID lane_id_counter_{1};
    int segment_id_counter_ = 1;

};


