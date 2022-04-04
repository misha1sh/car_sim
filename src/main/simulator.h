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
};

struct SimulatorParams {
    double pixels_per_meter = 1;
    std::string map_path = "../data/moscow_hard.osm";  // moscow_easy moscow_hard

    bool enable_cars = true;
    int cars_count = 100;

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

    Car ReadCar(const PointI& pos);
    void WriteCar(const Car& car);
    void SpawnCars(int count);

    void RunTick();
    void SimulateCars(double delta);

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

    std::vector<PointI> cars_;
    std::vector<PointI> new_cars_;
};

