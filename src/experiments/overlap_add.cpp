/*
 * overlap_add.cpp
 *
 *  Created on: Apr 9, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include "overlap_add.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <algorithm>

#include <iostream>
namespace oflow {

cv::UMat OverlapAdd(const cv::UMat &x_n, const cv::UMat &h_n, size_t L) {
  // N
  auto n_row = h_n.rows + L - 1;  // This should be nearest power of 2
  auto n_col = h_n.cols + L - 1;

  auto nx_col = x_n.cols;
  auto nx_row = x_n.rows;
  // M
  auto m_kernel_row = h_n.rows;
  auto m_kernel_col = h_n.cols;
  // Left and Bottom pads will be the same value as right and top.
  auto top_pad = h_n.rows - 1;
  auto right_pad = h_n.cols - 1;

  cv::UMat y = cv::UMat::zeros(m_kernel_row + nx_row - 1,
                               m_kernel_col + nx_col - 1, CV_32F);

  for (size_t i_row = 0; i_row < nx_row; i_row += L) {
    auto il_row = (i_row + L - 1 < nx_row) ? i_row + L - 1 : nx_row;
    auto height = il_row - i_row;
    for (size_t i_col = 0; i_col < nx_col; i_col += L) {
      auto il_col = (i_col + L - 1 < nx_col) ? i_col + L - 1 : nx_col;

      auto width = il_col - i_col;

      cv::UMat sub = cv::UMat::zeros(height + 1 + 2 * top_pad,
                                     width + 1 + 2 * right_pad, CV_32F);
      x_n(cv::Rect(i_col, i_row, width + 1, height + 1))
          .copyTo(sub(cv::Rect(right_pad, top_pad, width + 1, height + 1)));

      cv::UMat yt;

      cv::matchTemplate(sub, h_n, yt, CV_TM_CCORR);

      auto k_row = std::min(n_row, static_cast<size_t>(i_row - y.rows));
      auto k_col = std::min(n_col, static_cast<size_t>(i_col - y.cols));

      // Overlap and add blocks
      cv::UMat y_region = y(cv::Rect(i_col, i_row, k_col, k_row));
      cv::UMat yt_region = yt(cv::Rect(0, 0, k_col, k_row));
      cv::add(y_region, yt_region, y_region);
    }
  }
  return y;
}
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

cv::UMat Idft(const cv::UMat &x_n) {
  cv::Mat x = x_n.getMat(cv::ACCESS_READ);
  x.convertTo(x, CV_32F);
  cv::Mat planes[] = {x, cv::Mat::zeros(x_n.size(), CV_32F)};

  cv::idft(x.getUMat(cv::ACCESS_RW), x.getUMat(cv::ACCESS_RW),
           cv::DFT_SCALE | cv::DFT_INVERSE /*|cv::DFT_REAL_OUTPUT*/);

  cv::split(x, planes);
  return planes[0].getUMat(cv::ACCESS_RW);
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
