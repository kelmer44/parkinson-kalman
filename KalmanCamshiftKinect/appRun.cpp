/****************************************************************************
 *                                                                           *
 *  OpenNI 1.x Alpha                                                         *
 *  Copyright (C) 2011 PrimeSense Ltd.                                       *
 *                                                                           *
 *  This file is part of OpenNI.                                             *
 *                                                                           *
 *  OpenNI is free software: you can redistribute it and/or modify           *
 *  it under the terms of the GNU Lesser General Public License as published *
 *  by the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  OpenNI is distributed in the hope that it will be useful,                *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU Lesser General Public License *
 *  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
 *                                                                           *
 *  by Gabriel Sanmartín & Gabriel Moyà
 *  gabriel.sanmartin@usc.es & gabriel.moya@uib.es
 ****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

#include <math.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/highgui/highgui.hpp"

#include "CVKinectWrapper.h"
#include "CamShiftWrapper.h"
#include "Sequence.h"

#include "J_Enviar.h"
#include "J_Recibir.h"

using namespace cv;


int posX, posY;
int width, height;
Mat image;

/*SELECCION DEL AREA DE INTERES CON EL MOUSE */
Rect selection;
Point origin;
bool selectObject = false;

Rect selection_sec;
Point origin_sec;
bool selectObject_sec = false;

/* OBJETOS CAMSHIFT */
CamShiftWrapper *camshiftManager;
CamShiftWrapper *camshiftManager2;

/*SOCKETS*/
sockets::J_Recibir*			receiver;	 //Un receptor para la información enviada por la camara 4
sockets::J_Enviar*			sender;		///< El tercer socket es por si necesitamos enviar información (como p. ej. en la exp1)



KalmanFilter KF;
KalmanFilter KF2;
//vectores para representar las posiciones calculadas
vector<Point> historicoPOI,kalmanv;
vector<Point> historicoPOI2,kalmanv2;
//10,256,30
int vmin = 50, vmax = 256, smin = 100;
bool captureFromVideo = false;
int pNoiseCovar = 5, measurementNoiseCovar = 1;
bool includeVelocity = true;

ofstream fileMano1;
ofstream fileMano2;

int numFrames;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

float poweroften(int e){
	float ret = 1;
	int ac= 1;
	for(int i =0;i < e;i ++){
		ac*=10;
	}
	ret = ret / ac;
	return ret;
}


void onMouse( int event, int x, int y, int, void* )
{
	Point p;
	if( selectObject )
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);
		selection.height = std::abs(y - origin.y);

		selection &= Rect(0, 0, width, height);

		camshiftManager->setSelection(selection);
	}


	if( selectObject_sec )
	{
		selection_sec.x = MIN(x, origin_sec.x);
		selection_sec.y = MIN(y, origin_sec.y);
		selection_sec.width = std::abs(x - origin_sec.x);
		selection_sec.height = std::abs(y - origin_sec.y);

		selection_sec &= Rect(0, 0, width, height);

		camshiftManager2->setSelection(selection_sec);
	}

	switch( event )
	{
		case CV_EVENT_LBUTTONDOWN:
			origin = Point(x,y);
			selection = Rect(x,y,0,0);
			selectObject = true;
			break;
		case CV_EVENT_LBUTTONUP:
			selectObject = false;
			if( selection.width > 0 && selection.height > 0 )
				camshiftManager->setTrackObject(-1);
			p = selection.tl();
			p +=  Point(selection.width/2,selection.height/2);

			KF.statePre.at<float>(0) = x;
			KF.statePre.at<float>(1) = y;
			KF.statePre.at<float>(2) = 0;
			KF.statePre.at<float>(3) = 0;

			historicoPOI.push_back(p);
			kalmanv.push_back(p);

			break;

		case CV_EVENT_RBUTTONDOWN:
			origin_sec = Point(x,y);
			selection_sec = Rect(x,y,0,0);
			selectObject_sec = true;
			break;
		case CV_EVENT_RBUTTONUP:
			selectObject_sec = false;
			if( selection_sec.width > 0 && selection_sec.height > 0 )
				camshiftManager2->setTrackObject(-1);
			p = selection_sec.tl();
			p +=  Point(selection_sec.width/2,selection_sec.height/2);

			KF2.statePre.at<float>(0) = x;
			KF2.statePre.at<float>(1) = y;
			KF2.statePre.at<float>(2) = 0;
			KF2.statePre.at<float>(3) = 0;

			historicoPOI2.push_back(p);
			kalmanv2.push_back(p);

			break;

	}
}

void mouseHandler (int event, int x, int y, int flags, void *param){

	switch(event) {
	/* mouse move */
	case CV_EVENT_MOUSEMOVE:
		posX = x;
		posY = y;

		break;
	}
}



int main(int argc, char* argv[]){

	
	fileMano1.open ("mano1.txt");
	fileMano2.open ("mano2.txt");
	
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
	fileMano1 << "FILE HAND1. ";
	   fileMano1 << (now->tm_year + 1900) << '-' 
         << (now->tm_mon + 1) << '-'
         <<  now->tm_mday << "|"
		 << now->tm_hour << ":"
		 << now->tm_min
         << endl;

    fileMano1 << "FRAMENO, X, Y, Z\n";
	fileMano2 << "FILE HAND2. ";
	   fileMano2 << (now->tm_year + 1900) << '-' 
         << (now->tm_mon + 1) << '-'
         <<  now->tm_mday << "|"
		 << now->tm_hour << ":"
		 << now->tm_min
         << endl;
	fileMano2 << "FRAMENO, X, Y, Z\n";

	numFrames = 0;
	sender = new sockets::J_Enviar();
	sender->setPort(3305);


	Mat_<float> state;
	Mat processNoise;
	Mat_<float> measurement;

	Mat_<float> state2;
	Mat processNoise2;
	Mat_<float> measurement2;

	if(includeVelocity){
		KF = KalmanFilter(4, 2, 0);	
		KF.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   
													 0,1,0,1,  
													 0,0,1,0,  
													 0,0,0,1); //A
		state = Mat_<float>(4, 1); /* (x, y, Vx, Vy) */
		processNoise = Mat(4, 1, CV_32F);
		measurement = Mat_<float>(2,1); measurement.setTo(Scalar(1));


		KF2 = KalmanFilter(4, 2, 0);		
		KF2.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   
													 0,1,0,1,  
													 0,0,1,0,  
													 0,0,0,1); //A
		state2 = Mat_<float>(4, 1); /* (x, y, Vx, Vy) */
		processNoise2 = Mat(4, 1, CV_32F);
		measurement2 = Mat_<float>(2,1); measurement.setTo(Scalar(1));
	}
	else{
		KF = KalmanFilter(2, 2, 0);
		KF.transitionMatrix = *(Mat_<float>(2, 2) << 1, 0 , 0,1); //A

		state = Mat_<float>(2, 1); /* (x, y) */
		processNoise = Mat (2, 1, CV_32F);
		measurement = Mat_<float>(2,1);

		KF2 = KalmanFilter(2, 2, 0);
		KF2.transitionMatrix = *(Mat_<float>(2, 2) << 1, 0 , 0,1); //A

		state2 = Mat_<float>(2, 1); /* (x, y) */
		processNoise2 = Mat (2, 1, CV_32F);
		measurement2 = Mat_<float>(2,1);
	}

	measurement.setTo(Scalar(1));
	measurement2.setTo(Scalar(1));

	setIdentity(KF.measurementMatrix); //H
	setIdentity(KF.processNoiseCov, Scalar::all(4)); //Q
	setIdentity(KF.measurementNoiseCov, Scalar::all(100)); //R
	setIdentity(KF.errorCovPost, Scalar::all(0.1));

	setIdentity(KF2.measurementMatrix); //H
	setIdentity(KF2.processNoiseCov, Scalar::all(4)); //Q
	setIdentity(KF2.measurementNoiseCov, Scalar::all(100)); //R
	setIdentity(KF2.errorCovPost, Scalar::all(0.1));


	historicoPOI2.clear();
	kalmanv2.clear();
	
	/* Imágenes devueltas por el wrapper nuestro*/
	Mat *rgbImage = new Mat(480, 640, CV_8UC3, Scalar::all(0));
	Mat *depthImage = new Mat(480, 640, CV_8UC1, Scalar::all(0));
	Mat *comboImage = new Mat(480, 640, CV_8UC3, Scalar::all(0));
	Mat *rawDepth = new Mat(480, 640, CV_16UC1, Scalar::all(0));
	
	CVKinectWrapper *kinectManager = CVKinectWrapper::getInstance();
	
	bool ok = false;
	Sequence *seq = new Sequence("PinkGlove",FileStorage::READ,ok);
	//seq->setFrame(56);


	if(!captureFromVideo){
		if(!kinectManager->init("./SamplesConfig.xml")) 
			exit(0);
	}

	camshiftManager = new CamShiftWrapper();
	camshiftManager->setShowHist(true);
	camshiftManager->setShowHSV(true);
	camshiftManager->setShowBackproj(true);
	camshiftManager->setColor(Scalar(0,0,255));


	camshiftManager2 = new CamShiftWrapper();
	camshiftManager2->setShowHist(false);
	camshiftManager2->setShowHSV(false);
	camshiftManager2->setShowBackproj(false);
	camshiftManager2->setColor(Scalar(0,255,0));

	//namedWindow( "RGB");
	//namedWindow( "Depth" ); //Ventana de la imagen
	namedWindow( "Image" ); //Ventana de la imagen

	createTrackbar( "Vmin", "Image", &vmin, 256, 0 );
	createTrackbar( "Vmax", "Image", &vmax, 256, 0 );
	createTrackbar( "Smin", "Image", &smin, 256, 0 );

	createTrackbar( "pNoise", "Image", &pNoiseCovar, 100, 0 );
	createTrackbar( "Meas", "Image", &measurementNoiseCovar, 100, 0 );
	//namedWindow( "Combo"); //Ventana del mapa de altura de Kinect

	//cvSetMouseCallback( "Depth", mouseHandler, NULL );
	cvSetMouseCallback( "Image", onMouse, 0 );
	

	for(;;){
		//setIdentity(KF.processNoiseCov, Scalar::all(pNoiseCovar)); //Q
		//setIdentity(KF.measurementNoiseCov, Scalar::all(measurementNoiseCovar)); //R

		camshiftManager->setVmax(vmax);
		camshiftManager->setVmin(vmin);
		camshiftManager->setSmin(smin);

		camshiftManager2->setVmax(vmax);
		camshiftManager2->setVmin(vmin);
		camshiftManager2->setSmin(smin);

		if( kinectManager->update() || ok){
			//imshow("RGB", *rgbImage);
			if(captureFromVideo){
				if(!seq->readFrames(*rgbImage, *rawDepth))
					break;
				//imshow("RGB",*rgbImage);
				//Esto para el SEQ
				double min, max;
				minMaxLoc(*rawDepth, &min, &max, NULL, NULL);
				rawDepth->convertTo(*depthImage, CV_8UC1, 255/max);
			}
			else{

				kinectManager->getRGB(rgbImage);
				kinectManager->getRawDepth(rawDepth);
				kinectManager->getCombo(comboImage);
				kinectManager->getDisplayDepth(depthImage);
				//imshow("Combo", *comboImage);
				//imshow("Depth", *depthImage);
			}

			//resize(*depthImage, *depthImage, Size(depthImage->cols*2, depthImage->rows*2));


			imshow("Depth", *depthImage);
			rgbImage->copyTo(image);
			width = image.cols;
			height = image.rows;

			IplImage img = image;
			camshiftManager->updateImage(image);
			camshiftManager2->updateImage(image);

			//Esta mierda es para hacer que el trozo seleccionado salga en invertido cuando estamos
			//pulsando el raton (hace un NOT a cada bit)
			if( selectObject && selection.width > 0 && selection.height > 0 )
			{
				Mat roi(image, selection);
				bitwise_not(roi, roi);
			}

			if( selectObject_sec && selection_sec.width > 0 && selection_sec.height > 0 )
			{
				Mat roi(image, selection_sec);
				bitwise_not(roi, roi);
			}
			float remix[6];

			/*TRackeamos el objeto 1 */
			if(camshiftManager->getTrackObject()){

				Point * poi1 = camshiftManager->getPOI(); //Obtiene el centro del objeto capturado
				char centerPoint1[50];
				short result = rawDepth->at<short>(poi1->y, poi1->x);
				sprintf(centerPoint1, "POI1: (%d, %d): %d \n", poi1->x, poi1->y, result);
				putText(image, centerPoint1, cvPoint(10, 480-100), FONT_HERSHEY_PLAIN, 1, cv::Scalar(255,255,255,0));
				

				circle(*comboImage, *poi1,4,Scalar(255,0,0)); 


				//Actualización de Kalmarn
				Mat prediction = KF.predict();//predicton=kalman->state_post
				Point predictPt(prediction.at<float>(0),prediction.at<float>(1));

				measurement(0) = poi1->x;
				measurement(1) = poi1->y;

				//camshiftManager->calcHistogram(Rect(poi1->x-5,poi1->y-5,10,10));
				
				Point measPt(measurement(0),measurement(1));
				// generate measurement
				measurement += KF.measurementMatrix*state;

				Mat estimated = KF.correct(measurement);
				Point statePt(estimated.at<float>(0),estimated.at<float>(1));
				//camshiftManager->setTrackWindowPos(statePt);


				//Punto tras Kalman (debería ser similar no)?
				short result2;
				if(statePt.x>=0 && statePt.y >=0){
					kalmanv.push_back(statePt);
					result2 = rawDepth->at<short>(statePt.y, statePt.x);
				}
				


				 //plot points
				historicoPOI.push_back(*poi1);
				for (int i = 0; i < historicoPOI.size()-1; i++) {
					//Verde puntos de verdad
					line(image, historicoPOI[i], historicoPOI[i+1], Scalar(255,255,0), 1);
				}
				if(historicoPOI.size()>50){
					historicoPOI.erase(historicoPOI.begin());
				}
				for (int i = 0; i < kalmanv.size()-1; i++) {
					//Amarillo Kalman
					line(image, kalmanv[i], kalmanv[i+1], Scalar(0,255,255), 1);
				}
				if(kalmanv.size()>100){
					kalmanv.erase(kalmanv.begin());
				}

				remix[0] = statePt.x; remix[1] = statePt.y; remix[2] = result;
				
				fileMano1 << "" << numFrames << ", " << remix[0] << ", " << remix[1] <<", " << remix[2] << ")\n";
				//remix[0] = poi1->x; remix[1] = poi1->y; remix[2] = result2;

			}

			if(camshiftManager2->getTrackObject()){
				Point * poi1 = camshiftManager2->getPOI();
				char centerPoint1[50];
				short result = rawDepth->at<short>(poi1->y, poi1->x);
				sprintf(centerPoint1, "POI2: (%d, %d): %d \n", poi1->x, poi1->y, result);
				putText(image, centerPoint1, cvPoint(10, 480-50), FONT_HERSHEY_PLAIN, 1, cv::Scalar(255,255,255,0));
				
				
				circle(*comboImage, *poi1,4,Scalar(255,0,255)); 


				Mat prediction = KF2.predict();//predicton=kalman->state_post
				Point predictPt(prediction.at<float>(0),prediction.at<float>(1));

				measurement2(0) = poi1->x;
				measurement2(1) = poi1->y;

				//camshiftManager->calcHistogram(Rect(poi1->x-5,poi1->y-5,10,10));
				
				Point measPt(measurement2(0),measurement2(1));
				// generate measurement
				measurement2 += KF2.measurementMatrix*state2;

				Mat estimated = KF2.correct(measurement2);
				Point statePt(estimated.at<float>(0),estimated.at<float>(1));
				//camshiftManager->setTrackWindowPos(statePt);

				short result2;
				if(statePt.x>=0 && statePt.y >=0){
					kalmanv2.push_back(statePt);
					result2 = rawDepth->at<short>(statePt.y/2, statePt.x/2);
				}
				
				 //plot points
				historicoPOI2.push_back(*poi1);
				for (int i = 0; i < historicoPOI2.size()-1; i++) {
					line(image, historicoPOI2[i], historicoPOI2[i+1], Scalar(255,255,0), 1);
				}
				if(historicoPOI2.size()>50){
					historicoPOI2.erase(historicoPOI2.begin());
				}
				for (int i = 0; i < kalmanv2.size()-1; i++) {
					line(image, kalmanv2[i], kalmanv2[i+1], Scalar(0,255,255), 1);
				}
				if(kalmanv2.size()>100){
					kalmanv2.erase(kalmanv2.begin());
				}
			
				remix[3] = statePt.x; remix[4] = statePt.y; remix[5] = result;
				fileMano2 << "" << numFrames << ", " << remix[3] << ", " << remix[4] <<", " << remix[5] << ")\n";
				//remix[0] = poi1->x; remix[1] = poi1->y; remix[2] = result2;
			}
			
			sender->setBuffer((void *)remix, sizeof(remix));
			sender->sync();
			//imshow("Combo", *comboImage);
			imshow("Image", image);
		}
		//int k = waitKey(0);
		numFrames++;
		int k = waitKey(30);
		if(k == 27 ) exit(0);
	}
	
	fileMano1.close();
	fileMano2.close();
	return 0;
}
