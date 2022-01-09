#include "simulator.h"

#include "main/raster_map_builder.h"
#include "utils/verify.h"

void Simulator::CreateMap() {
    VERIFY(state_ == State::NO_MAP || state_ == State::PAUSED);
    state_ = State::LOADING_MAP;

    RasterMapBuilder builder(1  * 5);
    builder.CreateRoadsMap("../data/moscow_easy.osm");
    map_ = builder.GetMap();

    state_ = State::PAUSED;
}

PolygonI BuildCarPolygon(Coord pos, Coord size, Coord dir) {
    const auto perp = dir.RightPerpendicular();
    size = size / 2.;
    return PolygonI {{
                             (pos + dir * size.y + perp * size.x).asPointI(),
                             (pos - dir * size.y + perp * size.x).asPointI(),
                             (pos - dir * size.y - perp * size.x).asPointI(),
                             (pos + dir * size.y - perp * size.x).asPointI(),
                             (pos + dir * size.y + perp * size.x).asPointI(),
                     }};
}

Car Simulator::ReadCar(const PointI& pos) {
    Car res;
    VERIFY(map_->car_cells(pos.x(), pos.y()) == CarCellType::CENTER);

    res.pos = Coord(pos) + map_->car_data(pos.x(), pos.y());
    res.dir = map_->car_data(pos.x(), pos.y() + 1);
    res.speed = map_->car_data(pos.x() - 1, pos.y());
    res.size = map_->car_data(pos.x() + 1, pos.y());

    return res;
}


void Simulator::WriteCar(const Car& car) {
    PointI pos = car.pos.asPointI();

    map_->new_car_data(pos.x(), pos.y()) = car.pos - Coord(pos);
    map_->new_car_data(pos.x(), pos.y() + 1) = car.dir;
    map_->new_car_data(pos.x() - 1, pos.y()) = car.speed;
    map_->new_car_data(pos.x() + 1, pos.y()) = car.size;

    PolygonI poly = BuildCarPolygon(car.pos, car.size, car.speed);

    map_->new_car_cells.fill(poly, CarCellType::BODY);
    map_->new_car_cells(pos.x(), pos.y()) = CarCellType::CENTER;
}

void Simulator::SimulateCars(double delta) {
    map_->new_car_cells.fill(CarCellType::NONE);
    map_->new_car_data.fill({0, 0});
    for (const auto& car_pos : cars_) {
        Car car = ReadCar(car_pos);
        car.pos += car.speed * map_->pixels_per_meter * delta;
    }

    std::swap(map_->car_data, map_->new_car_data);
    std::swap(map_->car_cells, map_->new_car_cells);
}

void Simulator::RunTick() {
    VERIFY(state_ == State::PAUSED)
    state_ = State::RUNNING;

    SimulateCars(0.001);

    state_ = State::PAUSED;
}

RasterMapPtr Simulator::GetMap() {
    return map_;
}
