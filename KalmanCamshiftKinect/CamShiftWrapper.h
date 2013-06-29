/****************************************************************************
 *                                                                           *
 *  Clase que encapsula el trabajo con CAMSHIFT y lo simplifica              *
 *  para poder aplicarlo varias veces sobre la misma imagen					 *
 *																			 *
 ****************************************************************************/

#ifndef CAMSHIFTWRAPPER_H_
#define CAMSHIFTWRAPPER_H_

#include <math.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;


class CamShiftWrapper {

public:
	CamShiftWrapper();
	void updateImage(Mat imagen);
	void setSelection(Rect sel);
	int  getTrackObject();
	void setTrackObject(int val);
	void setShowHist(bool v);
	void setShowBackproj(bool v);
	void setShowHSV(bool v);
	void setColor(Scalar c);
	void setTrackWindowPos(Point p);
	void calcHistogram(Rect selection);
	void setVmin(int);
	void setVmax(int);
	void setSmin(int);
	Point* getPOI();

private:


	Mat image;
	int trackObject;
	int vmin;
	int vmax;
	int smin;
	Rect selection;
	Point origin;

	/* Imágenes para el camshift1 */
	Mat hsv;
	Mat mask;
	Mat hue;
	Mat hist;
	Mat histimg;
	Mat backproj;

	int hsize; //Tamaño del histograma
	float hranges[2]; //rangos del histograma??
	const float* phranges;
	Rect trackWindow;

	bool showHist;
	bool showBackproj;
	bool showHSV;

	Scalar color;
	Point *POI;

	void track();
};

#endif /* CAMSHIFTWRAPPER_H_ */
