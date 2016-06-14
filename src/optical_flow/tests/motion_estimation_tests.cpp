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
#include <string>
#include <ostream>

using ::testing::Return;
using ::testing::DefaultValue;
using ::testing::_;

namespace oflow {

TEST(MotionEstimation, NoFramesToRead) {
  std::shared_ptr<MockReader> mock(new MockReader{"some_file.mov"});
  MotionEstimation<MockReader, cv::Mat> me(mock);
  std::shared_ptr<cv::Mat> a_ =
      std::make_shared<cv::Mat>(cv::Mat(256, 256, CV_8U));
  cv::randu(*a_, 0, 256);
  // Stuck using shared pointers because it was a pain to use unique ptrs with
  // google mock
  std::shared_ptr<Image<>> frame1{new Image<>(a_)};
  EXPECT_CALL(*mock, ReadFrameMat()).WillOnce(Return(nullptr));

  std::shared_ptr<MockFlow> mflow{new MockFlow()};
  ASSERT_THROW(me.EstimateMotion<MockFlow>(mflow), MotionEstimationException);
}

TEST(MotionEstimation, OnlyOneFrameInImageSequence) {
  std::shared_ptr<MockReader> mock(new MockReader{"some_file.mov"});
  MotionEstimation<MockReader, cv::Mat> me(mock);
  std::shared_ptr<cv::Mat> a_ =
      std::make_shared<cv::Mat>(cv::Mat(256, 256, CV_8U));
  cv::randu(*a_, 0, 256);
  // Stuck using shared pointers because it was a pain to use unique ptrs with
  // google mock
  std::shared_ptr<Image<>> frame1{new Image<>(a_)};
  EXPECT_CALL(*mock, ReadFrameMat())
      .WillOnce(Return(frame1))
      .WillOnce(Return(nullptr));

  std::shared_ptr<MockFlow> mflow{new MockFlow()};
  ASSERT_THROW(me.EstimateMotion<MockFlow>(mflow), MotionEstimationException);
}

TEST(MotionEstimation, ReadTwoImages) {
  std::shared_ptr<MockReader> mock(new MockReader{"some_file.mov"});
  MotionEstimation<MockReader, cv::Mat> me(mock);
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
  EXPECT_CALL(*mock, ReadFrameMat())
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
  MotionEstimation<MockReader, cv::Mat> me(mock);
  std::vector<float> points_to_thresh{1, 2, 3, 4, 5};
  double percent_of_max{.5};
  auto actual = me.ThresholdVector(points_to_thresh, percent_of_max);
  std::vector<uint8_t> expected{0, 0, 1, 1, 1};
  ASSERT_EQ(expected, actual);
}

TEST(MotionEstimation, PointsToMat) {
  std::shared_ptr<MockReader> mock(new MockReader{"some_file.mov"});
  MotionEstimation<MockReader, cv::Mat> me(mock);
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
  cv::connectedComponentsWithStats(img, labels, stats, centroids);
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

namespace stats {

TEST(OflowStats, TestUpdateCentroidAndOrientation) {
  // Need to store centroids and orientations for each optical flow frame
  cv::Mat img = cv::Mat(data2);
  // assume that img has already been thresholded.
  img = img.reshape(1, 8);
  img = img.t();
  cv::Mat orientations;
  cv::Mat centroids;
  UpdateCentroidAndOrientation(img, &orientations, &centroids);
  UpdateCentroidAndOrientation(img, &orientations, &centroids);
  UpdateCentroidAndOrientation(img, &orientations, &centroids);
  ASSERT_EQ(3, orientations.total());
  ASSERT_EQ(3, centroids.rows);
}

TEST(OflowStats, TestGetLogicalVector) {
  std::vector<uint8_t> logical_data{{1, 0, 0, 1}};
  cv::Mat_<uint8_t> logical_mat = cv::Mat_<uint8_t>(2, 2, &logical_data[0]);
  std::cout << "logical mat " << std::endl << logical_mat << std::endl;

  std::vector<double> intensity_data{{1, 2, 3, 4}};
  cv::Mat_<double> intensity_mat = cv::Mat_<double>(2, 2, &intensity_data[0]);
  std::cout << "intensity Mat " << std::endl << intensity_mat << std::endl;
  cv::Mat expected_output = (cv::Mat_<double>(2, 2) << 1, 0, 0, 4);
  std::cout << "expected output " << std::endl << expected_output << std::endl;
  cv::Mat actual_output = GetLogicalVector(intensity_mat, logical_mat);
  ASSERT_EQ(expected_output.total(), actual_output.total());
  for (size_t i = 0; i < expected_output.total(); ++i) {
    ASSERT_DOUBLE_EQ(actual_output.at<double>(i),
                     expected_output.at<double>(i));
  }
}
TEST(OflowStats, TestGetHistoAround) {
  std::vector<uint8_t> data3{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,
                              1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
  cv::Mat thresholded_motion = cv::Mat(data3);
  thresholded_motion = thresholded_motion.reshape(1, 8);
  thresholded_motion = thresholded_motion.t();
  int disk_size = 3;
  cv::Mat gray_scale_image(thresholded_motion.rows, thresholded_motion.cols,
                           thresholded_motion.type());
  cv::randu(gray_scale_image, 0, 256);
  cv::Mat bg_histogram;
  GetHistoAround(thresholded_motion, disk_size, gray_scale_image,
                 &bg_histogram);
  GetHistoAround(thresholded_motion, disk_size, gray_scale_image,
                 &bg_histogram);
  GetHistoAround(thresholded_motion, disk_size, gray_scale_image,
                 &bg_histogram);
  std::cout << "histogram = " << std::endl << bg_histogram << std::endl;
}

TEST(OflowStats, TestUpdateHistogram) {
  std::vector<uint8_t> data3{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,
                              1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
  cv::Mat thresholded_motion = cv::Mat(data3);
  thresholded_motion = thresholded_motion.reshape(1, 8);
  thresholded_motion = thresholded_motion.t();
  cv::Mat motion_mags(thresholded_motion.rows, thresholded_motion.cols, CV_32F);
  cv::randu(motion_mags, 0, .2);
  cv::Mat histogram;
  int num_bins = 25;
  const float range[2] = {0, 0.2};
  UpdateHistogram(thresholded_motion, motion_mags, &histogram, range, num_bins);
}

TEST(OflowStats, TestCumSum) {
  std::vector<float> data3{{1, 2, 3, 4, 5, 6}};
  const cv::Mat data_mat = cv::Mat(1, 6, CV_32F, &data3[0]);
  std::vector<float> expected{1, 3, 6, 10, 15, 21};
  const cv::Mat expected_mat = cv::Mat(1, 6, CV_32F, &expected[0]);
  cv::Mat actual = CumSum(data_mat);
  for (int row = 0; row < data_mat.rows; ++row) {
    for (int col = 0; col < data_mat.cols; ++col) {
      ASSERT_EQ(expected_mat.at<float>(row, col), actual.at<float>(row, col));
    }
  }
}

TEST(OflowStats, TestGetCentroidCdf) {
  std::vector<float> data3{{1, 2, 3, 4, 5, 6}};
  const cv::Mat centroids = cv::Mat(3, 2, CV_32F, &data3[0]);
  const cv::Mat gray_frame = cv::Mat::zeros(8, 8, CV_8U);
  cv::Mat x_cent_cdf, y_cent_cdf;
  GetCentroidCdf(centroids, gray_frame, &x_cent_cdf, &y_cent_cdf);
  std::cout << "x_cent_cdf = " << std::endl << x_cent_cdf.t() << std::endl;
  std::cout << "y_cent_cdf = " << std::endl << y_cent_cdf.t() << std::endl;
}

TEST(OflowStats, TestGetOrientationCdf) {
  std::vector<float> data3{{1, 2, 3, 4, 5, 6}};
  const cv::Mat orientations = cv::Mat(6, 1, CV_32F, &data3[0]);
  cv::Mat orientation_cdf;
  GetOrientationCdf(orientations, &orientation_cdf);
  std::cout << "orientation_cdf = " << std::endl << orientation_cdf.t()
            << std::endl;
}

bool IsEqual(const cv::Mat &a, const cv::Mat &b, double tolerance = 0.0) {
  if (tolerance != 0.0) {
    cv::Mat diff = cv::abs(a - b);
    double min_val;
    double max_val;
    cv::Point min_loc;
    cv::Point max_loc;
    cv::minMaxLoc(diff, &min_val, &max_val, &min_loc, &max_loc);
    return max_val <= tolerance;
  }
  return cv::countNonZero(a != b) == 0;
}

TEST(OflowStats, TestGetCdf) {
  cv::Mat data = (cv::Mat_<float>(1, 9) << 1, 2, 3, 4, 5, 6, 7, 8, 9);
  cv::Mat output;
  int num_bins = 3;
  float max_val = 10;
  // This range has to be created to match the histc function in matlab.
  const float range[2] = {0, max_val};
  GetCdf(data, &output, num_bins, range);
  cv::Mat expected = (cv::Mat_<float>(3, 1) << 0.44444445, 1, 1);
  cv::Mat comparison;
  ASSERT_TRUE(IsEqual(expected, output, 1e-6));
}

TEST(OflowStats, TestNormalizeHistogramCdf) {
  std::vector<float> hist_data{{1, 2, 3, 4, 5, 6}};
  const cv::Mat hist(1, 6, CV_32F, &hist_data[0]);
  cv::Mat normalized_hist_cdf;
  NormalizeHistogramCdf(hist, &normalized_hist_cdf);
  cv::Mat expected = (cv::Mat_<float>(1, 6) << 0.047619049, 0.14285715,
                      0.2857143, 0.47619048, 0.71428573, 1);
  bool eq = IsEqual(expected, normalized_hist_cdf);
  ASSERT_TRUE(eq) << " expected = " << std::endl << expected << std::endl
                  << "actual = " << std::endl << normalized_hist_cdf
                  << std::endl;
}

TEST(OflowStats, TestConstructFeatureString) {
  cv::Mat mat1 = (cv::Mat_<float>(1, 2) << 1.0, 2.0);
  cv::Mat mat2 = (cv::Mat_<float>(1, 2) << 3, 4);
  std::string expected_string("1\t2\t1\t3\t4\t1\t\n");
  std::setprecision(8);
  std::cout << "expected string = " << expected_string << std::endl;
  std::vector<cv::Mat> feature_vectors{mat1, mat2};
  ASSERT_STREQ(expected_string.c_str(),
               ConstructFeatureString(feature_vectors).c_str());
}

// This test verifies that if you call getMat() that you modify the original
// umat data.
TEST(OpencvStuff, TestGettingMatFromUMat) {
  cv::UMat a = cv::UMat::zeros(5, 5, CV_32F);
  cv::Mat b = a.getMat(cv::ACCESS_RW);
  for (size_t i = 0; i < b.total(); ++i) {
    b.at<float>(i) = i;
  }
  // std::cout << "b == " << std::endl << b << std::endl;
  cv::Mat c = a.getMat(cv::ACCESS_RW);
  // std::cout << "c == " << std::endl << c << std::endl;

  for (size_t i = 0; i < b.total(); ++i) {
    ASSERT_FLOAT_EQ(b.at<float>(i), c.at<float>(i));
  }
}

}  // end namespace stats

}  // end namepsace oflow
