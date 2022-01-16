#include "map_painter.h"

#include "utils/verify.h"

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
    textFont.setPixelSize(12);
}

void MapPainter::setMap(RasterMapPtr map) {
    map_ = std::move(map);
}

void MapPainter::setDrawSettings(DrawSettingsPtr draw_settings) {
    draw_settings_ = std::move(draw_settings);
}

struct Color {
    uchar r, g, b;
};

template <typename T1, typename T2, typename T3>
inline Color RGB(const T1& r, const T2& g, const T3& b) {
    return Color {
            static_cast<uchar>(r),
            static_cast<uchar>(g),
            static_cast<uchar>(b)
    };
}

inline void setPixels(uchar* pix, int& pixIdx, const Color& color) {
    pix[pixIdx] = color.b;
    pix[pixIdx + 1] = color.g;
    pix[pixIdx + 2] = color.r;
    pixIdx += 4;
}

inline Color DirToRGB(const Coord& dir) {
    return RGB(dir.x * 127 + 127, dir.y * 127 + 127, 0);
}

inline Color CarTypeToRGB(const CarCellType& car_type) {
    if (car_type == CarCellType::CENTER) {
        return RGB(0, 0, 255);
    }
    if (car_type == CarCellType::BODY) {
        return RGB(0, 0, 128);
    }
    VERIFY(false && "Unknown car cell type");
}



inline bool DrawDir(RasterDataPoint& data, int imageX, int imageY, int& pixIdx, uchar* pix) {
    const auto dir = data.getOrDefault(imageX, imageY, {0, 0});
    if (dir != Coord{0, 0}) {
        setPixels(pix, pixIdx, DirToRGB(dir));
        return true;
    }

    return false;
}

inline bool DrawCarType(RasterDataEnum<CarCellType>& data, int imageX, int imageY, int& pixIdx, uchar* pix) {
    const auto car_type = data.getOrDefault(imageX, imageY, CarCellType::NONE);
    if (car_type != CarCellType::NONE) {
        setPixels(pix, pixIdx, CarTypeToRGB(car_type));
        return true;
    }

    return false;
}



void MapPainter::paintMap(QPainter &painter, QPaintEvent* event, Camera camera) {
    int elapsedSinceLastRender = last_render_time_.restart();
    double currentFps = 0;
    if (elapsedSinceLastRender != 0) {
        currentFps = 1000. / elapsedSinceLastRender;
    }
    avg_fps_ = currentFps * 0.3 + avg_fps_ * 0.7;

//    painter.fillRect(event->rect(), background);
    if (!map_ || !draw_settings_) {
        painter.setPen(textPen);
        painter.setFont(textFont);
        painter.drawText(10, 10,  QString::fromStdString(fmt::format("No map or draw settings")));
        return;
    }

    // everything in coordinate system relative to screen
    PointI screenSize {event->rect().width(), event->rect().height()};
    Coord mapSize {map_->road_dir.sizeX(),  map_->road_dir.sizeY()};

//    mapSize.y = screenSize.y() * 1. * mapSize.x  / screenSize.x();
//    mapSize.x = screenSize.x() * 1. * mapSize.y  / screenSize.y();

//    Coord onScreenMapSize {camera.size * mapSize};
    Coord onScreenMapSize {camera.camera1 - camera.camera0};
    Coord cameraCorner {camera.camera0};

    PointI cameraCornerI {cameraCorner.asPointI()};

    Coord rescaleCoef {onScreenMapSize / Coord{screenSize}};

    if (img.width() != screenSize.x() || img.height() != screenSize.y()) {
        img = QImage{screenSize.x(), screenSize.y(),  QImage::Format::Format_RGB32};
    }

    const DrawSettings cur_draw_settings = *draw_settings_;
    const int y_size =  map_->size.y;

//    static int ff = 0;
//    if (ff % 100 == 0) {
//        map_->new_car_cells.writeToFile("new_car_cells" + std::to_string(ff)+".bmp");
//    }
//    ff++;

    int pixIdx = 0;
    uchar* pix = img.bits();
    for (int y = 0; y < screenSize.y(); y++) {
        for (int x = 0; x < screenSize.x(); x++) {
            const int imageX = static_cast<int>(x * rescaleCoef.x + cameraCornerI.x());
            const int imageY = y_size - (static_cast<int>((y) * rescaleCoef.y + cameraCornerI.y()));

            if ((-10 <= imageX && imageX <= -1) ||
                (-10 <= imageY && imageY <= -1) ||
                (mapSize.x <= imageX && imageX <= mapSize.x + 10) ||
                (mapSize.y <= imageY && imageY <= mapSize.y + 10)) {
                setPixels(pix, pixIdx, RGB(255, 255, 255));
                continue;
            }

            if (cur_draw_settings.draw_prev_car_data) {
                if (DrawDir(map_->new_car_data, imageX, imageY, pixIdx, pix)) {
                    continue;
                }
            }

            if (cur_draw_settings.draw_car_data) {
                if (DrawDir(map_->car_data, imageX, imageY, pixIdx, pix)) {
                    continue;
                }
            }

            if (cur_draw_settings.draw_prev_car_type) {
                if (DrawCarType(map_->new_car_cells, imageX, imageY, pixIdx, pix)) {
                    continue;
                }
            }

            if (cur_draw_settings.draw_car_type) {
                if (DrawCarType(map_->car_cells, imageX, imageY, pixIdx, pix)) {
                    continue;
                }
            }

            if (cur_draw_settings.draw_decision1) {
                if (DrawDir(map_->decision1, imageX, imageY, pixIdx, pix)) {
                    continue;
                }
            }

            if (cur_draw_settings.draw_decision2) {
                if (DrawDir(map_->decision2, imageX, imageY, pixIdx, pix)) {
                    continue;
                }
            }

            if (cur_draw_settings.draw_road_dir) {
                if (DrawDir(map_->road_dir, imageX, imageY, pixIdx, pix)) {
                    continue;
                }
            }


            setPixels(pix, pixIdx, {0, 0, 0});
        }
    }

    painter.drawImage(event->rect(), img);



    painter.setPen(textPen);
    painter.setFont(textFont);
    painter.drawText(10, 10,  QString::fromStdString(fmt::format("FPS: {:.2f}", currentFps)));
    painter.drawText(10, 25,  QString::fromStdString("Directions:"));



    if (directionsCircleImg.width() != 40) {
        directionsCircleImg = QImage{40, 40, QImage::Format::Format_ARGB32};
        directionsCircleImg.fill(QColor{0, 0, 0, 0});
        for (double angle = 0; angle < 2 * M_PI; angle += 2. * M_PI / 200.) {
            const Coord dir {sin(angle), cos(angle)};
            const Coord dir2 {sin(angle + M_PI / 2), cos(angle + M_PI / 2)};
            for (double i = 5; i < 15; i += 0.5) {
                const auto pos = (Coord{20 + dir.x * i, 20 + dir.y * i}).asPointI(); // dir2
                directionsCircleImg.setPixelColor(pos.x(), pos.y(), QColor(dir.x * 127 + 127, dir.y * 127 + 127, 0));
            }
        }
    }

    painter.drawImage(QPoint{10, 30}, directionsCircleImg);

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
