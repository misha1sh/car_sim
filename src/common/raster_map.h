#pragma once

#include "entities.h"
#include "raster_data.h"


enum class CarCellType : char {
    NONE = 0,
    CENTER,
    BODY,
};

struct RasterMap {
    // direction where road goes
    // (0, 0) -- means no road
    // length -- maximum allowed speed
    RasterDataPoint road_dir;

    RasterDataEnum<CarCellType> car_cells;

    // direction and speed of car
    RasterDataPoint car_speed;
};

typedef std::shared_ptr<const RasterMap> RoadsRasterMapPtr;