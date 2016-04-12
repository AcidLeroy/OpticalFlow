/*
 * overlap_add.h
 *
 *  Created on: Apr 9, 2016
 *      Author: Cody W. Eilar <Cody.Eilar@Gmail.com>
 */
#include <opencv2/imgproc/imgproc.hpp>

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

}  // end namespace oflow

#endif /* SRC_EXPERIMENTS_OVERLAP_ADD_H_ */
