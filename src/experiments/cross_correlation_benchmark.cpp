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
#include "overlap_add.h"

CELERO_MAIN

namespace oflow {
class CrossCorrelationFixture : public ::celero::TestFixture {
 public:
  virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues()
      const override {
    // std::cout << "Calling getExperimentalValues" << std::endl;
    return std::vector<std::pair<int64_t, uint64_t>>{
        //        std::make_pair(64, 0),   std::make_pair(128, 0),
        //        std::make_pair(256, 0),
        std::make_pair(512, 0),  std::make_pair(1024, 0),
        std::make_pair(2048, 0), std::make_pair(4096, 0),
        std::make_pair(8192, 0),
        //        		std::make_pair(16384, 0),
        //        std::make_pair(32768, 0)
    };
  }
  /// Before each run, build a vector of random integers.
  virtual void setUp(int64_t experimentValue) override {
    // std::cout << "experimental value is: " << experimentValue << std::endl;
    CreateFramesNoGpu(experimentValue, experimentValue);
    CreateFramesGpu(experimentValue, experimentValue);
  }

  void CreateFramesNoGpu(int num_cols, int num_rows) {
    frame_no_gpu =
        std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_32F));
    cv::randu(*frame_no_gpu, 0, 256);
    template_no_gpu = std::make_shared<cv::Mat>(
        cv::Mat(template_size, template_size, CV_32F));
  }

  void CreateFramesGpu(int num_cols, int num_rows) {
    frame_gpu =
        std::make_shared<cv::UMat>(cv::UMat(num_rows, num_cols, CV_32F));
    template_gpu = std::make_shared<cv::UMat>(
        cv::UMat(template_size, template_size, CV_32F));
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
  const int template_size = 17;
};

const static int number_of_operations = 5;
const static int number_of_samples = 3;

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

constexpr int L = 256;

BENCHMARK_F(CrossCorrelationBenchmark, Decompose, CrossCorrelationFixture,
            number_of_samples, number_of_operations) {
  if (frame_gpu->rows > L) {
    int num_iters =
        (frame_gpu->rows / L) * (frame_gpu->rows / L);  // Assume rows == cols
    cv::UMat result;
    for (size_t i = 0; i < num_iters; ++i) {
      cv::UMat sub_region = (*frame_gpu)(cv::Rect(0, 0, L - 1, L - 1));
      cv::matchTemplate(sub_region, *template_gpu, result, CV_TM_CCORR);
    }
  } else {
    cv::UMat result;
    cv::matchTemplate(*frame_gpu, *template_gpu, result, CV_TM_CCORR);
  }
}

BENCHMARK_F(CrossCorrelationBenchmark, OverlapAdd, CrossCorrelationFixture,
            number_of_samples, number_of_operations) {
  if (frame_gpu->rows > L) {
    cv::UMat result = OverlapAdd(*frame_gpu, *template_gpu, L);
  } else {
    cv::UMat result;
    cv::matchTemplate(*frame_gpu, *template_gpu, result, CV_TM_CCORR);
  }
}
// BENCHMARK_F(CrossCorrelationBenchmark, NoGpuDftIdft, CrossCorrelationFixture,
//            number_of_samples, number_of_operations) {
//  cv::UMat t = Dft(*template_gpu);
//  cv::UMat x = Dft(*frame_gpu);
//  cv::Mat combo = x.getMat(cv::ACCESS_RW).mul(t.getMat(cv::ACCESS_RW));
//  cv::UMat reslt = Idft(combo.getUMat(cv::ACCESS_RW));
//}

}  // end namespace oflow
