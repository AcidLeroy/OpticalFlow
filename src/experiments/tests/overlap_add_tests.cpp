/*
 * test_overlap_add.cpp
 *
 *  Created on: Apr 9, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include "../overlap_add.h"

#include "gtest/gtest.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
#include <limits>
#include <algorithm>

namespace oflow {

// Verify I agree with MATLAB results
TEST(OverlapAdd, Dft) {
  std::vector<uint8_t> test_vals{8, 3, 4, 1, 5, 9, 6, 7, 2};
  std::vector<float> real_dft_vals{45, 0, 0, 0, 13.5, 0, 0, 0, 13.5};
  std::vector<float> imginary_dft_vals{0,       0, 0,      0,      7.7942,
                                       -5.1962, 0, 5.1962, -7.7942};

  cv::UMat x_n(test_vals, true);
  x_n = x_n.reshape(1, 3);
  EXPECT_EQ(3, x_n.rows);
  EXPECT_EQ(3, x_n.cols);

  auto actual_val = Dft(x_n);
  cv::Mat mat_val = actual_val.getMat(0).t();  // For some reason this is the
                                               // transposed version of the
                                               // MATLAB code.

  // Just need to get within the ball park
  for (size_t i = 0; i < real_dft_vals.size(); ++i) {
    auto mat_i = i * 2;
    ASSERT_NEAR(real_dft_vals[i], mat_val.at<float>(mat_i), 1e-3);
    ASSERT_NEAR(imginary_dft_vals[i], mat_val.at<float>(mat_i + 1), 1e-3);
  }
}

std::string type2str(int type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch (depth) {
    case CV_8U:
      r = "8U";
      break;
    case CV_8S:
      r = "8S";
      break;
    case CV_16U:
      r = "16U";
      break;
    case CV_16S:
      r = "16S";
      break;
    case CV_32S:
      r = "32S";
      break;
    case CV_32F:
      r = "32F";
      break;
    case CV_64F:
      r = "64F";
      break;
    default:
      r = "User";
      break;
  }

  r += "C";
  r += (chans + '0');

  return r;
}

TEST(OverlapAdd, ZeroPads) {
  std::vector<uint8_t> not_padded{1, 2, 3, 4};
  cv::UMat not_padded_mat(not_padded, true);
  not_padded_mat = not_padded_mat.reshape(1, 2);

  std::vector<uint8_t> padded{1, 2, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  cv::UMat padded_mat(padded, true);
  padded_mat = padded_mat.reshape(1, 4);

  int num_rows = 4;
  int num_cols = 4;

  cv::UMat actual_mat = ZeroPad(not_padded_mat, num_rows, num_cols);

  ASSERT_EQ(padded_mat.rows, actual_mat.rows);
  ASSERT_EQ(padded_mat.cols, actual_mat.cols);
  ASSERT_EQ(padded_mat.type(), actual_mat.type())
      << " padded = " << type2str(padded_mat.type())
      << " actual = " << type2str(actual_mat.type())
      << " not_padded_mat = " << type2str(not_padded_mat.type()) << std::endl;

  cv::Mat regular_mat = actual_mat.getMat(cv::ACCESS_RW);
  bool eq = std::equal(regular_mat.begin<uchar>(), regular_mat.end<uchar>(),
                       regular_mat.begin<uchar>());
  ASSERT_TRUE(eq);
}

TEST(OverlapAdd, Idft) {
  std::vector<uint8_t> test_vals{8, 3, 4, 1, 5, 9, 6, 7, 2};
  std::vector<float> real_dft_vals{45, 0, 0, 0, 13.5, 0, 0, 0, 13.5};
  std::vector<float> imginary_dft_vals{0,       0, 0,      0,      7.7942,
                                       -5.1962, 0, 5.1962, -7.7942};

  cv::UMat x_n(test_vals, true);
  x_n.convertTo(x_n, CV_32F);
  x_n = x_n.reshape(1, 3);
  EXPECT_EQ(3, x_n.rows);
  EXPECT_EQ(3, x_n.cols);

  cv::UMat dft_val = Dft(x_n);
  cv::UMat orig = Idft(dft_val);
  cv::Mat output = orig.getMat(cv::ACCESS_RW);
  cv::Mat x = x_n.getMat(cv::ACCESS_RW);
  //  std::cout << "output = " << std::endl << output << std::endl;
  //  std::cout << "test_array " << std::endl << x << std::endl;
  // Just need to get within the ball park
  ASSERT_EQ(x.rows, output.rows);
  ASSERT_EQ(x.cols, output.cols);
  ASSERT_EQ(x.type(), output.type()) << "x = " << type2str(x.type())
                                     << " output = " << type2str(output.type());

  for (size_t row = 0; row < output.rows; ++row) {
    for (size_t col = 0; col < output.cols; ++col) {
      ASSERT_NEAR(x.at<float>(row, col), output.at<float>(row, col), 1e-3);
    }
  }
}

}  // end namepsace oflow
