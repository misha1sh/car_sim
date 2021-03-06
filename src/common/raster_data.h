#pragma once

#include "common/entities.h"
#include "utils/verify.h"

#include <range/v3/view/transform.hpp>
#include <range/v3/range/conversion.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>


template <typename T, typename CV_T>
class RasterData {
public:
    explicit RasterData(int len_x, int len_y, const T& default_value) :
        len_x_(len_x),
        len_y_(len_y),
        data_(len_y, len_x, toCV(default_value)), // swap to view mem
        default_value_(default_value)
    {
    }

    inline T& operator()(int x, int y) {
        // y x swapped
        CV_T& t = data_.template at<CV_T>(y, x);
        return *reinterpret_cast<T*>(&t);
    }

    inline T& operator()(const PointI& p) {
        return (*this)(p.x, p.y);
    }

    inline T& operator()(const PointF& c) {
        return (*this)(c.asPointI());
    }


    inline T getOrDefault(const PointI& p, T default_value) {
        if (p.x < 0 || p.y < 0 || p.x >= len_x_ || p.y >= len_y_) {
            return default_value;
        }

//        return {0.5 + x / len_x_ / 2., 0.5 + y / len_y_ / 2.};
        return (*this)(p);
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

    void fillBox(const PointI& p1, const PointI& p2, const T& value) {
        cv::rectangle(data_,
                      cv::Point{p1.x, p1.y},
                      cv::Point{p2.x, p2.y},
                      toCV(value),
                      cv::LineTypes::FILLED);
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


    void text(const PointI& pos, float size, const std::string& text, const T& color) {
        int baseline;
        auto text_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, size, 2, &baseline);
        text_size.height += baseline;
        cv::Mat_<CV_T> mat(text_size.height, text_size.width, toCV(default_value_));
//        cv::Mat_<cv::Vec3f> mat(text_size.height, text_size.width, {0, 0, 0});
        cv::putText(mat,
                    text,
                    {0, text_size.height - baseline},
                    cv::FONT_HERSHEY_SIMPLEX,
                    size,
//                    {1, 1, 1},
                    toCV(color),
                    2,
                    true
        );
//        cv::imshow("test", mat);
//        cv::waitKey(0);

        cv::flip(mat, mat, 0);

        for (int x = 0; x < text_size.width; x++) {
            for (int y = 0; y < text_size.height; y++) {
                if (x + pos.x > data_.cols || y + pos.y > data_.rows) {
                    continue;
                }
                data_.template at<CV_T>(y + pos.y, x + pos.x) = mat.template at<CV_T>(y, x);
            }
        }
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
    T default_value_;

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

using RasterDataPoint = RasterData<PointF, cv::Vec2d>;


template <typename T>
using RasterDataT = RasterData<T, T>;

template <typename T>
using RasterDataEnum = RasterData<T, unsigned char>;
