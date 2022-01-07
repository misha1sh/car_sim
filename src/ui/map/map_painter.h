#pragma once


#include "common/raster_map.h"
#include "ui/draw_settings.h"
#include "ui/map/camera.h"

#include <QPainter>
#include <QPaintEvent>
#include <QElapsedTimer>


class MapPainter {
public:
    MapPainter();
    void setMap(RasterMapPtr map);
    void setDrawSettings(DrawSettingsPtr draw_settings);
    void paintMap(QPainter& painter, QPaintEvent* event, Camera camera);

private:
    QElapsedTimer last_render_time_{};
    double avg_fps_{0};
    RasterMapPtr map_{nullptr};
    DrawSettingsPtr draw_settings_{};

    QBrush background;
    QBrush circleBrush;
    QFont textFont;
    QPen circlePen;
    QPen textPen;

    QImage img{};
    QImage directionsCircleImg{};

};


typedef std::shared_ptr<MapPainter> MapPainterPtr;