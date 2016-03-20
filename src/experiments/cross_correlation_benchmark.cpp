/*
 * cross_correlation_benchmark.cpp
 *
 *  Created on: Mar 18, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include <celero/Celero.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <random>
#include <memory>
#include <iostream>

CELERO_MAIN

namespace oflow {
class CrossCorrelationFixture : public ::celero::TestFixture {
 public:
  virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues()
      const override {
    // std::cout << "Calling getExperimentalValues" << std::endl;
    return std::vector<std::pair<int64_t, uint64_t>>{
        std::make_pair(64, 0),   std::make_pair(128, 0),
        std::make_pair(256, 0),  std::make_pair(512, 0),
        std::make_pair(1024, 0), std::make_pair(2048, 0),
        std::make_pair(4096, 0)};
  }
  /// Before each run, build a vector of random integers.
  virtual void setUp(int64_t experimentValue) override {
    // std::cout << "experimental value is: " << experimentValue << std::endl;
    CreateFramesNoGpu(experimentValue, experimentValue);
    CreateFramesGpu(experimentValue, experimentValue);
  }

  void CreateFramesNoGpu(int num_cols, int num_rows) {
    frame_no_gpu =
        std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
    cv::randu(*frame_no_gpu, 0, 256);
    template_no_gpu =
        std::make_shared<cv::Mat>(cv::Mat(template_size, template_size, CV_8U));
  }

  void CreateFramesGpu(int num_cols, int num_rows) {
    frame_gpu = std::make_shared<cv::UMat>(cv::UMat(num_rows, num_cols, CV_8U));
    template_gpu = std::make_shared<cv::UMat>(
        cv::UMat(template_size, template_size, CV_8U));
    cv::randu(*frame_gpu, 0, 256);
  }

  virtual void tearDown() override {
    frame_no_gpu.reset();
    template_no_gpu.reset();
    frame_gpu.reset();
    template_gpu.reset();
  }

  std::shared_ptr<cv::Mat> frame_no_gpu, template_no_gpu;
  std::shared_ptr<cv::UMat> frame_gpu, template_gpu;

  // template_size x template_size
  const int template_size = 8;
};

const static int number_of_operations = 100;
const static int number_of_samples = 5;

BASELINE_F(CrossCorrelationBenchmark, NoGpuSupport, CrossCorrelationFixture,
           number_of_samples, number_of_operations) {
  cv::Mat result;
  cv::matchTemplate(*frame_no_gpu, *template_no_gpu, result, CV_TM_CCORR);
}

BENCHMARK_F(CrossCorrelationBenchmark, GpuSupport, CrossCorrelationFixture,
            number_of_samples, number_of_operations) {
  cv::UMat result;
  cv::matchTemplate(*frame_gpu, *template_gpu, result, CV_TM_CCORR);
}
}  // end namespace oflow
