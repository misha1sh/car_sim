#pragma once

#include "common/entities.h"

#include <vector>
#include <unordered_map>
#include <memory>



struct RoadsVectorMap;
typedef std::shared_ptr<const RoadsVectorMap> RoadsVectorMapPtr;

struct RoadsVectorMap {
    std::unordered_map<ID, Node> nodes;
    std::unordered_map<ID, Road> roads;
    std::unordered_map<ID, std::vector<ID>> roads_for_node;

public:
    static RoadsVectorMapPtr Create(std::unordered_map<ID, Node> nodes, std::unordered_map<ID, Road> roads) {
        auto res = std::make_shared<RoadsVectorMap>();
        res->nodes = std::move(nodes);
        res->roads = std::move(roads);
        res->BuildIndex();
        return res;
    }

private:
    void BuildIndex() {
        for (const auto& [road_id, road] : roads) {
            for (const auto& node_id : road.nodes) {
                roads_for_node[node_id].push_back(road_id);
            }
        }
    }
};

