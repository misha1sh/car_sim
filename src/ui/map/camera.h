#pragma once

#include "common/entities.h"

struct Camera {
    // [0, 1]
    Coord center{0.5, 0.5};
    // (0, 1]
    Coord size{1, 1};
};
