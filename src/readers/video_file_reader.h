/*
 * video_file_reader.h
 *
 *  Created on: Feb 10, 2016
 *      Author: cody
 */

#ifndef SRC_READERS_VIDEO_FILE_READER_H_
#define SRC_READERS_VIDEO_FILE_READER_H_
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

namespace oflow {

class VideoFileReader {
  explicit VideoFileReader(const std::string &filename) : filename_{filename} {}
  std::vector<uint32_t> ReadFrame() {
    std::vector<uint32_t> frame;
    return frame;
  }

 private:
  std::string filename_;
};

} /* namespace oflow */

#endif /* SRC_READERS_VIDEO_FILE_READER_H_ */
