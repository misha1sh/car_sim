#pragma once

#include <QPainter>
#include <QPaintEvent>
#include <QElapsedTimer>

#include "common/raster_map.h"
#include "ui/map/camera.h"

class MapPainter {
public:
    MapPainter();
    void setMap(RasterMapPtr map);
    void paintMap(QPainter& painter, QPaintEvent* event, Camera camera);

private:
    QElapsedTimer last_render_time_{};
    double avg_fps_{0};
    RasterMapPtr map_{nullptr};

    QBrush background;
    QBrush circleBrush;
    QFont textFont;
    QPen circlePen;
    QPen textPen;

    QImage img{};

};


typedef std::shared_ptr<MapPainter> MapPainterPtr;