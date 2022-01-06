#include "glmapwidget.h"

#include <QPainter>

GLMapWidget::GLMapWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    map_painter_(),
    frame_update_timer(this) {
    setAutoFillBackground(false);
    connect(&frame_update_timer, &QTimer::timeout, this, &GLMapWidget::animate);
    frame_update_timer.start(1000 / 30);

//    this->installEventFilter(this);
}

void GLMapWidget::setPainter(MapPainterPtr map_painter) {
    map_painter_ = map_painter;
}

void GLMapWidget::animate() {
    update();
}

void GLMapWidget::paintEvent(QPaintEvent *event) {
    if (!map_painter_) {
        return;
    }
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    map_painter_->paintMap(painter, event, camera_);
    painter.end();
}

void GLMapWidget::wheelEvent(QWheelEvent *event) {
    if (mouse_dragging_) {
        return;
    }
    const Coord oldLeftBorder = camera_.center - camera_.size / 2.;
    const Coord oldRightBorder = camera_.center + camera_.size / 2.;


    const Coord mousePos{event->position().x, event->position().y()};
    const Coord zoomCenterOnScreen = mousePos / Coord{size().width(), size().height()};
    const Coord zoomCenter = oldLeftBorder + (oldRightBorder - oldLeftBorder) * zoomCenterOnScreen;

    const double mouseDelta = event->angleDelta().y();
    const double zoomSpeed = 0.0004;

    const double zoomDelta = zoomSpeed * mouseDelta;

    const Coord leftCoef = (zoomCenter - oldLeftBorder) / (oldRightBorder - oldLeftBorder);
    const Coord rightCoef = Coord{1, 1} - leftCoef;

    const Coord newLeftBorder = (oldLeftBorder + leftCoef * zoomDelta).Crop(-0.5, 1.5);
    const Coord newRightBorder = (oldRightBorder - rightCoef * zoomDelta).Crop(-0.5, 1.5);

    if (newRightBorder.x - newLeftBorder.x < 0.001 || newRightBorder.y() - newLeftBorder.y() < 0.001) {
        return;
    }

    camera_.center = (newRightBorder + newLeftBorder) / 2.;
    camera_.size = (newRightBorder - newLeftBorder);
    const double sz = std::max(camera_.size.x, camera_.size.y());
    camera_.size = {sz, sz};
//    camera_.cameraSize =
//            (camera_.cameraSize - delta * zoomSpeed).Crop(0.00001, 1);
}

void GLMapWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton) && mouse_dragging_) {
        const Coord cur_mouse_pos = {event->pos().x, event->pos().y()};
        const Coord delta = cur_mouse_pos - dragging_mouse_start_pos_;

        const Coord deltaPercent = delta / Coord{size().width(), size().height()};
        camera_.center = (dragging_camera_start_pos_ - deltaPercent * camera_.size).Crop(0, 1);
    } else {
        mouse_dragging_ = false;
    }
}

void GLMapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        mouse_dragging_ = true;
        dragging_mouse_start_pos_ = {event->pos().x, event->pos().y()};
        dragging_camera_start_pos_ = camera_.center;
    }
}

void GLMapWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        mouse_dragging_ = false;
    }
}

void GLMapWidget::leaveEvent(QEvent* event) {
    mouse_dragging_ = false;
}

//bool GLMapWidget::moveEvent(QObject* object, QEvent* event)
//{
//    if(event->type() == QEvent::MouseMove)
//    {
//        mouseMoveFunction(static_cast<QMouseEvent*>(event));
//    }
//    return false;
//}
