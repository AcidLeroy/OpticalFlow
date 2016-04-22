/*
 * dft_benchmark.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include <celero/Celero.h>
#include <opencv2/imgproc/imgproc.hpp>
CELERO_MAIN

namespace oflow {

constexpr int num_samples = 2;
constexpr int num_iterations = 100;
constexpr int num_rows = 4096;
constexpr int num_cols = 4096;
cv::UMat a = cv::UMat(num_rows, num_cols, CV_32F);
cv::Mat b = cv::Mat(num_rows, num_cols, CV_32F);

void CreateUMat() { cv::randu(a, 0, 256); }
void CreateMat() { cv::randu(b, 0, 256); }
void DftUMat() {
  // CreateUMat();
  cv::dft(a, a);
  cv::idft(a, a, cv::DFT_SCALE | cv::DFT_INVERSE);
}
void DftMat() {
  // CreateMat();
  cv::dft(b, b);
  cv::idft(b, b, cv::DFT_SCALE | cv::DFT_INVERSE);
}

BASELINE(UMatBenchmarks, Baseline, num_samples, num_iterations) { DftUMat(); }

BENCHMARK(UMatBenchmarks, NoGPU, num_samples, num_iterations) { DftMat(); }

}  // end namespace oflow
