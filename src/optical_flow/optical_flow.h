/*
 * optical_flow.h
 *
 *  Created on: Feb 11, 2016
 *      Author: cody
 */

#ifndef SRC_OPTICAL_FLOW_OPTICAL_FLOW_H_
#define SRC_OPTICAL_FLOW_OPTICAL_FLOW_H_
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

namespace oflow {

template <typename T = float>
class OpticalFlow {
 public:
  OpticalFlow() = default;
  OpticalFlow(std::vector<T> vx, std::vector<T> vy, std::vector<T> orientation,
              std::vector<T> magnitude,
              std::vector<cv::Point_<float>> value_locations, int num_rows,
              int num_cols)
      : vx_(std::move(vx)),
        vy_(std::move(vy)),
        orientation_(std::move(orientation)),
        magnitude_(std::move(magnitude)),
        value_locations_(std::move(value_locations)),
        num_rows_(num_rows),
        num_cols_(num_cols) {}

  std::vector<T>& GetVx() { return vx_; }
  cv::UMat GetVxMat() { return PointsToMat(vx_); }
  std::vector<T>& GetVy() { return vy_; }
  std::vector<T>& GetOrientation() { return orientation_; }
  std::vector<T>& GetMagnitude() { return magnitude_; }
  std::vector<cv::Point_<float>>& GetPoints() { return value_locations_; }
  int GetNumRows() { return num_rows_; }
  int GetNumCols() { return num_cols_; }

  bool IsEmpty() {
    return vx_.empty() && vy_.empty() && orientation_.empty() &&
           magnitude_.empty();
  }

 private:
  cv::UMat PointsToMat(const std::vector<T>& intensities) const {
    cv::UMat test_array =
        cv::UMat::zeros(num_rows_, num_cols_, cv::DataType<T>::type);
    for (size_t i = 0; i < value_locations_.size(); ++i) {
      test_array.getMat(cv::ACCESS_RW)
          .at<T>(value_locations_[i].x, value_locations_[i].y) = intensities[i];
    }
    return test_array;
  }
  std::vector<T> vx_;
  std::vector<T> vy_;
  std::vector<T> orientation_;
  std::vector<T> magnitude_;
  std::vector<cv::Point_<float>> value_locations_;
  int num_rows_ = 0;
  int num_cols_ = 0;
};

template <>
class OpticalFlow<cv::Mat> {
public:
private:
	 cv::Mat vx_;
	 cv::Mat vy_;
	 cv::Mat orientation_;
	 cv::Mat magnitude_;
};
} /* namespace oflow */

#endif /* SRC_OPTICAL_FLOW_OPTICAL_FLOW_H_ */
