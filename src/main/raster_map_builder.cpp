#include "raster_map_builder.h"

#include "rasterize/roads_map_reader.h"
#include "rasterize/roads_rasterizer.h"
#include "utils/verify.h"


RasterMapBuilder::RasterMapBuilder(double pixels_per_meter) :
        pixels_per_meter_{pixels_per_meter}
{
}

void RasterMapBuilder::CreateRoadsMap(std::filesystem::path osm_input_file_path) {
    RoadsMapReader reader;
    auto roads_vector_map = reader.ReadRoads(osm_input_file_path);

    const auto image_size = (roads_vector_map->stats.max_xy - roads_vector_map->stats.min_xy) * pixels_per_meter_;

    VERIFY(image_size.x < 7000 && image_size.y < 7000);

    map_ = std::make_shared<RasterMap>(image_size.x, image_size.y);

    RoadsRasterizer rasterizer;
    rasterizer.RasterizeRoads(roads_vector_map, *map_);
}

