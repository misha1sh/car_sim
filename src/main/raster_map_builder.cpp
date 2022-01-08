#include "raster_map_builder.h"

#include "fmt/core.h"
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

//    auto roads_vector_map = RoadsVectorMap::Create(
//            {{1, Node{1, {0, 300}}},
//             {2, Node{2, {100, 300}}},
//             {3, Node{3, {200, 300}}},
//             {4, Node{4, {200, 0}}},
//             {5, Node{5, {200, 600}}}
//            },
//            {
//                    {1, Road{1, 3, false, {1, 2}}},
//                    {2, Road{2, 2, false, {2, 3, 4}}},
//                    {3, Road{3, 2, false, {3, 5}}}
//            },
//            false
//            );

    auto image_size = (roads_vector_map->stats.max_xy - roads_vector_map->stats.min_xy) * pixels_per_meter_ ;

    if (image_size.x > 7000 || image_size.y > 7000) {
        const auto new_image_size = image_size.Crop(0, 7000.);
        fmt::print(stderr, "Image too big: {}x{} px. It will be cropped to {}x{} px\n", image_size.x, image_size.y, new_image_size.x, new_image_size.y);
        image_size = new_image_size;
    }

//    VERIFY(image_size.x < 10000 && image_size.y < 10000);

    map_ = std::make_shared<RasterMap>(image_size.x, image_size.y);

    RoadsRasterizer rasterizer;
    rasterizer.RasterizeRoads(roads_vector_map, *map_);
}

