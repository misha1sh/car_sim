#include "raster_map_builder.h"

#include "fmt/core.h"
#include "rasterize/roads_map_reader.h"
#include "rasterize/roads_rasterizer.h"
#include "utils/verify.h"

RasterMapBuilder::RasterMapBuilder(double pixels_per_meter) :
        map_holder_(),
        map_(nullptr),
        pixels_per_meter_{pixels_per_meter}
{
}

PointI CropImageSize(PointI image_size) {
    if (image_size.x > 7000 || image_size.y > 7000) {
        const auto new_image_size = image_size.asPointF().Crop(0, 7000.).asPointI();
        fmt::print(stderr, "Image too big: {}x{} px. It will be cropped to {}x{} px\n",
                   image_size.x, image_size.y, new_image_size.x, new_image_size.y);
        image_size = new_image_size;
    }
    return image_size;
}

void RasterMapBuilder::CreateRoadsMap(std::filesystem::path osm_input_file_path) {
    RoadsMapReader reader;
    auto roads_vector_map = reader.ReadRoads(osm_input_file_path);


//    auto roads_vector_map = RoadsVectorMap::Create(
//            {{1, Node{1, {0, 300}}},
//             {2, Node{2, {100, 300}}},
//             {3, Node{3, {200, 300}}},
//             {4, Node{4, {200, 0}}},
//             {5, Node{5, {200, 400}}}
//            },
//            {
//                    {1, Road{1, 3, false, {1, 2}}},
//                    {2, Road{2, 2, false, {2, 3, 4}}},
//                    {3, Road{3, 2, false, {3, 5}}}
//            },
//            false
//            );
//
//


// ALEST!!!!
//    auto roads_vector_map = RoadsVectorMap::Create(
//            {{1, Node{1, {0, 300}}},
//             {2, Node{2, {100, 300}}},
//             {3, Node{3, {200, 300}}},
//             {4, Node{4, {200, 0}}},
//            },
//            {
//                    {1, Road{1, 3, false, {1, 2}}},
//                    {2, Road{2, 2, false, {2, 3, 4}}},
//            },
//            false
//    );

//    auto roads_vector_map = RoadsVectorMap::Create(
//            {{1, Node{1, {0, 0}}},
//             {2, Node{2, {200, 200}}},
//             {3, Node{3, {300, 300}}},
//            },
//            {
//                    {1, Road{2, 2, false, {1, 2, 3}}},
//            },
//            false
//    );

//
//    auto roads_vector_map = RoadsVectorMap::Create(
//            {{1, Node{1, {0, 0}}},
//             {2, Node{2, {0, 100}}},
//             {3, Node{3, {0, 200}}},
//             {4, Node{4, {100, 200}}},
//            },
//            {
//                    {1, Road{1, 2, false, {1, 2, 3, 4, 1}}},
//            },
//            false
//    );

//    auto roads_vector_map = RoadsVectorMap::Create(
//            {{1, Node{1, {0, 0}}},
//             {2, Node{2, {100, 0}}},
//             {3, Node{3, {-100, 0}}},
//             {4, Node{4, {0, 100}}},
//             {5, Node{5, {-40, 70}}}
//            },
//            {
//                    {1, Road{/*id*/ 1, /*lanes_count*/ 2, /* is_oneway */ false, {2, 1, 3}}},
//                    {2, Road{/*id*/ 2, /*lanes_count*/ 6, /* is_oneway */ true, {1, 4}}},
//                    {3, Road{3, 1, false, {1, 5}}}
//            },
//            false
//    );

//
//    auto roads_vector_map = RoadsVectorMap::Create(
//            {{1, Node{1, {0, 0}}},
//             {2, Node{2, {12.5, 12.5}}},
//             {3, Node{3, {50, 50}}},
//             {4, Node{4, {175 / 2., 25 / 2.}}},
//             {5, Node{5, {225 / 2., 0}}}
//            },
//            {
//                    {1, Road{/*id*/ 1, /*lanes_count*/ 2, /* is_oneway */ false, {1, 2, 3, 4, 5}}},
//            },
//            false
//    );

    const auto image_size = ((roads_vector_map->stats.max_xy - roads_vector_map->stats.min_xy) * pixels_per_meter_).asPointI() ;

    const MetersToImageProjector image_projector{
        roads_vector_map->stats.min_xy, roads_vector_map->stats.max_xy,
        image_size.x, image_size.y
    };

    const auto cropped_image_size = CropImageSize(image_size);

    VERIFY(cropped_image_size.x < 10000 && cropped_image_size.y < 10000);

    map_holder_.Init(std::make_unique<RasterMap>(cropped_image_size.x, cropped_image_size.y, pixels_per_meter_, image_projector));

    auto guard = map_holder_.Get(map_);

    RoadsRasterizer rasterizer(roads_vector_map);
    rasterizer.RasterizeRoads(*map_);

//    for (double i = 0; i < 0.999; i += 0.001) {
//        map_->lane_id(cropped_image_size.x * i, cropped_image_size.y * i) = int(i * 1000);
//    }
//    map_->lane_id(0, 0) = 255;
//    map_->lane_id(cropped_image_size.x - 1, cropped_image_size.y - 1) = 254;
}

