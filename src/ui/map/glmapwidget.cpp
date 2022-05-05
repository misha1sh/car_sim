#include "glmapwidget.h"

#include <QPainter>

GLMapWidget::GLMapWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    map_painter_(),
    frame_update_timer(this) {
    setAutoFillBackground(false);
    connect(&frame_update_timer, &QTimer::timeout, this, &GLMapWidget::animate);
    frame_update_timer.start(1000 / 300);

//    this->installEventFilter(this);
}

void GLMapWidget::setPainter(MapPainterPtr map_painter) {
    map_painter_ = std::move(map_painter);
}

void GLMapWidget::setMapSize(PointF map_size) {
    camera_.camera0 = PointF(0., map_size.y - height());
    camera_.camera1 = PointF(width(), map_size.y);
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
    const PointF oldLeftBorder = camera_.camera0;//camera_.center - camera_.size / 2.;
    const PointF oldRightBorder = camera_.camera1; //camera_.center + camera_.size / 2.;


    const PointF mousePos{event->position().x(), event->position().y()};
    const PointF zoomCenterOnScreen = mousePos / PointF{size().width(), size().height()};
    const PointF zoomCenter = oldLeftBorder + (oldRightBorder - oldLeftBorder) * zoomCenterOnScreen;

    const double mouseDelta = event->angleDelta().y();
    const double zoomSpeed = 0.4;

//    const double additionalSpeedCoef = pow((oldRightBorder.x - oldLeftBorder.x) / map_size_.x, 1.5 );

    const double zoomDelta = zoomSpeed * mouseDelta;

    PointF leftCoef = (zoomCenter - oldLeftBorder) / (oldRightBorder - oldLeftBorder);
    PointF rightCoef = PointF{1, 1} - leftCoef;

    const double resCoef = width() * 1. / height();
    leftCoef.y /= resCoef;
    rightCoef.y /= resCoef;

    const PointF newLeftBorder = (oldLeftBorder + leftCoef * zoomDelta );
    const PointF newRightBorder = (oldRightBorder - rightCoef * zoomDelta );

    if (newRightBorder.x - newLeftBorder.x < 40. || newRightBorder.y - newLeftBorder.y < 40.) {
        return;
    }

    camera_.camera0 = newLeftBorder;
    camera_.camera1 = newRightBorder;

//    camera_.center = (newRightBorder + newLeftBorder) / 2.;
//    camera_.size = (newRightBorder - newLeftBorder);
//    const double sz = std::max(camera_.size.x, camera_.size.y);
//    camera_.size = {sz, sz};
//    camera_.cameraSize =
//            (camera_.cameraSize - delta * zoomSpeed).Crop(0.00001, 1);
}

void GLMapWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton) && mouse_dragging_) {
        const PointF cur_mouse_pos = {event->pos().x(), event->pos().y()};
        const PointF delta = cur_mouse_pos - dragging_mouse_start_pos_;

        const PointF deltaPercent = delta / PointF{size().width(), size().height()};
        const PointF deltaCoord = deltaPercent * (camera_.camera1 - camera_.camera0);

        camera_.camera0 = dragging_camera_start_pos0_ - deltaCoord;
        camera_.camera1 = dragging_camera_start_pos1_ - deltaCoord;
//        camera_.center = (dragging_camera_start_pos_ - deltaPercent * camera_.size).Crop(0, 1);
    } else {
        mouse_dragging_ = false;
    }
}

void GLMapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        mouse_dragging_ = true;
        dragging_mouse_start_pos_ = {event->pos().x(), event->pos().y()};
        dragging_camera_start_pos0_ = camera_.camera0;
        dragging_camera_start_pos1_ = camera_.camera1;
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
