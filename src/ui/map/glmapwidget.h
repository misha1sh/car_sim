#pragma once

#include "map_painter.h"

#include <QOpenGLWidget>
#include <QTimer>

class GLMapWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit GLMapWidget(QWidget *parent);

public slots:
    void animate();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    MapPainter map_painter;
    QTimer frame_update_timer;
};