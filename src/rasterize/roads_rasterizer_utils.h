#pragma once

#include "common/entities.h"


PolygonF ExpandPolyline(const PolylineF& polyline, double width);
PolygonF ExpandPoint(const PointF& point, double radius);