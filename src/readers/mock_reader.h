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

  MOCK_METHOD0(ReadFrame, std::shared_ptr<Image>());
};

} /* namespace oflow */

#endif /* SRC_READERS_MOCK_READER_H_ */
