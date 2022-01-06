#pragma once

#include "common/raster_map.h"

#include <filesystem>

class RasterMapBuilder {
public:
    explicit RasterMapBuilder(size_t x_len, size_t y_len);

    void CreateRoadsMap(std::filesystem::path osm_input_file_path);
    inline std::shared_ptr<RasterMap>& GetMap() {
        return map_;
    }

private:
    std::shared_ptr<RasterMap> map_;
};
