//#include "opencv2/highgui/highgui.hpp"
//#include "opencv/cv.h"
//#include "opencv/highgui.h"
//
//#include <stdio.h>
//
//#include "Sequence.h"
//#include "CVKinectWrapper.h"
//
//#define HORIZONTAL 1
//using namespace cv;
//int frameNumber =0;
//int main(int argc, char **argv)
//{
//	bool ok;
//
//	Mat rgbImage(480, 640, CV_8UC3, Scalar::all(0));
//	Mat rawDepth(480, 640, CV_16UC1, Scalar::all(0));
//
//	CVKinectWrapper *wrapper = CVKinectWrapper::getInstance();
//	if(!wrapper->init("SamplesConfig.xml")) exit(0);
//
//	Sequence *seq = new Sequence("Pink Glove X",FileStorage::WRITE,ok);
//
//	if(!ok){
//
//		cout << "Couldn't create / open the video files";
//		return -1;
//	}
//
//	for(;;) {
//
//		if(! wrapper->update()) break;
//
//		wrapper->getRGB(&rgbImage);
//		wrapper->getRawDepth(&rawDepth);
//
//
//
//		char textDepth[20];
//		sprintf(textDepth, "Frame %d \n", frameNumber);
//		putText(rgbImage, textDepth, cvPoint(10, 30), FONT_HERSHEY_PLAIN, 1, cv::Scalar(255,255,255,0));
//			
//
//
//		seq->grabFrames(rgbImage, rawDepth);
//
//		imshow("RGB", rgbImage);
//
//		frameNumber++;
//
//		int key = waitKey(100/30);
//		if( key == 27 ) exit(0); //Esc
//	}
//
//	return 0;
//}
