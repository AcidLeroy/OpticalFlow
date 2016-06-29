

#include "../readers/video_file_reader.h"
#include "../datatypes/image.h"
#include "../optical_flow/motion_estimation.h"
#include "../optical_flow/optical_flow.h"
#include "../optical_flow/lk_flow.h"
#include "../optical_flow/farneback_flow.h"

#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"

#include <string>
#include <iostream>

int main(int argc, const char* argv[]) {
  const char* keys =
      "{ h help           |                 | print help message }"
      "{ c classification | 0               | classification of video being "
      "processed }"
      "{ v video          | test.mov        | use video as input }";

  cv::CommandLineParser cmd(argc, argv, keys);

  if (cmd.has("help")) {
    std::cout << "Usage: extract_features [options]" << std::endl;
    std::cout << "Available options:" << std::endl;
    cmd.printMessage();
    return EXIT_SUCCESS;
  }

  std::string vdofile = cmd.get<std::string>("video");
  // std::cout << "vdofile = " << vdofile << std::endl;
  int classification = cmd.get<int>("classification");
  // std::cout << "Classification is : " << classification << std::endl;

  if (!cmd.check()) {
    cmd.printErrors();
    return 0;
  }

  // oflow::VideoFileReader f(vdofile);
  std::shared_ptr<oflow::VideoFileReader> f(
      new oflow::VideoFileReader(vdofile));
  typedef cv::UMat img_type;
  oflow::MotionEstimation<oflow::VideoFileReader, img_type> me(f);
  // std::shared_ptr<oflow::FarneBackFlow> my_flow(new oflow::FarneBackFlow());
  std::shared_ptr<oflow::FarneBackFlow> my_flow(new oflow::FarneBackFlow());
  auto features = me.EstimateMotion(my_flow);
  std::cout << oflow::stats::PrintFeatures(features, classification);
}
