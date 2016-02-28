/*
 * motion_estimation.h
 *
 *  Created on: Feb 10, 2016
 *      Author: cody
 */

#ifndef SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_
#define SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_

#include <memory>
#include "optical_flow.h"

namespace oflow {

class MotionEstimationException : public std::runtime_error {
 public:
  explicit MotionEstimationException(const std::string& what_arg)
      : runtime_error(what_arg) {}
  explicit MotionEstimationException(const char* what_arg)
      : runtime_error(what_arg) {}
};

template <typename ReaderType>
class MotionEstimation {
 public:
  explicit MotionEstimation(std::shared_ptr<ReaderType> reader)
      : reader_(reader){};
  template <typename OpticalFlowType>
  void EstimateMotion(std::shared_ptr<OpticalFlowType> flow) {
    auto current_frame = reader_->ReadFrame();
    auto next_frame = reader_->ReadFrame();
    if (next_frame == nullptr)
      throw MotionEstimationException("There is only one frame to read!");

    while (1) {
      auto stats = flow->CalculateVectors(*current_frame, *next_frame);
      std::swap(current_frame, next_frame);
      next_frame = reader_->ReadFrame();
      if (next_frame == nullptr) break;
    }
  }

 private:
  std::shared_ptr<ReaderType> reader_;
};

} /* namespace oflow */

#endif /* SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_ */
