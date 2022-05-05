#include "map_painter.h"

#include "utils/verify.h"

#include <fmt/core.h>


MapPainter::MapPainter() {
    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));

    textPen = QPen(Qt::white);
    textFont.setPixelSize(12);
}

void MapPainter::setMap(RasterMapHolder map) {
    map_holder_ = std::move(map);
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

inline Color DirToRGB(const PointF& dir) {
    return RGB(dir.x * 127 + 127, dir.y * 127 + 127, 0);
}

unsigned int IntHash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

inline Color IdToRGB(int id) {
    unsigned int hash = IntHash(static_cast<unsigned int>(id));
    return RGB(hash & 0x0000ff, (hash & 0x00ff00) >> 2, (hash & 0xff0000) >> 4);
}

//inline Color CarTypeToRGB(const CarCellType& car_type) {
//    if (car_type == CarCellType::CENTER) {
//        return RGB(0, 0, 255);
//    }
//    if (car_type == CarCellType::BODY) {
//        return RGB(0, 0, 128);
//    }
//    return RGB(0, 0, 0);
//    // VERIFY(false && "Unknown car cell type");
//}



inline bool DrawDir(RasterDataPoint& data, int imageX, int imageY, int& pixIdx, uchar* pix) {
    const auto dir = data(imageX, imageY); // data.getOrDefault({imageX, imageY}, {0, 0});
    if (dir != PointF{0, 0}) {
        setPixels(pix, pixIdx, DirToRGB(dir));
        return true;
    }

    return false;
}

inline bool DrawCarType(RasterDataT<int>& data, int imageX, int imageY, int& pixIdx, uchar* pix) {
    const auto car_type = data(imageX, imageY); //data.getOrDefault({imageX, imageY}, 0);
    if (car_type != 0) {
        setPixels(pix, pixIdx, RGB(0, 0, 128));
        return true;
    }

    return false;
}

inline bool DrawBorder(const PointI& mapSizeI, const int imageX, const int imageY, int& pixIdx, uchar* pix) {
    if (0 <= imageX && imageX < mapSizeI.x &&
        0 <= imageY && imageY < mapSizeI.y) {
        return false;
    }

    if ((-10 <= imageX && imageX <= -1) ||
        (-10 <= imageY && imageY <= -1) ||
        (mapSizeI.x <= imageX && imageX <= mapSizeI.x + 10) ||
        (mapSizeI.y <= imageY && imageY <= mapSizeI.y + 10)) {
        // border
        setPixels(pix, pixIdx, RGB(255, 255, 255));
    } else {
        // outer space
        setPixels(pix, pixIdx, RGB(10, 10, 10));
    }

    return true;
}
// #define STORE_IN_TEMP_MAP

void MapPainter::paintMap(QPainter &painter, QPaintEvent* event, Camera camera) {
    int elapsedSinceLastRender = last_render_time_.restart();
    double currentFps = 0;
    if (elapsedSinceLastRender != 0) {
        currentFps = 1000. / elapsedSinceLastRender;
    }
    avg_fps_ = currentFps * 0.3 + avg_fps_ * 0.7;

//    painter.fillRect(event->rect(), background);
    if (!map_holder_ || !draw_settings_) {
        painter.setPen(textPen);
        painter.setFont(textFont);
        painter.drawText(10, 10,  QString::fromStdString(fmt::format("No map or draw settings")));
        return;
    }

    {

    #ifdef STORE_IN_TEMP_MAP
        {
            std::unique_ptr<RasterMap> main_map;
            auto guard = map_holder_.Get(main_map);
            VERIFY(main_map);
            if (!map_ || map_->size != main_map->size || map_->pixels_per_meter != main_map->pixels_per_meter) {
                map_ = std::make_unique<RasterMap>(main_map->sizeI.x(), main_map->sizeI.y(), main_map->pixels_per_meter);
                std::cerr << "Created map copy for rendering" << std::endl;
            }
            main_map->CopyTo(*map_);
        }
    #else
         auto guard = map_holder_.Get(map_);
    #endif

        const DrawSettings cur_draw_settings = *draw_settings_;


        // everything in coordinate system relative to screen
        PointF screenSizeF {event->rect().width() / cur_draw_settings.resoultion_coef,
                           event->rect().height() / cur_draw_settings.resoultion_coef};
        PointI screenSize {screenSizeF.asPointI()};
        PointF mapSize {map_->size};
        PointI mapSizeI {map_->sizeI};

    //    mapSize.y = screenSize.y() * 1. * mapSize.x  / screenSize.x();
    //    mapSize.x = screenSize.x() * 1. * mapSize.y  / screenSize.y();

    //    PointF onScreenMapSize {camera.size * mapSize};
        PointF onScreenMapSize {camera.camera1 - camera.camera0};
        PointF cameraCorner {camera.camera0};

        PointI cameraCornerI {cameraCorner.asPointI()};

        PointF rescaleCoef {onScreenMapSize / PointF{screenSize}};

        if (img.width() != screenSize.x || img.height() != screenSize.y) {
            img = QImage{screenSize.x, screenSize.y,  QImage::Format::Format_RGB32};
        }

        const int y_size =  map_->size.y;

    //    static int ff = 0;
    //    if (ff % 100 == 0) {
    //        map_->new_car_cells.writeToFile("new_car_cells" + std::to_string(ff)+".bmp");
    //    }
    //    ff++;

//        auto& new_car_data = map_->new_car_data;
//        auto& car_data = map_->car_data;
        auto& new_car_cells = map_->new_car_cells;
        auto& car_cells = map_->car_cells;
        auto& lane_id = map_->lane_id;
        auto& lane_dir = map_->lane_dir;
        auto& debug = map_->debug;

        int pixIdx = 0;
        uchar* pix = img.bits();
        for (int y = 0; y < screenSize.y; y++) {
            for (int x = 0; x < screenSize.x; x++) {
                const int imageX = static_cast<int>(x * rescaleCoef.x + cameraCornerI.x);
                const int imageY = y_size - (static_cast<int>((y) * rescaleCoef.y + cameraCornerI.y));

                if (DrawBorder(mapSizeI, imageX, imageY, pixIdx, pix)) {
                    continue;
                }

                if (cur_draw_settings.draw_debug) {
                    const unsigned int value = debug(imageX, imageY);
                    if (value != 0) {
                        setPixels(pix, pixIdx, RGB((value & 0xff0000) >> 12,
                                                   (value & 0x00ff00) >> 4,
                                                   (value & 0x0000ff)));
                        continue;
                    }
                }

//                if (cur_draw_settings.draw_prev_car_data) {
//                    if (DrawDir(new_car_data, imageX, imageY, pixIdx, pix)) {
//                        continue;
//                    }
//                }
//
//                if (cur_draw_settings.draw_car_data) {
//                    if (DrawDir(car_data, imageX, imageY, pixIdx, pix)) {
//                        continue;
//                    }
//                }

                if (cur_draw_settings.draw_prev_car_type) {
                    if (DrawCarType(new_car_cells, imageX, imageY, pixIdx, pix)) {
                        continue;
                    }
                }

                if (cur_draw_settings.draw_car_type) {
                    if (DrawCarType(car_cells, imageX, imageY, pixIdx, pix)) {
                        continue;
                    }
                }

                if (cur_draw_settings.draw_road_dir || cur_draw_settings.draw_road_id) {
                    const auto id = lane_id(imageX, imageY);
                    if (id != 0) {
                        if (cur_draw_settings.draw_road_id) {
                            setPixels(pix, pixIdx, IdToRGB(id));
                            continue;
                        }
                        if (cur_draw_settings.draw_road_dir) {
                            const auto dir = lane_dir.at(id);
                            setPixels(pix, pixIdx, DirToRGB(dir));
                            continue;
                        }
                    }
                }


                setPixels(pix, pixIdx, {0, 0, 0});
            }
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
            const PointF dir {sin(angle), cos(angle)};
            const PointF dir2 {sin(angle + M_PI / 2), cos(angle + M_PI / 2)};
            for (double i = 5; i < 15; i += 0.5) {
                const auto pos = (PointF{20 + dir.x * i, 20 + dir.y * i}).asPointI(); // dir2
                directionsCircleImg.setPixelColor(pos.x, pos.y, QColor(dir.x * 127 + 127, dir.y * 127 + 127, 0));
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
