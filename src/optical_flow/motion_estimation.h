/*
 * motion_estimation.h
 *
 *  Created on: Feb 10, 2016
 *      Author: cody
 */

#ifndef SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_
#define SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_

#include "optical_flow.h"
#include "vector_statistics.h"
#include "../utilities/utilities.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <typeinfo>
#include <sstream>
#include <fstream>
namespace oflow {

class MotionEstimationException : public std::runtime_error {
 public:
  explicit MotionEstimationException(const std::string& what_arg)
      : runtime_error(what_arg) {}
  explicit MotionEstimationException(const char* what_arg)
      : runtime_error(what_arg) {}
};

namespace stats {
/**
 * Get the histogram for the image around the motion.
 */
template <typename T>
void GetHistoAround(const T& thresholded_motion, int disk_size,
                    const T& gray_scale_image, T* bg_histogram) {
  T dialated;
  // Get disk
  T disk = cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                     cv::Size(disk_size, disk_size));
  // Dilate motion to get pixels around motion
  cv::dilate(thresholded_motion, dialated, disk);
  T diff_image = dialated - thresholded_motion;
  diff_image.convertTo(diff_image, CV_8U);
  T background;
  gray_scale_image.copyTo(background, diff_image);
  // Get histogram of background intensity
  constexpr int num_bins = 25;
  constexpr float range[] = {0, 256};  // the upper boundary is exclusive
  const float* hist_range = {range};
  bool uniform = true;
  bool accumulate = false;

  T hist;
  cv::calcHist(&background, 1, 0, T(), hist, 1, &num_bins, &hist_range, uniform,
               accumulate);
  if (bg_histogram->empty()) {
    hist.copyTo(*bg_histogram);
  } else {
    (*bg_histogram) = (*bg_histogram) + hist;
  }
}

template <typename T>
void ShowImage(const std::string& name, const T& image) {
  cv::namedWindow(name, 1);
  cv::imshow(name, image);
}

void UpdateHistogram(const cv::Mat_<uint8_t>& thresholded_image,
                     const cv::Mat& intensities, cv::Mat* histogram,
                     const float range[2], int num_bins) {
  // Get histogram of background intensity
  const float* hist_range = {range};
  bool uniform = true;
  bool accumulate = false;
  cv::Mat masked_image;
  intensities.copyTo(masked_image, thresholded_image);
  //  ShowImage("Masked Image", masked_image);
  //  cv::waitKey(0);

  cv::Mat hist;
  cv::calcHist(&masked_image, 1, 0, cv::Mat(), hist, 1, &num_bins, &hist_range,
               uniform, accumulate);
  if (histogram->empty()) {
    hist.copyTo(*histogram);
  } else {
    (*histogram) = (*histogram) + hist;
  }
}

cv::Mat GetLogicalVector(const cv::Mat& intensity_image,
                         const cv::Mat_<uint8_t>& logical_image) {
  cv::Mat valid_values;
  intensity_image.copyTo(valid_values, logical_image);
  return valid_values;
}

void UpdateCentroidAndOrientation(const cv::Mat& thresholded_image,
                                  cv::Mat* orientations, cv::Mat* centroids) {
  cv::Mat labels, stats, current_centroids;
  cv::connectedComponentsWithStats(thresholded_image, labels, stats,
                                   current_centroids);
  // Don't care about background centroid, hence the range.
  if (centroids->empty()) {
    current_centroids(cv::Range(1, current_centroids.rows),
                      cv::Range(0, current_centroids.cols)).copyTo(*centroids);
  } else {
    cv::vconcat(*centroids,
                current_centroids(cv::Range(1, current_centroids.rows),
                                  cv::Range(0, current_centroids.cols)),
                *centroids);
  }

  // std::cout << "centroids are" << std::endl << current_centroids <<
  // std::endl;
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(thresholded_image, contours, cv::RETR_LIST,
                   cv::CHAIN_APPROX_NONE);
  for (int i = 0; i < contours.size(); ++i) {
    // Can only fit an ellipse with 5 points, skip others
    if (contours[i].size() >= 5) {
      cv::RotatedRect result = cv::fitEllipse(contours[i]);
      orientations->push_back(result.angle);
    }
  }
}

cv::Mat CumSum(const cv::Mat& histogram) {
  cv::Mat cumsum =
      cv::Mat::zeros(histogram.rows, histogram.cols, histogram.type());
  cumsum.at<float>(0) = (histogram.at<float>(0));
  for (int i = 1; i < histogram.total(); ++i) {
    cumsum.at<float>(i) = cumsum.at<float>(i - 1) + histogram.at<float>(i);
  }
  return cumsum;
}

void GetCdf(const cv::Mat& input, cv::Mat* output, int num_bins,
            const float range[2]) {
  bool uniform = true;
  bool accumulate = false;
  // This range has to be created to match the histc function in matlab.
  const float new_range[2] = {range[0], range[1] + num_bins + 1};
  const float* i_range = {new_range};
  cv::Mat hist;

  cv::calcHist(&input, 1, 0, cv::Mat(), hist, 1, &num_bins, &i_range, uniform,
               accumulate);
  auto sum = cv::sum(hist);
  cv::divide(CumSum(hist), sum, *output);
}

void GetCentroidCdf(const cv::Mat& centroids, const cv::Mat& gray_frame,
                    cv::Mat* x_cent_cdf, cv::Mat* y_cent_cdf) {
  int num_bins = 25;

  const float xrange[2] = {1, static_cast<float>(gray_frame.cols)};
  cv::Mat_<float> x_cent = centroids.col(0);
  GetCdf(x_cent, x_cent_cdf, num_bins, xrange);
  const float yrange[2] = {1, static_cast<float>(gray_frame.rows)};
  cv::Mat_<float> y_cent = centroids.col(1);
  GetCdf(y_cent, y_cent_cdf, num_bins, yrange);
}

void GetOrientationCdf(const cv::Mat& orientation, cv::Mat* orient_cent_cdf) {
  int num_bins = 25;
  const float xrange[2] = {0, 180};
  GetCdf(orientation, orient_cent_cdf, num_bins, xrange);
}

void NormalizeHistogramCdf(const cv::Mat& hist, cv::Mat* normalized_hist_cdf) {
  auto sum = cv::sum(hist);
  cv::divide(CumSum(hist), sum, *normalized_hist_cdf);
}

std::string ConstructFeatureString(
    const std::vector<cv::Mat>& feature_vectors) {
  std::stringstream oss;
  for (auto& mat : feature_vectors) {
    for (size_t i = 0; i < mat.total(); ++i) {
      oss << mat.at<float>(i) << "\t";
    }
    oss << "1\t";
  }
  oss << std::endl;
  return oss.str();
}

std::vector<std::string> GetLabels() {
  std::vector<std::string> labels{"CenX_CDF",       "CenY_CDF",
                                  "Orient_CDF",     "Histo_CDF",
                                  "Motion_mag_CDF", "Motion_orient_CDF"};
  return labels;
}

std::string PrintFeatures(const std::vector<cv::Mat>& feature_vectors,
                          int classification) {
  auto labels = GetLabels();
  std::string oss;
  for (size_t i = 0; i < feature_vectors.size(); ++i) {
    oss += cv_utils::PrintMatVector(feature_vectors[i]) + ",";
  }

  oss += std::to_string(classification) + '\n';

  return oss;
}

void PrintMinMax(const cv::Mat& mat, const std::string& name) {
  auto const min_max = cv_utils::GetMinMax(mat);
  std::cout << name << " min = " << min_max[0] << " max = " << min_max[1]
            << std::endl;
}
void PrintMinMax(const cv::UMat& mat, const std::string& name) {
  PrintMinMax(mat.getMat(cv::ACCESS_RW), name);
}

void UpdateStats(const cv::Mat& binary_image, const cv::Mat& next_mat,
                 const cv::Mat& magnitude, const cv::Mat& orientation,
                 cv::Mat* orientations, cv::Mat* centroids,
                 cv::Mat* bg_histogram, cv::Mat* magnitude_histogram,
                 cv::Mat* orientation_histogram) {
  cv::Mat bin;
  stats::ShowImage("bin before", binary_image);

  binary_image.convertTo(bin, CV_8U);

  stats::UpdateCentroidAndOrientation(bin, orientations, centroids);
  constexpr int num_bins = 25;
  stats::GetHistoAround(binary_image, num_bins, next_mat, bg_histogram);

  const float magnitude_range[2] = {0, 1};
  // Update magnitude histogram
  stats::UpdateHistogram(bin, magnitude, magnitude_histogram, magnitude_range,
                         num_bins);
  //  stats::ShowImage("bin after", bin);
  //  cv::waitKey(0);
  // Update orientation histogram
  float orientation_range[2];
  orientation_range[0] = 0;
  orientation_range[1] = M_PI;

  stats::UpdateHistogram(binary_image, orientation, orientation_histogram,
                         orientation_range, num_bins);
}

void UpdateStats(const cv::UMat& binary_image, const cv::UMat& next_mat,
                 const cv::Mat& magnitude, const cv::Mat& orientation,
                 cv::Mat* orientations, cv::Mat* centroids,
                 cv::Mat* bg_histogram, cv::Mat* magnitude_histogram,
                 cv::Mat* orientation_histogram) {
  stats::UpdateStats(binary_image.getMat(cv::ACCESS_RW),
                     next_mat.getMat(cv::ACCESS_RW), magnitude, orientation,
                     orientations, centroids, bg_histogram, magnitude_histogram,
                     orientation_histogram);
}

}  // End namespace stats

template <typename ReaderType, typename mat_type>
class MotionEstimation {
 public:
  explicit MotionEstimation(std::shared_ptr<ReaderType> reader)
      : reader_(reader){};

  template <typename OpticalFlowType>
  std::vector<cv::Mat> EstimateMotion(std::shared_ptr<OpticalFlowType> flow) {
    auto current_frame = reader_->template ReadFrame<mat_type>();
    if (current_frame == nullptr)
      throw MotionEstimationException("There are no frames to read!");
    auto next_frame = reader_->template ReadFrame<mat_type>();
    if (next_frame == nullptr)
      throw MotionEstimationException("There is only one frame to read!");

    while (1) {
      OpticalFlow<cv::Mat> stats =
          flow->CalculateVectors(*current_frame, *next_frame);
      mat_type binary_image;
      auto min_max = cv_utils::GetMinMax(stats.GetMagnitude());
      cv::threshold(stats.GetMagnitude(), binary_image, 0.75 * min_max[1], 1,
                    cv::THRESH_BINARY);
      // stats::PrintMinMax(binary_image, "binary image");

      //      stats::ShowImage("binary", binary_image);
      //      stats::ShowImage("current_frame", *current_frame->GetMat());
      //      stats::ShowImage("Magnitude", stats.GetMagnitude());
      //      stats::ShowImage("Velocity X", stats.GetVx());
      //      stats::ShowImage("Velocity Y", stats.GetVy());
      //      cv::waitKey(0);

      stats::UpdateStats(binary_image, *(next_frame->GetMat()),
                         stats.GetMagnitude(), stats.GetOrientation(),
                         &orientations_, &centroids_, &bg_histogram_,
                         &magnitude_histogram_, &orientation_histogram_);

      std::swap(current_frame, next_frame);
      next_frame = reader_->template ReadFrame<mat_type>();
      if (next_frame == nullptr) break;
    }
    //    std::cout << "x centroids = " << std::endl << centroids_ << std::endl;
    // Collect cdfs from video
    cv::Mat x_cent_cdf, y_cent_cdf, orient_cent_cdf, bg_cdf, motion_mag_cdf,
        motion_orient_cdf;

    // std::cout << "Magnitude Histogram " << magnitude_histogram_ << std::endl;

    NormalizeInternalHistogramCdfs(*(current_frame->GetMat()), &x_cent_cdf,
                                   &y_cent_cdf, &orient_cent_cdf, &bg_cdf,
                                   &motion_mag_cdf, &motion_orient_cdf);

    std::vector<cv::Mat> features{x_cent_cdf,      y_cent_cdf,
                                  orient_cent_cdf, bg_cdf,
                                  motion_mag_cdf,  motion_orient_cdf};

    return features;
  }

  void NormalizeInternalHistogramCdfs(const cv::Mat& gray_frame,
                                      cv::Mat* x_cent_cdf, cv::Mat* y_cent_cdf,
                                      cv::Mat* orient_cent_cdf, cv::Mat* bg_cdf,
                                      cv::Mat* motion_mag_cdf,
                                      cv::Mat* motion_orient_cdf) {
    // blob centroid
    stats::GetCentroidCdf(centroids_, gray_frame, x_cent_cdf, y_cent_cdf);

    // blob orientation
    stats::GetOrientationCdf(orientations_, orient_cent_cdf);

    // background histogram
    stats::NormalizeHistogramCdf(bg_histogram_, bg_cdf);

    // motion vector magnitudes
    stats::NormalizeHistogramCdf(magnitude_histogram_, motion_mag_cdf);

    // motion vector orientations
    stats::NormalizeHistogramCdf(orientation_histogram_, motion_orient_cdf);
  }

  void NormalizeInternalHistogramCdfs(const cv::UMat& gray_frame,
                                      cv::Mat* x_cent_cdf, cv::Mat* y_cent_cdf,
                                      cv::Mat* orient_cent_cdf, cv::Mat* bg_cdf,
                                      cv::Mat* motion_mag_cdf,
                                      cv::Mat* motion_orient_cdf) {
    NormalizeInternalHistogramCdfs(gray_frame.getMat(cv::ACCESS_READ),
                                   x_cent_cdf, y_cent_cdf, orient_cent_cdf,
                                   bg_cdf, motion_mag_cdf, motion_orient_cdf);
  }

  template <typename T = double>
  std::vector<uint8_t> ThresholdVector(const std::vector<T>& points_to_thresh,
                                       double percent_of_max) const {
    auto max_val =
        std::max_element(points_to_thresh.cbegin(), points_to_thresh.cend());
    auto max_thresh = *max_val * percent_of_max;
    std::vector<uint8_t> result;
    result.reserve(points_to_thresh.size());
    for (auto& i : points_to_thresh) {
      if (i < max_thresh)
        result.push_back(0);
      else
        result.push_back(1);
    }
    return result;
  }

  template <typename T = float>
  cv::Mat PointsToMat(int num_rows, int num_cols,
                      const std::vector<T>& intensities,
                      std::vector<cv::Point_<float>>& points) {
    cv::Mat test_array = cv::Mat_<T>::zeros(num_rows, num_cols);
    for (size_t i = 0; i < points.size(); ++i) {
      test_array.at<uchar>(points[i].x, points[i].y) = intensities[i];
    }
    return test_array;
  }

 private:
  std::shared_ptr<ReaderType> reader_;
  // Centroids for each optical flow frame
  cv::Mat_<float> centroids_;
  // orientations for each optical flow frame
  cv::Mat_<float> orientations_;
  // Background histogram
  cv::Mat_<float> bg_histogram_;
  // Histogram for orientations of motion vectors
  cv::Mat_<float> orientation_histogram_;
  // Histogram for magnitudes of motion vectors
  cv::Mat_<float> magnitude_histogram_;
};

} /* namespace oflow */

#endif /* SRC_OPTICAL_FLOW_MOTION_ESTIMATION_H_ */
