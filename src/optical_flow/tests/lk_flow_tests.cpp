/*
 * lk_flow_tests.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "image.h"
#include "lk_flow.h"
#include <opencv2/imgproc/imgproc.hpp>

namespace oflow {

TEST(LKFlowTests, InitialFrame) {
  const int num_rows = 256;
  const int num_cols = 256;
  auto a = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
  Image prev(a);
  Image next;
  LKFlow flow;
  flow.CalculateVectors(prev, next);
}
}  // end namespace oflow
