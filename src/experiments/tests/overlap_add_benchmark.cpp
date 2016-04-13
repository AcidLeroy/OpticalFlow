/*
 * overlap_add_benchmark.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include <celero/Celero.h>
#include <opencv2/imgproc/imgproc.hpp>
CELERO_MAIN

namespace oflow {

constexpr int num_samples = 100;
constexpr int num_iterations = 1000;
constexpr int num_rows = 512;
constexpr int num_cols = 512;

void CreateUMat() { cv::UMat a = cv::UMat::zeros(num_rows, num_cols, CV_32F); }
void CreateMat() { cv::Mat a = cv::Mat::zeros(num_rows, num_cols, CV_32F); }

void CreateUmatThenConvertRW() {
  cv::UMat a = cv::UMat::zeros(num_rows, num_cols, CV_32F);
  cv::Mat b = a.getMat(cv::ACCESS_RW);
}

void CreateUmatThenConvertFast() {
  cv::UMat a = cv::UMat::zeros(num_rows, num_cols, CV_32F);
  cv::Mat b = a.getMat(cv::ACCESS_FAST);
}
void CreateUmatThenConvertRead() {
  cv::UMat a = cv::UMat::zeros(num_rows, num_cols, CV_32F);
  cv::Mat b = a.getMat(cv::ACCESS_READ);
}

BASELINE(UMatBenchmarks, Baseline, num_samples, num_iterations) {
  CreateUMat();
}

BENCHMARK(UMatBenchmarks, CreateMat, num_samples, num_iterations) {
  CreateMat();
}

BENCHMARK(UMatBenchmarks, RW, num_samples, num_iterations) {
  CreateUmatThenConvertRW();
}

BENCHMARK(UMatBenchmarks, Fast, num_samples, num_iterations) {
  CreateUmatThenConvertFast();
}

BENCHMARK(UMatBenchmarks, ReadOnly, num_samples, num_iterations) {
  CreateUmatThenConvertRead();
}

}  // end namespace oflow
