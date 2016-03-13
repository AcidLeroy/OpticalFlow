/*
 * lk_flow_benchmark.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include <celero/Celero.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <random>
#include <memory>
#include <iostream>
#include "lk_flow.h"
#include "image.h"

CELERO_MAIN

namespace oflow {

const static int number_of_iters = 10;
const static int number_of_trials = 10;
class LKFlowFixture : public celero::TestFixture {
 public:
  virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues()
      const override {
    // std::cout << "Calling getExperimentalValues" << std::endl;
    return std::vector<std::pair<int64_t, uint64_t>>{
        std::make_pair(256, 0), std::make_pair(512, 0), std::make_pair(1024, 0),
        std::make_pair(2048, 0)};
  }
  /// Before each run, build a vector of random integers.
  virtual void setUp(int64_t experimentValue) override {
    // std::cout << "experimental value is: " << experimentValue << std::endl;
    CreateFramesNoGpu(experimentValue, experimentValue);
    CreateFramesGpu(experimentValue, experimentValue);
  }

  void CreateFramesNoGpu(int num_cols, int num_rows) {
    a_no_gpu = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
    b_no_gpu = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
    c_no_gpu = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
    cv::randu(*a_no_gpu, 0, 256);
    cv::randu(*b_no_gpu, 0, 256);
    cv::randu(*c_no_gpu, 0, 256);
    frame1_no_gpu.reset(new Image<cv::Mat>(a_no_gpu));
    frame2_no_gpu.reset(new Image<cv::Mat>(b_no_gpu));
    frame3_no_gpu.reset(new Image<cv::Mat>(c_no_gpu));
    flow_no_gpu.reset(new LKFlow<cv::Mat>());
  }

  void CreateFramesGpu(int num_cols, int num_rows) {
    a_gpu = std::make_shared<cv::UMat>(cv::UMat(num_rows, num_cols, CV_8U));
    b_gpu = std::make_shared<cv::UMat>(cv::UMat(num_rows, num_cols, CV_8U));
    c_gpu = std::make_shared<cv::UMat>(cv::UMat(num_rows, num_cols, CV_8U));
    cv::randu(*a_gpu, 0, 256);
    cv::randu(*b_gpu, 0, 256);
    cv::randu(*c_gpu, 0, 256);
    frame1_gpu.reset(new Image<cv::UMat>(a_gpu));
    frame2_gpu.reset(new Image<cv::UMat>(b_gpu));
    frame3_gpu.reset(new Image<cv::UMat>(c_gpu));
    flow_gpu.reset(new LKFlow<cv::UMat>());
  }

  std::shared_ptr<cv::Mat> a_no_gpu, b_no_gpu, c_no_gpu;
  std::unique_ptr<Image<cv::Mat>> frame1_no_gpu, frame2_no_gpu, frame3_no_gpu;
  std::unique_ptr<LKFlow<cv::Mat>> flow_no_gpu;

  std::shared_ptr<cv::UMat> a_gpu, b_gpu, c_gpu;
  std::unique_ptr<Image<cv::UMat>> frame1_gpu, frame2_gpu, frame3_gpu;
  std::unique_ptr<LKFlow<cv::UMat>> flow_gpu;
};

BASELINE_F(LKFlowBenchMark, NoGpuSupport, LKFlowFixture, number_of_trials,
           number_of_iters) {
  celero::DoNotOptimizeAway(
      flow_no_gpu->CalculateVectors(*frame1_no_gpu, *frame2_no_gpu));
  celero::DoNotOptimizeAway(
      flow_no_gpu->CalculateVectors(*frame2_no_gpu, *frame3_no_gpu));
  celero::DoNotOptimizeAway(
      flow_no_gpu->CalculateVectors(*frame3_no_gpu, *frame2_no_gpu));
}

BENCHMARK_F(LKFlowBenchMark, GpuSupport, LKFlowFixture, number_of_trials,
            number_of_iters) {
  celero::DoNotOptimizeAway(
      flow_gpu->CalculateVectors(*frame1_gpu, *frame2_gpu));
  celero::DoNotOptimizeAway(
      flow_gpu->CalculateVectors(*frame2_gpu, *frame3_gpu));
  celero::DoNotOptimizeAway(
      flow_gpu->CalculateVectors(*frame3_gpu, *frame2_gpu));
}

}  // End namepsace oflow
