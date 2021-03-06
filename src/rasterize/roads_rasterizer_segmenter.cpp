#include "roads_rasterizer.h"

template <typename T>
std::optional<T> OptionalFromPtr(const T* ptr) {
    if (ptr) {
        return std::make_optional<T>(*ptr);
    }
    return std::nullopt;
}

RoadSegment RoadsRasterizer::CreateIngoingSegment(const Node& node1, const Node& node2, const Road& road, const ID segment_group_id) {
    const auto p1 = /* node1.c;*/ (node1.c + node2.c) / 2.;
    const auto p2 = node2.c;
    const auto dir = (p2 - p1).Norm();
    return RoadSegment{
            .n1 = node1,
            .n2 = node2,
            .road = road,
            .p1 = p1,
            .p2 = p2,
            .dir = dir,
            .segment_group_id = segment_group_id,
            .id= segment_id_counter_++
    };
}

RoadSegment RoadsRasterizer::CreateOutgoingSegment(const Node& node1, const Node& node2, const Road& road, const ID segment_group_id) {
    const auto p1 = node1.c;
    const auto p2 = /*node2.c;*/ (node1.c + node2.c) / 2.;
    const auto dir = (p2 - p1).Norm();
    return RoadSegment{
            .n1 = node1,
            .n2 = node2,
            .road = road,
            .p1 = p1,
            .p2 = p2,
            .dir = dir,
            .segment_group_id = segment_group_id,
            .id= segment_id_counter_++
    };
}

ID RoadsRasterizer::GetMergedNodeID(ID node_id) {
    const auto node_to_replace_with = crossroad_nodes_merge_.find(node_id);
    if (node_to_replace_with != crossroad_nodes_merge_.end()) {
        return node_to_replace_with->second;
    }
    return node_id;
}

const Node& RoadsRasterizer::GetMergedNode(const ID node_id) {
    const ID merged_id = GetMergedNodeID(node_id);
    const auto merged_node_it = nodes_.find(merged_id);
    VERIFY(merged_node_it != nodes_.end())
    return merged_node_it->second;
}

std::tuple<std::vector<RoadSegment>, std::vector<RoadSegment>, std::vector<SegmentsGroup>>
RoadsRasterizer::BuildIngoingAndOutgoingSegments(const Node& center_node_before_merge,
                                const std::vector<RoadsVectorMap::RoadForNode>& roads_for_node) {
    std::vector<RoadSegment> ingoing_segments{}, outgoing_segments{};
    std::vector<SegmentsGroup> segments_groups{};

    ID segment_id_counter = 0;

    const auto center_node = GetMergedNode(center_node_before_merge.id);

    for (const auto& road_for_node : roads_for_node) {
        const auto& road = roads_.at(road_for_node.road_id);
        if (road_for_node.next_node &&
                GetMergedNodeID(*road_for_node.next_node) != center_node.id) {
            const auto next_node = GetMergedNode(*road_for_node.next_node);

            const ID segment_group_id = segment_id_counter++;
            outgoing_segments.push_back(
                    CreateOutgoingSegment(center_node, next_node, road, segment_group_id)
            );
            RoadSegment outgoing_segment = outgoing_segments.back();


            RoadSegment* ingoing_segment = nullptr;
            if (!road.is_one_way) {
                ingoing_segments.push_back(
                        CreateIngoingSegment(next_node, center_node, road, segment_group_id)
                );
                ingoing_segment = &ingoing_segments.back();
            }

            segments_groups.push_back(SegmentsGroup{
                    segment_group_id,
                    /* straight */ outgoing_segment,
                    /* reverse */ OptionalFromPtr(ingoing_segment),
                    /* into_node */ OptionalFromPtr(ingoing_segment),
                    /* out_of_node */ OptionalFromPtr(&outgoing_segment),
                    /* offset */ 0.
            });
        }


        if (road_for_node.prev_node &&
                GetMergedNodeID(*road_for_node.prev_node) != center_node.id) {
            const auto prev_node = GetMergedNode(*road_for_node.prev_node);

            const ID segment_group_id = segment_id_counter++;
            ingoing_segments.push_back(
                    CreateIngoingSegment(prev_node, center_node, road, segment_group_id)
            );
            RoadSegment ingoing_segment = ingoing_segments.back();


            RoadSegment* outgoing_segment = nullptr;
            if (!road.is_one_way) {
                outgoing_segments.push_back(
                        CreateOutgoingSegment(center_node, prev_node, road, segment_group_id)
                );
                outgoing_segment = &outgoing_segments.back();
            }

            segments_groups.push_back(SegmentsGroup{
                    segment_group_id,
                    /* straight */ ingoing_segment,
                    /* reverse */ OptionalFromPtr(outgoing_segment),
                    /* into_node */ OptionalFromPtr(&ingoing_segment),
                    /* out_of_node */ OptionalFromPtr(outgoing_segment),
                    /* offset */ 0.
            });
        }
    }

    return {ingoing_segments, outgoing_segments, segments_groups};
}
