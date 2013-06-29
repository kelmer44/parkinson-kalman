/*
 * Utils.cpp
 *
 *  Created on: 02/11/2011
 *      Author: biel
 */

#include "Utils.h"


Utils::Utils() {
	// TODO Auto-generated constructor stub
}

Mat Utils::BinarySkin(Mat roi){

	int max_RGB = 0;
	int min_RGB = 0;

	Mat prob(roi.rows, roi.cols, CV_8UC1, Scalar::all(0));

	MatIterator_<Vec3b> it = roi.begin<Vec3b>(),
					    it_end = roi.end<Vec3b>();

	MatIterator_<uchar> itBin = prob.begin<uchar>(),
						    itBin_end = prob.end<uchar>();


	for(; it != it_end; ++it, ++itBin){

		Vec3b pixel = *it;

		//Càlcul màxim i mínims
		max_RGB = __max(pixel[0],pixel[1]);
		max_RGB = __max(max_RGB, pixel[2]);
		min_RGB= __min(pixel[0],pixel[1]);
		min_RGB = __min(min_RGB, pixel[2]);


		if((pixel[2]>95)&&(pixel[1]>40)&&(pixel[0]>20)&&(max_RGB-min_RGB>15)&&
				(abs(pixel[2]-pixel[1])>15)&&
				(pixel[2]>pixel[1])&&(pixel[2]>pixel[0])){

			itBin[0]= 255;

		}
	}
	return prob;
}

Scalar hsv2rgb( float hue ){

    int rgb[3], p, sector;
    static const int sector_data[][3]=
        {{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
    hue *= 0.033333333333333333333333333333333f;
    sector = cvFloor(hue);
    p = cvRound(255*(hue - sector));
    p ^= sector & 1 ? 255 : 0;

    rgb[sector_data[sector][0]] = 255;
    rgb[sector_data[sector][1]] = 0;
    rgb[sector_data[sector][2]] = p;

    return cvScalar(rgb[2], rgb[1], rgb[0],0);
}

MatND Utils::MakeHistogram(Mat image, Rect region, bool draw, String name){

	MatND hist;
	Mat dst(480, 640, CV_8UC3);
	dst = image.clone();
	cvtColor(dst, dst, CV_BGR2HSV);
	int hdims = 16;
	int histSize[] = {hdims};
	float hranges[] = { 0, 180 };
	const float* ranges[] = {hranges};

	int channels[] = {0};

	Mat roi(dst, region);

	calcHist(&roi,1,channels, Mat(), hist, 1, histSize,
			ranges,true, false);

	//DEBUG
	if(draw){

		double maxVal = 0;
		minMaxLoc(hist, 0, &maxVal, 0, 0);

		Mat histImg = Mat::zeros(480, 640, CV_8UC3);

		int scale = histImg.cols /hdims;

		for( int h = 0; h < hdims; h++ ){

			float binVal = (hist.at<float>(h)/maxVal)*histImg.rows;

			rectangle( histImg, Point(h*scale, histImg.rows),
					Point( (h+1)*scale, histImg.rows-binVal),
					hsv2rgb(h*180.f/hdims),CV_FILLED );

		}

		Mat dst(240,320,  CV_8UC3);
		cv::resize(histImg, dst,dst.size());
		namedWindow( name);
		cvMoveWindow(name.c_str() ,1050,0);
		imshow( name, dst );

		histImg.release();
		//END_DEBUG
	}

	return hist;
}



Utils::~Utils() {
	// TODO Auto-generated destructor stub
}

