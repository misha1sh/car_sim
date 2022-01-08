#pragma once

#include "common/entities.h"
#include "projections/lon_lat_to_meters_projector.h"
#include "projections/meters_to_image_projector.h"

#include <range/v3/all.hpp>

#include <memory>
#include <optional>
#include <numeric>
#include <vector>
#include <unordered_map>



struct RoadsVectorMap;
typedef std::shared_ptr<const RoadsVectorMap> RoadsVectorMapPtr;

struct RoadsVectorMap {
    std::unordered_map<ID, Node> nodes{};
    std::unordered_map<ID, Road> roads{};

    struct RoadForNode {
        ID road_id{};
        std::optional<ID> prev_node{};
        std::optional<ID> next_node{};
    };
    std::unordered_map<ID, std::vector<RoadForNode>> roads_for_node{};

    struct Stats{
        Coord center{};
        Coord min_xy{};
        Coord max_xy{};
    } stats{};

public:
    static RoadsVectorMapPtr Create(std::unordered_map<ID, Node> nodes, std::unordered_map<ID, Road> roads, bool project_meters=true) {
        auto res = std::make_shared<RoadsVectorMap>();
        res->nodes = std::move(nodes);
        res->roads = std::move(roads);
        res->BuildIndex();

        if (project_meters) {
            res->ProjectToMeters();
        }
        res->CalcBounds();

        return res;
    }

private:
    void BuildIndex() {
        for (const auto& [road_id, road] : roads) {
            for (int i = 0; i < road.nodes.size(); i++) {
                roads_for_node[road.nodes[i]].push_back({
                    road_id,
                    i > 0?
                        std::make_optional(road.nodes[i - 1]) :
                        std::nullopt,
                    i + 1 < road.nodes.size()?
                        std::make_optional(road.nodes[i + 1]) :
                        std::nullopt
                });
            }
        }
    }


    Coord FindCenter() const {
        const auto sum = std::accumulate(nodes.begin(), nodes.end(),
                               Coord{0, 0}, [](const Coord& coord, const auto& node) {
                    return node.second.c + coord;
                });
        return sum / nodes.size();
    }

    void CalcBounds() {
        double min_x = +std::numeric_limits<double>::infinity();
        double min_y = +std::numeric_limits<double>::infinity();
        double max_x = -std::numeric_limits<double>::infinity();
        double max_y = -std::numeric_limits<double>::infinity();
        for (const auto& [node_id_with_road, _] : roads_for_node) {
            const auto& c = nodes[node_id_with_road].c;
            min_x = std::min(min_x, c.x);
            min_y = std::min(min_y, c.y);
            max_x = std::max(max_x, c.x);
            max_y = std::max(max_y, c.y);
        }
//        const auto [min_x, max_x] = ranges::minmax(nodes | ranges::views::values | ranges::views::transform([](const auto& node) {
//            return node.c.x;
//        }));
//        const auto [min_y, max_y] = ranges::minmax(nodes | ranges::views::values | ranges::views::transform([](const auto& node) {
//            return node.c.y;
//        }));


        stats.min_xy = {min_x - 200., min_y - 200.};
        stats.max_xy = {max_x + 200., max_y + 200.};
    }

    void ProjectToMeters() {
        stats.center = FindCenter();
        const LonLatToMetersProjector projector(stats.center);
        for (auto& [node_id, node] : nodes) {
            node.c = projector.project(node.c);
        }
    }
};

