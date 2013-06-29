/*
 * Sequence.h
 *
 *  Created on: 02/08/2011
 *      Author: biel
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

class Sequence {
public:

	Sequence(string fileName, int fAction, bool &ok);
	virtual ~Sequence();
	void grabFrames(Mat rgb, Mat auxDepth);
	bool readFrames(Mat &rgb, Mat &depth);
	void setFrame(int f);

private:

	int action;

	VideoWriter vrgb;
	VideoWriter vdepth;

	VideoCapture crgb;
	VideoCapture cdepth;


};

#endif /* SEQUENCE_H_ */
