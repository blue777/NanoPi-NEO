
#include "display_rgb565_spi.h"

class Display_ST7789_spi : public Display_RGB565_spi8
{
public:
	Display_ST7789_spi( int nRotate, int nGpioCS=-1, int nGpioDC=1, int nGpioReset=203, int nGpioBackLight=0 ) :
		Display_RGB565_spi8(
			240,
			320,
			DISP_CTRL_MIRROR_V | DISP_CTRL_MIRROR_H,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			50 * 1000000 )
	{
	}
};

class Display_ST7789_IPS_240x240_spi : public Display_RGB565_spi8
{
public:
	Display_ST7789_IPS_240x240_spi( int nRotate, int nGpioCS=-1, int nGpioDC=201, int nGpioReset=1, int nGpioBackLight=65 ) :
		Display_RGB565_spi8(
			240,
			240,
			0,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			50 * 1000000 )
	{
	}

	virtual	int	Init()
	{
		if( 0 != Display_RGB565_spi8::Init() )
		{
			return	-1;
		}
		
		// INVON
		WriteReg(	0x21 );
		
		// Gamma
//		WriteReg( 	0x26,	0x01	);	// Gamma=2.2
//		WriteReg( 	0x26,	0x02	);	// Gamma=1.8
//		WriteReg( 	0x26,	0x08	);	// Gamma=1.0

/*
		WriteReg( 	0xB2, 0x0C, 0x0C, 0x00, 0x33, 0x33 );
		WriteReg( 	0xB7, 0x35 );
		WriteReg( 	0xBB, 0x1A );
		WriteReg( 	0xC0, 0x2C );
		WriteReg( 	0xC2, 0x01 );
		WriteReg( 	0xC3, 0x0B );
		WriteReg( 	0xC4, 0x20 );
		WriteReg( 	0xC6, 0x0F );
		WriteReg( 	0xD0, 0xA4 ,0xA1 );
		WriteReg( 	0xE0, 0x00, 0x19, 0x1E, 0x0A, 0x09, 0x15, 0x3D, 0x44, 0x51, 0x12, 0x03, 0x00, 0x3F, 0x3F );
		WriteReg( 	0xE1, 0x00, 0x18, 0x1E, 0x0A, 0x09, 0x25, 0x3F, 0x43, 0x52, 0x33, 0x03, 0x00, 0x3F, 0x3F );
//*/

		return	0;
	}
};
