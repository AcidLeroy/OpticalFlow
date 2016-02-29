/*
 * motion_estimation_tests.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "mock_reader.h"
#include "motion_estimation.h"
#include "lk_flow.h"
#include "mock_flow.h"
#include <iostream>
#include <memory>

using ::testing::Return;
using ::testing::DefaultValue;
using ::testing::_;

namespace oflow {

TEST(MotionEstimation, NoFramesToRead) {
  std::shared_ptr<MockReader> mock(new MockReader{"some_file.mov"});
  MotionEstimation<MockReader> me(mock);
  std::shared_ptr<cv::Mat> a_ =
      std::make_shared<cv::Mat>(cv::Mat(256, 256, CV_8U));
  cv::randu(*a_, 0, 256);
  // Stuck using shared pointers because it was a pain to use unique ptrs with
  // google mock
  std::shared_ptr<Image> frame1{new Image(a_)};
  EXPECT_CALL(*mock, ReadFrame()).WillOnce(Return(nullptr));

  std::shared_ptr<MockFlow> mflow{new MockFlow()};
  ASSERT_THROW(me.EstimateMotion<MockFlow>(mflow), MotionEstimationException);
}

TEST(MotionEstimation, OnlyOneFrameInImageSequence) {
  std::shared_ptr<MockReader> mock(new MockReader{"some_file.mov"});
  MotionEstimation<MockReader> me(mock);
  std::shared_ptr<cv::Mat> a_ =
      std::make_shared<cv::Mat>(cv::Mat(256, 256, CV_8U));
  cv::randu(*a_, 0, 256);
  // Stuck using shared pointers because it was a pain to use unique ptrs with
  // google mock
  std::shared_ptr<Image> frame1{new Image(a_)};
  EXPECT_CALL(*mock, ReadFrame())
      .WillOnce(Return(frame1))
      .WillOnce(Return(nullptr));

  std::shared_ptr<MockFlow> mflow{new MockFlow()};
  ASSERT_THROW(me.EstimateMotion<MockFlow>(mflow), MotionEstimationException);
}

TEST(MotionEstimation, ReadTwoImages) {
  std::shared_ptr<MockReader> mock(new MockReader{"some_file.mov"});
  MotionEstimation<MockReader> me(mock);
  std::shared_ptr<cv::Mat> a_ =
      std::make_shared<cv::Mat>(cv::Mat(256, 256, CV_8U));
  cv::randu(*a_, 0, 256);
  std::shared_ptr<cv::Mat> b_ =
      std::make_shared<cv::Mat>(cv::Mat(256, 256, CV_8U));
  cv::randu(*b_, 0, 256);
  // Stuck using shared pointers because it was a pain to use unique ptrs with
  // google mock
  std::shared_ptr<Image> frame1{new Image(a_)};
  std::shared_ptr<Image> frame2{new Image(b_)};
  EXPECT_CALL(*mock, ReadFrame())
      .WillOnce(Return(frame1))
      .WillOnce(Return(frame2))
      .WillOnce(Return(nullptr));

  std::shared_ptr<MockFlow> mock_flow{new MockFlow()};

  EXPECT_CALL(*mock_flow, CalculateVectors(_, _))
      .WillOnce(Return(OpticalFlow<>()));

  me.EstimateMotion<MockFlow>(mock_flow);
}

TEST(MotionEstimation, ThresholdFlowStats) {
  std::shared_ptr<MockReader> mock(new MockReader{"some_file.mov"});
  MotionEstimation<MockReader> me(mock);
  std::vector<float> points_to_thresh{1, 2, 3, 4, 5};
  double percent_of_max{.5};
  auto actual = me.ThresholdVector(points_to_thresh, percent_of_max);
  std::vector<uint8_t> expected{0, 0, 1, 1, 1};
  ASSERT_EQ(expected, actual);
}

}  // end namepsace oflow
