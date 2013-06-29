/*
 * Sequence.cpp
 *
 *  Created on: 02/08/2011
 *      Author: biel
 */
//TODO: repassar els noms dels fitxer, fileName
#include "Sequence.h"
/* Open a file, to grab or read a sequence
 * string fileName: the name of file
 * int fAction = enum { READ=0, WRITE=1}
 * if fAction > 0 then grabFrames
 */
Sequence::Sequence(string fileName, int fAction, bool &ok){

	bool op1,op2;
	action = fAction;

	string d = "Depth.avi";
	string rgb = "RGB.avi";

	if(action == FileStorage::WRITE){

		op1 = vrgb.open(fileName+rgb, CV_FOURCC('D','I','V','X'), 30.0f, Size(640,480), true);
	    op2 = vdepth.open(fileName+d,0, 30.0f, Size(640,480), true);

	}else{

		op1 = crgb.open(fileName+rgb);
		op2 = cdepth.open(fileName+d);
	}

	ok = op1 && op2;
}

void Sequence::grabFrames(Mat rgb, Mat auxDepth){

	Mat depth(480, 640, CV_8UC3);

	for (int i = 0; i < 640;i++) {
		for(int j = 0; j < 480; j++){

			Point2i punt;
			punt.x = i;
			punt.y = j;

			short dval = auxDepth.at<short>(punt);
			unsigned char cc[2];

			cc[0] = dval;
			cc[1] = (dval) >> 8;

			depth.at<Vec3b>(punt) = Vec3b(cc[0], cc[1], 0);
		}
	}

	
	vrgb << rgb;
	vdepth << depth;

}

void Sequence::setFrame(int f){
	crgb.set(CV_CAP_PROP_POS_FRAMES, f);
	cdepth.set(CV_CAP_PROP_POS_FRAMES, f);
}

bool Sequence::readFrames(Mat &rgb, Mat &depth){
	
	Mat auxDepth(480, 640,  CV_8UC3);

	if (crgb.read(rgb) && cdepth.read(auxDepth)){

		for (int i = 0; i < 640; i++) {
			for(int j = 0; j < 480; j++){

				Point2i punt;
				punt.x = i;
				punt.y = j;

				Vec3b dval = auxDepth.at<Vec3b>(punt);
				short aux  = (dval[0] ) + ((dval[1]) << 8);
				depth.at<short>(punt) = aux;
			}
		}

		return true;
	}
	return false;
}


Sequence::~Sequence() {

}
