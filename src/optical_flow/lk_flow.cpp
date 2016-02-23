/*
 * lk_flow.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: cody
 */

#include "lk_flow.h"
#include "optical_flow.h"
#include "vector_statistics.h"
#include <vector>
#include <iostream>
#include "opencv2/video/tracking.hpp"
namespace oflow {
OpticalFlow<> LKFlow::CalculateVectors(const Image &previous_frame,
                                       const Image &next_frame) {
  auto previous_mat = previous_frame.GetMat();
  auto next_mat = next_frame.GetMat();
  // Assume we are always called with both images
  if (previous_mat != nullptr && next_mat != nullptr) {
    if (need_to_init_) {
      InitializePoints(previous_mat);
    }

    if (!points_[0].empty()) {
      std::vector<uchar> status;
      std::vector<float> err;

      cv::calcOpticalFlowPyrLK(*previous_mat, *next_mat, points_[0], points_[1],
                               status, err, win_size, 3, termcrit_, 0, 0.001);
      SanitizePoints(status);
    }
    VectorStatistics<> vs{points_};
    OpticalFlow<> of{vs.VelocityX(), vs.VelocityY(), vs.Orientation(),
                     vs.Magnitude()};
    std::swap(points_[1], points_[0]);
    return of;
  } else {
    // Return empty structure
    return OpticalFlow<>{};
  }
}

void LKFlow::InitializePoints(const std::shared_ptr<cv::Mat> &previous_mat) {
  // Get features to track
  cv::goodFeaturesToTrack(*previous_mat, points_[0], kMaxCorners_,
                          kQualityLevel_, kMinDistance_, kMask_, kBlockSize_,
                          kUseHarrisDetector_, kK_);
  std::cout << "Number of points is: " << points_[0].size() << std::endl;
  cv::cornerSubPix(*previous_mat, points_[0], sub_pix_win_size_,
                   cv::Size(-1, -1), termcrit_);
  need_to_init_ = false;
}

void LKFlow::SanitizePoints(const std::vector<uchar> &status) {
  size_t i, k;
  for (i = k = 0; i < points_[1].size(); i++) {
    if (!status[i]) continue;

    points_[1][k++] = points_[1][i];
    points_[0][k] = points_[0][i];
    // cv::circle(image, points_[1][i], 3, cv::Scalar(0, 255, 0), -1, 8);
  }
  points_[1].resize(k);
  points_[0].resize(k);
}

} /* namespace oflow */
