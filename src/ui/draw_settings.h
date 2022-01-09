#pragma once

#include <memory>

struct DrawSettings {
    volatile bool draw_road_dir{true};
    volatile bool draw_decision1{true};
    volatile bool draw_decision2{true};
    volatile bool draw_car_type{true};
    volatile bool draw_prev_car_type{false};
    volatile bool draw_car_data{true};
    volatile bool draw_prev_car_data{false};
};

typedef std::shared_ptr<DrawSettings> DrawSettingsPtr;