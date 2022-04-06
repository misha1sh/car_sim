#pragma once

#include <memory>

struct DrawSettings {
    volatile bool draw_road_id{false};
    volatile bool draw_road_dir{true};
    volatile bool draw_car_type{true};
    volatile bool draw_prev_car_type{false};
    volatile bool draw_car_data{true};
    volatile bool draw_prev_car_data{false};
    volatile bool draw_debug{true};

    double resoultion_coef{3}; // 1 means 1:1, 2 means 2 map pixels -> 1 screen pixel for each dimension
};

typedef std::shared_ptr<DrawSettings> DrawSettingsPtr;