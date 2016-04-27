/*
 * overlap_add.h
 *
 *  Created on: Apr 9, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#ifndef SRC_EXPERIMENTS_OVERLAP_ADD_H_
#define SRC_EXPERIMENTS_OVERLAP_ADD_H_

namespace oflow {
/**
 * Perform overlap and add with input image, x_n, divided into L segments and
 * convolved with square kernel h_n.
 */
cv::UMat OverlapAdd(const cv::UMat &x_n, const cv::UMat &h_n, size_t L);
cv::UMat Dft2(const cv::UMat &x_n, const cv::UMat &h_n);
cv::UMat Dft(const cv::UMat &x_n);
cv::UMat Idft(const cv::UMat &x_n);
/// Pad block image out to num_rows and num cols.
cv::UMat ZeroPad(const cv::UMat &block, int num_rows, int num_cols);

template <typename mat_type>
std::vector<mat_type> SubdivideImage(const mat_type &image_to_divide,
                                     size_t num_rows, size_t num_cols) {
  auto img_rows = image_to_divide.rows;
  auto img_cols = image_to_divide.cols;
  if (num_rows >= img_rows && num_cols >= img_cols) {
    mat_type a;
    image_to_divide.copyTo(a);
    std::vector<mat_type> subdivisions{a};
  }
  std::vector<mat_type> subdivisions;
  return subdivisions;
}

template <typename mat_type>
void convolveDFT(mat_type &A, mat_type &B, mat_type &C) {
  // reallocate the output array if needed
  C.create(abs(A.rows - B.rows) + 1, abs(A.cols - B.cols) + 1, A.type());
  cv::Size dftSize;
  // calculate the size of DFT transform
  dftSize.width = cv::getOptimalDFTSize(A.cols + B.cols - 1);
  dftSize.height = cv::getOptimalDFTSize(A.rows + B.rows - 1);
  // allocate temporary buffers and initialize them with 0's
  mat_type tempA(dftSize, A.type(), cv::Scalar::all(0));
  mat_type tempB(dftSize, B.type(), cv::Scalar::all(0));
  // copy A and B to the top-left corners of tempA and tempB, respectively
  mat_type roiA(tempA, Rect(0, 0, A.cols, A.rows));
  A.copyTo(roiA);
  mat_type roiB(tempB, Rect(0, 0, B.cols, B.rows));
  B.copyTo(roiB);
  // now transform the padded A & B in-place;
  // use "nonzeroRows" hint for faster processing
  dft(tempA, tempA, 0, A.rows);
  dft(tempB, tempB, 0, B.rows);
  // multiply the spectrums;
  // the function handles packed spectrum representations well
  mulSpectrums(tempA, tempB, tempA);
  // transform the product back from the frequency domain.
  // Even though all the result rows will be non-zero,
  // you need only the first C.rows of them, and thus you
  // pass nonzeroRows == C.rows
  dft(tempA, tempA, cv::DFT_INVERSE + cv::DFT_SCALE, C.rows);
  // now copy the result back to C.
  tempA(Rect(0, 0, C.cols, C.rows)).copyTo(C);
  // all the temporary buffers will be deallocated automatically
}

}  // end namespace oflow

#endif /* SRC_EXPERIMENTS_OVERLAP_ADD_H_ */
