/*
 * vector_statistics.h
 *
 *  Created on: Feb 20, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include <array>
#include <opencv2/imgproc/imgproc.hpp>

#ifndef SRC_OPTICAL_FLOW_VECTOR_STATISTICS_H_
#define SRC_OPTICAL_FLOW_VECTOR_STATISTICS_H_
namespace oflow {
using vector_type = std::array<std::vector<cv::Point2f>, 2>;
class VectorStatistics {
 public:
  explicit VectorStatistics(const vector_type &points) : points_{points} {};

 private:
  const vector_type &points_;
};
}  // end namespace oflow

#endif /* SRC_OPTICAL_FLOW_VECTOR_STATISTICS_H_ */
