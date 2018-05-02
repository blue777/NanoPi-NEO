#ifndef	__DISPLAY_RGB565_SPI_H_INCLUDED__
#define	__DISPLAY_RGB565_SPI_H_INCLUDED__

#include <thread>
#include <vector>
#include "ctrl_spi.h"
#include "ctrl_gpio.h"
#include "img_conv.h"

#include "display_if.h"
//#include <mipi_display.h>

enum
{
	MIPI_DCS_ENTER_SLEEP_MODE	= 0x10,
	MIPI_DCS_EXIT_SLEEP_MODE	= 0x11,
	MIPI_DCS_ENTER_NORMAL_MODE	= 0x13,
	MIPI_DCS_EXIT_INVERT_MODE	= 0x20,
	MIPI_DCS_SET_GAMMA_CURVE	= 0x26,
	MIPI_DCS_SET_DISPLAY_OFF	= 0x28,
	MIPI_DCS_SET_DISPLAY_ON		= 0x29,
	MIPI_DCS_SET_COLUMN_ADDRESS	= 0x2A,
	MIPI_DCS_SET_PAGE_ADDRESS	= 0x2B,
	MIPI_DCS_WRITE_MEMORY_START	= 0x2C,
	MIPI_DCS_WRITE_LUT			= 0x2D,

	MIPI_DCS_SET_ADDRESS_MODE	= 0x36,
	MIPI_DCS_SET_PIXEL_FORMAT	= 0x3A,

	MIPI_DCS_SET_DISPLAY_BRIGHTNESS = 0x51,		/* MIPI DCS 1.3 */
	MIPI_DCS_WRITE_CONTROL_DISPLAY  = 0x53,		/* MIPI DCS 1.3 */
	MIPI_DCS_WRITE_POWER_SAVE	= 0x55,		/* MIPI DCS 1.3 */
	MIPI_DCS_SET_CABC_MIN_BRIGHTNESS = 0x5E,	/* MIPI DCS 1.3 */
};

enum
{
	DISP_CTRL_SWAP_HV	= 0x01,	// 
	DISP_CTRL_MIRROR_H	= 0x02,
	DISP_CTRL_MIRROR_V	= 0x04,
	DISP_CTRL_BGR		= 0x80,
};




class Display_RGB565_spi : public DisplayIF
{
public:
	Display_RGB565_spi(
		int	nDDRAM_Width,
		int	nDDRAM_Height,
		int nDispCtrl,
		int	nRotate,
		int nGpioCS,
		int nGpioDC,
		int nGpioReset,
		int nGpioBackLight = -1,
		int	nSpiSpeed = 10000000,
		int nSpiMode = SPI_MODE_0 ) :
		m_tDDRAM(nDDRAM_Width,nDDRAM_Height),
		m_iCS( nGpioCS ),
		m_iDC( nGpioDC ),
		m_iRST(nGpioReset),
		m_iBL( nGpioBackLight),
		m_iSPI( nSpiSpeed, nSpiMode )
	{
		// Set initial state
		m_iCS	<< 1;
		m_iBL	<< 0;	// Backlight Off
		m_iRST	<< 1;	// Reset = high
		m_iDC	<< 0;	// CommandMode

		switch( nRotate )
		{
		case 0:		nDispCtrl	^= 0x00;									break;
		case 90:	nDispCtrl	^= DISP_CTRL_SWAP_HV  | DISP_CTRL_MIRROR_V;	break;
		case 180:	nDispCtrl	^= DISP_CTRL_MIRROR_H | DISP_CTRL_MIRROR_V;	break;
		case 270:	nDispCtrl	^= DISP_CTRL_MIRROR_H | DISP_CTRL_SWAP_HV;	break;
		}

		m_nDispCtrl	= nDispCtrl;
	}

	virtual	int	Init()
	{
		printf( "Display_RGB565_spi<%d,%d>::Init()\n", m_tDDRAM.width, m_tDDRAM.height );

		if( m_nDispCtrl & DISP_CTRL_SWAP_HV )
		{
			m_tDispSize.width	= m_tDDRAM.height;
			m_tDispSize.height	= m_tDDRAM.width;
		}
		else
		{
			m_tDispSize.width	= m_tDDRAM.width;
			m_tDispSize.height	= m_tDDRAM.height;
		}

		m_iFrameBuf.resize( m_tDDRAM.width * m_tDDRAM.height * 2 );
		return	0;
	}

	virtual int	DispClear()
	{
		printf( "Display_RGB565_spi<%d,%d>::DispClear()\n", m_tDDRAM.width, m_tDDRAM.height );

		memset( m_iFrameBuf.data(), 0, m_iFrameBuf.size() ); 
		return	TransferRGB( 0, 0, m_tDispSize.width, m_tDispSize.height, m_iFrameBuf.data(), m_iFrameBuf.size() );
	}

	virtual int	DispOn()
	{
		printf( "Display_RGB565_spi<%d,%d>::DispOn()\n", m_tDDRAM.width, m_tDDRAM.height );

		// Enable back light
		m_iBL	<< 1;
		
		return	0;
	}

	virtual int	DispOff()
	{
		printf( "Display_RGB565_spi<%d,%d>::DispOff()\n", m_tDDRAM.width, m_tDDRAM.height );

		// Disable back light
		m_iBL	<< 0;
		
		return	0;
	}

	virtual int	Quit()
	{
		printf( "Display_RGB565_spi<%d,%d>::Quit()\n", m_tDDRAM.width, m_tDDRAM.height );

		m_tDispSize.width	= 0;
		m_tDispSize.height	= 0;

		return	0;
	}

	virtual	int	WriteImageBGRA( int x, int y, const uint8_t* image, int stride, int cx, int cy )
	{
		if( _CalcTransArea( x, y, image, stride, 4, cx, cy ) )
		{
			ImageConvert::BGRA8888toRGB565( image, stride, cx, cy, m_iFrameBuf.data(), cx * 2 );

			return	TransferRGB( x, y, cx, cy, m_iFrameBuf.data(), cx * cy * 2 );
		}
		
		return	-1;
	}

	virtual	int WriteImageGRAY( int x, int y, const uint8_t* image, int stride, int cx, int cy )
	{
		if( _CalcTransArea( x, y, image, stride, 1, cx, cy ) )
		{
			ImageConvert::GRAY8toRGB565( image, stride, cx, cy, m_iFrameBuf.data(), cx * 2 );

			return	TransferRGB( x, y, cx, cy, m_iFrameBuf.data(), cx * cy * 2  );
		}
		
		return	-1;
	}
	
	virtual	int GetBPP()
	{
		return	16;
	}

protected:

	static	void	SleepM(int sleep)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
	}

	virtual	int		TransferRGB( int x, int y, int cx, int cy, const uint8_t * image, int image_bytes )=0;

protected:
	const DispSize			m_tDDRAM;
	int						m_nDispCtrl; 
	ctrl_spi				m_iSPI;
	GpioOut					m_iCS;
	GpioOut 				m_iRST;
	GpioOut 				m_iDC;
	GpioOut 				m_iBL;
	std::vector<uint8_t>	m_iFrameBuf;
};


class Display_RGB565_spi8 : public Display_RGB565_spi
{
public:
	Display_RGB565_spi8(
		int	nDDRAM_width,
		int	nDDRAM_height,
		int nDispCtrl,
		int	nRotate,
		int	nGpioCS,
		int nGpioDC,
		int nGpioReset,
		int nGpioBackLight = -1,
		int	nSpiSpeed = 10000000,
		int nSpiMode = SPI_MODE_0 ) :
		Display_RGB565_spi( nDDRAM_width, nDDRAM_height, nDispCtrl, nRotate, nGpioCS, nGpioDC, nGpioReset, nGpioBackLight, nSpiSpeed, nSpiMode )
	{
	}

	virtual	int	Init()
	{
		uint8_t	nMADCTL	= 0;
		
		if( m_nDispCtrl & DISP_CTRL_MIRROR_V )	nMADCTL	|= 1 << 7;
		if( m_nDispCtrl & DISP_CTRL_MIRROR_H )	nMADCTL	|= 1 << 6;
		if( m_nDispCtrl & DISP_CTRL_SWAP_HV )	nMADCTL	|= 1 << 5;
		if( m_nDispCtrl & DISP_CTRL_BGR )		nMADCTL	|= 1 << 3;		
		
		// ILI9341:	15.4. Reset Timing
		// ST7735:	9.16 Reset Timing
		m_iRST	<< 1;	// Reset = high
		SleepM(1);
		m_iRST	<< 0;	// Reset = Low
		SleepM(10);		// >10usec
		m_iRST	<< 1;	// Reset = high
		SleepM(120);	// Note 7: It is necessary to wait 5msec after releasing RESX before sending commands.

		// ILI9341:	8.2.29. Memory Access Control (36h)
		// ST7735:	10.1.26 MADCTL (36h): Memory Data Access Control
		WriteReg(	MIPI_DCS_SET_ADDRESS_MODE,
					nMADCTL );

		// ILI9341:	8.2.33. COLMOD: Pixel Format Set (3Ah)
		// ST7735:	10.1.29 COLMOD (3Ah): Interface Pixel Format
		WriteReg(	MIPI_DCS_SET_PIXEL_FORMAT,
					0x55 );	// RGB565
		
		// ILI9341: 8.2.15. Display Inversion OFF (20h)
		WriteReg(	MIPI_DCS_EXIT_INVERT_MODE );
		
		// ILI9341: 8.2.14. Normal Display Mode ON (13h)
		WriteReg(	MIPI_DCS_ENTER_NORMAL_MODE );

		return	Display_RGB565_spi::Init();
	}

	virtual	int	DispOn()
	{
		// ILI9341:	8.2.12. Sleep Out (11h)
		// ST7735:	10.1.11 SLPOUT (11h): Sleep Out
		WriteReg(	MIPI_DCS_EXIT_SLEEP_MODE );
		SleepM(120);

		// ILI9341:	8.2.19. Display ON (29h)
		// ST7735:	10.1.18 DISPON (29h): Display On
		WriteReg(	MIPI_DCS_SET_DISPLAY_ON );
		SleepM(120);

		return	Display_RGB565_spi::DispOn();
	}
	
	virtual int	DispOff()
	{
		int	 ret	= Display_RGB565_spi::DispOff();

		// 10.1.17 DISPOFF (28h): Display Off
		WriteReg(	MIPI_DCS_SET_DISPLAY_OFF );
		SleepM(20);

		// 10.1.10 SLPIN (10h): Sleep In
		WriteReg(	MIPI_DCS_ENTER_SLEEP_MODE );

		return	ret;
	}

	virtual int	Quit()
	{
		// 9.12 Power ON/OFF Sequence
		m_iRST	<< 0;

		return	Display_RGB565_spi::Quit();
	}


protected:

	virtual	void	WriteSpi( uint8_t cmd, int data_size, const uint8_t * data )
	{
		// ChipSelect
		m_iCS	<< 0;

		// Command
		m_iDC	<< 0;
		m_iSPI.write( &cmd, sizeof(cmd) );

		if( 0 < data_size )
		{
			m_iDC	<< 1;
			m_iSPI.write( data, data_size);
		}
		
		m_iCS	<< 1;
	}

	template<typename ...Args>
	void	WriteReg( uint8_t cmd, Args ...args )
	{
		uint8_t	data[]	= { (uint8_t)args... };
	
		WriteSpi( cmd, sizeof(data), data );
	}

	virtual	int		TransferRGB( int x, int y, int cx, int cy, const uint8_t * image, int image_bytes )
	{
		int		ret;
		int		ex		= x+cx-1;
		int		ey		= y+cy-1;
		
		// 10.1.19 CASET (2Ah): Column Address Set
		WriteReg(	MIPI_DCS_SET_COLUMN_ADDRESS,
					(0xFF & (x >> 8)),
					(0xFF & x),
					(0xFF & (ex >> 8)),
					(0xFF & ex) );

		// 10.1.20 RASET (2Bh): Row Address Set
		WriteReg(	MIPI_DCS_SET_PAGE_ADDRESS,
					(0xFF & (y >> 8)),
					(0xFF & y),
					(0xFF & (ey >> 8)),
					(0xFF & ey) );

		// 10.1.21 RAMWR (2Ch): Memory Write
		WriteReg(	MIPI_DCS_WRITE_MEMORY_START );
		m_iCS	<< 0;
		m_iDC	<< 1;
		ret		= m_iSPI.write( image, image_bytes );
		m_iCS	<< 1;

/*
		// Transfer pixel data
		m_iCS	<< 0;
		m_iDC	<< 0;
		m_iSPI	<< MIPI_DCS_WRITE_MEMORY_START;
		m_iDC	<< 1;
		ret		= m_iSPI.write( image, cx * 2 * cy );
		m_iCS	<< 1;
*/		

		return	ret;
	}
};


class Display_RGB565_spi16 : public Display_RGB565_spi
{
public:
	Display_RGB565_spi16(
		int	nDDRAM_width,
		int	nDDRAM_height,
		int nDispCtrl,
		int	nRotate,
		int nGpioCS,
		int nGpioDC,
		int nGpioReset,
		int nGpioBackLight = -1,
		int	nSpiSpeed = 10000000,
		int nSpiMode = SPI_MODE_0 ) :
		Display_RGB565_spi( nDDRAM_width, nDDRAM_height, nDispCtrl, nRotate, nGpioCS, nGpioDC, nGpioReset, nGpioBackLight, nSpiSpeed, nSpiMode )
	{
	}

protected:

	void	SpiWrite( int _cmd, int _data )
	{
		uint8_t	cmd[2]	= { ((uint8_t*)&_cmd)[1],  ((uint8_t*)&_cmd)[0] };
		uint8_t	data[2]	= { ((uint8_t*)&_data)[1], ((uint8_t*)&_data)[0] };
		
		m_iCS	<< 0;

		m_iDC	<< 0;
		m_iSPI	<< cmd;	

		m_iDC	<< 1;
		m_iSPI	<< data;

		m_iCS	<< 1;
	}
};

#endif	//__DISPLAY_RGB565_SPI_H_INCLUDED__
