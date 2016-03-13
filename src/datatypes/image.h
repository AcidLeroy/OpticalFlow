/*
 * image.h
 *
 *  Created on: Feb 11, 2016
 *      Author: cody
 */

#ifndef SRC_DATATYPES_IMAGE_H_
#define SRC_DATATYPES_IMAGE_H_
#include <opencv2/imgproc/imgproc.hpp>
#include <memory>

namespace oflow {

/**
 * Class that assumes gray scale images
 */
template <typename mat_type = cv::Mat>
class Image {
 public:
  explicit Image(const std::shared_ptr<mat_type> &mat) : mat_(mat) {}
  Image() = default;
  Image(Image &img) = default;
  Image(Image &&img) = default;
  int GetRows() { return mat_->rows; }
  int GetCols() { return mat_->cols; }
  std::shared_ptr<mat_type> GetMat() const { return mat_; }

 private:
  std::shared_ptr<mat_type> mat_;
};

template <>
class Image<cv::UMat> {
 public:
  explicit Image(const std::shared_ptr<cv::UMat> &mat) : mat_(mat) {}
  Image() = default;
  Image(Image &img) = default;
  Image(Image &&img) = default;
  int GetRows() { return mat_->rows; }
  int GetCols() { return mat_->cols; }
  std::shared_ptr<cv::UMat> GetMat() const { return mat_; }

 private:
  std::shared_ptr<cv::UMat> mat_;
};

} /* namespace oflow */

#endif /* SRC_DATATYPES_IMAGE_H_ */
