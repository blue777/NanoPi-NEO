
#include "common/i2c_oled_ssd1306.h"
#include "common/img_halftone.h"
#include "common/perf_log.h"

#include <opencv2/opencv.hpp>



int main()
{
    I2C_OLED_SSD1306  oled;

    cv::VideoCapture    cap(0);

    if(!cap.isOpened()) 
    {
        printf("No capture devices.\n");
        return -1;
    }
    
    cap.set( CV_CAP_PROP_FRAME_WIDTH, 640 );
    cap.set( CV_CAP_PROP_FRAME_HEIGHT, 360 );
    cap.set( CV_CAP_PROP_FPS, 30 );

    oled.Init();
    oled.DisplayOn();
    
    while( 1 )
    {
        cv::Mat src,dst;

        cap >> src;

        cv::resize( src, src, cv::Size(128,64), 0, 0, cv::INTER_LANCZOS4 );
        cv::cvtColor( src, dst, CV_BGR2GRAY); 

		{
//			PerfLog  iPerf("dither");

			ImageHalftoning::ErrDiff_FloydSteinberg( dst.data, dst.step, dst.cols, dst.rows );
//			ImageHalftoning::ErrDiff_Burkes( dst.data, dst.step, dst.cols, dst.rows );
//			ImageHalftoning::ErrDiff_Stucki( dst.data, dst.step, dst.cols, dst.rows );
//			ImageHalftoning::ErrDiff_Atkinson( dst.data, dst.step, dst.cols, dst.rows );
//			ImageHalftoning::PatternDither_2x2( dst.data, dst.step, dst.cols, dst.rows );
		}

//		cv::imwrite("src.png",src);
//		cv::imwrite("dst.png",dst);

//		PerfLog  iPerf("disp");
		oled.WriteImage(dst.data, dst.step);
	}

    return  0;
}
