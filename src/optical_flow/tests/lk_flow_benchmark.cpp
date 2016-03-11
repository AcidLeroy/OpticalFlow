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
        std::make_pair(2056, 0)};
  }
  /// Before each run, build a vector of random integers.
  virtual void setUp(int64_t experimentValue) override {
    // std::cout << "experimental value is: " << experimentValue << std::endl;
    CreateFrames(experimentValue, experimentValue);
  }

  void CreateFrames(int num_cols, int num_rows) {
    a_ = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
    b_ = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
    c_ = std::make_shared<cv::Mat>(cv::Mat(num_rows, num_cols, CV_8U));
    cv::randu(*a_, 0, 256);
    cv::randu(*b_, 0, 256);
    cv::randu(*c_, 0, 256);
    frame1_.reset(new Image(a_));
    frame2_.reset(new Image(b_));
    frame3_.reset(new Image(c_));
    flow_.reset(new LKFlow());
  }

  std::shared_ptr<cv::Mat> a_, b_, c_;
  std::unique_ptr<Image> frame1_, frame2_, frame3_;
  std::unique_ptr<LKFlow> flow_;
};

BASELINE_FIXED_F(LKFlowBenchMark, ThreeFrameOpticalFlow, LKFlowFixture,
                 number_of_trials, number_of_iters, .001) {
  celero::DoNotOptimizeAway(flow_->CalculateVectors(*frame1_, *frame2_));
  celero::DoNotOptimizeAway(flow_->CalculateVectors(*frame2_, *frame3_));
  celero::DoNotOptimizeAway(flow_->CalculateVectors(*frame3_, *frame2_));
}

// BASELINE_F(LKFlowBenchMark, CalculateAllPoints, LKFlowFixture,
// number_of_trials,
//           number_of_iters) {
//  flow_->UseAllPointsInImage(true);
//  celero::DoNotOptimizeAway(flow_->CalculateVectors(*frame1_, *frame2_));
//  celero::DoNotOptimizeAway(flow_->CalculateVectors(*frame2_, *frame3_));
//  celero::DoNotOptimizeAway(flow_->CalculateVectors(*frame3_, *frame2_));
//}

}  // End namepsace oflow
