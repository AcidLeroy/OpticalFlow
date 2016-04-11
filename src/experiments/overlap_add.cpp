/*
 * overlap_add.cpp
 *
 *  Created on: Apr 9, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include "overlap_add.h"

#include <opencv2/imgproc/imgproc.hpp>

namespace oflow {
cv::UMat OverlapAdd(const cv::UMat &x_n, const cv::UMat &h_n) { return x_n; }
cv::UMat Dft2(const cv::UMat &x_n, const cv::UMat &h_n) {
  cv::UMat x_dft = Dft(x_n);
  cv::UMat h_dft = Dft(h_n);
  cv::UMat x_h = x_dft.mul(h_dft);
  return x_h;
}

cv::UMat Dft(const cv::UMat &x_n) {
  cv::Mat x = x_n.getMat(0);
  cv::Mat planes[] = {cv::Mat_<float>(x), cv::Mat::zeros(x_n.size(), CV_32F)};

  cv::Mat complexI;  // Complex plane to contain the DFT coefficients
                     // {[0]-Real,[1]-Img}
  cv::merge(planes, 2, complexI);

  cv::UMat u_complex;
  complexI.copyTo(u_complex);
  cv::dft(u_complex, u_complex);  // Applying DFT
  return u_complex;
}

cv::UMat ZeroPad(const cv::UMat &block, int num_rows, int num_cols) {
  assert(num_cols >= block.cols);
  int right = num_cols - block.cols;
  assert(num_rows >= block.rows);
  int bottom = num_rows - block.rows;
  cv::UMat padded;
  copyMakeBorder(block, padded, 0, bottom, 0, right, cv::BORDER_CONSTANT,
                 cv::Scalar(0));
  return padded;
}
}  // end namespace oflow
