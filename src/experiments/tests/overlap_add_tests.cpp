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
  cv::Mat mat_val = actual_val.getMat(0).t();

  std::cout << "actual value" << std::endl << mat_val.t() << std::endl;
  for (size_t i = 0; i < real_dft_vals.size(); ++i) {
    auto mat_i = i * 2;
    ASSERT_FLOAT_EQ(real_dft_vals[i], mat_val.at<float>(mat_i));
    ASSERT_FLOAT_EQ(imginary_dft_vals[i], mat_val.at<float>(mat_i + 1));
  }
}

}  // end namepsace oflow
