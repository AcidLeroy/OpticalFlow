/*
 * vector_statistics_tests.cpp
 *
 *  Created on: Feb 20, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <array>
#include "vector_statistics.h"
#include <cmath>

namespace oflow {

TEST(VectorStatistics, CvConstructor) {
  vector_type points_;
  VectorStatistics<> vs(points_);
}
TEST(VectorStatistics, BadSizes) {
  cv::Point2f a_pt{1.0, 2.0}, b_pt{2.0, 3.0};
  std::vector<cv::Point2f> list_a{a_pt}, list_b{b_pt, a_pt};
  vector_type test_vecs{list_a, list_b};
  ASSERT_THROW(VectorStatistics<> vs{test_vecs}, VectorStatisticsException);
}

TEST(VectorStatistics, VelocityX) {
  cv::Point2f a_pt{1.0, 2.0}, b_pt{2.0, 3.0};
  std::vector<cv::Point2f> list_a{a_pt}, list_b{b_pt};
  vector_type test_vecs{list_a, list_b};
  VectorStatistics<> vs{test_vecs};
  double expected_velocity_x = a_pt.x - b_pt.x;
  auto actual_velocity_x = vs.VelocityX();
  ASSERT_DOUBLE_EQ(expected_velocity_x, actual_velocity_x[0]);
}

TEST(VectorStatistics, VelocityY) {
  cv::Point2f a_pt{1.0, 2.0}, b_pt{2.0, 3.0};
  std::vector<cv::Point2f> list_a{a_pt}, list_b{b_pt};
  vector_type test_vecs{list_a, list_b};
  VectorStatistics<> vs{test_vecs};
  double expected_velocity_y = a_pt.y - b_pt.y;
  auto actual_velocity_y = vs.VelocityY();
  ASSERT_DOUBLE_EQ(expected_velocity_y, actual_velocity_y[0]);
}

TEST(VectorStatistics, Magnitude) {
  cv::Point2f a_pt{1.0, 2.0}, b_pt{2.0, 3.0};
  std::vector<cv::Point2f> list_a{a_pt, b_pt}, list_b{b_pt, a_pt};
  vector_type test_vecs{list_a, list_b};
  VectorStatistics<> vs{test_vecs};
  double mag_1 = std::sqrt(std::pow(static_cast<double>(1.0 - 2.0), 2) +
                           std::pow(static_cast<double>(2.0 - 3.0), 2));
  double mag_2 = std::sqrt(std::pow(static_cast<double>(2.0 - 1.0), 2) +
                           std::pow(static_cast<double>(3.0 - 2.0), 2));
  std::vector<double> expected_mags{mag_1, mag_2};
  ASSERT_DOUBLE_EQ(expected_mags[0], vs.Magnitude()[0]);
  ASSERT_DOUBLE_EQ(expected_mags[1], vs.Magnitude()[1]);
}

TEST(VectorStatistics, Orientation) {
  cv::Point2f a_pt{3.0, 0.0}, b_pt{5.0, 5.0};
  std::vector<cv::Point2f> list_a{a_pt, b_pt}, list_b{b_pt, a_pt};
  vector_type test_vecs{list_a, list_b};
  VectorStatistics<> vs{test_vecs};
  // numbers calculated using the MATLAB formula:
  // costheta = dot(a,b)/(norm(a)*norm(b));
  // theta = acos(costheta);
  double expected_orientation = 0.785398163397448;
  auto actual_orientation = vs.Orientation();
  ASSERT_DOUBLE_EQ(expected_orientation, actual_orientation[0]);
}

}  // End namespace oflow
