//	g++ -Ofast -std=c++11 ina226_reg.c -o ina226_reg.o -lpthread `freetype-config --cflags` `freetype-config --libs`



#include <map>
#include <string>
#include <ctime>
#include <iostream>
#include <chrono>
#include <thread>
#include <map>
#include <functional>

#include "common/img_font.h"
#include "common/string_util.h"
#include "common/ctrl_i2c.h"
#include "common/display_st7735_spi.h"
#include "common/ctrl_pmoni.h"


//#define	FONT_PATH	"/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
#define	FONT_PATH		"/usr/share/fonts/truetype/takao-gothic/TakaoPGothic.ttf"



int	main( int argc, const char * argv[] )
{
	PMoni_INA226	ina226( "/dev/i2c-0",0x40 );

	Display_ST7735_IPS_80x160_spi	iDisp(90);
	int		interval	= 500;

	ina226.SetSamplingDuration( interval );
	
	// shuntV = reg * v / (ref_r - reg)
	//	v 		= 3.3
	//	ref_r	= 100
	//	reg		= 0.2
	//	shuntV_1LSB	= 0.0000025
	ina226.SetAlertFunc( PMoni_INA226::ALERT_SHUNT_UNDER_VOLT, 0.2 * 3.3 / (100 - 0.2) / 0.0000025 );
	
	iDisp.Init();
	iDisp.DispClear();
	iDisp.DispOn();

	ImageFont		iFontVA( FONT_PATH, 30, false );
	ImageFont		iFontREG( FONT_PATH, 30, false );
	const int		nBufWidth		= iDisp.GetSize().width;
	const int		nBufHeight		= iDisp.GetSize().height;


	std::this_thread::sleep_for(std::chrono::seconds(1));

	// Sampling thread (main)
	{
	 	std::chrono::high_resolution_clock::time_point	iTpNext, iTpStart, iTpCurr;
	 	
	 	iTpStart	= std::chrono::high_resolution_clock::now();
	 	iTpNext		= iTpStart;

		while( 1 )
		{
			char	szBuf[256];

			{
				int		wait;

				iTpCurr	= std::chrono::high_resolution_clock::now();
				wait	= std::chrono::duration_cast<std::chrono::milliseconds>(iTpNext-iTpCurr).count()-1;
				if( 0 < wait )
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(wait));
					iTpCurr	= std::chrono::high_resolution_clock::now();
				}
				iTpNext	+= std::chrono::milliseconds(interval);
			}

			// 
			{
				const double	ref_r	= 100.0;

				const double	v		= ina226.GetV();
				int16_t			shunt	= ina226.ReadShuntRaw();
				const double	shuntV	= 0.0000025 * shunt;

				double			reg		= ref_r * shuntV / (v - shuntV);

				uint8_t		img[ nBufWidth * nBufHeight]	= {0};
				char		szBuf[256];
				int			l, t, r, b;

				sprintf( szBuf, "%.0f [uV]", shuntV * 1000000 );
				iFontVA.CalcRect( l, t, r, b, szBuf );
 				iFontVA.DrawTextGRAY( nBufWidth - r, 0, szBuf, 255, img, nBufWidth, nBufWidth, nBufHeight );

				sprintf( szBuf, "%.0f [mΩ]",reg * 1000 );
 				iFontREG.CalcRect( l, t, r, b, szBuf );
				iFontREG.DrawTextGRAY( nBufWidth - r, 40, szBuf, 255, img, nBufWidth, nBufWidth, nBufHeight );

				iDisp.WriteImageGRAY( 0, iDisp.GetSize().height - nBufHeight, img, nBufWidth, nBufWidth, nBufHeight );

				printf( "ShuntRaw = %d, ShuntV = %.3f [mV]\n", shunt, shuntV * 1000 );
			}
    	}
	}

	return	0;
}
