/*
 * Utils.h
 *
 *  Created on: 02/11/2011
 *      Author: biel
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;


class Utils {
public:
	Utils();
	virtual ~Utils();

	static Mat BinarySkin(Mat roi);
	static MatND MakeHistogram(Mat image, Rect region, bool draw, String name);
};

#endif /* UTILS_H_ */
