/*
 * optical_flow_mat_benchmark.cpp
 *
 *  Created on: May 22, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */

#include <celero/Celero.h>

#include "../readers/video_file_reader.h"
#include "../datatypes/image.h"
#include "../optical_flow/motion_estimation.h"
#include "../optical_flow/optical_flow.h"
#include "../optical_flow/lk_flow.h"
#include "../optical_flow/farneback_flow.h"

#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
CELERO_MAIN

namespace oflow {

const std::string filename{
    "/Users/cody/Dropbox/CodyShared/PreProposal_AOLMEFall2015/"
    "ActivityClassifier/data/writing/Cropped/seg2.mov"};

BASELINE(OpticalFlowBenchmarks, NoGpuSupport, 3, 3) {
  std::shared_ptr<oflow::VideoFileReader> f(
      new oflow::VideoFileReader(filename));
  oflow::MotionEstimation<oflow::VideoFileReader, cv::Mat> me(f);
  std::shared_ptr<oflow::FarneBackFlow> my_flow(new oflow::FarneBackFlow());
  me.EstimateMotion(my_flow);
}

BENCHMARK(OpticalFlowBenchmarks, GpuSupport, 3, 3) {
  std::shared_ptr<oflow::VideoFileReader> f(
      new oflow::VideoFileReader(filename));
  oflow::MotionEstimation<oflow::VideoFileReader, cv::UMat> me(f);
  std::shared_ptr<oflow::FarneBackFlow> my_flow(new oflow::FarneBackFlow());
  me.EstimateMotion(my_flow);
}
}
