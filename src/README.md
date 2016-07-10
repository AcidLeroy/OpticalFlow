# SRC directory

This directory contains many different source codes for achieving the `extract_features`
pipeline. The C++ code is the workhorse of the entire project,
the MATLAB code is the inspiration for the C++ code, most of
the python code is for running AWS services, and finally the R
code is used for classifying the output of the C++ code.
Below is a breif description of what is contained in
each of the directories:

- *aws_scripts* - This is all python code for running
`extract_features` on the cloud.
- *datatypes* - A small collection of C++ objects used for
general purpose data strucutures
- *experiments* - Most of the code in here is for bench
marking various aspects of the extract features program.
There are speed comparisons between using `Mat` and `UMat`
datatypes, as well as comparisons for different implementations
of overlap and add.
- *ipython_notebooks* - Any python notebooks used for
generating plots related to `extract_features`.
- *matlab* - The original code used validate that
classification of activities is possible using optical flow
- *optical_flow* - This is where the majority of the C++ library code lives
to extract features using optical flow. There are several
implementations inside the directory. All of the classes are
templatized.
- *programs* - There are a few misellaneous programs inside
this directory, but it also contains the main program of `extract_features`.
- *r_code* - This is where the classifier R code resides.
- *readers* - Originally, the intent of this directory
was to contain several types of video readers. I.e. I wanted
a way to read video formats but I also wanted to be able to read data stored in an hdf5 file. The HDF5 code was never implemented.
- *utilities* - A library of useful utilities for working with videos for this project. Mostly they are related
to *OpenCV* functions. 
