#pragma once

#include "roads_vector_map.h"

#include <filesystem>


class RoadsExtractor {
public:
    explicit RoadsExtractor(std::filesystem::path osm_input_file_path);

    RoadsVectorMapPtr ExtractRoads();

private:
    std::filesystem::path osm_input_file_path_;
};

