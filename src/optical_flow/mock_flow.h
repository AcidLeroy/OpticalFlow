/*
 * mock_flow.h
 *
 *  Created on: Feb 27, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#ifndef SRC_OPTICAL_FLOW_MOCK_FLOW_H_
#define SRC_OPTICAL_FLOW_MOCK_FLOW_H_
#include "gmock/gmock.h"
#include "image.h"
#include "optical_flow.h"

namespace oflow {
class MockFlow {
 public:
  MOCK_METHOD2(CalculateVectors, OpticalFlow<>(const Image &previous_frame,
                                               const Image &next_frame));
  MOCK_CONST_METHOD0(GetTrackedPoints, vector_type());
  MOCK_CONST_METHOD0(ArePointsInitialized, bool());
  MOCK_CONST_METHOD0(ReinitializePointsToTrack, void());
};
}

#endif /* SRC_OPTICAL_FLOW_MOCK_FLOW_H_ */
