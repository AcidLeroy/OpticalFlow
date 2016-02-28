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

class LKFlowFixture : public celero::TestFixture {
 public:
  /// Before each run, build a vector of random integers.
  virtual void setUp(int64_t experimentValue) override {
    a_ = std::make_shared<cv::Mat>(cv::Mat(num_rows_, num_cols_, CV_8U));
    b_ = std::make_shared<cv::Mat>(cv::Mat(num_rows_, num_cols_, CV_8U));
    c_ = std::make_shared<cv::Mat>(cv::Mat(num_rows_, num_cols_, CV_8U));
    cv::randu(*a_, 0, 256);
    cv::randu(*b_, 0, 256);
    cv::randu(*c_, 0, 256);
    frame1_.reset(new Image(a_));
    frame2_.reset(new Image(b_));
    frame3_.reset(new Image(c_));
    flow_.reset(new LKFlow());
  }

  /// After each run, clear the vector of random integers.
  virtual void tearDown() {}

  std::shared_ptr<cv::Mat> a_, b_, c_;
  std::unique_ptr<Image> frame1_, frame2_, frame3_;
  std::unique_ptr<LKFlow> flow_;
  int num_cols_ = 2048;
  int num_rows_ = 2048;
};

BASELINE_F(LKFlowBenchMark, BigFlow256X256, LKFlowFixture, 10, 10) {
  celero::DoNotOptimizeAway(flow_->CalculateVectors(*frame1_, *frame2_));
  celero::DoNotOptimizeAway(flow_->CalculateVectors(*frame2_, *frame3_));
  celero::DoNotOptimizeAway(flow_->CalculateVectors(*frame3_, *frame2_));
}

}  // End namepsace oflow
