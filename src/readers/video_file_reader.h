/*
 * video_file_reader.h
 *
 *  Created on: Feb 10, 2016
 *      Author: cody
 */

#ifndef SRC_READERS_VIDEO_FILE_READER_H_
#define SRC_READERS_VIDEO_FILE_READER_H_
#include "image.h"

#include "opencv2/opencv.hpp"

#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>

namespace oflow {

class VideoFileReader {
 public:
  explicit VideoFileReader(const std::string &filename) : filename_{filename} {
    if (filename_ == "") {
      cap_.reset(new cv::VideoCapture());
      cap_->open(0);
    } else {
      cap_.reset(new cv::VideoCapture(filename_));
    }

    if (!cap_->isOpened()) {
      throw std::runtime_error("OPENCV could not open camera or file.");
    }
  };
  template <typename mat_type>
  std::shared_ptr<Image<mat_type>> ReadFrame() {
    mat_type color_frame;
    auto x = cap_->read(color_frame);
    if (!x) {
      return nullptr;
    }
    auto gray_frame = std::make_shared<mat_type>();
    cv::cvtColor(color_frame, *gray_frame, cv::COLOR_BGR2GRAY);
    return std::make_shared<Image<mat_type>>(Image<mat_type>(gray_frame));
  }

 private:
  std::string filename_;
  std::unique_ptr<cv::VideoCapture> cap_;
};

} /* namespace oflow */

#endif /* SRC_READERS_VIDEO_FILE_READER_H_ */
