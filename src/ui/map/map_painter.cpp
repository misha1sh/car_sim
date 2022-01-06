#include "map_painter.h"
#include <fmt/core.h>


MapPainter::MapPainter() {
    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));

    background = QBrush(QColor(10, 10, 10));
    circleBrush = QBrush(gradient);
    circlePen = QPen(Qt::black);
    circlePen.setWidth(1);
    textPen = QPen(Qt::white);
    textFont.setPixelSize(14);
}

void MapPainter::setMap(RasterMapPtr map) {
    map_ = std::move(map);
}

//struct RGBColor {
//    int r, g, n;
//};
//
//template <typename T1, typename T2, typename T3>
//inline RGBColor AsRGBColor(const T1& r, const T2& g, const T3& b) {
//    return {
//        static_cast<int>(r),
//        static_cast<int>(g),
//        static_cast<int>(b)
//    };
//}
//

template <typename T1, typename T2, typename T3>
inline void setPixels(uchar* pix, int& pixIdx, const T1& r, const T2& g, const T3& b) {
    pix[pixIdx] = static_cast<uchar>(b);
    pix[pixIdx + 1] = static_cast<uchar>(g);
    pix[pixIdx + 2] = static_cast<uchar>(r);
    pixIdx += 4;
}

void MapPainter::paintMap(QPainter &painter, QPaintEvent* event, Camera camera) {
    int elapsedSinceLastRender = last_render_time_.restart();
    double currentFps = 0;
    if (elapsedSinceLastRender != 0) {
        currentFps = 1000. / elapsedSinceLastRender;
    }
    avg_fps_ = currentFps * 0.3 + avg_fps_ * 0.7;

//    painter.fillRect(event->rect(), background);
    if (!map_) {
        return;
    }

    // everything in coordinate system relative to screen
    PointI screenSize {event->rect().width(), event->rect().height()};
    Coord mapSize {map_->road_dir.sizeX(),  map_->road_dir.sizeY()};
    Coord onScreenMapSize {camera.size * mapSize};
    Coord cameraCenter {camera.center * mapSize};
    Coord cameraCorner {cameraCenter - onScreenMapSize / 2};

    PointI cameraCornerI {cameraCorner.asI()};

    Coord rescaleCoef {onScreenMapSize / Coord{screenSize}};

    if (img.width() != screenSize.x || img.height() != screenSize.y()) {
        img = QImage{screenSize.x, screenSize.y(),  QImage::Format::Format_RGB32};
    }

    int pixIdx = 0;
    uchar* pix = img.bits();
    for (int y = 0; y < screenSize.y(); y++) {
        for (int x = 0; x < screenSize.x; x++) {
            const PointI screenCoord{x, y};
//            const Coord imageCoord = screenCoord * rescaleCoef + cameraCorner;
//
//            const int imageX = static_cast<int>(imageCoord.x);
//            const int imageY = static_cast<int>(imageCoord.y());


            const int imageX = static_cast<int>(x * rescaleCoef.x) + cameraCornerI.x;
            const int imageY = static_cast<int>(y * rescaleCoef.y()) + cameraCornerI.y();

            const auto road_dir = map_->road_dir.getOrDefault(imageX, imageY, {0, 0});
            if (road_dir != Coord{0, 0}) {
                setPixels(pix, pixIdx, road_dir.x * 127 + 127, road_dir.y() * 127 + 127, 0);
                continue;
            }
//pixIdx % 100 < 0 ? 100 : 200
            setPixels(pix, pixIdx, 0, 0, 0);
//            pixData[x] =  qRgb(100, 0, 0);
        }
    }

    painter.drawImage(event->rect(), img);

    painter.setPen(textPen);
    painter.setFont(textFont);
    painter.drawText(10, 10,  QString::fromStdString(fmt::format("FPS: {:.2f}", avg_fps_)));
    //    painter.translate(100, 100);

//    static int elapsed = 4;
//    elapsed += 1000 / 60;
//    elapsed %= 10000;
//    painter.save();
//    painter.setBrush(circleBrush);
//    painter.setPen(circlePen);
//    painter.rotate(elapsed * 0.030);
//
//    qreal r = elapsed / 1000.0;
//    int n = 30;
//    for (int i = 0; i < n; ++i) {
//        painter.rotate(30);
//        qreal factor = (i + r) / n;
//        qreal radius = 0 + 120.0 * factor;
//        qreal circleRadius = 1 + factor * 20;
//        painter.drawEllipse(QRectF(radius, -circleRadius,
//                                    circleRadius * 2, circleRadius * 2));
//    }
//    painter.restore();
//
//    painter.setPen(textPen);
//    painter.setFont(textFont);
//    painter.drawText(QRect(-50, -50, 100, 100), Qt::AlignCenter, QStringLiteral("Qt"));

}
