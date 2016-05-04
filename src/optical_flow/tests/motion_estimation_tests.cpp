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
  std::shared_ptr<Image<>> frame1{new Image<>(a_)};
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
  std::shared_ptr<Image<>> frame1{new Image<>(a_)};
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
  std::shared_ptr<Image<>> frame1{new Image<>(a_)};
  std::shared_ptr<Image<>> frame2{new Image<>(b_)};
  EXPECT_CALL(*mock, ReadFrame())
      .WillOnce(Return(frame1))
      .WillOnce(Return(frame2))
      .WillOnce(Return(nullptr));

  std::shared_ptr<LKFlow<cv::Mat>> flow;
  flow.reset(new LKFlow<cv::Mat>());

  // WTF was I thinking when I wrote this??
  //  EXPECT_CALL(*mock_flow, CalculateVectors(_, _))
  //      .WillOnce(Return(OpticalFlow<>()));
  //
  me.EstimateMotion<LKFlow<cv::Mat>>(flow);
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

TEST(MotionEstimation, PointsToMat) {
  std::shared_ptr<MockReader> mock(new MockReader{"some_file.mov"});
  MotionEstimation<MockReader> me(mock);
  cv::Mat original_image = cv::Mat::zeros(3, 3, CV_32F);
  std::vector<cv::Point_<float>> points{{0, 0}, {1, 1}, {2, 2}};
  std::vector<float> magnitudes{1.0, 2.0, 3.0};
  auto test_array = me.PointsToMat<float>(
      original_image.rows, original_image.cols, magnitudes, points);

  ASSERT_EQ(magnitudes[0], test_array.at<uchar>(0, 0));
  ASSERT_EQ(magnitudes[1], test_array.at<uchar>(1, 1));
  ASSERT_EQ(magnitudes[2], test_array.at<uchar>(2, 2));
}

std::vector<uint8_t> data{{1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1,
                           1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1,
                           1, 1, 0, 0, 0, 0, 1, 1, 1, 1}};

TEST(OpenCvStuff, ConnectedComponents) {
  cv::Mat labels, stats, centroids;

  cv::Mat img = cv::Mat(data);
  img = img.reshape(1, 8);
  int nccomps = cv::connectedComponentsWithStats(img, labels, stats, centroids);
  ASSERT_EQ(3, nccomps);  // label 0 is reserved for background label, hence 3
                          // connected components
}

std::vector<uint8_t> data2{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,
                            1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

TEST(OpenCvStuff, OrientationOfConnectedComponents) {
  cv::Mat labels, stats, centroids;
  cv::Mat img = cv::Mat(data2);
  img = img.reshape(1, 8);
  img = img.t();
  std::cout << "img = " << std::endl << img << std::endl;
  int nccomps = cv::connectedComponentsWithStats(img, labels, stats, centroids);
  std::cout << "centroids = " << std::endl << centroids << std::endl;
  // Get labels for 2
  cv::Mat label2 = cv::Mat::zeros(labels.rows, labels.cols, CV_8U);
  for (int row = 0; row < labels.rows; ++row) {
    for (int col = 0; col < labels.cols; ++col) {
      if (labels.at<int>(row, col) == 2) {
        label2.at<uchar>(row, col) = 123;
      }
    }
  }
  std::vector<std::vector<cv::Point>> contours;

  cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
  std::cout << "label2 = " << std::endl << label2 << std::endl;
  for (int i = 0; i < contours.size(); ++i) {
    std::cout << "The size is: " << contours[i].size() << std::endl;
    std::cout << "contours = " << std::endl << contours[i] << std::endl;
    cv::RotatedRect result = cv::fitEllipse(contours[i]);
    std::cout << "angle = " << result.angle << std::endl;
  }
}

TEST(OpenCvStuff, AppendRow) {
  std::vector<uint16_t> my_data{{1, 2, 3, 4}};
  std::vector<uint16_t> data_to_append{{5, 6, 7, 8}};
  cv::Mat original_mat = cv::Mat(my_data);
  original_mat = original_mat.reshape(1, 2);
  cv::Mat mat_to_append = cv::Mat(data_to_append);
  mat_to_append = mat_to_append.reshape(1, 2);
  std::cout << "Original_mat " << std::endl << original_mat << std::endl;
  cv::Mat new_mat;
  cv::vconcat(original_mat, mat_to_append, new_mat);
  std::cout << "New mat = " << std::endl << new_mat << std::endl;
}

void UpdateCentroidAndOrientation(const cv::Mat &thresholded_image,
                                  std::vector<double> *orientations,
                                  cv::Mat *centroids) {
  cv::Mat labels, stats, current_centroids;
  cv::connectedComponentsWithStats(thresholded_image, labels, stats,
                                   current_centroids);
  // Don't care about background centroid, hence the range.
  if (centroids->empty()) {
    current_centroids(cv::Range(1, current_centroids.rows),
                      cv::Range(0, current_centroids.cols)).copyTo(*centroids);
  } else {
    cv::vconcat(*centroids,
                current_centroids(cv::Range(1, current_centroids.rows),
                                  cv::Range(0, current_centroids.cols)),
                *centroids);
  }
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(thresholded_image, contours, cv::RETR_LIST,
                   cv::CHAIN_APPROX_NONE);
  for (int i = 0; i < contours.size(); ++i) {
    // Can only fit an ellipse with 5 points, skip others
    if (contours[i].size() >= 5) {
      cv::RotatedRect result = cv::fitEllipse(contours[i]);
      orientations->push_back(result.angle);
    }
  }
}

TEST(OpenCvStuff, TestUpdateCentroidAndOrientation) {
  // Need to store centroids and orientations for each optical flow frame
  cv::Mat img = cv::Mat(data2);
  // assume that img is already been thresholded.
  img = img.reshape(1, 8);
  img = img.t();
  std::vector<double> orientations;
  cv::Mat centroids;
  UpdateCentroidAndOrientation(img, &orientations, &centroids);
  UpdateCentroidAndOrientation(img, &orientations, &centroids);
  UpdateCentroidAndOrientation(img, &orientations, &centroids);
  ASSERT_EQ(3, orientations.size());
  ASSERT_EQ(3, centroids.rows);
}

}  // end namepsace oflow
