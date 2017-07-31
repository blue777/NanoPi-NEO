
#include "common/display_ssd1306_i2c.h"
#include "common/img_halftone.h"
#include "common/perf_log.h"

#include <opencv2/opencv.hpp>

//  apt-get install libcv-dev libopencv-dev

int main()
{
    Display_SSD1306_i2c disp(180,0);
    cv::VideoCapture    cap(0);

    if(!cap.isOpened()) 
    {
        printf("No capture devices.\n");
        return -1;
    }
    
    cap.set( CV_CAP_PROP_FRAME_WIDTH, 640 );
    cap.set( CV_CAP_PROP_FRAME_HEIGHT, 360 );
    cap.set( CV_CAP_PROP_FPS, 30 );

    disp.Init();
    disp.DispClear();
    disp.DispOn();
    
    while( 1 )
    {
        cv::Mat src,dst;

        cap >> src;

        cv::resize( src, src, cv::Size(disp.GetSize().width,disp.GetSize().height), 0, 0, cv::INTER_AREA );
        cv::cvtColor( src, dst, CV_BGR2GRAY); 

		{
			PerfLog  iPerf("dither");

			ImageHalftoning::ErrDiff_LinearFloydSteinberg( dst.data, dst.step, dst.cols, dst.rows );
//			ImageHalftoning::ErrDiff_LinearStucki( dst.data, dst.step, dst.cols, dst.rows );
//			ImageHalftoning::ErrDiff_FloydSteinberg( dst.data, dst.step, dst.cols, dst.rows );
//			ImageHalftoning::ErrDiff_Burkes( dst.data, dst.step, dst.cols, dst.rows );
//			ImageHalftoning::ErrDiff_Stucki( dst.data, dst.step, dst.cols, dst.rows );
//			ImageHalftoning::ErrDiff_Atkinson( dst.data, dst.step, dst.cols, dst.rows );
//			ImageHalftoning::PatternDither_2x2( dst.data, dst.step, dst.cols, dst.rows );
		}

//		cv::imwrite("src.png",src);
//		cv::imwrite("dst.png",dst);

//		PerfLog  iPerf("disp");
		disp.WriteImageGRAY( 0, 0, dst.data, dst.step, dst.cols, dst.rows );
	}

    return  0;
}
