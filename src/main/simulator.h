#pragma once

#include "common/entities.h"
#include "common/raster_map.h"

class Simulator {
public:
    enum class State {
        NO_MAP,
        LOADING_MAP,
        MAP_LOADED,
        RUNNING,
        PAUSED
    };

    void CreateMap();
    void RunTick();

    RasterMapPtr GetMap();

    inline const State& GetState() const {
        return state_;
    }

private:
    State state_{State::NO_MAP};
    RasterMapPtr map_{};
};

