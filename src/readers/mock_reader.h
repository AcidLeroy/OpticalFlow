/*
 * mock_reader.h
 *
 *  Created on: Feb 10, 2016
 *      Author: cody
 */

#ifndef SRC_READERS_MOCK_READER_H_
#define SRC_READERS_MOCK_READER_H_
#include <string>
#include <memory>
#include <cstdint>
#include "image.h"

#include "gmock/gmock.h"

namespace oflow {

class MockReader {
 public:
  // Mock uninteresting constructor
  explicit MockReader(const std::string& filename) {}

  MOCK_METHOD0(ReadFrameUMat, std::shared_ptr<Image<cv::UMat>>());
  MOCK_METHOD0(ReadFrameMat, std::shared_ptr<Image<cv::Mat>>());

  template <typename T>
  std::shared_ptr<Image<T>> ReadFrame();
};

template <>
std::shared_ptr<Image<cv::UMat>> MockReader::ReadFrame() {
  return ReadFrameUMat();
}

template <>
std::shared_ptr<Image<cv::Mat>> MockReader::ReadFrame() {
  return ReadFrameMat();
}

} /* namespace oflow */

#endif /* SRC_READERS_MOCK_READER_H_ */
