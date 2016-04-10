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
cv::UMat OverlapAdd(const cv::UMat &x_n, const cv::UMat &h_n);
cv::UMat Dft2(const cv::UMat &x_n, const cv::UMat &h_n);
cv::UMat Dft(const cv::UMat &x_n);

}  // end namespace oflow

#endif /* SRC_EXPERIMENTS_OVERLAP_ADD_H_ */
