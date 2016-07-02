/*
 * test_utilities.cpp
 *
 *  Created on: Jun 21, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../utilities.h"
#include <opencv2/core.hpp>
#include <vector>
#include <iostream>
#include <array>

namespace oflow {
namespace cv_utils {

TEST(utilities, CvTypeTest) {
  std::vector<float> a{{1.1111111, 2.2222222}};
  cv::Mat a_mat = cv::Mat(1, 2, CV_32F, &a[0]);

  std::cout << "value is: " << PrintMatVector(a_mat) << std::endl;
}

TEST(Utilities, StringCorrectness) {
  std::vector<int> a{{1, 2, 3, 4}};
  cv::Mat a_mat = cv::Mat(1, 4, CV_32S, &a[0]);
  std::string expected{"1 2 3 4"};
  std::string actual = PrintMatVector(a_mat);
  ASSERT_STREQ(expected.c_str(), actual.c_str());
}

TEST(Utilities, GetMinMax) {
  std::vector<int> a{{1, 2, 3, 4}};
  cv::Mat a_mat = cv::Mat(1, 4, CV_32S, &a[0]);
  std::array<double, 2> expected{{1, 4}};
  auto actual = GetMinMax(a_mat);
  ASSERT_EQ(expected[0], actual[0]);
  ASSERT_EQ(expected[1], actual[1]);
}

}  // end namespace cv_utils
}  // end namepace oflow
