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

// With this test I wanted to verify that I can get a rectangle to a Mat object
// and not a
TEST(OverlapAdd, GetSubregion) {
  cv::Mat a(4, 4, CV_32F);
  for (size_t i = 0; i < a.total(); ++i) {
    a.at<float>(i) = i;
  }

  // This returns a reference to the innards of a! Hazahh!
  cv::Mat b = a(cv::Rect(0, 0, 1, 1));
  float expected_val = 69.0;
  b.at<float>(0) = expected_val;
  ASSERT_FLOAT_EQ(expected_val, a.at<float>(0));
}

cv::Mat Get4x4() {
  cv::Mat a(4, 4, CV_32F);
  for (size_t i = 0; i < a.total(); ++i) {
    a.at<float>(i) = i;
  }
  return a;
}

cv::UMat GetU16x16() {
  cv::Mat a(16, 16, CV_32F);
  for (size_t i = 0; i < a.total(); ++i) {
    a.at<float>(i) = i;
  }
  return a.getUMat(cv::ACCESS_RW);
}

TEST(OverlapAdd, OverlapAdd) {
  cv::UMat x = GetU16x16();
  size_t L = 4;  // break image into 4x4 blocks
  cv::UMat h = cv::UMat::ones(3, 3, CV_32F);
  cv::UMat actual_result = OverlapAdd(x, h, L);
  cv::Mat actual = actual_result.getMat(cv::ACCESS_RW);

  auto top_pad = h.rows - 1;
  auto bottom_pad = h.rows - 1;
  auto right_pad = h.cols - 1;
  auto left_pad = h.cols - 1;
  cv::copyMakeBorder(x, x, top_pad, bottom_pad, left_pad, right_pad,
                     cv::BORDER_CONSTANT, cv::Scalar(0));
  cv::UMat expected_result;
  cv::matchTemplate(x, h, expected_result, CV_TM_CCORR);
  cv::Mat expected = expected_result.getMat(cv::ACCESS_RW);
  for (size_t i = 0; i < expected.total(); ++i) {
    ASSERT_FLOAT_EQ(expected.at<float>(i), actual.at<float>(i));
  }
}

TEST(OverlapAdd, DISABLED_TestSum) {
  cv::UMat full_x = GetU16x16();
  cv::UMat sub_x;
  full_x(cv::Rect(0, 0, 8, 4)).copyTo(sub_x);
  std::cout << "sub_x is " << std::endl << sub_x.getMat(cv::ACCESS_RW)
            << std::endl;
  cv::UMat h = cv::UMat::ones(3, 3, CV_32F);
  cv::UMat sub1_x, sub2_x;
  sub_x(cv::Rect(0, 0, 4, 4)).copyTo(sub1_x);
  sub_x(cv::Rect(4, 0, 4, 4)).copyTo(sub2_x);
  std::cout << "sub1_x = " << std::endl << sub1_x.getMat(cv::ACCESS_RW)
            << std::endl;
  std::cout << "sub2_x = " << std::endl << sub2_x.getMat(cv::ACCESS_RW)
            << std::endl;
  auto top_pad = h.rows - 1;
  auto bottom_pad = h.rows - 1;
  auto right_pad = h.cols - 1;
  auto left_pad = h.cols - 1;
  cv::copyMakeBorder(sub1_x, sub1_x, top_pad, bottom_pad, left_pad, right_pad,
                     cv::BORDER_CONSTANT, cv::Scalar(0));
  cv::copyMakeBorder(sub2_x, sub2_x, top_pad, bottom_pad, left_pad, right_pad,
                     cv::BORDER_CONSTANT, cv::Scalar(0));
  cv::copyMakeBorder(sub_x, sub_x, top_pad, bottom_pad, left_pad, right_pad,
                     cv::BORDER_CONSTANT, cv::Scalar(0));
  std::cout << "sub1_x = " << std::endl << sub1_x.getMat(cv::ACCESS_RW)
            << std::endl;
  std::cout << "sub2_x = " << std::endl << sub2_x.getMat(cv::ACCESS_RW)
            << std::endl;
  cv::UMat expected_result;
  cv::matchTemplate(sub_x, h, expected_result, CV_TM_CCORR);
  std::cout << "expected_result_x = " << std::endl
            << expected_result.getMat(cv::ACCESS_RW) << std::endl;

  cv::UMat actual1, actual2;
  cv::UMat actual_combined = cv::UMat::zeros(expected_result.size(), CV_32F);
  cv::matchTemplate(sub1_x, h, actual1, CV_TM_CCORR);
  cv::matchTemplate(sub2_x, h, actual2, CV_TM_CCORR);
  std::cout << "actual1 = " << std::endl << actual1.getMat(cv::ACCESS_RW)
            << std::endl;
  std::cout << "actual2 = " << std::endl << actual2.getMat(cv::ACCESS_RW)
            << std::endl;
  cv::add(actual_combined(cv::Rect(0, 0, 6, 6)), actual1(cv::Rect(0, 0, 6, 6)),
          actual_combined(cv::Rect(0, 0, 6, 6)));
  cv::add(actual_combined(cv::Rect(4, 0, 6, 6)), actual2(cv::Rect(0, 0, 6, 6)),
          actual_combined(cv::Rect(4, 0, 6, 6)));
  std::cout << "actual_combined = " << std::endl
            << actual_combined.getMat(cv::ACCESS_RW) << std::endl;
}

TEST(SubdivideImage, DISABLED_NoOp) {
  cv::Mat a = Get4x4();
  size_t num_rows = 4;
  size_t num_cols = 4;
  auto subimage = SubdivideImage<cv::Mat>(a, num_rows, num_cols);
  ASSERT_EQ(1, subimage.size());
  bool eq = cv::countNonZero(a != subimage[0]) == 0;
  ASSERT_TRUE(eq);
}

TEST(RandomThought, DoesThisWork) {
  cv::Mat m = cv::Mat::zeros(4, 4, CV_32F);
  // cv::UMat* ptr;  //= new cv::UMat(m.rows, m.cols, m.type());
  // dont ask me why the hell we are using a pointer to the smart mat object (my
  // project leader insists)
  // m.copyTo(*ptr);

  cv::UMat* ptr = new cv::UMat(m.getUMat(cv::ACCESS_READ));
}

}  // end namepsace oflow
