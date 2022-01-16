#pragma once

#include "entities.h"
#include "raster_data.h"


enum class CarCellType : unsigned char {
    NONE = 0,
    CENTER = 128,
    BODY = 254,
};

struct RasterMap {
    // direction where road goes
    // (0, 0) -- means no road
    // length -- maximum allowed speed
    RasterDataPoint road_dir;

    RasterDataPoint decision1;
    RasterDataPoint decision2;

    RasterDataEnum<CarCellType> car_cells;
    RasterDataPoint car_data;

    RasterDataEnum<CarCellType> new_car_cells;
    RasterDataPoint new_car_data;

    Coord size;
    double pixels_per_meter;

    RasterMap(int x_len, int y_len, double pixels_per_meter_):
            road_dir(x_len, y_len, {0, 0}),
            decision1(x_len, y_len, {0, 0}),
            decision2(x_len, y_len, {0, 0}),
            car_cells(x_len, y_len, CarCellType::NONE),
            car_data(x_len, y_len, {0, 0}),
            new_car_cells(x_len, y_len, CarCellType::NONE),
            new_car_data(x_len, y_len, {0, 0}),
            size(x_len, y_len),
            pixels_per_meter(pixels_per_meter_)
    {
    }
};

typedef std::shared_ptr<RasterMap> RasterMapPtr;