
#include <opencv2/opencv.hpp>
#include "common/perf_log.h"
#include "usr_displays.h"

//	http://docs.opencv.org/trunk/d7/d8b/tutorial_py_face_detection.html

int main()
{
	std::vector<DisplayIF*>	display	= GetUsrDisplays();

	cv::CascadeClassifier   face_cascade;
	cv::CascadeClassifier   eye_cascade;
	cv::Mat                 img;
	cv::Mat                 gray;

	face_cascade.load( "/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml" );
	eye_cascade.load( "/usr/share/opencv/haarcascades/haarcascade_eye.xml" );

	for( auto it : display )
	{
		it->Init();
		it->DispOn();
		it->DispClear();
	}

	cv::VideoCapture    cap(0);
 	if( cap.isOpened() ) 
	{
		cap.set( CV_CAP_PROP_FRAME_WIDTH, 320 );	
		cap.set( CV_CAP_PROP_FRAME_HEIGHT, 240 );
		cap.set( CV_CAP_PROP_FPS, 15 );
	}
	else
	{
		printf("No capture devices.\n");
		return -1;
	}
   
	while( 1 )
	{
		std::vector<cv::Rect>   faces;

		cap >> img;

		// face detect
		{
			PerfLog  iPerf("face_detect");
	
			cv::cvtColor( img, gray, CV_BGR2GRAY ); 

			face_cascade.detectMultiScale( gray, faces, 1.3, 5 );
	
			for( int i = 0; i < faces.size(); i++ )
			{
				cv::Mat                 roi_color   = cv::Mat( img, faces[i] );
				cv::Mat                 roi_gray    = cv::Mat( gray, faces[i] );
				std::vector<cv::Rect>   eyes;
		
				cv::rectangle( img, faces[i].tl(), faces[i].br(), cv::Scalar(255,0,0), 2 );
				
				eye_cascade.detectMultiScale( roi_gray, eyes );
				
				for( int j = 0; j < eyes.size(); j++ )
				{
					cv::rectangle( roi_color, eyes[i].tl(), eyes[i].br(), cv::Scalar(0,255,0), 2 );
				}
			}
		}

		// draw image
		{
			cv::Mat	src;
			cv::cvtColor( img, src, cv::COLOR_BGR2BGRA );
			
			for( auto it : display )
			{
				cv::Mat	disp;
				cv::resize( src, disp, cv::Size(it->GetSize().width,it->GetSize().height), 0, 0, cv::INTER_AREA );
				PerfLog  iPerf("disp_BGRA");
				it->WriteImageBGRA( 0, 0, disp.data, disp.step, disp.cols, disp.rows );
			}
		}
	}

	return  0;
}