

#include "../readers/video_file_reader.h"
#include "../datatypes/image.h"
#include "../optical_flow/motion_estimation.h"
#include "../optical_flow/optical_flow.h"
#include "../optical_flow/lk_flow.h"
#include "../optical_flow/farneback_flow.h"

#include "opencv2/core/utility.hpp"
//#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"

#include <string>
#include <iostream>

void RunFarneBackFlow(const std::string& video_file, int classification) {
  std::shared_ptr<oflow::VideoFileReader> f(
      new oflow::VideoFileReader(video_file));
  typedef cv::UMat img_type;
  oflow::MotionEstimation<oflow::VideoFileReader, img_type> me(f);
  std::shared_ptr<oflow::FarneBackFlow> my_flow(new oflow::FarneBackFlow());
  auto features = me.EstimateMotion(my_flow);
  std::cout << oflow::stats::PrintFeatures(features, classification);
}

void RunLkFlow(const std::string& video_file, int classification) {
  std::shared_ptr<oflow::VideoFileReader> f(
      new oflow::VideoFileReader(video_file));
  typedef cv::UMat img_type;
  oflow::MotionEstimation<oflow::VideoFileReader, img_type> me(f);
  std::shared_ptr<oflow::LKFlow<img_type>> my_flow(
      new oflow::LKFlow<img_type>());
  auto features = me.EstimateMotion(my_flow);
  std::cout << oflow::stats::PrintFeatures(features, classification);
}

int main(int argc, const char* argv[]) {
  const char* keys =
      "{ h help           |                 | print help message }"
      "{ c classification | 0               | classification of video being "
      "processed }"
      "{ v video          | test.mov        | use video as input }"
      "{ t type           | farneback       | type of optical flow to use "
      "(farneback or lucas)}";

  cv::CommandLineParser cmd(argc, argv, keys);

  if (cmd.has("help")) {
    std::cout << "Usage: extract_features [options]" << std::endl;
    std::cout << "Available options:" << std::endl;
    cmd.printMessage();
    return EXIT_SUCCESS;
  }

  std::string vdofile = cmd.get<std::string>("video");
  int classification = cmd.get<int>("classification");
  std::string type = cmd.get<std::string>("type");

  if (!cmd.check()) {
    cmd.printErrors();
    return 0;
  }

  if (type.compare("farneback") == 0) {
    RunFarneBackFlow(vdofile, classification);
  } else if (type.compare("lucas") == 0) {
    RunLkFlow(vdofile, classification);
  }
}
