#pragma once

#include "common/entities.h"
#include "common/raster_map.h"

struct Car {
    Coord pos;
    Coord dir;
    Coord speed;
    Coord size;
};

class Simulator {
public:
    enum class State {
        NO_MAP,
        LOADING_MAP,
        MAP_LOADED,
        RUNNING,
        PAUSED
    };

    Car ReadCar(const PointI& pos);
    void WriteCar(const Car& car);
    void CreateMap();
    void RunTick();
    void SimulateCars(double delta);

    RasterMapPtr GetMap();

    inline const State& GetState() const {
        return state_;
    }

private:
    State state_{State::NO_MAP};
    RasterMapPtr map_{};

    std::vector<PointI> cars_;
    std::vector<PointI> new_cars_;
};

