/*
 * vector_statististics_benchmark.cpp
 *
 *  Created on: Feb 20, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include <celero/Celero.h>
#include "vector_statistics.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <array>

CELERO_MAIN

namespace oflow {

vector_type baseline_vector{std::vector<cv::Point2d>(8),
                            std::vector<cv::Point2d>(8)};

BASELINE(VectorStatistics, Constructor, 100, 1e5) {
  celero::DoNotOptimizeAway(VectorStatistics(baseline_vector));
}

vector_type big_vector{std::vector<cv::Point2d>(1024),
                       std::vector<cv::Point2d>(1024)};
BENCHMARK(VectorStatistics, BigConstructor, 100, 1e5) {
  celero::DoNotOptimizeAway(VectorStatistics(big_vector));
}

}  // end namespace oflow
