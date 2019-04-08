
#include "display_rgb565_spi.h"

class Display_ST7735_spi : public Display_RGB565_spi8
{
public:
	Display_ST7735_spi( int nRotate, int nGpioCS=-1, int nGpioDC=1, int nGpioReset=201, int nGpioBackLight=65 ) :
		Display_RGB565_spi8(
			128,
			160,
			0,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			10 * 1000000 )
	{
	}

	virtual	int	Init()
	{
		if( 0 != Display_RGB565_spi8::Init() )
		{
			return	-1;
		}
		
		// FRMCTR1,2,3
		// Frame rate =fosc/((RTNA + 20) x (LINE + FPA + BPA))
		//			  = 333k / ((0x01+20) x (LINE + 0x2C + 0x2D)) = 63.68 fps
//		WriteReg(	0xB1, 0x01, 0x2C, 0x2D );

		// INVCTR
		WriteReg(	0xB4, 0x07 );
		
		// PWCTR1,2,3,4
		WriteReg(	0xC0, 0xA2, 0x02, 0x84 );
		WriteReg(	0xC1, 0xC5 );
		WriteReg(	0xC2, 0x0A, 0x00 );
		WriteReg(	0xC3, 0x8A, 0x2A );
		WriteReg(	0xC4, 0x8A, 0xEE );

		// VMCTR1
		WriteReg(	0xC5, 0x0E );
		
		// INVOFF
		WriteReg(	0x20 );

		// GMCTRP1
		// GMCTRN1
		WriteReg(	0xE0, 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10 );
		WriteReg(	0xE1, 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10 );
      
		// NORON
		WriteReg(	0x13 );

		return	0;
	}
};



class Display_ST7735_IPS_80x160_spi : public Display_RGB565_spi8
{
public:
	Display_ST7735_IPS_80x160_spi( int nRotate, int nGpioCS=-1, int nGpioDC=201, int nGpioReset=1, int nGpioBackLight=65 ) :
		Display_RGB565_spi8(
			80,
			160,
			DISP_CTRL_BGR,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			10 * 1000000 )
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

		return	0;
	}

	virtual	int		TransferRGB( int x, int y, int cx, int cy, const uint8_t * image, int image_bytes )
	{
		int		ox	= 26;	// (132 -  80) / 2
		int		oy	= 1;	// (162 - 160) / 2
		
		if( m_nDispCtrl & DISP_CTRL_SWAP_HV )
		{
			ox	= 1;
			oy	= 26;
		}

		if( m_nDispCtrl & DISP_CTRL_MIRROR_H ){}
		if( m_nDispCtrl & DISP_CTRL_MIRROR_V ){}

		return	Display_RGB565_spi8::TransferRGB(
			x + ox,
			y + oy,
			cx,
			cy,
			image,
			image_bytes );
	}
};
