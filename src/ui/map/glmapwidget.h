#pragma once

#include "map_painter.h"

#include <QOpenGLWidget>
#include <QTimer>


class GLMapWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit GLMapWidget(QWidget *parent);
    void setPainter(MapPainterPtr map_painter);
    void setMapSize(PointF size);

public slots:
    void animate();

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
private:
    bool mouse_dragging_{false};
    PointF dragging_mouse_start_pos_{0, 0};
    PointF dragging_camera_start_pos0_{0, 0};
    PointF dragging_camera_start_pos1_{0, 0};
    MapPainterPtr map_painter_;
    QTimer frame_update_timer;
    Camera camera_{};
    PointF map_size_{};

};