/*
 * video_file_reader.cpp
 *
 *  Created on: Feb 10, 2016
 *      Author: cody
 */

#include "video_file_reader.h"
#include "image.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <memory>

namespace oflow {

std::shared_ptr<Image<cv::Mat>> VideoFileReader::ReadFrame() {
  cv::UMat color_frame;
  auto x = cap_->read(color_frame);
  if (!x) {
    return nullptr;
  }
  auto gray_frame = std::make_shared<cv::Mat>();
  cv::cvtColor(color_frame, *gray_frame, cv::COLOR_BGR2GRAY);
  return std::make_shared<Image<cv::Mat>>(Image<cv::Mat>(gray_frame));
}

} /* namespace oflow */
