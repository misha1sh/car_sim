#include "roads_map_reader.h"

#include <boost/lexical_cast.hpp>
#include <fmt/core.h>
#include <range/v3/all.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/visitor.hpp>

#include <cstring>
#include <unordered_set>
#include <filesystem>
#include <QElapsedTimer>

inline bool contains_string(const char* str, const std::vector<const char*>& strs) {
    return std::ranges::any_of(strs, [&](const char* other_str) {
        return strcmp(str, other_str) == 0;
    });
}

bool is_road(const osmium::Way& way) {
    thread_local std::vector<const char*> suitable_tags = {
            "primary", "secondary", "tertiary", "motorway", "trunk",
            "unclassified", "tertiary", "residential", "unclassified",
            "primary_link", "secondary_link", "tertiary_link", "motorway_link", "trunk_link"
    };
    const char* tag = way.tags()["highway"];
    return tag && contains_string(tag, suitable_tags);
}

int find_lanes_count(const osmium::Way& way) {
    const char* lanes_str = way.tags()["lanes"];
    int lanes_count;
    if (!lanes_str || !boost::conversion::try_lexical_convert(lanes_str, lanes_count)) {
        return 1;
    }
    return std::min(lanes_count, 2);
}

bool is_one_way(const osmium::Way& way) {
    const char* oneway_str = way.tags()["oneway"];
    return oneway_str && (strcmp(oneway_str, "true") == 0 || strcmp(oneway_str, "yes") == 0);
}

std::vector<ID> extract_nodes(const osmium::Way& way) {
    return way.nodes() |
        ranges::views::transform([](const auto& nr) {
            return nr.ref();
        }) | ranges::to_vector;
}


struct RoadsMapExtractor : public osmium::handler::Handler {
    std::unordered_map<ID, Node> nodes = {};
    std::unordered_map<ID, Road> roads = {};

    size_t invalid_node_roads_cnt = 0;
    size_t invalid_location_nodes_cnt = 0;

    void way(const osmium::Way& way) {
        if (!is_road(way)) {
            return;
        }

        if (!ranges::all_of(way.nodes(), [this](const auto& nr) {
                return nodes.contains(nr.ref());
            })) {
            invalid_node_roads_cnt++;
            return;
        }

        roads[way.id()] = Road {
          .id = way.id(),
          .lanes_count = find_lanes_count(way),
          .is_one_way = is_one_way(way),
          .nodes = extract_nodes(way),
        };
    }

    void node(const osmium::Node& node) {
        if (!node.location().valid()) {
            invalid_location_nodes_cnt++;
            return;
        }

        nodes[node.id()] = Node {
            .id = node.id(),
            .c = {node.location().lat_without_check(), node.location().lon_without_check()}
        };
    }

    void printReport() {
        fmt::print("Extraction res:\n");
        fmt::print("  - nodes {} (invalid_location_nodes = {})\n", nodes.size(), invalid_location_nodes_cnt);
        fmt::print("  - roads {} (invalid_node_roads = {})\n", roads.size(), invalid_node_roads_cnt);
    }
};

RoadsVectorMapPtr RoadsMapReader::ReadRoads(const std::filesystem::path& osm_input_file_path) {
    osmium::io::Reader reader{osm_input_file_path.string(), osmium::osm_entity_bits::node | osmium::osm_entity_bits::way};
    QElapsedTimer timer;
    timer.start();

    RoadsMapExtractor extractor;
    osmium::apply(reader, extractor);
    extractor.printReport();
    fmt::print("Extraction took {} ms\n", timer.restart());

    return RoadsVectorMap::Create(std::move(extractor.nodes), std::move(extractor.roads));
}
