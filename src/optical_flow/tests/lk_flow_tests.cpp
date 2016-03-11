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

class TestLKFlow : public ::testing::Test {
 protected:
  virtual void SetUp() {
    a_ = std::make_shared<cv::Mat>(cv::Mat(num_rows_, num_cols_, CV_8U));
    b_ = std::make_shared<cv::Mat>(cv::Mat(num_rows_, num_cols_, CV_8U));
    c_ = std::make_shared<cv::Mat>(cv::Mat(num_rows_, num_cols_, CV_8U));
    cv::randu(*a_, 0, 256);
    cv::randu(*b_, 0, 256);
    cv::randu(*c_, 0, 256);
    empty_ =
        std::make_shared<cv::Mat>(cv::Mat::zeros(num_rows_, num_cols_, CV_8U));
  }
  const int num_rows_ = 256;
  const int num_cols_ = 256;
  std::shared_ptr<cv::Mat> a_, b_, c_, empty_;
};

TEST_F(TestLKFlow, PointsSwapped) {
  Image prev(a_);
  Image next(b_);
  Image next1(c_);
  LKFlow flow;
  auto of = flow.CalculateVectors(prev, next);
  auto tracker_points = flow.GetTrackedPoints();
  of = flow.CalculateVectors(next, next1);
  auto tracker_points1 = flow.GetTrackedPoints();
  ASSERT_EQ(tracker_points[0][0], tracker_points1[1][0]);
}

TEST_F(TestLKFlow, ArePointsInitialized) {
  Image prev(a_);
  Image next(b_);
  LKFlow flow;
  auto of = flow.CalculateVectors(prev, next);
  auto tracker_points = flow.GetTrackedPoints();

  ASSERT_TRUE(flow.ArePointsInitialized());
}

TEST_F(TestLKFlow, ReinitializePointsToTrack) {
  Image prev(a_);
  Image next(b_);
  LKFlow flow;
  auto of = flow.CalculateVectors(prev, next);
  auto tracker_points = flow.GetTrackedPoints();
  flow.ReinitializePointsToTrack();

  ASSERT_FALSE(flow.ArePointsInitialized());
}

TEST(LKFlowTests, InitialFrame) {
  const int num_rows = 256;
  const int num_cols = 256;
  auto a = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
  Image prev(a);
  Image empyt_frame;
  LKFlow flow;
  auto of = flow.CalculateVectors(prev, empyt_frame);
  ASSERT_TRUE(of.IsEmpty());
}

TEST(LKFlowTests, TwoFrames) {
  const int num_rows = 256;
  const int num_cols = 256;
  auto a = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
  auto b = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
  cv::randu(*a, cv::Scalar::all(0), cv::Scalar::all(255));
  cv::randu(*b, cv::Scalar::all(0), cv::Scalar::all(255));
  Image previous_frame(a), next_frame(b);
  LKFlow flow;
  auto of = flow.CalculateVectors(previous_frame, next_frame);
  ASSERT_FALSE(of.IsEmpty());
}

TEST(LkFlowTests, NoFrames) {
  Image previous_frame, next_frame;
  LKFlow flow;
  auto of = flow.CalculateVectors(previous_frame, next_frame);
  ASSERT_TRUE(of.IsEmpty());
}

TEST_F(TestLKFlow, UseAllPointsInImage) {
  Image previous_frame(a_), next_frame(b_);
  LKFlow flow;
  flow.UseAllPointsInImage(true);
  auto of = flow.CalculateVectors(previous_frame, next_frame);
  EXPECT_FLOAT_EQ(num_rows_ * num_cols_, of.GetMagnitude().size());
  EXPECT_FLOAT_EQ(num_rows_ * num_cols_, of.GetOrientation().size());
  EXPECT_FLOAT_EQ(num_rows_ * num_cols_, of.GetVx().size());
  EXPECT_FLOAT_EQ(num_rows_ * num_cols_, of.GetVy().size());
}

}  // end namespace oflow
