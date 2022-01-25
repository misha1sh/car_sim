#pragma once

#include "common/raster_map.h"

#include <filesystem>

class RasterMapBuilder {
public:
    explicit RasterMapBuilder(double pixels_per_meter);

    void CreateRoadsMap(std::filesystem::path osm_input_file_path);
    inline RasterMapHolder& GetMapHolder() {
        return map_holder_;
    }

private:
    RasterMapHolder map_holder_{};
    std::unique_ptr<RasterMap> map_{nullptr};
    double pixels_per_meter_{1.};
};
