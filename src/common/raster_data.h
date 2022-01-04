#pragma once

#include "common/entities.h"

#include <opencv2/opencv.hpp>
#include <vector>


template <typename T, typename CV_T>
class RasterData {
public:
    explicit RasterData(size_t len_x, size_t len_y, const T& default_value) :
        len_x_(len_x),
        len_y_(len_y),
        data_(len_x, len_y, toCV(default_value))
    {
    }

    T& operator()(size_t x, size_t y) {
        return reinterpret_cast<T&>(data_.at(x, y));
    }

    const T& operator()(size_t x, size_t y) const {
        return reinterpret_cast<const T&>(data_.at(x, y));
    }

    void fill(const PolygonI& polygon, const T& value) {
        cv::fillConvexPoly(data_,
                           polygon,
                           toCV(value),
                           cv::LineTypes::FILLED);
    }

    inline size_t sizeX() {
        return len_x_;
    }

    inline size_t sizeY() {
        return len_y_;
    }

private:
    size_t len_x_, len_y_;
    cv::Mat_<CV_T> data_;

    inline CV_T toCV(const T& t) const {
        return reinterpret_cast<CV_T>(t);
    }

    inline T fromCV(const CV_T& t) const {
        return reinterpret_cast<T>(t);
    }
    //    std::vector<T> data_;
};

using RasterDataPoint = RasterData<PointF, cv::Vec2d>;

template <typename T>
using RasterDataEnum = RasterData<T, char>;
