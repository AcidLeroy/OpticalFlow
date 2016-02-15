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

// std::random_device RandomDevice;
// std::uniform_int_distribution<int> UniformDistribution(0, 1024);
//
// BASELINE(DemoSimple, Baseline, 10, 1000000) {
//  celero::DoNotOptimizeAway(
//      static_cast<float>(sin(UniformDistribution(RandomDevice))));
//}

BASELINE(DemoSimple, SharedPtrConstruction, 10, 100000) {
  const int num_rows = 256;
  const int num_cols = 256;
  auto a = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
  celero::DoNotOptimizeAway(oflow::Image(a));
}
}  // end namespace oflow
