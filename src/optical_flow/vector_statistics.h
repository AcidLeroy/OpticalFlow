/*
 * vector_statistics.h
 *
 *  Created on: Feb 20, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include <array>
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>
#include <string>

#ifndef SRC_OPTICAL_FLOW_VECTOR_STATISTICS_H_
#define SRC_OPTICAL_FLOW_VECTOR_STATISTICS_H_
namespace oflow {

class VectorStatisticsException : public std::runtime_error {
 public:
  explicit VectorStatisticsException(const std::string& what_arg)
      : runtime_error(what_arg) {}
  explicit VectorStatisticsException(const char* what_arg)
      : runtime_error(what_arg) {}
};
// vector type consists of two vectors of points. The first vector represents
// the previous image's points, and the second represents the second's.
using vector_type = std::array<std::vector<cv::Point_<float>>, 2>;

template <typename T = float>
class VectorStatistics {
 public:
  explicit VectorStatistics(const vector_type& points) : points_(points) {
    if (points_[0].size() != points_[1].size()) {
      throw VectorStatisticsException(std::string(
          "points[0].size = " + std::to_string(points_[0].size()) +
          " != points[1].size() = " + std::to_string(points_[1].size())));
    }
  };
  std::vector<T> VelocityX() {
    std::vector<T> velocity;
    velocity.reserve(points_[0].size());
    for (size_t i = 0; i < points_[0].size(); ++i) {
      velocity.push_back(points_[0][i].x - points_[1][i].x);
    }
    return velocity;
  }
  std::vector<T> VelocityY() {
    std::vector<T> velocity;
    velocity.reserve(points_[0].size());

    for (size_t i = 0; i < points_[0].size(); ++i) {
      velocity.push_back(points_[0][i].y - points_[1][i].y);
    }
    return velocity;
  }
  std::vector<T> Magnitude() {
    std::vector<T> mag;
    mag.reserve(points_[0].size());
    for (size_t i = 0; i < points_[0].size(); ++i) {
      mag.push_back(std::sqrt((points_[0][i].x - points_[1][i].x) *
                                  (points_[0][i].x - points_[1][i].x) +
                              (points_[0][i].y - points_[1][i].y) *
                                  (points_[0][i].y - points_[1][i].y)));
    }
    return mag;
  }
  std::vector<T> Orientation() {
    std::vector<T> theta;
    theta.reserve(points_[0].size());
    for (size_t i = 0; i < points_[0].size(); ++i) {
      T dot_prod =
          points_[0][i].x * points_[1][i].x + points_[0][i].y * points_[1][i].y;
      T mag = std::sqrt(points_[0][i].x * points_[0][i].x +
                        points_[0][i].y * points_[0][i].y) *
              std::sqrt(points_[1][i].x * points_[1][i].x +
                        points_[1][i].y * points_[1][i].y);
      theta.push_back(std::acos(dot_prod / mag));
    }
    return theta;
  }

 private:
  const vector_type& points_;
};

}  // end namespace oflow

#endif /* SRC_OPTICAL_FLOW_VECTOR_STATISTICS_H_ */
