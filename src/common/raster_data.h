#pragma once

#include "common/entities.h"
#include "utils/verify.h"

#include <range/v3/all.hpp>
#include <opencv2/opencv.hpp>
#include <vector>


template <typename T, typename CV_T>
class RasterData {
public:
    explicit RasterData(int len_x, int len_y, const T& default_value) :
        len_x_(len_x),
        len_y_(len_y),
        data_(len_y, len_x, toCV(default_value)) // swap to view mem
    {
    }

    inline T& operator()(int x, int y) {
        // y x swapped
        CV_T& t = data_.template at<CV_T>(y, x);
        return *reinterpret_cast<T*>(&t);
    }

    inline T& operator()(const PointI& p) {
        return (*this)(p.x(), p.y());
    }

    inline T& operator()(const Coord& c) {
        return (*this)(c.asPointI());
    }


    inline T getOrDefault(int x, int y, T default_value) {
        if (x < 0 || y < 0 || x >= len_x_ || y >= len_y_) {
            return default_value;
        }

//        return {0.5 + x / len_x_ / 2., 0.5 + y / len_y_ / 2.};
        return (*this)(x, y);
    }


//    inline const T& operator()(int x, int y) const {
//        return reinterpret_cast<const T&>(data_.at(x, y));
//    }

    void fill(const T& value) {
        data_.setTo(toCV(value));
    }

    void fill(const PolygonI& polygon, const T& value) {
        // TODO: can be optimized
        // TODO: add support for inners()
        std::vector<cv::Point> points = polygon.outer() |
                ranges::views::transform([](const auto& pointI) {
                    return cv::Point{pointI.x(), pointI.y()};
                }) | ranges::to_vector;

//        for (const auto& point : points) {
//            (*this)(point.x, point.y) = value;
//        }

        cv::fillPoly(data_,
                       points,
                       toCV(value),
                       cv::LineTypes::LINE_4);
    }

    void fillConvex(const PolygonI& polygon, const T& value) {
        // TODO: add support for inners()
        std::vector<cv::Point> points = polygon.outer() |
                                        ranges::views::transform([](const auto& pointI) {
                                            return cv::Point{pointI.x(), pointI.y()};
                                        }) | ranges::to_vector;
        cv::fillConvexPoly(data_,
                     points,
                     toCV(value),
                     cv::LineTypes::LINE_4);
    }

    inline int sizeX() {
        return len_x_;
    }

    inline int sizeY() {
        return len_y_;
    }

    void show() {
        cv::imshow("test", data_);
        cv::waitKey(0);
    }



    void copyTo(RasterData<T, CV_T>& other) {
        VERIFY(len_x_ == other.len_x_ && len_y_ == other.len_y_);
        data_.copyTo(other.data_);
    }

    void writeToFile(const std::string& path) {
        auto img = data_.clone();
//        std::cerr << img.channels() << std::endl;
//        for (int i = 0; i  < 10; i++) {
//            for (int j = 0; j < 10; j++) {
//                std::cerr << (int)(*this)(i, j) << " ";
//            }
//            std::cerr << std::endl;
//        }
//        std::cerr << std::endl;


        if (img.channels() == 2) {
            std::vector<cv::Mat> channels(2);
            cv::split(img, channels);
            channels.push_back(channels.back());
            channels.back().setTo(0);
            cv::Mat res;
            cv::merge(channels, res);
            cv::imwrite(path, res);
            return;
        }
        cv::imwrite(path, img);
    }

private:
    int len_x_, len_y_;
    cv::Mat_<CV_T> data_;

    inline CV_T toCV(const T& t) const {
        T t_copy = t;
        return *reinterpret_cast<CV_T*>(&t_copy);
    }

    inline T fromCV(const CV_T& t) const {
        CV_T t_copy = t;
        return *reinterpret_cast<T*>(&t_copy);
    }
    //    std::vector<T> data_;
};

using RasterDataPoint = RasterData<Coord, cv::Vec2d>;

template <typename T>
using RasterDataT = RasterData<T, T>;

template <typename T>
using RasterDataEnum = RasterData<T, unsigned char>;
