/*
 * lk_flow.h
 *
 *  Created on: Feb 11, 2016
 *      Author: cody
 */

#ifndef SRC_OPTICAL_FLOW_FARNEBACK_FLOW_H_
#define SRC_OPTICAL_FLOW_FARNEBACK_FLOW_H_
#include "optical_flow.h"
#include "vector_statistics.h"
#include "image.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/video/tracking.hpp"
#include <array>

#include <vector>

namespace oflow {
namespace utils {
template <typename mat_type = cv::Mat>
static OpticalFlow<cv::Mat> GetOpticalFlow(const mat_type& flow) {
  cv::Mat mag = cv::Mat::zeros(flow.rows, flow.cols, CV_32F);
  cv::Mat vx = cv::Mat::zeros(flow.rows, flow.cols, CV_32F);
  cv::Mat vy = cv::Mat::zeros(flow.rows, flow.cols, CV_32F);
  cv::Mat orient = cv::Mat::zeros(flow.rows, flow.cols, CV_32F);
  for (int y = 0; y < flow.rows; ++y)
    for (int x = 0; x < flow.cols; ++x) {
      const cv::Point2f& fxy = flow.template at<cv::Point2f>(y, x);
      float magnitude = std::sqrt(std::pow(fxy.x, 2) + std::pow(fxy.y, 2));
      mag.at<float>(y, x) = magnitude;
      vx.at<float>(y, x) = fxy.x;
      vy.at<float>(y, x) = fxy.y;
      // unit vector lies on x=1 y = 0
      float dot_product = fxy.y * 0 + fxy.x * 1;
      if (magnitude != 0) {
        orient.at<float>(y, x) = std::acos(dot_product / magnitude);
      } else {
        orient.at<float>(y, x) = 0;
      }
    }
  return OpticalFlow<cv::Mat>(vx, vy, orient, mag);
}

template <>
OpticalFlow<cv::Mat> GetOpticalFlow(const cv::UMat& flow) {
  return GetOpticalFlow(flow.getMat(cv::ACCESS_RW));
}
}

/**
 * Farneback optical flow
 */

class FarneBackFlow {
 public:
  template <typename mat_type>
  OpticalFlow<cv::Mat> CalculateVectors(const Image<mat_type>& previous_frame,
                                        const Image<mat_type>& next_frame) {
    auto previous_mat = previous_frame.GetMat();
    auto next_mat = next_frame.GetMat();
    // Assume we are always called with both images
    if (previous_mat != nullptr && next_mat != nullptr) {
      mat_type uflow;
      cv::calcOpticalFlowFarneback(*previous_mat, *next_mat, uflow, 0.5, 3, 5,
                                   3, 5, 1.1, 0);
      return utils::GetOpticalFlow(uflow);
    }
    // Return empty structure
    return OpticalFlow<cv::Mat>{};
  }
};

} /* namespace oflow */

#endif /* SRC_OPTICAL_FLOW_FARNEBACK_FLOW_H_ */
