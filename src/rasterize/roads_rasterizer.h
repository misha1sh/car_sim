#pragma once

#include "common/roads_vector_map.h"
#include "common/raster_map.h"

class RoadsRasterizer {
public:
    void RasterizeRoads(const RoadsVectorMapPtr& vector_map, RasterMap& map);
};


