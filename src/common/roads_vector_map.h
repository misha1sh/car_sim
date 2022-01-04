#pragma once

#include "common/entities.h"
#include "projections/lon_lat_to_meters_projector.h"
#include "projections/meters_to_image_projector.h"

#include <memory>
#include <numeric>
#include <vector>
#include <unordered_map>



struct RoadsVectorMap;
typedef std::shared_ptr<const RoadsVectorMap> RoadsVectorMapPtr;

struct RoadsVectorMap {
    std::unordered_map<ID, Node> nodes;
    std::unordered_map<ID, Road> roads;
    std::unordered_map<ID, std::vector<ID>> roads_for_node;

    struct Stats{
        Coord center;
        Coord min_xy;
        Coord max_xy;
    } stats;

    std::unique_ptr<LonLatToMetersProjector> lon_lat_projector;
    std::unique_ptr<MetersToImageProjector> meters_projector;

public:
    static RoadsVectorMapPtr Create(std::unordered_map<ID, Node> nodes, std::unordered_map<ID, Road> roads) {
        auto res = std::make_shared<RoadsVectorMap>();
        res->nodes = std::move(nodes);
        res->roads = std::move(roads);
        res->BuildIndex();

        res->ProjectToMeters();
        res->CalcBounds();

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

    Coord FindCenter() const {
        return std::accumulate(nodes.begin(), nodes.end(),
                               Coord{0, 0}, [](const Coord& coord, const auto& node) {
                    return node.second.c + coord;
                });
    }

    void CalcBounds() {
        const auto [min_x, max_x] = std::minmax_element(nodes.begin(), nodes.end(), [](const auto& n1, const auto& n2) {
            return n1.second.c.xy.x < n2.second.c.xy.x;
        });
        const auto [min_y, max_y] = std::minmax_element(nodes.begin(), nodes.end(), [](const auto& n1, const auto& n2) {
            return n1.second.c.xy.y < n2.second.c.xy.y;
        });
        stats.min_xy = {min_x, min_y};
        stats.max_xy = {max_x, max_y};
    }

    void ProjectToMeters() {
        stats.center = FindCenter();
        const LonLatToMetersProjector projector(stats.center);
        for (auto& [node_id, node] : nodes) {
            node.c = projector.project(node.c);
        }
    }
};

