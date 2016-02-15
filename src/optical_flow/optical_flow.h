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

class OpticalFlow {
 public:
  std::vector<double> GetVx() { return vx_; }
  std::vector<double> GetVy() { return vy_; }
  std::vector<double> GetOrientation() { return orientation_; }
  std::vector<double> GetMagnitude() { return magnitude_; }
  void SetVx(std::vector<double> &vx) { vx_ = std::move(vx); }
  void SetVy(std::vector<double> &vy) { vy_ = std::move(vy); }
  void SetOrientation(std::vector<double> &orientation) {
    orientation_ = std::move(orientation);
  }
  void SetMagnitude(std::vector<double> &magnitude) {
    magnitude_ = std::move(magnitude);
  }

 private:
  std::vector<double> vx_;
  std::vector<double> vy_;
  std::vector<double> orientation_;
  std::vector<double> magnitude_;
};

} /* namespace oflow */

#endif /* SRC_OPTICAL_FLOW_OPTICAL_FLOW_H_ */
