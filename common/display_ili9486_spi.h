
#include "display_rgb565_spi.h"

// 
class Display_ILI9486_spi : public Display_RGB565_spi8
{
public:
	Display_ILI9486_spi( int nRotate, int nGpioCS=-1, int nGpioDC=201, int nGpioReset=1, int nGpioBackLight=65 ) :
		Display_RGB565_spi8(
			320,
			480,
			DISP_CTRL_MIRROR_V | DISP_CTRL_BGR,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			50 * 1000000,
			SPI_MODE_0
			)
	{
	}

	virtual	int	Init()
	{
		if( 0 != Display_RGB565_spi8::Init() )
		{
			return	-1;
		}

		WriteReg(	MIPI_DCS_SET_PIXEL_FORMAT,
					0x66 );	// RGB666

		//************* Start Initial Sequence **********//
		WriteReg( 0xF1,	0x36, 0x04, 0x00, 0x3C, 0x0F, 0x8F );
		WriteReg( 0xF2, 0x18, 0xA3, 0x12, 0x02, 0xb2, 0x12, 0xFF, 0x10, 0x00 );
		WriteReg( 0xF8, 0x21, 0x04 );
		WriteReg( 0xF9, 0x00, 0x08 );  
		WriteReg( 0xC0, 0x0f, 0x0f );
		WriteReg( 0xC1, 0x42 );
		WriteReg( 0xC2, 0x22 );
		WriteReg( 0xC5, 0x01, 0x29, 0x80 );
		WriteReg( 0xB6, 0x00, 0x02, 0x3b );
		WriteReg( 0xB1, 0xB0, 0x11 );
		WriteReg( 0xB4, 0x02);
		WriteReg( 0xE0, 0x0F, 0x18, 0x15, 0x09, 0x0B, 0x04, 0x49, 0x64, 0x3D, 0x08, 0x15, 0x06, 0x12, 0x07, 0x00 );
		WriteReg( 0xE1, 0x0F, 0x38, 0x35, 0x0a, 0x0c, 0x03, 0x4A, 0x42, 0x36, 0x04, 0x0F, 0x03, 0x1F, 0x1B, 0x00 );

		m_iFrameBuf.resize( m_tDDRAM.width * m_tDDRAM.height * 3 );
		return	0;
	}

	virtual	int	WriteImageBGRA( int x, int y, const uint8_t* image, int stride, int cx, int cy )
	{
		if( _CalcTransArea( x, y, image, stride, 4, cx, cy ) )
		{
			ImageConvert::BGRA8888toRGB888( image, stride, cx, cy, m_iFrameBuf.data(), cx * 3 );

			return	TransferRGB( x, y, cx, cy, m_iFrameBuf.data(), cx * cy * 3 );
		}
		
		return	-1;
	}

	virtual	int WriteImageGRAY( int x, int y, const uint8_t* image, int stride, int cx, int cy )
	{
		if( _CalcTransArea( x, y, image, stride, 1, cx, cy ) )
		{
			ImageConvert::GRAY8toRGB888( image, stride, cx, cy, m_iFrameBuf.data(), cx * 3 );

			return	TransferRGB( x, y, cx, cy, m_iFrameBuf.data(), cx * cy * 3  );
		}
		
		return	-1;
	}
};


//	https://www.waveshare.com/wiki/3.5inch_RPi_LCD_(A)
//	https://www.waveshare.com/3.5inch-rpi-lcd-a.htm
class Display_WaveShare35_spi : public Display_RGB565_spi8
{
public:
	Display_WaveShare35_spi( int nRotate, int nGpioCS=67, int nGpioDC=201, int nGpioReset=1, int nGpioBackLight=-1 ) :
		Display_RGB565_spi8(
			320,
			480,
			DISP_CTRL_MIRROR_V | DISP_CTRL_BGR,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			25 * 1000000,
			SPI_MODE_0
			)
	{
	}

	virtual	int	Init()
	{
		if( 0 != Display_RGB565_spi8::Init() )
		{
			return	-1;
		}

		WriteReg( 0xB0, 0x00 );
		WriteReg( 0xc2, 0x44 ); 
		WriteReg( 0xc5, 0x0, 0x0, 0x0, 0x0 );
		WriteReg( 0xe0, 0xf, 0x1f, 0x1c, 0xc, 0xf, 0x8, 0x48, 0x98, 0x37, 0xa, 0x13, 0x4, 0x11, 0x0d, 0x0 );
		WriteReg( 0xe1, 0xf, 0x32, 0x2e, 0xb, 0xd, 0x5, 0x47, 0x75, 0x37, 0x6, 0x10, 0x3, 0x24, 0x20, 0x0 );
		WriteReg( 0xe2, 0xf, 0x32, 0x2e, 0xb, 0xd, 0x5, 0x47, 0x75, 0x37, 0x6, 0x10, 0x3, 0x24, 0x20, 0x0 );

		return	0;
	}

	virtual	void	WriteSpi( uint8_t _cmd, int data_size, const uint8_t * _data )
	{
		uint8_t		cmd[]	= { 0x00, _cmd };

		// ChipSelect
		m_iCS	<< 0;

		// Command
		m_iDC	<< 0;
		m_iSPI.write( cmd, sizeof(cmd) );

		if( 0 < data_size )
		{
			std::vector<uint8_t>	data;
			int						i;

			for( i = 0; i < data_size; i++ )
			{
				data.push_back( 0x00 );
				data.push_back( _data[i] );
			}

			m_iDC	<< 1;
			m_iSPI.write( data.data(), data.size() );
			data_size	= data.size();
		}

		// ChipSelect
		m_iCS	<< 1;
//		printf( "SpiWrite16:0x%02x, %d\n", _cmd, data_size );		
	}
};




