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
#include <array>

#include <vector>

namespace oflow {

/**
 * Lucas-Kanade optical flow interface
 */
class LKFlow {
 public:
  OpticalFlow<> CalculateVectors(const Image &previous_frame,
                                 const Image &next_frame);

 protected:
  void InitializePoints(const std::shared_ptr<cv::Mat> &previous_mat);
  void SanitizePoints(const std::vector<uchar> &status);

 private:
  bool need_to_init_ = true;
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
