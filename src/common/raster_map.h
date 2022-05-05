#pragma once

#include "entities.h"
#include "raster_data.h"

#include "projections/meters_to_image_projector.h"
#include "utils/guard_holder.h"
#include "utils/verify.h"

#include <set>
#include <unordered_set>
#include <memory>

//enum class CarCellType : unsigned char {
//    NONE = 0,
//    CENTER = 128,
//    BODY = 254,
//};



struct CrossroadLane {
    enum class EndType : unsigned char {
        STRAIGHT,
        LEFT,
        RIGHT
    };

    bool goes_into_crossroad;
    PointF start_point;
    PointF end_point;
    int lane_num;

    int traffic_lights_id;
    int traffic_lights_state;
    std::vector<std::pair<CrossroadLane, EndType>> end_lanes;
};

struct TrafficLights {
    PointF center;
    std::vector<std::set<int>> lanes;
};

struct RasterMap {
    // >0 = road_edge id
    // -1 = crossroad
    RasterDataT<int> lane_id;
    std::unordered_map<int, PointF> lane_dir;


    std::unordered_map<int, CrossroadLane> crossroad_lanes;
    std::unordered_map<int, TrafficLights> traffic_lights;

    RasterDataT<int> car_cells;
//    RasterDataPoint car_data;

    RasterDataT<int> new_car_cells;
//    RasterDataPoint new_car_data;

    RasterDataT<int> debug;

    // IMPORTANT : DO NOT FORGET TO ADD TO COPY

    PointF size;
    PointI sizeI;
    double pixels_per_meter;

    MetersToImageProjector image_projector;

    RasterMap(int x_len, int y_len, double pixels_per_meter_,
              MetersToImageProjector image_projector_):
            lane_id(x_len, y_len, 0),
            lane_dir{},
            crossroad_lanes{},
            car_cells(x_len, y_len, 0),
            new_car_cells(x_len, y_len, 0),
            debug(x_len, y_len, 0),
            // IMPORTANT : DO NOT FORGET TO ADD TO COPY
            size(x_len, y_len),
            sizeI(x_len, y_len),
            pixels_per_meter(pixels_per_meter_),
            image_projector(image_projector_)
    {
    }

    void CopyTo(RasterMap& other) {
        VERIFY(sizeI == other.sizeI);
        VERIFY(pixels_per_meter == other.pixels_per_meter);

        lane_id.copyTo(other.lane_id);
        other.lane_dir = lane_dir;
        other.crossroad_lanes = crossroad_lanes;
        car_cells.copyTo(other.car_cells);
        new_car_cells.copyTo(other.new_car_cells);
        debug.copyTo(other.debug);
    }

};


typedef std::unique_ptr<RasterMap> RasterMapPtr;
typedef guard_holder::GuardHolder<RasterMap> RasterMapHolder;