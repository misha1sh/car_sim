#pragma once

#include "common/entities.h"


#include <memory>

class Projector {
public:
    explicit Projector(Coord center);
    Coord project(const Coord& coord);

private:
    std::string proj_string_;
    void* C_;
    void* P_;
};

