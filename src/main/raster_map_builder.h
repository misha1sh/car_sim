#pragma once

#include "common/raster_map.h"

#include <filesystem>

class RasterMapBuilder {
public:
    explicit RasterMapBuilder(double pixels_per_meter);

    void CreateRoadsMap(std::filesystem::path osm_input_file_path);
    inline std::shared_ptr<RasterMap>& GetMap() {
        return map_;
    }

private:
    std::shared_ptr<RasterMap> map_;
    double pixels_per_meter_;
};
