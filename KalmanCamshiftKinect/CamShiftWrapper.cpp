/*
 * CamShiftWrapper.cpp
 *
 *  Created on: 02/11/2011
 *      Author: Gabriel
 */

#include "CamShiftWrapper.h"

CamShiftWrapper::CamShiftWrapper(): 
									trackObject(0),
									vmin(10),
									vmax(256),
									smin(30),
									hsize(16),
									showHist(true),
									showBackproj(false),
									showHSV(false)
{
	histimg  = Mat::zeros(200, 320, CV_8UC3);
	hranges[0] = 0;
	hranges[1] = 180;
	phranges = hranges;
	color = Scalar(255,0, 0);
	POI = new Point(0,0);


}


void CamShiftWrapper::updateImage(Mat imagen){
	image = imagen;
	cvtColor(imagen, hsv, CV_BGR2HSV);	 //Transforma la imagen a HSV (Hue, Saturation, Value)
	//for(i=0;i< (heightc);i++) for(j=0;j<(widthc);j++)
	//{
	//	temp=datac[i*stepc+j*channelsc+1]+units;/*increas the saturaion component is the second arrray.*/
	//	if(temp>255) datac[i*stepc+j*channelsc+1]=255;
	//	else datac[i*stepc+j*channelsc+1]=temp;
	//}
	if(showHSV)
		imshow("HSV", hsv);
	track();
	if(showHist)
		imshow( "Histogram", histimg );
}


void CamShiftWrapper::setSelection(Rect sel){
	selection = sel;
}
void CamShiftWrapper::setTrackObject(int val){
	trackObject = val;
}

void CamShiftWrapper::setVmin(int v){
	vmin = v;
}
void CamShiftWrapper::setVmax(int v){
	vmax = v;
}
void CamShiftWrapper::setSmin(int v){
	smin = v;
}


void CamShiftWrapper::setTrackWindowPos(Point p){
	trackWindow.x = p.x;
	trackWindow.y = p.y;
}
int CamShiftWrapper::getTrackObject(){
	return trackObject;
}

void CamShiftWrapper::setShowHist(bool v){
	showHist = v;
}

void CamShiftWrapper::setShowHSV(bool v){
	showHSV = v;
}

void CamShiftWrapper::setShowBackproj(bool v){
	showBackproj = v;
}

void CamShiftWrapper::setColor(Scalar c){
	color = c;
}

Point* CamShiftWrapper::getPOI(){
		return POI;
}

void CamShiftWrapper::calcHistogram(Rect selection){
		Mat roi(hue, selection), maskroi(mask, selection);



		calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
		//normalize(hist, hist, 0, 255, CV_MINMAX);
			
		//imshow("Hue", roi);
		//imshow("Hist", hist);
		//imshow("Mask", maskroi);

		//Histograma
			{
				histimg = Scalar::all(0);
				int binW = histimg.cols / hsize;
				Mat buf(1, hsize, CV_8UC3);
				for( int i = 0; i < hsize; i++ )
					buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);
				cvtColor(buf, buf, CV_HSV2BGR);
				
				for( int i = 0; i < hsize; i++ )
				{
					int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows/255);
					rectangle( histimg, Point(i*binW,histimg.rows),
							Point((i+1)*binW,histimg.rows - val),
							Scalar(buf.at<Vec3b>(i)), -1, 8 );
				}
			}
}


void CamShiftWrapper::track(){
	if(trackObject)  
	{
		int _vmin = vmin, _vmax = vmax;

		//Elimina los valores de Value situados entre vmin y vmax
		//Elimina los valores de saturacion situados entre smin y 256
		inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),	Scalar(180, 256, MAX(_vmin, _vmax)), mask);
		//imshow("MASK", mask);
		int ch[] = {0, 0};
		hue.create(hsv.size(), hsv.depth());

		//matriz origen, núm matrices origen, matriz destino, núm matrices destino, orden de mezcla de canales (del canal 0 en origen al canal 0 en destino), numero de pares de indices de canales
		//Lo que hace es extraer el hue de HSV
		mixChannels(&hsv, 1, &hue, 1, ch, 1);
		
		//imshow("HUE", hue);º


		//Realiza el cálculo del histograma
		if( trackObject < 0 )
		{
			calcHistogram(selection);
			trackWindow = selection;
			trackObject = 1;
			
		}

		calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
		//imshow("Back Project", backproj);
		backproj &= mask;
		//erode(backproj, backproj, Mat());
		//dilate(backproj,backproj, Mat());
		//imshow("Back Project AND", backproj);
		RotatedRect trackBox = CamShift(backproj, trackWindow,
								TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ));
		POI = new Point(trackBox.center);


		if( trackWindow.area() <= 1 || trackWindow.area() > (image.rows * image.cols)*0.3)
		{
			int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
			//trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
			//					trackWindow.x + r, trackWindow.y + r) &
			//					Rect(0, 0, cols, rows);
			trackWindow = Rect(0, 0, cols, rows);
		}

		if( showBackproj ) {
			imshow("BackProj",backproj);
		}
		
		rectangle( image, trackWindow.tl(), trackWindow.br(), Scalar(255,0,0), 2, CV_AA );

		ellipse( image, trackBox, color, 3, CV_AA );
		circle( image, *POI, 1, color );
	}
}
