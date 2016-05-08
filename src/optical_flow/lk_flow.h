/*
 * lk_flow.h
 *
 *  Created on: Feb 11, 2016
 *      Author: cody
 */

#ifndef SRC_OPTICAL_FLOW_LK_FLOW_H_
#define SRC_OPTICAL_FLOW_LK_FLOW_H_
#include "optical_flow.h"
#include "vector_statistics.h"
#include "image.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/video/tracking.hpp"
#include <array>

#include <vector>

namespace oflow {

/**
 * Lucas-Kanade optical flow interface
 */
template <typename mat_type>
class LKFlow {
 public:
  OpticalFlow<> CalculateVectors(const Image<mat_type> &previous_frame,
                                 const Image<mat_type> &next_frame) {
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

        cv::calcOpticalFlowPyrLK(*previous_mat, *next_mat, points_[0],
                                 points_[1], status, err, win_size, 3,
                                 termcrit_, 0, 0.001);
        SanitizePoints(status);
        VectorStatistics<> vs{points_};
        OpticalFlow<> of{vs.VelocityX(),    vs.VelocityY(), vs.Orientation(),
                         vs.Magnitude(),    points_[0],     previous_mat->rows,
                         previous_mat->cols};
        std::swap(points_[1], points_[0]);
        return of;
      }

      std::swap(points_[1], points_[0]);
    }
    // Return empty structure
    return OpticalFlow<>{};
  }
  vector_type GetTrackedPoints() const { return points_; }

  bool ArePointsInitialized() const { return !need_to_init_; }
  void ReinitializePointsToTrack() { need_to_init_ = true; }
  void UseAllPointsInImage(bool use_all) {
    need_to_init_ = true;
    use_all_points_ = use_all;
  }

 protected:
  void InitializePoints(const std::shared_ptr<mat_type> &previous_mat) {
    points_[0].clear();
    if (use_all_points_) {
      points_[0].reserve(previous_mat->rows * previous_mat->cols);
      for (size_t row = 0; row < previous_mat->rows; ++row) {
        for (size_t col = 0; col < previous_mat->cols; ++col) {
          points_[0].push_back(cv::Point_<float>(row, col));
        }
      }
    } else {
      // Get features to track
      cv::goodFeaturesToTrack(*previous_mat, points_[0], kMaxCorners_,
                              kQualityLevel_, kMinDistance_, kMask_,
                              kBlockSize_, kUseHarrisDetector_, kK_);
      cv::cornerSubPix(*previous_mat, points_[0], sub_pix_win_size_,
                       cv::Size(-1, -1), termcrit_);
    }
    need_to_init_ = false;
  }

  void SanitizePoints(const std::vector<uchar> &status) {
    size_t i, k;
    for (i = k = 0; i < points_[1].size(); i++) {
      if (!status[i]) continue;
      points_[1][k++] = points_[1][i];
      points_[0][k] = points_[0][i];
    }
    points_[1].resize(k);
    points_[0].resize(k);
  }

 private:
  bool need_to_init_ = true;
  bool use_all_points_ = false;
  vector_type points_;

  const int kMaxCorners_ = 500;  // Maximum number of corners to return. If
                                 // there are more corners than are found, the
                                 // strongest of them is returned.
  const double kQualityLevel_ =
      0.01;  // Parameter characterizing the minimal accepted quality of image
             // corners. The parameter value is multiplied by the best corner
             // quality measure, which is the minimal eigenvalue (see
             // cornerMinEigenVal() ) or the Harris function response (see
             // cornerHarris() ). The corners with the quality measure less than
             // the product are rejected. For example, if the best corner has
             // the quality measure = 1500, and the qualityLevel=0.01 , then all
             // the corners with the quality measure less than 15 are rejected.
  const double kMinDistance_ =
      10;  // Minimum possible Euclidean distance between the returned corners.
  const cv::Mat kMask_;  // Optional region of interest. If the image is not
  // empty (it needs to have the type CV_8UC1 and the same
  // size as image ), it specifies the region in which the
  const int kBlockSize_ = 3;  // Size of an average block for computing a
                              // derivative covariation matrix over each pixel
                              // neighborhood.
  const bool kUseHarrisDetector_ =
      false;  // Parameter inidicating whether to use Harris detector
  const double kK_ = 0.04;  // Free parameter of the Harris detector
  cv::Size sub_pix_win_size_{
      10, 10};  // Half of the side length of the search window.
  cv::TermCriteria termcrit_ =
      cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20,
                       0.03);  // Criteria for termination of the iterative
                               // process of corner refinement.
  cv::Size win_size{31, 31};
};

} /* namespace oflow */

#endif /* SRC_OPTICAL_FLOW_LK_FLOW_H_ */
