#include "simulator.h"

#include "main/raster_map_builder.h"
#include "utils/verify.h"

void Simulator::CreateMap() {
    VERIFY(state_ == State::NO_MAP || state_ == State::PAUSED);
    state_ = State::LOADING_MAP;

    RasterMapBuilder builder(1  * 5);
    builder.CreateRoadsMap("../data/moscow_hard.osm");
    map_ = builder.GetMap();

    state_ = State::PAUSED;
}

void Simulator::RunTick() {
    VERIFY(state_ == State::PAUSED)
    state_ = State::RUNNING;



    state_ = State::PAUSED;
}

RasterMapPtr Simulator::GetMap() {
    return map_;
}
