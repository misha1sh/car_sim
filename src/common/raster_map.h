#pragma once

#include "entities.h"
#include "raster_data.h"

#include "utils/guard_holder.h"
#include "utils/verify.h"

#include <memory>

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

    // IMPORTANT : DO NOT FORGET TO ADD TO COPY

    Coord size;
    PointI sizeI;
    double pixels_per_meter;

    RasterMap(int x_len, int y_len, double pixels_per_meter_):
            road_dir(x_len, y_len, {0, 0}),
            decision1(x_len, y_len, {0, 0}),
            decision2(x_len, y_len, {0, 0}),
            car_cells(x_len, y_len, CarCellType::NONE),
            car_data(x_len, y_len, {0, 0}),
            new_car_cells(x_len, y_len, CarCellType::NONE),
            new_car_data(x_len, y_len, {0, 0}),
            // IMPORTANT : DO NOT FORGET TO ADD TO COPY
            size(x_len, y_len),
            sizeI(x_len, y_len),
            pixels_per_meter(pixels_per_meter_)
    {
    }

    void CopyTo(RasterMap& other) {
        VERIFY(sizeI.x() == other.sizeI.x() && sizeI.y() == other.sizeI.y());
        VERIFY(pixels_per_meter == other.pixels_per_meter);

        road_dir.copyTo(other.road_dir);
        decision1.copyTo(other.decision1);
        decision2.copyTo(other.decision2);
        car_cells.copyTo(other.car_cells);
        car_data.copyTo(other.car_data);
        new_car_cells.copyTo(other.new_car_cells);
        new_car_data.copyTo(other.new_car_data);
    }

};


typedef std::unique_ptr<RasterMap> RasterMapPtr;
typedef guard_holder::GuardHolder<RasterMap> RasterMapHolder;