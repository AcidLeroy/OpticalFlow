/*
 * motion_estimation.h
 *
 *  Created on: Feb 10, 2016
 *      Author: cody
 */

#ifndef SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_
#define SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_

#include <memory>
#include <vector>
#include <algorithm>
#include "optical_flow.h"

namespace oflow {

class MotionEstimationException : public std::runtime_error {
 public:
  explicit MotionEstimationException(const std::string& what_arg)
      : runtime_error(what_arg) {}
  explicit MotionEstimationException(const char* what_arg)
      : runtime_error(what_arg) {}
};

template <typename ReaderType>
class MotionEstimation {
 public:
  explicit MotionEstimation(std::shared_ptr<ReaderType> reader)
      : reader_(reader){};

  template <typename OpticalFlowType>
  void EstimateMotion(std::shared_ptr<OpticalFlowType> flow) {
    auto current_frame = reader_->ReadFrame();
    if (current_frame == nullptr)
      throw MotionEstimationException("There are no frames to read!");
    auto next_frame = reader_->ReadFrame();
    if (next_frame == nullptr)
      throw MotionEstimationException("There is only one frame to read!");

    while (1) {
      auto stats = flow->CalculateVectors(*current_frame, *next_frame);
      auto thresh_mag = ThresholdVector(stats.GetMagnitude(), 0.25);
      auto thresh_bin_image =
          PointsToMat(stats.GetNumRows(), stats.GetNumRows(), thresh_mag,
                      stats.GetPoints());
      std::swap(current_frame, next_frame);
      next_frame = reader_->ReadFrame();
      if (next_frame == nullptr) break;
    }
  }

  template <typename T = double>
  std::vector<uint8_t> ThresholdVector(const std::vector<T>& points_to_thresh,
                                       double percent_of_max) const {
    auto max_val =
        std::max_element(points_to_thresh.cbegin(), points_to_thresh.cend());
    auto max_thresh = *max_val * percent_of_max;
    std::vector<uint8_t> result;
    result.reserve(points_to_thresh.size());
    for (auto& i : points_to_thresh) {
      if (i < max_thresh)
        result.push_back(0);
      else
        result.push_back(1);
    }
    return result;
  }

  template <typename T = float>
  cv::Mat PointsToMat(int num_rows, int num_cols,
                      const std::vector<T>& intensities,
                      std::vector<cv::Point_<float>>& points) {
    cv::Mat test_array = cv::Mat_<T>::zeros(num_rows, num_cols);
    for (size_t i = 0; i < points.size(); ++i) {
      test_array.at<uchar>(points[i].x, points[i].y) = intensities[i];
    }
    return test_array;
  }

 private:
  std::shared_ptr<ReaderType> reader_;
};

} /* namespace oflow */

#endif /* SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_ */
