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
  }
  const int num_rows_ = 256;
  const int num_cols_ = 256;
  std::shared_ptr<cv::Mat> a_, b_;
};

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
  auto b = std::make_shared<cv::Mat>(cv::Mat::ones(num_rows, num_cols, CV_8U));
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

}  // end namespace oflow
