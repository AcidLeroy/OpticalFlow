# Optical Flow
[![Build Status](https://travis-ci.org/AcidLeroy/OpticalFlow.svg?branch=master)](https://travis-ci.org/AcidLeroy/OpticalFlow)

This library contains C++ code that is used to extract features from videos using optical Flow.
Currently I have implemented two types of optical flow, *Farneback* and *Lucas Kanade*. The first is a dense optical flow method and the latter uses a more sparse representation.
The idea of the of the C++ code was to attempt to capture
the essense of the MATLAB code located in src/matlab.

Furthermore, R code is included to then classify the features.
This code is located in src/r_code. The idea is to take the output of the C++ code and run it as the input to the R code
to correctly classify the motions in the video. Particularly,
This is an attempt to classiy typing vs no typing and writing
vs no writing in videos provide through AOLME (Advancing
  Out of School Learning in Mathematics and Engineering).

  ## Building
  In order to build the C++, you must have the following
  requirements:

  - cmake
  - OpenCV >= 3.1

  In order to run the scripts, you must have:
  - MATLAB >= 2015b
  - R

  If you have trouble, refer to the .travis.yml file. All
  the necessary building tools are installed in that file.
