/*
 * utilities.h
 *
 *  Created on: Jun 21, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#ifndef SRC_UTILITIES_UTILITIES_H_
#define SRC_UTILITIES_UTILITIES_H_

#include <opencv2/core.hpp>
#include <cstdint>
#include <string>
#include <iostream>
#include <array>

namespace oflow {
namespace cv_utils {

// Default Case
template <int T>
struct cv_type {
  typedef int type;
};

template <>
struct cv_type<CV_32F> {
  typedef float type;
};

template <>
struct cv_type<CV_64F> {
  typedef double type;
};

template <>
struct cv_type<CV_32S> {
  typedef int type;
};

template <>
struct cv_type<CV_16S> {
  typedef int16_t type;
};

template <>
struct cv_type<CV_16U> {
  typedef uint16_t type;
};

template <typename T>
std::string CreateMatString(const cv::Mat &mat) {
  std::string oss;
  for (size_t i = 0; i < mat.total() - 1; ++i) {
    oss += std::to_string(mat.at<T>(i)) + " ";
  }
  oss += std::to_string(mat.at<T>(mat.total() - 1));
  return oss;
}

std::string PrintMatVector(const cv::Mat &mat) {
  switch (mat.type()) {
    case CV_32F:
      return CreateMatString<float>(mat);
    case CV_64F:
      return CreateMatString<double>(mat);
    default:
      return CreateMatString<int32_t>(mat);
  }
}

std::array<double, 2> GetMinMax(const cv::Mat &mat) {
  double min_val;
  double max_val;
  cv::Point min_loc;
  cv::Point max_loc;
  cv::minMaxLoc(mat, &min_val, &max_val, &min_loc, &max_loc);
  return std::array<double, 2>{{min_val, max_val}};
}

std::string type2str(int type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);
  std::cout << "type= " << type << std::endl;

  switch (depth) {
    case CV_8U:
      r = "8U";
      break;
    case CV_8S:
      r = "8S";
      break;
    case CV_16U:
      r = "16U";
      break;
    case CV_16S:
      r = "16S";
      break;
    case CV_32S:
      r = "32S";
      break;
    case CV_32F:
      r = "32F";
      break;
    case CV_64F:
      r = "64F";
      break;
    default:
      r = "User";
      break;
  }

  r += "C";
  r += (chans + '0');

  return r;
}

}  // end namespace cv_utils
}  // end namespace oflow

#endif /* SRC_UTILITIES_UTILITIES_H_ */
