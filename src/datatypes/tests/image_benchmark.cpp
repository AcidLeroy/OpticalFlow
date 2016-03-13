/*
 * image_benchmark.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: cody
 */

#include <celero/Celero.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <random>
#include <memory>
#include "image.h"

CELERO_MAIN

namespace oflow {

const int num_rows = 256;
const int num_cols = 256;
auto a = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));

BASELINE(ImageTests, SharedPtrConstruction, 10, 1e4) {
  celero::DoNotOptimizeAway(oflow::Image<>(a));
}

BENCHMARK(ImageTests, CreateMatWithShared, 10, 1e4) {
  const int num_rows = 256;
  const int num_cols = 256;
  auto a = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
  celero::DoNotOptimizeAway(oflow::Image<>(a));
}

const int small_num_rows = 32;
const int small_num_cols = 32;
auto small_a =
    std::make_shared<cv::Mat>(cv::Mat(small_num_rows, small_num_cols, CV_8U));
BENCHMARK(ImageTests, SmallerImage, 10, 1e4) {
  celero::DoNotOptimizeAway(oflow::Image<>(small_a));
}
const int large_num_rows = 1024;
const int large_num_cols = 1024;
auto large_a =
    std::make_shared<cv::Mat>(cv::Mat(large_num_rows, large_num_cols, CV_8U));
BENCHMARK(ImageTests, LargerImage, 10, 1e4) {
  celero::DoNotOptimizeAway(oflow::Image<>(large_a));
}

}  // end namespace oflow
