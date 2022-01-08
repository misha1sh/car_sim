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

    RasterDataPoint decision1;
    RasterDataPoint decision2;

//    RasterDataEnum<CarCellType> car_cells;

    // direction and speed of car
    RasterDataPoint car_speed;

    Coord size;

    RasterMap(int x_len, int y_len):
            road_dir(x_len, y_len, {0, 0}),
            decision1(x_len, y_len, {0, 0}),
            decision2(x_len, y_len, {0, 0}),
//            car_cells(x_len, y_len, CarCellType::NONE),
            car_speed(x_len, y_len, {0, 0}),
            size(x_len, y_len)
    {
    }
};

typedef std::shared_ptr<RasterMap> RasterMapPtr;