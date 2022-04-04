#pragma once

#include "common/entities.h"
#include "projections/lon_lat_to_meters_projector.h"
#include "projections/meters_to_image_projector.h"

//#include <range/v3/view/transform.hpp>
//#include <range/v3/view/map.hpp>
//#include <range/v3/algorithm/minmax.hpp>
//#include <range/v3/view/all.hpp>
#include <fmt/core.h>

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
        PointF center{};
        PointF min_xy{};
        PointF max_xy{};
    } stats{};

public:
    static RoadsVectorMapPtr Create(std::unordered_map<ID, Node> nodes, std::unordered_map<ID, Road> roads, bool project_meters=true) {
        auto res = std::make_shared<RoadsVectorMap>();
        res->nodes = std::move(nodes);
        res->roads = std::move(roads);

        if (project_meters) {
            res->ProjectToMeters();
        }
        res->BuildIndex();
//        res->ReduceNodesCount();
//        res->BuildIndex();


        res->CalcBounds();

        return res;
    }

private:
    void BuildIndex() {
        roads_for_node.clear();
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


    PointF FindCenter() const {
        const auto sum = std::accumulate(nodes.begin(), nodes.end(),
                               PointF{0, 0}, [](const PointF& coord, const auto& node) {
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

//        const auto good_nodes = roads_for_node | ranges::views::remove_if([](const auto& node) {
//            return node.first;
//        }) | ranges::views::values | ranges::to_vector;
//
//        const auto [min_x, max_x] = ranges::minmax( | ranges::views::transform([](const auto& node) {
//            return node.c.x;
//        }));
//        const auto [min_y, max_y] = ranges::minmax(nodes | ranges::views::values | ranges::views::transform([](const auto& node) {
//            return node.c.y;
//        }));


        stats.min_xy = {min_x - 30., min_y - 30.};
        stats.max_xy = {max_x + 30., max_y + 30.};
    }

    void ProjectToMeters() {
        stats.center = FindCenter();
        const LonLatToMetersProjector projector(stats.center);
        for (auto& [node_id, node] : nodes) {
            node.c = projector.project(node.c);
        }
    }

    void ReduceNodesCount() {
        int reduced_nodes_cnt = 0;
        for (auto& [road_id, road] : roads) {
            if (road.nodes.empty()) {
                continue;
            }


            std::vector<ID> new_nodes;
            for (int i = 0; i < road.nodes.size(); i++) {
                const ID node_id = road.nodes[i];
                if (i == road.nodes.size() - 1 || i == 0 || roads_for_node[node_id].size() > 1) {
                    new_nodes.push_back(node_id);
                }
                const auto prev_node = nodes[road.nodes[i - 1]].c;
                const auto next_node = nodes[road.nodes[i - 1]].c;
                if ((prev_node - next_node).Len() < 200.) {
                    reduced_nodes_cnt++;
                    continue;
                } // TODO: better algo
                new_nodes.push_back(node_id);
            }
            road.nodes = new_nodes;

//            auto prev_node = nodes[road.nodes[0]].c;
//            for (int i = 1; i < road.nodes.size(); i++) {
//                const auto cur_node = nodes[node_id].c;
//                if (roads_for_node[node_id].size() <= 1 &&
//                    (prev_node - cur_node).Len() < 40.) {
//                    continue;
//                }
//                new_nodes.push_back()
//            }
        }

        fmt::print("Reduced {} nodes, which were too close\n", reduced_nodes_cnt);

    }
};

