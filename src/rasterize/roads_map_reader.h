#pragma once

#include "common/roads_vector_map.h"

#include <filesystem>


class RoadsMapReader {
public:
    RoadsVectorMapPtr ReadRoads(const std::filesystem::path& osm_input_file_path);
};

