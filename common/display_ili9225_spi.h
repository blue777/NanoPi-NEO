
#include "display_rgb565_spi.h"

class Display_ILI9225_spi : public Display_RGB565_spi16
{
public:
	Display_ILI9225_spi( int nRotate, int nGpioCS=-1, int nGpioDC=1, int nGpioReset=201, int nGpioBackLight=65 ) :
		Display_RGB565_spi16(
			176,
			220,
			DISP_CTRL_MIRROR_V | DISP_CTRL_BGR,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			30 * 1000000 )
	{
	}

	virtual	int	Init()
	{
		int	nEntryMode	= 0;

		if( m_nDispCtrl & DISP_CTRL_SWAP_HV )		nEntryMode	|= 1 << 3;
		if( !(m_nDispCtrl & DISP_CTRL_MIRROR_H) )	nEntryMode	|= 1 << 4;
		if( !(m_nDispCtrl & DISP_CTRL_MIRROR_V) )	nEntryMode	|= 1 << 5;
		if( m_nDispCtrl & DISP_CTRL_BGR )			nEntryMode	|= 1 << 12;

		// Hardware reset
		m_iRST	<< 1;	// Reset = high
		SleepM(1);
		m_iRST	<< 0;	// Reset = Low
		SleepM(10);		// >10usec
		m_iRST	<< 1;	// Reset = high
		SleepM(50);		// Note 7: It is necessary to wait 5msec after releasing RESX before sending commands.
		
		// ILI9225:	8.2.5. Entry Mode (R03h)
		SpiWrite( 0x0003, nEntryMode );

		// 8.2.9. RGB Input Interface Control 1 (R0Ch)
		// RGB565
		SpiWrite( 0x000C, 0x0001 );

		return	Display_RGB565_spi::Init();
	}

	virtual	int	DispOn()
	{
		// ILI9225:	8.2.11. Power Control 1 (R10h)
		//	 DSTB, STB = 0
		SpiWrite( 0x0010, 0x0A00 );
		
		// ILI9225: 8.2.12. Power Control 2 (R11h)
		SpiWrite( 0x0011, 0x1037 );
		SleepM(50);

		// ILI9225 : 8.2.6. Display Control 1 (R07h)
		SpiWrite( 0x0007, 0x0017 );	// REV=On
		SleepM(120);

		return	Display_RGB565_spi::DispOn();
	}
	
	virtual int	DispOff()
	{
		int	 ret	= Display_RGB565_spi::DispOff();

		// ILI9225 : 8.2.6. Display Control 1 (R07h)
		SpiWrite( 0x0007, 0x0000 );
		SleepM(50);

		// ILI9225 : 8.2.12. Power Control 2 (R11h)
		SpiWrite( 0x0011, 0x0007 );
		SleepM(50);

		// ILI9225:	8.2.11. Power Control 1 (R10h)
		SpiWrite( 0x0010, 0x0A01 );
		
		return	ret;
	}

	virtual int	Quit()
	{
		// 9.12 Power ON/OFF Sequence
		m_iRST	<< 0;

		return	Display_RGB565_spi::Quit();
	}

protected:
	virtual	int		TransferRGB( int x, int y, int cx, int cy, const uint8_t * image, int image_bytes )
	{
		int		ret;
		uint8_t	WriteData[2]	= { 0x00, 0x22 };

		int	xs	= x;
		int	xe	= xs+cx-1;
		int	xa	= xs;
		int	ys	= y;
		int	ye	= ys+cy-1;
		int	ya	= ys;
		
		if( m_nDispCtrl & DISP_CTRL_MIRROR_H )
		{
			xs	= m_tDispSize.width  - x - cx;
			xe	= xs + cx - 1;
			xa	= xe;
		}
		
		if( m_nDispCtrl & DISP_CTRL_MIRROR_V )
		{
			ys	= m_tDispSize.height - y - cy;
			ye	= ys + cy - 1;
			ya	= ye;
		}

		if( !(m_nDispCtrl & DISP_CTRL_SWAP_HV) )
		{
			// ILI9225:	8.2.24. Horizontal and Vertical RAM Address Position (R36h/R37h, R38h/R39h)
			SpiWrite( 0x0036, xe );
			SpiWrite( 0x0037, xs );
			SpiWrite( 0x0038, ye );
			SpiWrite( 0x0039, ys );
	
			// ILI9225: 8.2.16. RAM Address Set (R20h, R21h)
			SpiWrite( 0x0020, xa );
			SpiWrite( 0x0021, ya );
		}
		else
		{
			// ILI9225:	8.2.24. Horizontal and Vertical RAM Address Position (R36h/R37h, R38h/R39h)
			SpiWrite( 0x0036, ye );
			SpiWrite( 0x0037, ys );
			SpiWrite( 0x0038, xe );
			SpiWrite( 0x0039, xs );
	
			// ILI9225: 8.2.16. RAM Address Set (R20h, R21h)
			SpiWrite( 0x0020, ya );
			SpiWrite( 0x0021, xa );
		}

		// ILI9225:	8.2.17. Write Data to GRAM (R22h)
		m_iCS	<< 0;
		m_iDC	<< 0;
		m_iSPI	<< WriteData;
		m_iDC	<< 1;
		ret		= m_iSPI.write( image, cx * 2 * cy );
		m_iCS	<< 1;
		
		return	ret;
	}
};
