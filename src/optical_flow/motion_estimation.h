/*
 * motion_estimation.h
 *
 *  Created on: Feb 10, 2016
 *      Author: cody
 */

#ifndef SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_
#define SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_

namespace oflow {

template <typename ReaderType>
class MotionEstimation {
 public:
  template <typename OpticalFlowType>
  void EstimateMotion() {
    auto next_frame = reader.ReadFrame();
    auto previous_frame = &next_frame;
    OpticalFlowType flow_algo;

    if (next_frame != nullptr) {
      OpticalFlow optical_flow =
          CalculateVectors(previous_frame, next_frame);
    }
  }

 private:
  ReaderType reader;
};

} /* namespace oflow */

#endif /* SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_ */
