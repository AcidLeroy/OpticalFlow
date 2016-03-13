/*
 * test_image.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: cody
 */
#include "gtest/gtest.h"
#include "image.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <chrono>
#include <ctime>
#include <iostream>
#include <cstddef>

namespace oflow {
TEST(ImageTests, DefaultConstructor) {
  Image<> img;
  ASSERT_EQ(nullptr, img.GetMat());
}
TEST(ImageTests, TestMatConstructor) {
  const int num_rows = 256;
  const int num_cols = 256;
  auto a = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
  Image<> img(a);
}

TEST(ImageTests, TestSize) {
  const int num_rows = 256;
  const int num_cols = 256;
  auto a = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
  Image<> img(a);
  ASSERT_EQ(num_rows, img.GetRows());
  ASSERT_EQ(num_cols, img.GetCols());
}

TEST(ImageTests, SpeedOfSecond) {
  const int num_rows = 256;
  const int num_cols = 256;
  const size_t num_iters = 1e6;
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  auto a = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
  for (size_t i = 0; i < num_iters; ++i) {
    Image<> img(a);
  }
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
}

}  // end namespace oflow
