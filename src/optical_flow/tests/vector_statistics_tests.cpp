/*
 * vector_statistics_tests.cpp
 *
 *  Created on: Feb 20, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <array>
#include "vector_statistics.h"

namespace oflow {

TEST(VectorStatistics, CvConstructor) {
  vector_type points_;
  VectorStatistics vs(points_);
}

}  // End namespace oflow
