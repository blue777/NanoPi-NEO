
#include "display_rgb565_spi.h"

class Display_ILI9341_spi : public Display_RGB565_spi8
{
public:
	Display_ILI9341_spi( int nRotate, int nGpioCS=-1, int nGpioDC=1, int nGpioReset=201, int nGpioBackLight=65 ) :
		Display_RGB565_spi8(
			240,
			320,
			DISP_CTRL_MIRROR_V | DISP_CTRL_BGR,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			40 * 1000000 )
	{
	}
};


class Display_ILI9341_spi_TM24 : public Display_RGB565_spi8
{
public:
	Display_ILI9341_spi_TM24( int nRotate, int nGpioCS=-1, int nGpioDC=1, int nGpioReset=201, int nGpioBackLight=65 ) :
		Display_RGB565_spi8(
			240,
			320,
			DISP_CTRL_MIRROR_V | DISP_CTRL_MIRROR_H | DISP_CTRL_BGR,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			40 * 1000000 )
	{
	}
	
	virtual	int	Init()
	{
		if( 0 != Display_RGB565_spi8::Init() )
		{
			return	-1;
		}

		/************* Start Initial Sequence **********/
		WriteReg(	0xCF, 0x00, 0x81, 0X30 );
		WriteReg(	0xED, 0x64, 0x03, 0X12, 0X81 );
		WriteReg(	0xE8, 0x85, 0x10, 0x7A );
		WriteReg(	0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02 );
		
		WriteReg(	0xF7, 0x20 );
		WriteReg(	0xEA, 0x00, 0x00 );
		
		WriteReg(	0xC0, 0x21 );		// Power control, VRH[5:0]
		WriteReg(	0xC1, 0x11 );		// Power control, SAP[2:0];BT[3:0]
		WriteReg(	0xC5, 0x3F, 0x3C );	// VCM control
		WriteReg(	0xC7, 0xA7 );		// VCM control2
//		WriteReg(	0x3A, 0x55 ); 

		WriteReg(	0xB1, 0x00, 0x1B);
		WriteReg(	0xB6, 0x0A, 0xA2);	// Display Function Control
		WriteReg(	0xF2, 0x00 );		// 3Gamma Function Disable
		WriteReg(	0x26, 0x01 );		// Gamma curve selected
		WriteReg(	0xE0, 0x0F, 0x23, 0x1F, 0x0B, 0x0E, 0x08, 0x4B, 0XA8, 0x3B, 0x0A, 0x14, 0x06, 0x10, 0x09, 0x00);	 //Set Gamma
		WriteReg(	0XE1, 0x00, 0x1C, 0x20, 0x04, 0x10, 0x08, 0x34, 0x47, 0x44, 0x05, 0x0B, 0x09, 0x2F, 0x36, 0x0F ); //Set Gamma

		return	0;
	}
};



class Display_ILI9341_spi_TM22 : public Display_RGB565_spi8
{
public:
	Display_ILI9341_spi_TM22( int nRotate, int nGpioCS=-1, int nGpioDC=1, int nGpioReset=201, int nGpioBackLight=65 ) :
		Display_RGB565_spi8(
			240,
			320,
			DISP_CTRL_MIRROR_V | DISP_CTRL_BGR,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			40 * 1000000 )
	{
	}

	virtual	int	Init()
	{
		if( 0 != Display_RGB565_spi8::Init() )
		{
			return	-1;
		}
		
		WriteReg(	0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02 );
		WriteReg(	0xCF, 0x00, 0xC1, 0X30 );
		WriteReg(	0xE8, 0x85, 0x00, 0x78 );
		WriteReg(	0xEA, 0x00, 0x00 );
		WriteReg(	0xED, 0x64, 0x03, 0x12, 0x81 );
		WriteReg(	0xF7, 0x20 );

		WriteReg(	0xC0, 0x23 );		// Power control, VRH[5:0]
		WriteReg(	0xC1, 0x10 );		// Power control, SAP[2:0];BT[3:0]
		WriteReg(	0xC5, 0x3E, 0x28 );	// VCM control
		WriteReg(	0xC7, 0x86 );		// VCM control2

		WriteReg(	0xB1, 0x00, 0x18 ); 
		WriteReg(	0xB6, 0x08, 0x82, 0x27 );    // Display Function Control 

		WriteReg(	0xF2, 0x00 );   	// 3Gamma Function Disable 
		WriteReg(	0x26, 0x01 );   	// Gamma curve selected 
		WriteReg(	0xE0, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00);  //Set Gamma 
		WriteReg(	0xE1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F);    //Set Gamma 

		return	0;
	}
};