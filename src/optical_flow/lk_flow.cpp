/*
 * lk_flow.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: cody
 */

#include "lk_flow.h"
#include "optical_flow.h"
#include <vector>
#include "opencv2/video/tracking.hpp"
namespace oflow {
OpticalFlow LKFlow::CalculateVectors(const Image &previous_frame,
                                     const Image &next_frame) {
  OpticalFlow of;
  auto previous_mat = previous_frame.GetMat();
  auto next_mat = next_frame.GetMat();
  if (previous_mat == nullptr || next_mat == nullptr) {
    return of;
  }

  if (need_to_init_) {
    InitializePoints(previous_mat);
  }
}

void LKFlow::InitializePoints(const std::shared_ptr<cv::Mat> &previous_mat) {
  // Get features to track
  cv::goodFeaturesToTrack(*previous_mat, points_[1], kMaxCorners_,
                          kQualityLevel_, kMinDistance_, kMask_, kBlockSize_,
                          kUseHarrisDetector_, kK_);
  cv::cornerSubPix(*previous_mat, points_[1], sub_pix_win_size_,
                   cv::Size(-1, -1), termcrit_);
  need_to_init_ = false;
}

} /* namespace oflow */
