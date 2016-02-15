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

#include "gmock/gmock.h"

namespace oflow {

class MockReader {
 public:
  // Mock un interesting constructor
  explicit MockReader(const std::string &filename) {}
  MOCK_CONST_METHOD0(ReadFrame, std::vector<uint32_t>());
};

} /* namespace oflow */

#endif /* SRC_READERS_MOCK_READER_H_ */
