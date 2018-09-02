
//
//	apt-get install libcv-dev libopencv-dev
//  apt-get install opencv-doc
//

/*
// This file overrides the built-in C++ (simple) runner
// For more information see http://docs.c9.io:8080/#!/api/run-method-run
{
  "script": [
	"set -e",
	"if [ \"$debug\" == true ]; then ",
	"/usr/bin/g++ -ggdb3 -std=c++11 $file -o $file.o `pkg-config --cflags opencv` `pkg-config --libs opencv`",
	"chmod 755 \"$file.o\"",
	"node $HOME/.c9/bin/c9gdbshim.js \"$file.o\" $args",
	"else",
	"/usr/bin/g++ -O3 -std=c++11 $file -o $file.o `pkg-config --cflags opencv` `pkg-config --libs opencv`",
	"chmod 755 $file.o",
	"$file.o $args",
	"fi"
  ],
  "info": "Running $file (C++ OpenCV)",
  "debugger": "gdb",
  "$debugDefaultState": false,
  "env": {},
  "selector": "^.*\\.(cpp|cc)$"
}
//*/


#include <thread>
#include <opencv2/opencv.hpp>
#include "common/perf_log.h"

#include "common/display_st7735_spi.h"
#include "common/display_st7789_spi.h"
#include "common/display_ili9328_spi.h"
#include "common/display_ili9341_spi.h"
#include "common/display_ili9225_spi.h"
//#include "common/display_ssd1306_i2c.h"
#include "common/display_ili9486_spi.h"
#include "common/display_fbdev.h"

void	CreateTestPattern( cv::Mat& image, int cx, int cy )
{
	int		blk_y	= cy / 7;
	int		next_y	= cy - blk_y * 6;
	int		y		= 0;

	image	= cv::Mat::zeros( cy, cx, CV_8UC4 );

	// Dot large
	for( y = 0; y < next_y; y++ )
	{
		for( int x = 0; x < cx; x++ )
		{
			if( 1 & ((x/4) + (y/4)) )
			{
				image.at<cv::Vec4b>(y,x)	= cv::Vec4b(255,255,255,255);
			}
		}
	}

	// Dot
	y		= next_y;
	next_y	+= blk_y;
	for( y = 0; y < next_y; y++ )
	{
		for( int x = 0; x < cx; x++ )
		{
			if( 1 & (x+y) )
			{
				image.at<cv::Vec4b>(y,x)	= cv::Vec4b(255,255,255,255);
			}
		}
	}

	// Graygradation
	y		= next_y;
	next_y	+= blk_y;
	for( int x = 0; x < 8; x++ )
	{
		cv::rectangle(
			image,
			cv::Point2i( (x+0) * cx / 8, y ),
			cv::Point2i( (x+1) * cx / 8, next_y),
			cv::Scalar(x*36,x*36,x*36,x*36),	// B,G,R,A
			CV_FILLED );
	}

	// Graygradation
	y		= next_y;
	next_y	+= blk_y;
	for( int x = 0; x < 256; x++ )
	{
		cv::rectangle(
			image,
			cv::Point2i( (x+0) * cx / 256, y ),
			cv::Point2i( (x+1) * cx / 256, next_y),
			cv::Scalar(x,x,x,x),	// B,G,R,A
			CV_FILLED );
	}

	// Red
	y		= next_y;
	next_y	+= blk_y;
	for( int x = 0; x < 256; x++ )
	{
		cv::rectangle(
			image,
			cv::Point2i( (x+0) * cx / 256, y ),
			cv::Point2i( (x+1) * cx / 256, next_y),
			cv::Scalar(0,0,x,255),	// B,G,R,A
			CV_FILLED );
	}

	// Green
	y		= next_y;
	next_y	+= blk_y;
	for( int x = 0; x < 256; x++ )
	{
		cv::rectangle(
			image,
			cv::Point2i( (x+0) * cx / 256, y ),
			cv::Point2i( (x+1) * cx / 256, next_y),
			cv::Scalar(0,x,0,255),	// B,G,R,A
			CV_FILLED );
	}

	// Blue
	y		= next_y;
	next_y	+= blk_y;
	for( int x = 0; x < 256; x++ )
	{
		cv::rectangle(
			image,
			cv::Point2i( (x+0) * cx / 256, y ),
			cv::Point2i( (x+1) * cx / 256, next_y),
			cv::Scalar(x,0,0,255),	// B,G,R,A
			CV_FILLED );
	}
}

int main()
{
	std::vector<DisplayIF*>	display;
	cv::Mat		src	= cv::imread("TestImages/lena_std.tif");

//	display.push_back( new Display_SSD1306_i2c(180) );		// for SSD1306
//	display.push_back( new Display_SSD1306_i2c(180,2) );	// for SH1106
//	display.push_back( new Display_ILI9341_spi_TM24(0) );	// for Tianma2.4" Panel
//	display.push_back( new Display_ILI9341_spi_TM22(0,199) );	// for Tianma2.2" Panel
//	display.push_back( new Display_ILI9328_spi_TM22(0,200) );	// for Tianma2.2" Panel
//	display.push_back( new Display_ILI9225_spi(0,198) );		// for basic ILI9225
//	display.push_back( new Display_ST7735_spi(180) );		// for KMR1.8
//	display.push_back( new Display_ST7789_spi(0) );		// for KMR1.8
//	display.push_back( new Display_WaveShare35_spi(90) );		// for WaveShare 3.5 inch LCD 320x480
//	display.push_back( new Display_WaveShare40_spi(0) );		// for WaveShare 3.5 inch LCD 320x480
//	display.push_back( new Display_fbdev("/dev/fb1"));
//	display.push_back( new Display_ILI9486_spi(0) );
	display.push_back( new 	Display_ST7789_IPS_240x240_spi(0) );

	////////////////////////////////////////////////////////////////
	// DisplayIF::Init()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::Init()\n");
	getchar();
	for( auto it : display )
	{
		it->Init();
	}

	////////////////////////////////////////////////////////////////
	// DisplayIF::DispOn()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::DispOn()\n");
	getchar();
	for( auto it : display )
	{
		it->DispOn();
	}


	////////////////////////////////////////////////////////////////
	// DisplayIF::WriteImageBGRA()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::WriteImageBGRA() TestPattern\n");
	getchar();
	for( auto it : display )
	{
		cv::Mat	img;
		
		CreateTestPattern( img, it->GetSize().width, it->GetSize().height );

		it->WriteImageBGRA( 0, 0, img.data, img.step, img.cols, img.rows );
		it->Flush();
	}

	////////////////////////////////////////////////////////////////
	// DisplayIF::DispOff()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::DispClear()\n");
	getchar();
	for( auto it : display )
	{
		it->DispClear();
	}

	////////////////////////////////////////////////////////////////
	// DisplayIF::WriteImageBGRA()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::WriteImageBGRA() GRAY\n");
	getchar();
	for( auto it : display )
	{
		cv::Mat	img	= cv::Mat( it->GetSize().height, it->GetSize().width, CV_8UC4 );
		
		cv::rectangle(
			img,
			cv::Point2i( 0, 0 ),
			cv::Point2i( it->GetSize().width, it->GetSize().height ),
			cv::Scalar(128,128,128,128),	// B,G,R,A-->yellow??
			CV_FILLED );

		it->WriteImageBGRA( 0, 0, img.data, img.step, img.cols, img.rows );
		it->Flush();
	}

	////////////////////////////////////////////////////////////////
	// DisplayIF::WriteImageBGRA()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::WriteImageBGRA() WHITE\n");
	getchar();
	for( auto it : display )
	{
		cv::Mat	img	= cv::Mat( it->GetSize().height, it->GetSize().width, CV_8UC4 );
		
		cv::rectangle(
			img,
			cv::Point2i( 0, 0 ),
			cv::Point2i( it->GetSize().width, it->GetSize().height ),
			cv::Scalar(255,255,255,255),
			CV_FILLED );

		it->WriteImageBGRA( 0, 0, img.data, img.step, img.cols, img.rows );
		it->Flush();
	}

	////////////////////////////////////////////////////////////////
	// DisplayIF::WriteImageBGRA()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::WriteImageBGRA() RED\n");
	getchar();
	for( auto it : display )
	{
		cv::Mat	img	= cv::Mat( it->GetSize().height, it->GetSize().width, CV_8UC4 );
		
		cv::rectangle(
			img,
			cv::Point2i( 0, 0 ),
			cv::Point2i( it->GetSize().width, it->GetSize().height ),
			cv::Scalar(0,0,255,0),
			CV_FILLED );

		it->WriteImageBGRA( 0, 0, img.data, img.step, img.cols, img.rows );
		it->Flush();
	}

	////////////////////////////////////////////////////////////////
	// DisplayIF::WriteImageBGRA()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::WriteImageBGRA() GREEN\n");
	getchar();
	for( auto it : display )
	{
		cv::Mat	img	= cv::Mat( it->GetSize().height, it->GetSize().width, CV_8UC4 );

		cv::rectangle(
			img,
			cv::Point2i( 0, 0 ),
			cv::Point2i( it->GetSize().width, it->GetSize().height ),
			cv::Scalar(0,255,0,0),
			CV_FILLED );

		it->WriteImageBGRA( 0, 0, img.data, img.step, img.cols, img.rows );
		it->Flush();
	}

	////////////////////////////////////////////////////////////////
	// DisplayIF::WriteImageBGRA()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::WriteImageBGRA() BLUE\n");
	getchar();
	for( auto it : display )
	{
		cv::Mat	img	= cv::Mat( it->GetSize().height, it->GetSize().width, CV_8UC4 );
		
		cv::rectangle(
			img,
			cv::Point2i( 0, 0 ),
			cv::Point2i( it->GetSize().width, it->GetSize().height ),
			cv::Scalar(255,0,0,0),
			CV_FILLED );

		it->WriteImageBGRA( 0, 0, img.data, img.step, img.cols, img.rows );
		it->Flush();
	}




	////////////////////////////////////////////////////////////////
	// DisplayIF::WriteImageGRAY()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::WriteImageGRAY()\n");
	getchar();
	for( auto it : display )
	{
		cv::Mat	img;
		cv::cvtColor( src, img, cv::COLOR_BGR2GRAY );
		cv::resize( img, img, cv::Size(it->GetSize().width,it->GetSize().height), 0, 0, cv::INTER_AREA );
		
		it->WriteImageGRAY( 0, 0, img.data, img.step, img.cols, img.rows );
		it->Flush();
	}

	////////////////////////////////////////////////////////////////
	// DisplayIF::WriteImageBGRA()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::WriteImageBGRA()\n");
	getchar();
	for( auto it : display )
	{
		cv::Mat	img;
		cv::cvtColor( src, img, cv::COLOR_BGR2BGRA );
		cv::resize( img, img, cv::Size(it->GetSize().width,it->GetSize().height), 0, 0, cv::INTER_AREA );

		it->WriteImageBGRA( 0, 0, img.data, img.step, img.cols, img.rows );
		it->Flush();
	}

	////////////////////////////////////////////////////////////////
	// DisplayIF::DispOff()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::DispOff()\n");
	getchar();
	for( auto it : display )
	{
		it->DispOff();
	}

	////////////////////////////////////////////////////////////////
	// DisplayIF::Quit()
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DisplayIF::DispOff()\n");
	getchar();
	for( auto it : display )
	{
		it->Quit();
	}	

	////////////////////////////////////////////////////////////////
	// DispOn with pre-transfered image.
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to DispOn with pre-transfered image.\n");
	getchar();
	for( auto it : display )
	{
		it->Init();

		int		size	= std::min( it->GetSize().width, it->GetSize().height );
		cv::Mat	img;
		cv::cvtColor( src, img, cv::COLOR_BGR2BGRA );
		cv::resize( img, img, cv::Size(size,size), 0, 0, cv::INTER_AREA );

		it->DispClear();
		it->WriteImageBGRA( (it->GetSize().width-img.cols)/2, (it->GetSize().height-img.rows)/2, img.data, img.step, img.cols, img.rows );
		it->Flush();
		it->DispOn();
	}

	printf( "Hit any key to DispOff and Quit.\n");
	getchar();
	for( auto it : display )
	{
		it->DispOff();
		it->Quit();
	}	

	////////////////////////////////////////////////////////////////
	// Scroll test
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to Scroll(GRAY) test\n");
	getchar();
	for( auto it : display )
	{
		it->Init();
		it->DispClear();
		it->DispOn();
	}

	for( auto it : display )
	{
		cv::Mat	img;
		cv::cvtColor( src, img, cv::COLOR_BGR2GRAY );
		cv::resize( img, img, cv::Size(it->GetSize().width,it->GetSize().height), 0, 0, cv::INTER_AREA );
		
		for( int y = -img.rows; y < img.rows; y++ )
		{
			it->WriteImageGRAY( y, y, img.data, img.step, img.cols, img.rows );
			it->Flush();
		}
	}

	for( auto it : display )
	{
		it->DispOff();
		it->Quit();
	}	

	////////////////////////////////////////////////////////////////
	// Scroll test
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to Scroll(BGRA) test\n");
	getchar();
	for( auto it : display )
	{
		it->Init();
		it->DispClear();
		it->DispOn();
	}

	for( auto it : display )
	{
		cv::Mat	img;
		cv::cvtColor( src, img, cv::COLOR_BGR2BGRA );
		cv::resize( img, img, cv::Size(it->GetSize().width,it->GetSize().height), 0, 0, cv::INTER_AREA );

		for( int y = -img.rows; y < img.rows; y++ )
		{
			it->WriteImageBGRA( y, y, img.data, img.step, img.cols, img.rows );
			it->Flush();
		}
	}

	for( auto it : display )
	{
		it->DispOff();
		it->Quit();
	}

	////////////////////////////////////////////////////////////////
	// Performance test
	////////////////////////////////////////////////////////////////
	printf( "Hit any key to Performance test\n");
	getchar();
	for( auto it : display )
	{
		it->Init();
		it->DispClear();
		it->Flush();
		it->DispOn();
	}

	for( auto it : display )
	{
		int		loop	= 100;
		cv::Mat	img, gray, bgra;
		cv::resize( src, img, cv::Size(it->GetSize().width,it->GetSize().height), 0, 0, cv::INTER_AREA );
		cv::cvtColor( img, bgra, cv::COLOR_BGR2BGRA );
		cv::cvtColor( img, gray, cv::COLOR_BGR2GRAY );

    	std::chrono::high_resolution_clock::time_point   start	= std::chrono::high_resolution_clock::now();
		for( int i = 0; i < loop; i++ )
		{
			it->DispClear();
			it->WriteImageGRAY( 0, 0, gray.data, gray.step, gray.cols, gray.rows );
			it->DispClear();
			it->WriteImageBGRA( 0, 0, bgra.data, bgra.step, bgra.cols, bgra.rows );
		}
		std::chrono::high_resolution_clock::time_point   end	= std::chrono::high_resolution_clock::now();
 
        double elapsed = std::chrono::duration_cast<std::chrono::seconds>(end-start).count();
		printf( "Framerate Per Sec = %.3f [fps]\n", 4 * loop / elapsed );
	}

	for( auto it : display )
	{
		it->DispOff();
		it->Quit();
	}

	return  0;
}
