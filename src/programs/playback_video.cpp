/*
 * playback_video.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include "../readers/video_file_reader.h"
#include "../datatypes/image.h"

#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"

#include <string>
#include <iostream>

int main(int argc, const char* argv[]) {
  const char* keys =
      "{ h help           |                 | print help message }"
      "{ v video          | test.mov        | use video as input }";

  cv::CommandLineParser cmd(argc, argv, keys);

  if (cmd.has("help")) {
    std::cout << "Usage: playback_video [options]" << std::endl;
    std::cout << "Available options:" << std::endl;
    cmd.printMessage();
    return EXIT_SUCCESS;
  }

  std::string vdofile = cmd.get<std::string>("video");
  std::cout << "vdofile = " << vdofile << std::endl;

  if (!cmd.check()) {
    cmd.printErrors();
    return 0;
  }

  oflow::VideoFileReader f(vdofile);

  while (true) {
    auto image = f.ReadFrame();
    if (image == nullptr) {
      std::cout << "No more frames to read." << std::endl;
      break;
    }
    auto frame = image->GetMat();
    cv::imshow("Read image", *frame);
    if (::cv::waitKey(30) >= 0) break;
  }
}
