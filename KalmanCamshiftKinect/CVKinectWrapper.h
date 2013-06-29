/****************************************************************************
 *                                                                           *
 *  OpenCVNIWrapper                                                          *
 *  Copyright (C) 2011 PrimeSense Ltd.                                       *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU Lesser General Public License as published *
 *  by the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  See <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
 *  By Gabriel Sanmartín & Gabriel Moyà                                      *
 *  gabriel.sanmartin@usc.es & gabriel.moya@uib.es                           *
 ****************************************************************************/

#ifndef CVKINECTWRAPPER_H_
#define CVKINECTWRAPPER_H_

#include <XnOS.h>
#include <XnCppWrapper.h>

#include <math.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/highgui/highgui.hpp"

using namespace xn;
using namespace cv;
using namespace std;


#define DISPLAY_MODE_OVERLAY	1
#define DISPLAY_MODE_DEPTH		2
#define DISPLAY_MODE_IMAGE		3
#define DEFAULT_DISPLAY_MODE	DISPLAY_MODE_OVERLAY

class CVKinectWrapper {

public:

	bool init(string CalibFilePath);
	static CVKinectWrapper* getInstance();

	bool update();

	void getRGB(Mat *rgb);
	void getRawDepth(Mat *rawDepth);
	void getCombo(Mat *combo);
	void getDisplayDepth(Mat *displayDepth);

private:
	static CVKinectWrapper *_instance;

	CVKinectWrapper();
	virtual ~CVKinectWrapper();


	string CalibFilePath;
	bool started;

	Context g_context;

	ScriptNode g_scriptNode;
	DepthGenerator g_depth;
	ImageGenerator g_image;

	DepthMetaData g_depthMD;
	ImageMetaData g_imageMD;

	Mat *_rgbImage;
	Mat *_depthImage;
	Mat *_comboImage;
	Mat *_rawDepth;
};

#endif /* CVKINECTWRAPPER_H_ */
