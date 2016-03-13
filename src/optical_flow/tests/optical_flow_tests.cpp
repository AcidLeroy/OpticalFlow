/*
 * optical_flow_tests.cpp
 *
 *  Created on: Mar 7, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include "gtest/gtest.h"

#include "optical_flow.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <memory>

namespace oflow {
class OpticalFlowTests : public ::testing::Test {
 protected:
  void SetUp() override {
    std::vector<float> vx{1.0, 2.0, 3.0};
    std::vector<float> vy(vx);
    std::vector<float> magnitude(vx);
    std::vector<float> orientation(vx);  //  Don't really care about orientation
    std::vector<cv::Point_<float>> value_locations{cv::Point_<float>(0, 0),
                                                   cv::Point_<float>(1, 1),
                                                   cv::Point_<float>(2, 2)};

    of.reset(new OpticalFlow<>(vx, vy, magnitude, orientation, value_locations,
                               num_rows_, num_cols_));
  }

  int num_rows_ = 5, num_cols_ = 9;
  std::unique_ptr<OpticalFlow<>> of;
};

TEST_F(OpticalFlowTests, CorrectSizeOfMat) {
  auto vx_mat = of->GetVxMat();
  ASSERT_EQ(vx_mat.rows, num_rows_);
}

TEST_F(OpticalFlowTests, CorrectValuesForVx) {
  auto vx_mat = of->GetVxMat();
  ASSERT_EQ(vx_mat.at<float>(0, 0), 1.0);
  ASSERT_EQ(vx_mat.at<float>(1, 1), 2.0);
  ASSERT_EQ(vx_mat.at<float>(2, 2), 3.0);
}

}  // end namespace oflow
