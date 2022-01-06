#include "raster_map_builder.h"

#include "rasterize/roads_map_reader.h"
#include "rasterize/roads_rasterizer.h"

RasterMapBuilder::RasterMapBuilder(size_t x_len, size_t y_len) :
    map_{new RasterMap(x_len, y_len)}
{
}

void RasterMapBuilder::CreateRoadsMap(std::filesystem::path osm_input_file_path) {
    RoadsMapReader reader;
    auto roads_vector_map = reader.ReadRoads(osm_input_file_path);

    RoadsRasterizer rasterizer;
    rasterizer.RasterizeRoads(roads_vector_map, *map_);
}

