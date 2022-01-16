#include <QElapsedTimer>
#include "simulator.h"

#include "main/raster_map_builder.h"
#include "utils/verify.h"

#include <fmt/core.h>

void Simulator::CreateMap() {
    VERIFY(state_ == State::NO_MAP || state_ == State::PAUSED);
    state_ = State::LOADING_MAP;

    QElapsedTimer timer;
    timer.start();

    fmt::print("Building map\n");

    RasterMapBuilder builder(1  * 5);
    builder.CreateRoadsMap("../data/moscow_hard.osm"); // easy
    map_ = builder.GetMap();

    fmt::print("Building took {} ms\n", timer.restart());


    const int cars_count = 1;
    fmt::print("Spawning {} cars\n", cars_count);
    SpawnCars(cars_count);
//    map_->new_car_cells.fill(CarCellType::CENTER);
//    map_->new_car_cells.writeToFile("new_car_cells.bmp");
//    map_->new_car_cells.writeToFile("new_car_cellsv2.bmp");
//    map_->new_car_data.writeToFile("new_car_data.bmp");
    std::swap(map_->car_data, map_->new_car_data);
    std::swap(map_->car_cells, map_->new_car_cells);
    std::swap(cars_, new_cars_);
    fmt::print("Spawning cars took {} ms\n", timer.restart());



    state_ = State::PAUSED;
}

PolygonI BuildCarPolygon(Coord pos, Coord size, Coord dir) {
    dir = dir.Norm();
    const auto perp = dir.RightPerpendicular();
    size = size / 2.;
    return PolygonI {{
//
//                             Coord(pos.x + 5, pos.y + 5).asPointI(),
//                             Coord(pos.x - 5, pos.y + 5).asPointI(),
//                             Coord(pos.x - 5, pos.y - 5).asPointI(),
//                             Coord(pos.x + 5, pos.y - 5).asPointI(),
                             Coord(pos.x + 5, pos.y + 5).asPointI(),

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

    res.pos = map_->car_data(pos.x(), pos.y());
    res.dir = map_->car_data(pos.x(), pos.y() + 1);
    res.speed = map_->car_data(pos.x() - 1, pos.y());
    res.size = map_->car_data(pos.x() + 1, pos.y());

    return res;
}


void Simulator::WriteCar(const Car& car) {
    PointI pos = car.pos.asPointI();

    map_->new_car_data(pos.x(), pos.y()) = car.pos; // pos in absoulute coords
    map_->new_car_data(pos.x(), pos.y() + 1) = car.dir; // dir normalized
    map_->new_car_data(pos.x() - 1, pos.y()) = car.speed; // speed in meters / second
    map_->new_car_data(pos.x() + 1, pos.y()) = car.size; // size in pixels (double)

    PolygonI poly = BuildCarPolygon(car.pos, car.size, car.dir);
    // TODO : fill convex
    map_->new_car_cells.fill(poly, CarCellType::BODY);
    map_->new_car_cells(pos.x(), pos.y()) = CarCellType::CENTER;
}

void Simulator::SpawnCars(int count) {
    static int const_random_seed = 42;
    std::mt19937 rng(const_random_seed++);
    std::uniform_int_distribution<int> random_distribution(0, std::numeric_limits<int>::max());
    int x_sz = map_->size.asPointI().x();
    int y_sz = map_->size.asPointI().y();


    for (int i = 0; i < count; i++) {
        bool car_spawned = false;
        for (int tries_count = 0; tries_count < 100000; tries_count++) {
            // TODO: use normal random
            int x = random_distribution(rng) % x_sz;
            int y = random_distribution(rng) % y_sz;
            int map_type = random_distribution(rng) % 3;
            Coord road_dir;
            if (map_type == 0) {
                road_dir = map_->road_dir(x, y);
            } else if (map_type == 1) {
                road_dir = map_->decision1(x, y);
            } else if (map_type == 2) {
                road_dir = map_->decision2(x, y);
            } else {
                VERIFY(false && "UNKNOWN ROAD DIR");
            }
            if (road_dir == Coord{0, 0}) {
                continue;
            }
            // TODO: add check
//            if (map_->new_car_cells)
            Car car;
            car.pos = {x, y};
            car.dir = road_dir.Norm();
            car.size = Coord{2, 4} * map_->pixels_per_meter;
            car.speed = car.dir * 3;

            WriteCar(car);
            new_cars_.push_back({x, y});
            car_spawned = true;
            break;
        }

        if (!car_spawned) {
            std::cerr << "Unable to spawn remaining " << count - i << " cars" << std::endl;
            break;
        }
    }
}

void Simulator::SimulateCars(double delta) {
    map_->new_car_cells.fill(CarCellType::NONE);
    map_->new_car_data.fill({0, 0});
    new_cars_.resize(0);
    for (const auto& car_pos : cars_) {
        if (map_->car_cells(car_pos.x(), car_pos.y()) != CarCellType::CENTER) {
            // car dead
            continue;
        }
        Car car = ReadCar(car_pos);
        auto new_pos = car.pos + car.speed * map_->pixels_per_meter * delta;
//        std::cerr << "car from " << car.pos << " to " << new_pos <<  " speed:" << car.speed <<" dir:" << car.dir << std::endl;
        car.pos = new_pos;

        WriteCar(car);
        new_cars_.emplace_back(car.pos.x, car.pos.y);
    }

    std::swap(map_->car_data, map_->new_car_data);
    std::swap(map_->car_cells, map_->new_car_cells);
    std::swap(cars_, new_cars_);
}

void Simulator::RunTick() {
    VERIFY(state_ == State::PAUSED)
    state_ = State::RUNNING;

    SimulateCars(0.1);

    state_ = State::PAUSED;
}

RasterMapPtr Simulator::GetMap() {
    return map_;
}
