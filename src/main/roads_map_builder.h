#pragma once

#include "common/raster_map.h"

#include <filesystem>

class RoadsMapBuilder {
public:
    RoadsRasterMapPtr CreateRoadsMap(std::filesystem::path osm_input_file_path);
};
