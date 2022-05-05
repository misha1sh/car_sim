#pragma once

#include <QtCore>
#include "common/entities.h"
#include "common/raster_map.h"
#include "utils/automatic_counter.h"

struct Car {
    PointF pos;
    PointF dir;
    PointF size;
    double speed;
    int id;
    int age;
};

namespace actions {

struct GoStraight {
};

struct GoOnCrossroad {
    int start_lane_id;
    PointF start_point;
    PointF middle_point_1;
    PointF middle_point_2;
    PointF end_point;
    double progress;
    std::optional<CounterPointer> counter_pointer;
};

}  // namespace actions

typedef std::variant<actions::GoStraight, actions::GoOnCrossroad> CarAction;


struct TrafficLightsState {
    enum Command {
        Go,
        ClearCrossroad
    };

    Command command;
    int state;
    AutomaticCounter cars_counter;
    double time_to_next_state;
};

struct SimulatorParams {
    double pixels_per_meter = 5;
    std::string map_path = "../data/map (2).osm";  // moscow_easy moscow_hard

    bool enable_cars = true;
    int cars_count = 1000;

    double delta_time_per_simulation = 0.1;
};

class Simulator : public QObject {
Q_OBJECT
public:
    enum class State {
        NO_MAP,
        LOADING_MAP,
        MAP_LOADED,
        RUNNING,
        PAUSED
    };

    explicit Simulator(const SimulatorParams& params);

    void Initialize();
    void RunTick();
    RasterMapHolder GetMapHolder();

private:
    void WriteCar(const Car& car);
    void SpawnCars(int count);
    bool TrySpawnCar(int x, int y);

    bool HasCollisionAt(const Car& car, const PointF& pos, const PointF& dir, double forward_margin,
                        bool count_from_backward, bool with_new, bool ignore_colinear);

    void UpdateTrafficLights(double  delta);
    void SimulateCars(double delta);
    std::optional<CarAction> SimulateGoCrossroad(double delta, Car& car, actions::GoOnCrossroad action);
    std::optional<CarAction> SimulateGoStraight(double delta, Car& car, actions::GoStraight action);
    std::optional<CarAction> SimulateCar(double delta, Car& car);
    void Clear();

public slots:
    void RunTickSlot() {
        RunTick();
    };

private:
    SimulatorParams params_{};
    State state_{State::NO_MAP};
    RasterMapHolder map_holder_{};
    RasterMapPtr map_{};

    std::unordered_map<int, Car> last_cars_{};
    std::unordered_map<int, Car> cars_{};
    std::unordered_map<int, Car> new_cars_{};

    std::map<int, TrafficLightsState> traffic_lights_state_;

    std::map<int, CarAction> car_actions_{};


    int car_id_counter_ = 1;
    int died_in_collision_ = 0;
    int died_out_of_road_ = 0;
    int died_out_of_road2_ = 0;

    std::mt19937 rng_{42};
    std::uniform_int_distribution<int> random_int_{0, std::numeric_limits<int>::max()};
    std::uniform_real_distribution<> random_double_{0, 1};

};

