#pragma once

#include "common/entities.h"

#include <memory>

class LonLatToMetersProjector final {
public:
    explicit LonLatToMetersProjector(Coord center);
    Coord project(const Coord& coord) const;
    ~LonLatToMetersProjector();

private:
    std::string proj_string_;
    mutable void* C_;
    mutable void* P_;
};

