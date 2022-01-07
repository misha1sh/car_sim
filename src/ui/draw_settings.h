#pragma once

#include <memory>

struct DrawSettings {
    volatile bool draw_road_dir{true};
    volatile bool draw_decision1{true};
    volatile bool draw_decision2{true};
};

typedef std::shared_ptr<DrawSettings> DrawSettingsPtr;