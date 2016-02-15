/*
 * lk_flow.h
 *
 *  Created on: Feb 11, 2016
 *      Author: cody
 */

#ifndef SRC_OPTICAL_FLOW_LK_FLOW_H_
#define SRC_OPTICAL_FLOW_LK_FLOW_H_
#include "optical_flow.h"
#include "image.h"
#include <vector>

namespace oflow {

/**
 * Lucas-Kanade optical flow interface
 */
class LKFlow {
 public:
  OpticalFlow CalculateVectors(const Image &previous_frame,
                               const Image &next_frame);
};

} /* namespace oflow */

#endif /* SRC_OPTICAL_FLOW_LK_FLOW_H_ */
