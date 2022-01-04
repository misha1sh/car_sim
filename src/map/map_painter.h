#pragma once

#include <QPainter>
#include <QPaintEvent>

class MapPainter {
public:

    MapPainter();
    void paintMap(QPainter& painter, QPaintEvent* event);

private:
    QBrush background;
    QBrush circleBrush;
    QFont textFont;
    QPen circlePen;
    QPen textPen;
};
