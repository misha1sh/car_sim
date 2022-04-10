#include <QElapsedTimer>
#include "simulator.h"

#include "common/geometry.h"
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


void Simulator::WriteCar(const Car& car) {
//    PointI pos = car.pos.asPointI();

//    map_->new_car_data(pos.x, pos.y) = car.pos; // pos in absoulute coords
//    map_->new_car_data(pos.x, pos.y + 1) = car.dir; // dir normalized
//    map_->new_car_data(pos.x + 1, pos.y) = car.size; // size in pixels (double)
//    map_->new_car_data(pos.x - 1, pos.y) = {car.speed, car.decision_layer}; // speed in meters / second

    PolygonI poly = BuildCarPolygon(car.pos, car.size, car.dir);
    // TODO : fill convex
    map_->new_car_cells.fill(poly, car.id);
//    map_->new_car_cells(pos.x, pos.y) = CarCellType::CENTER;
}

bool Simulator::TrySpawnCar(int x, int y) {
    int id = map_->lane_id(x, y);
    if (id == 0) {
        return false;
    }
    PointF lane_dir = map_->lane_dir.at(id);

    Car car;
    car.pos = {x, y};
    car.dir = lane_dir.Norm();
    car.size = (PointF{0.3, 1.} * random_double_(rng_) + PointF{1.5, 4}) * map_->pixels_per_meter;
    car.speed = 3;
    car.decision_layer = random_int_(rng_) % 2 + 1;
    car.id = car_id_counter_++;

    WriteCar(car);
    car_actions_.insert({car.id, actions::GoStraight{}});
    new_cars_.push_back(car);

    return true;
}

void Simulator::SpawnCars(int count) {
    VERIFY(map_);
    int x_sz = map_->sizeI.x;
    int y_sz = map_->sizeI.y;


    for (int i = 0; i < count; i++) {
        bool car_spawned = false;
        for (int tries_count = 0; tries_count < 100000; tries_count++) {
            int x = random_int_(rng_) % x_sz;
            int y = random_int_(rng_) % y_sz;
            if (TrySpawnCar(x, y)) {
                car_spawned = true;
                break;
            }
        }

        if (!car_spawned) {
            std::cerr << "Unable to spawn remaining " << count - i << " cars" << std::endl;
            break;
        }
    }
}

void Simulator::Clear() {
    //    map_->new_car_cells.fill(CarCellType::NONE);
//    map_->new_car_data.fill({0, 0});
    /// CLEAR
    new_cars_.resize(0);
    for (const auto& car : last_cars_) {
        PolygonI poly = BuildCarPolygon(car.pos, car.size, car.dir);
        // TODO : fill convex
        map_->new_car_cells.fill(poly, 0);
//        PointI left_bottom = (car.pos - PointF{4, 4} * map_->pixels_per_meter).asPointI();
//        PointI right_top = (car.pos + PointF{4, 4} * map_->pixels_per_meter).asPointI();
//        map_->new_car_cells.fillBox(left_bottom, right_top, 0);
    }
    last_cars_ = cars_;
}

std::optional<CarAction> Simulator::SimulateGoCrossroad(double delta, Car& car, actions::GoOnCrossroad action) {
    double dist1 = car.speed * map_->pixels_per_meter * delta;
    double dist2 = 0;
    double step = 0.001;

    int steps_count = 0;
    double t;
    PointF old_pos = car.pos;
    PointF new_pos = car.pos;
    for (t = action.progress; t < 1.; t = std::min(1., t + step)) {
        steps_count++;

        new_pos = geometry::BeizerCurve(t, action.start_point, action.middle_point_1,
                                       action.middle_point_2, action.end_point);
        new_pos = map_->image_projector.projectF(new_pos);

        dist2 += geometry::Distance(old_pos, new_pos);
        if (dist2 >= dist1) {
            break;
        }

        old_pos = new_pos;
    }
    car.dir = (new_pos - old_pos).Norm();
    car.pos = new_pos;
    action.progress = t;
    map_->debug(car.pos) = 0xff0000;

    if (t == 1) {
        const auto car_front = car.pos + car.dir * car.size.y / 2.;
        const auto lane_id_at_front_center = map_->lane_id(car_front);
        if (!lane_id_at_front_center) {
            return std::nullopt;
        }
        car.dir = map_->lane_dir.at(lane_id_at_front_center);

        return actions::GoStraight{};
    }

//    int steps = 10;
//    for (int i = 0; i < steps; i++) {
//        const auto old_pos = car.pos;
//
//        PointF new_pos = car.pos + car.dir * (car.speed * map_->pixels_per_meter * delta / steps);
//        geometry::Distance(new_pos, car.pos);
//
//        const auto [new_progress, new_pos_projected] =
//        geometry::FindBeizerProjection(action.progress, 1.,
//                                       map_->image_projector.projectBackwards(new_pos),
//                                       action.start_point, action.middle_point_1,
//                                       action.middle_point_2, action.end_point);
//
//        car.pos = map_->image_projector.projectF(new_pos_projected);
//        car.dir = (car.pos - old_pos).Norm();
//        action.progress = new_progress;
//        dist2 += geometry::Distance(car.pos, old_pos);
//
//
//        if (new_progress > 0.99) {
//            return actions::GoStraight{};
//        }
//
//        if (dist2 > dist1) {
//            break;
//        }
//    }


//    std::cerr << dist1 << " " << dist2 << "\n";


    return action;
}

std::optional<CarAction> Simulator::SimulateGoStraight(double delta, Car& car, actions::GoStraight action) {
    const auto car_front = car.pos + car.dir * car.size.y / 2.;
    const auto perp = car.dir.RightPerpendicular();

    const auto lane_id_at_front_center = map_->lane_id(car_front);
    const auto lane_id_at_front_right = map_->lane_id(car_front + perp * car.size.x / 2.);
    const auto lane_id_at_front_left = map_->lane_id(car_front - perp * car.size.x / 2.);

    if (!lane_id_at_front_center || (!lane_id_at_front_left && !lane_id_at_front_right)) {
        died_out_of_road_++;
        return std::nullopt;
    }


    PointF new_pos = car.pos + car.dir * (car.speed * map_->pixels_per_meter * delta);
//    if(!map_->lane_dir.contains(lane_id_at_front_center)) {
//        std::cerr << lane_id_at_front_center << std::endl;
//    }
    PointF new_car_dir = map_->lane_dir.at(lane_id_at_front_center);


    // FEATURE: Slowdown near crossroad
    if (map_->crossroad_lanes.contains(lane_id_at_front_center)) {
        const auto start_lane = map_->crossroad_lanes.at(lane_id_at_front_center);
        const double proj = geometry::OrientedProjectionLength(start_lane.end_point, start_lane.start_point,
                                                               map_->image_projector.projectBackwards(car_front));
        if (start_lane.goes_into_crossroad &&
            proj < 1. &&
            !start_lane.end_lanes.empty()) {

            PointF start_point, end_point, middle_point_1, middle_point_2;
            bool is_bad_proj = true;
            for (int tries_count = 0; tries_count < 10 && is_bad_proj; tries_count++) {
                is_bad_proj = false;

                const auto end_lane = start_lane.end_lanes.at(
                        random_int_(rng_) % start_lane.end_lanes.size()
                );
                start_point = map_->image_projector.projectBackwards(new_pos);
                const auto start_dir = (start_lane.end_point - start_lane.start_point).Norm();

                const auto end_dir = (end_lane.end_point - end_lane.start_point).Norm();
                end_point = end_lane.start_point + end_dir * 3.;

                const double curvature_coef = 0.66;

                auto proj1 = geometry::OrientedProjectionLength(start_point, start_point + start_dir,
                                                                end_point);
                if (proj1 < 1.) {
                    proj1 = 1;
                    is_bad_proj = true;
                }
                middle_point_1  = start_point + start_dir * (proj1 * curvature_coef);

                auto proj2 = geometry::OrientedProjectionLength(end_point, end_point - end_dir,
                                                                start_point);
                if (proj2 < 1.) {
                    proj2 = 1.;
                    is_bad_proj = false;
                }
                middle_point_2 = end_point - end_dir * (proj2 * curvature_coef);
            }


// DRAW beizer curve
//            for (double t = 0; t < 1; t += 0.001) {
//                PointF b = geometry::BeizerCurve(t, start_point, middle_point_1, middle_point_2, end_point);
//                auto pr = map_->image_projector.project(b).asPointF();
//                map_->debug.fillBox((pr - PointF{2, 2}).asPointI(),
//                                    (pr + PointF{2, 2}).asPointI(), 0xff00ff);
//            }

            return actions::GoOnCrossroad{
               .start_point= start_point,
               .middle_point_1= middle_point_1,
               .middle_point_2= middle_point_2,
               .end_point= end_point,
               .progress=0.
            } ;
        }
    }




    // FEATURE : Steer left when we out right side out of road
    if (lane_id_at_front_left && lane_id_at_front_left == lane_id_at_front_center &&
        lane_id_at_front_right != lane_id_at_front_center ) {
        const auto ideal_dir = car.dir.Rotate(M_PI / 20);
        const auto [new_dir, rotated] = car.dir.RotateTowards(ideal_dir, M_PI / 10 * delta);
        new_car_dir = new_dir;
//        new_car_dir = ideal_dir;
    }

    if (lane_id_at_front_right && lane_id_at_front_right == lane_id_at_front_center &&
        lane_id_at_front_right != lane_id_at_front_center ) {
        const auto ideal_dir = car.dir.Rotate(M_PI / 20);
        const auto [new_dir, rotated] = car.dir.RotateTowards(ideal_dir, M_PI / 10 * delta);
        new_car_dir = new_dir;
//        new_car_dir = ideal_dir;
    }

    map_->debug(new_pos) = 0x00ff00;
    car.pos = new_pos;
    car.dir = new_car_dir;

    return action;
}

std::optional<CarAction> Simulator::SimulateCar(double delta, Car& car) {
    const auto* action = &car_actions_.at(car.id);

    if (const auto* go_straight = std::get_if<actions::GoStraight>(action)) {
        return SimulateGoStraight(delta, car, *go_straight);
    }

    if (const auto* go_on_crossroad = std::get_if<actions::GoOnCrossroad>(action)) {
        return SimulateGoCrossroad(delta, car, *go_on_crossroad);
    }
    VERIFY(false && "Unknown action type");

    return std::nullopt;
}

void Simulator::SimulateCars(double delta) {
    auto guard = map_holder_.Get(map_);

    VERIFY(map_);
    Clear();


    /// MAIN LOOP
    int died_in_collision = 0;
    int died_out_of_road = 0;
    for (Car car : cars_) {
        if (map_->car_cells(car.pos) != car.id) {
            died_in_collision++;
            continue;
        }
        if (car.pos.x < 10  * map_->pixels_per_meter || car.pos.y < 10  * map_->pixels_per_meter) {
            died_out_of_road++;
            continue;
        }
        if (car.pos.x + 10  * map_->pixels_per_meter > map_->size.x || car.pos.y + 10  * map_->pixels_per_meter > map_->size.y) {
            died_out_of_road++;
            continue;
        }

        const auto old_pos = car.pos;
        const auto old_dir = car.dir;

        const auto action_opt = SimulateCar(delta, car);
        if (!action_opt) {
            continue;
        }

        if (HasCollisionAt(car, car.pos, car.dir, 2., true) /*||
                HasCollisionAt(car, car.pos, car.dir, 2., false)*/) {
            car.pos = old_pos;
            car.dir = old_dir;
        } else {
            car_actions_.at(car.id) = *action_opt;
        }

        if (HasCollisionAt(car, car.pos, car.dir, 0., true)) {
            died_in_collision++;
            continue;
        }

        WriteCar(car);
        new_cars_.emplace_back(car);
    }

    int spawned_count = 0;
    if (new_cars_.size() < params_.cars_count) {
        int cars_to_spawn = params_.cars_count - new_cars_.size();
        SpawnCars(cars_to_spawn);
        // TODO: count real amount of spawned
        spawned_count = cars_to_spawn;
    }

    std::swap(map_->car_cells, map_->new_car_cells);
    std::swap(cars_, new_cars_);


    if (died_in_collision != 0 || died_out_of_road != 0 || spawned_count != 0) {
        std::cout<< "cars : " << "collisions=" << died_in_collision << " out_of_road="  << died_out_of_road << " spawned=" << spawned_count << "\n";
    }
}


bool Simulator::HasCollisionAt(const Car& car, const PointF& pos, const PointF& dir, const double forward_margin, bool with_new) {
    const auto perp = dir.RightPerpendicular();
    auto size = car.size / 2.;


    auto& m = with_new? map_->new_car_cells : map_->car_cells;
    auto check = [&](const PointF& p) -> bool {
        const int val = m(p.asPointI());
        return val != 0 && val != car.id;
    };

    if (check(pos + dir * (size.y + forward_margin) + perp * size.x)) {
        return true;
    }
    if (check(pos + dir * (size.y + forward_margin) - perp * size.x)) {
        return true;
    }
    if (check(pos - dir * size.y + perp * size.x)) {
        return true;
    }
    if (check(pos - dir * size.y - perp * size.x)) {
        return true;
    }
    return false;
}


void Simulator::RunTick() {
    VERIFY(state_ == State::PAUSED)
    state_ = State::RUNNING;

//    for (const auto& [traffic_lights_id, traffic_lights] : map_->traffic_lights) {
//        if (traffic_lights.lanes.empty()) {
//            continue;
//        }
//
//        if (!traffic_lights_state_.contains(traffic_lights_id)) {
//            traffic_lights_state_.insert({traffic_lights_id, TrafficLightsState{
//                .state = 0,
//                .traffic_lights_id = 5
//            }});
//        }
//        const auto& state = traffic_lights_state_.at(traffic_lights_id);
//        state.time_to_next_state -= params_.delta_time_per_simulation;
//
//        if (state.time_to_next_state < 0) {
//            state.time_to_next_state = 5.;
////            state.lanes
//        }
//    }

    if (params_.enable_cars) {
        SimulateCars(params_.delta_time_per_simulation);
    }

    state_ = State::PAUSED;
}

RasterMapHolder Simulator::GetMapHolder() {
    return map_holder_;
}
