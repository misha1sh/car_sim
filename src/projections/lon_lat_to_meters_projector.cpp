#include "lon_lat_to_meters_projector.h"

#include <fmt/core.h>
#include <proj.h>


LonLatToMetersProjector::LonLatToMetersProjector(PointF center) {
    C_ = proj_context_create(); // tmerc
    proj_string_ = fmt::format("+proj=ortho +lon_0={} +lat_0={} +units=m", center.x, center.y);
    P_ = proj_create_crs_to_crs (static_cast<PJ_CONTEXT*>(C_),
                                "EPSG:4326",
                                proj_string_.c_str(),
                                nullptr);
}

PointF LonLatToMetersProjector::project(const PointF &coord) const {
    const auto xy = proj_coord(coord.y, coord.x, 0, 0);
    const auto projection = proj_trans(static_cast<PJ*>(P_), PJ_FWD, xy);
    return {projection.xy.y, projection.xy.x};
}

LonLatToMetersProjector::~LonLatToMetersProjector() {
    proj_destroy(static_cast<PJ*>(P_));
    proj_context_destroy(static_cast<PJ_CONTEXT*>(C_));
}


