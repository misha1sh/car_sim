#pragma once

#include <QtCore>
#include "common/entities.h"
#include "common/raster_map.h"

struct Car {
    PointF pos;
    PointF dir;
    PointF size;
    double speed;
    int decision_layer;
    int id;
};

namespace actions {

struct GoStraight {
    int crossroad_lane_id{0};
};

struct GoOnCrossroad {
    PointF start_point;
    PointF middle_point_1;
    PointF middle_point_2;
    PointF end_point;
    double progress;
};

}  // namespace actions

typedef std::variant<actions::GoStraight, actions::GoOnCrossroad> CarAction;


struct TrafficLightsState {
    int state;
    double time_to_next_state;
};

struct SimulatorParams {
    double pixels_per_meter = 5;
    std::string map_path = "../data/moscow_easy.osm";  // moscow_easy moscow_hard

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

    void WriteCar(const Car& car);
    void SpawnCars(int count);
    bool TrySpawnCar(int x, int y);

    bool HasCollisionAt(const Car& car, const PointF& pos, const PointF& dir, const double forward_margin, bool with_new);

    void RunTick();
    void SimulateCars(double delta);
    std::optional<CarAction> SimulateGoCrossroad(double delta, Car& car, actions::GoOnCrossroad action);
    std::optional<CarAction> SimulateGoStraight(double delta, Car& car, actions::GoStraight action);
    std::optional<CarAction> SimulateCar(double delta, Car& car);
    void Clear();

    RasterMapHolder GetMapHolder();

    inline const State& GetState() const {
        return state_;
    }

public slots:
    void RunTickSlot() {
        RunTick();
    };

private:
    SimulatorParams params_{};
    State state_{State::NO_MAP};
    RasterMapHolder map_holder_{};
    RasterMapPtr map_{};

    std::vector<Car> last_cars_{};
    std::vector<Car> cars_{};
    std::vector<Car> new_cars_{};

    std::map<int, TrafficLightsState> traffic_lights_state_;

    std::map<int, CarAction> car_actions_{};
    std::map<int, CarAction> new_car_actions_{};


    int car_id_counter_ = 1;
    int died_in_collision_ = 0;
    int died_out_of_road_ = 0;

    std::mt19937 rng_{42};
    std::uniform_int_distribution<int> random_int_{0, std::numeric_limits<int>::max()};
    std::uniform_real_distribution<> random_double_{0, 1};

};

