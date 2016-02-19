/*
 * image.h
 *
 *  Created on: Feb 11, 2016
 *      Author: cody
 */

#ifndef SRC_DATATYPES_IMAGE_H_
#define SRC_DATATYPES_IMAGE_H_
#include <opencv2/imgproc/imgproc.hpp>

namespace oflow {

/**
 * Class that assumes gray scale images
 */
class Image {
 public:
  explicit Image(const std::shared_ptr<cv::Mat> &mat) : mat_(mat) {}
  Image() = default;
  int GetRows() { return mat_->rows; }
  int GetCols() { return mat_->cols; }
  std::shared_ptr<cv::Mat> GetMat() const { return mat_; }

 private:
  std::shared_ptr<cv::Mat> mat_;
};

} /* namespace oflow */

#endif /* SRC_DATATYPES_IMAGE_H_ */
