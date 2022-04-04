#include <QElapsedTimer>
#include "simulator.h"

#include "main/raster_map_builder.h"
#include "utils/verify.h"

#include <fmt/core.h>

Simulator::Simulator(const SimulatorParams& params):
        params_(params) {
}

void Simulator::Initialize() {
    VERIFY(state_ == State::NO_MAP);
    state_ = State::LOADING_MAP;

    QElapsedTimer timer;
    timer.start();

    fmt::print("Building map\n");

    RasterMapBuilder builder(params_.pixels_per_meter);
    builder.CreateRoadsMap(params_.map_path);
    map_holder_ = builder.GetMapHolder();
    fmt::print("Building took {} ms\n", timer.restart());

    if (params_.enable_cars) {
        const auto guard = map_holder_.Get(map_);
        const int cars_count = params_.cars_count;
        fmt::print("Spawning {} cars\n", cars_count);
        SpawnCars(cars_count);
//    map_->new_car_cells.writeToFile("new_car_cells.bmp");
//    map_->new_car_cells.writeToFile("new_car_cellsv2.bmp");
//    map_->new_car_data.writeToFile("new_car_data.bmp");
        std::swap(map_->car_data, map_->new_car_data);
        std::swap(map_->car_cells, map_->new_car_cells);
        std::swap(cars_, new_cars_);
        fmt::print("Spawning cars took {} ms\n", timer.restart());
    }

    state_ = State::PAUSED;
}

PolygonI BuildCarPolygon(PointF pos, PointF size, PointF dir) {
    dir = dir.Norm();
    const auto perp = dir.RightPerpendicular();
    size = size / 2.;
    return PolygonI {{
//
//                             PointF(pos.x + 5, pos.y + 5).asPointI(),
//                             PointF(pos.x - 5, pos.y + 5).asPointI(),
//                             PointF(pos.x - 5, pos.y - 5).asPointI(),
//                             PointF(pos.x + 5, pos.y - 5).asPointI(),
//                             PointF(pos.x + 5, pos.y + 5).asPointI(),

                             (pos + dir * size.y + perp * size.x).asPointI(),
                             (pos - dir * size.y + perp * size.x).asPointI(),
                             (pos - dir * size.y - perp * size.x).asPointI(),
                             (pos + dir * size.y - perp * size.x).asPointI(),
                             (pos + dir * size.y + perp * size.x).asPointI(),
                     }};
}

Car Simulator::ReadCar(const PointI& pos) {
    Car res;
    VERIFY(map_->car_cells(pos.x, pos.y) == CarCellType::CENTER);

    res.pos = map_->car_data(pos.x, pos.y);
    res.dir = map_->car_data(pos.x, pos.y + 1);
    res.size = map_->car_data(pos.x + 1, pos.y);
    res.speed = map_->car_data(pos.x - 1, pos.y).x;
    res.decision_layer = (int) map_->car_data(pos.x - 1, pos.y).y;

    return res;
}


void Simulator::WriteCar(const Car& car) {
    PointI pos = car.pos.asPointI();

    map_->new_car_data(pos.x, pos.y) = car.pos; // pos in absoulute coords
    map_->new_car_data(pos.x, pos.y + 1) = car.dir; // dir normalized
    map_->new_car_data(pos.x + 1, pos.y) = car.size; // size in pixels (double)
    map_->new_car_data(pos.x - 1, pos.y) = {car.speed, car.decision_layer}; // speed in meters / second

    PolygonI poly = BuildCarPolygon(car.pos, car.size, car.dir);
    // TODO : fill convex
    map_->new_car_cells.fill(poly, CarCellType::BODY);
    map_->new_car_cells(pos.x, pos.y) = CarCellType::CENTER;
}

void Simulator::SpawnCars(int count) {
    VERIFY(map_);
    static int const_random_seed = 42;
    std::mt19937 rng(const_random_seed++);
    std::uniform_int_distribution<int> random_distribution(0, std::numeric_limits<int>::max());
    int x_sz = map_->sizeI.x;
    int y_sz = map_->sizeI.y;


    for (int i = 0; i < count; i++) {
        bool car_spawned = false;
        for (int tries_count = 0; tries_count < 100000; tries_count++) {
            // TODO: use normal random
            int x = random_distribution(rng) % x_sz;
            int y = random_distribution(rng) % y_sz;
//            int map_type = random_distribution(rng) % 3;
            int id = map_->lane_id(x, y);
            if (id == 0) {
                continue;
            }
            PointF lane_dir = map_->lane_dir[id];

            // TODO: add check
//            if (map_->new_car_cells)
            Car car;
            car.pos = {x, y};
            car.dir = lane_dir.Norm();
            car.size = PointF{1.5, 4} * map_->pixels_per_meter;
            car.speed = 3;
            car.decision_layer = random_distribution(rng) % 2 + 1;

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
    auto guard = map_holder_.Get(map_);

    VERIFY(map_);
    map_->new_car_cells.fill(CarCellType::NONE);
    map_->new_car_data.fill({0, 0});
    new_cars_.resize(0);
    for (const auto& car_pos : cars_) {
        if (map_->car_cells(car_pos) != CarCellType::CENTER) {
            // car dead
            continue;
        }
        Car car = ReadCar(car_pos);
        const auto car_front = car.pos + car.dir * car.size.y;
        const auto perp = car.dir.RightPerpendicular();

//        const auto road_dir_at_front = map_->road_dir(car_front);
//        if (road_dir_at_front == PointF{0, 0}) {
//
//        }

        PointF new_pos = car.pos + car.dir * (car.speed * map_->pixels_per_meter * delta);
        PointF new_road_dir{0, 0};
        int try_number = 0;
        while (true) {
            auto lane_id = map_->lane_id(new_pos);
            if (lane_id == 0) {
                break;
            }
            new_road_dir = map_->lane_dir.at(lane_id);
            if (new_road_dir != PointF{0, 0}) {
                break;
            }

//            const PointF new_decision1 = map_->decision1(new_pos);
//            const PointF new_decision2 = map_->decision2(new_pos);
//            const bool no_decision1 = new_decision1 == PointF{0, 0};
//            const bool no_decision2 = new_decision2 == PointF{0, 0};
//            if (no_decision1 && no_decision2) {
//                if (try_number == 0) {
//                    new_pos = car.pos + perp + (car.dir) * (car.speed * map_->pixels_per_meter * delta);
//                } else if (try_number == 1) {
//                    new_pos = car.pos - perp + (car.dir) * (car.speed * map_->pixels_per_meter * delta);
//                } else {
//                    new_pos = car.pos;
//                    new_road_dir = car.dir;
//                    break;
//                }
//
//                try_number++;
//                continue;
//            } else if (no_decision2) {
//                car.decision_layer = 1;
//                new_road_dir = new_decision1;
//                break;
//            } else if (no_decision1) {
//                car.decision_layer = 2;
//                new_road_dir = new_decision2;
//                break;
//            } else if (car.decision_layer == 1) {
//                new_road_dir = new_decision1;
//                break;
//            } else if (car.decision_layer == 2) {
//                new_road_dir = new_decision2;
//                break;
//            } else {
//                VERIFY(false && "Unknown decision_layer");
//            }
        }

        car.pos = new_pos;
        car.dir = new_road_dir;

        WriteCar(car);
        new_cars_.emplace_back((int)car.pos.x, (int)car.pos.y);
    }

    std::swap(map_->car_data, map_->new_car_data);
    std::swap(map_->car_cells, map_->new_car_cells);
    std::swap(cars_, new_cars_);
}

void Simulator::RunTick() {
    VERIFY(state_ == State::PAUSED)
    state_ = State::RUNNING;

    if (params_.enable_cars) {
        SimulateCars(params_.delta_time_per_simulation);
    }

    state_ = State::PAUSED;
}

RasterMapHolder Simulator::GetMapHolder() {
    return map_holder_;
}
