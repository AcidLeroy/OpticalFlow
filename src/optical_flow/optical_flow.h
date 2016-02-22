/*
 * optical_flow.h
 *
 *  Created on: Feb 11, 2016
 *      Author: cody
 */

#ifndef SRC_OPTICAL_FLOW_OPTICAL_FLOW_H_
#define SRC_OPTICAL_FLOW_OPTICAL_FLOW_H_
#include <vector>
namespace oflow {

template <typename T = float>
class OpticalFlow {
 public:
  OpticalFlow() = default;
  OpticalFlow(std::vector<T> vx, std::vector<T> vy, std::vector<T> orientation,
              std::vector<T> magnitude)
      : vx_(std::move(vx)),
        vy_(std::move(vy)),
        orientation_(std::move(orientation)),
        magnitude_(std::move(orientation)) {}

  std::vector<T> GetVx() { return vx_; }
  std::vector<T> GetVy() { return vy_; }
  std::vector<T> GetOrientation() { return orientation_; }
  std::vector<T> GetMagnitude() { return magnitude_; }

  bool IsEmpty() {
    return vx_.empty() && vy_.empty() && orientation_.empty() &&
           magnitude_.empty();
  }

 private:
  std::vector<T> vx_;
  std::vector<T> vy_;
  std::vector<T> orientation_;
  std::vector<T> magnitude_;
};

} /* namespace oflow */

#endif /* SRC_OPTICAL_FLOW_OPTICAL_FLOW_H_ */
