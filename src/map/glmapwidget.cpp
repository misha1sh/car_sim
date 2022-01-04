#include "glmapwidget.h"

#include <QPainter>

GLMapWidget::GLMapWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    map_painter(),
    frame_update_timer(this) {
    setAutoFillBackground(false);
    connect(&frame_update_timer, &QTimer::timeout, this, &GLMapWidget::animate);
    frame_update_timer.start(1000 / 60);
}

void GLMapWidget::animate() {
    update();
}

void GLMapWidget::paintEvent(QPaintEvent *event) {
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    map_painter.paintMap(painter, event);
    painter.end();
}
