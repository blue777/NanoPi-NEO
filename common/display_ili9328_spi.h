
#include "display_rgb565_spi.h"

class Display_ILI9328_spi_TM22 : public Display_RGB565_spi16
{
public:
	Display_ILI9328_spi_TM22( int nRotate, int nGpioCS=-1, int nGpioDC=1, int nGpioReset=201, int nGpioBackLight=65 ) :
		Display_RGB565_spi16(
			240,
			320,
			DISP_CTRL_BGR | DISP_CTRL_MIRROR_V,
			nRotate,
			nGpioCS,
			nGpioDC,
			nGpioReset,
			nGpioBackLight,
			33 * 1000000,
			SPI_MODE_3 )
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
		
		// ILI9328:	7.2.5. Entry Mode (R03h)
		LCD_CtrlWrite_ILI9328( 0x0003, nEntryMode );

		// ILI9328: 7.2.11. RGB Display Interface Control 1 (R0Ch)
		// RGB565
		LCD_CtrlWrite_ILI9328( 0x000C, 0x0001 );

		LCD_CtrlWrite_ILI9328(0x00E5, 0x78F0);	// set SRAM internal timing 
		LCD_CtrlWrite_ILI9328(0x0001, 0x0000);	// set SS and SM bit 
		LCD_CtrlWrite_ILI9328(0x0002, 0x0400);	// set 1 line inversion 
//		LCD_CtrlWrite_ILI9328(0x0003, 0x1090);	// set GRAM write direction and BGR=1.  
		LCD_CtrlWrite_ILI9328(0x0004, 0x0000);	// Resize register 
		LCD_CtrlWrite_ILI9328(0x0008, 0x0202);	// set the back porch and front porch 
		LCD_CtrlWrite_ILI9328(0x0009, 0x0000);	// set non-display area refresh cycle ISC[3:0] 
		LCD_CtrlWrite_ILI9328(0x000A, 0x0000);	// FMARK function 
//		LCD_CtrlWrite_ILI9328(0x000C, 0x0000);	// RGB interface setting 
		LCD_CtrlWrite_ILI9328(0x000D, 0x0000);	// Frame marker Position 
		LCD_CtrlWrite_ILI9328(0x000F, 0x0000);	// RGB interface polarity 

		LCD_CtrlWrite_ILI9328(0x002B, 0x000D);	//Set Frame Rate 

		// ----------- Adjust the Gamma   Curve ----------// 
		LCD_CtrlWrite_ILI9328(0x0030, 0x0000); 
		LCD_CtrlWrite_ILI9328(0x0031, 0x0607); 
		LCD_CtrlWrite_ILI9328(0x0032, 0x0305); 
		LCD_CtrlWrite_ILI9328(0x0035, 0x0000); 
		LCD_CtrlWrite_ILI9328(0x0036, 0x1604); 
		LCD_CtrlWrite_ILI9328(0x0037, 0x0204); 
		LCD_CtrlWrite_ILI9328(0x0038, 0x0001); 
		LCD_CtrlWrite_ILI9328(0x0039, 0x0707); 
		LCD_CtrlWrite_ILI9328(0x003C, 0x0000); 
		LCD_CtrlWrite_ILI9328(0x003D, 0x000F); 

		//------------------ Set GRAM area ---------------// 
		LCD_CtrlWrite_ILI9328(0x0060, 0xA700);	// Gate Scan Line 
		LCD_CtrlWrite_ILI9328(0x0061, 0x0001);	// NDL,VLE, REV 
		LCD_CtrlWrite_ILI9328(0x006A, 0x0000);	// set scrolling line 
		
		//-------------- Partial Display Control ---------// 
		LCD_CtrlWrite_ILI9328(0x0080, 0x0000); 
		LCD_CtrlWrite_ILI9328(0x0081, 0x0000); 
		LCD_CtrlWrite_ILI9328(0x0082, 0x0000); 
		LCD_CtrlWrite_ILI9328(0x0083, 0x0000); 
		LCD_CtrlWrite_ILI9328(0x0084, 0x0000); 
		LCD_CtrlWrite_ILI9328(0x0085, 0x0000); 
		
		//-------------- Panel Control -------------------// 
		LCD_CtrlWrite_ILI9328(0x0092, 0x0600); 

		return	Display_RGB565_spi::Init();
	}

	virtual	int	DispOn()
	{
		//*************Power On sequence ******************//
		LCD_CtrlWrite_ILI9328(0x0010, 0x0000);	// SAP, BT[3:0], AP, DSTB, SLP, STB 
		LCD_CtrlWrite_ILI9328(0x0011, 0x0007);	// DC1[2:0], DC0[2:0], VC[2:0] 
		LCD_CtrlWrite_ILI9328(0x0012, 0x0000);	// VREG1OUT voltage 
		LCD_CtrlWrite_ILI9328(0x0013, 0x0000);	// VDV[4:0] for VCOM amplitude 
		LCD_CtrlWrite_ILI9328(0x0007, 0x0001);	
		SleepM(200);				// Dis-charge capacitor power voltage 
		
		LCD_CtrlWrite_ILI9328(0x0010, 0x1690);	// SAP, BT[3:0], AP, DSTB, SLP, STB 
		LCD_CtrlWrite_ILI9328(0x0011, 0x0227);	// Set DC1[2:0], DC0[2:0], VC[2:0] 
		SleepM(50);					// Delay 50ms   
		LCD_CtrlWrite_ILI9328(0x0012, 0x008C);	// External reference voltage= Vci;
		SleepM(50);					// Delay 50ms 
		LCD_CtrlWrite_ILI9328(0x0013, 0x1500);	// VDV[4:0] for VCOM amplitude  0X1000
		LCD_CtrlWrite_ILI9328(0x0029, 0x0004);	// VCM[5:0] for VCOMH 		 0X0009
		SleepM(50);					// Delay 50ms 
		LCD_CtrlWrite_ILI9328(0x0007, 0x0133);	// 262K color and display ON 

		return	Display_RGB565_spi::DispOn();
	}
	
	virtual int	DispOff()
	{
		int	 ret	= Display_RGB565_spi::DispOff();

		LCD_CtrlWrite_ILI9328(0x0007, 0x0131);	// Set D1=0, D0=1
		SleepM(10);
		LCD_CtrlWrite_ILI9328(0x0007, 0x0130);	// Set D1=0, D0=0
		SleepM(10);
		LCD_CtrlWrite_ILI9328(0x0007, 0x0000);	// display OFF
		//************* Power OFF sequence ************** //
		LCD_CtrlWrite_ILI9328(0x0010, 0x0080);	// SAP, BT[3:0], APE, AP, DSTB, SLP
		LCD_CtrlWrite_ILI9328(0x0011, 0x0000);	// DC1[2:0], DC0[2:0], VC[2:0]
		LCD_CtrlWrite_ILI9328(0x0012, 0x0000);	// VREG1OUT voltage
		LCD_CtrlWrite_ILI9328(0x0013, 0x0000);	// VDV[4:0] for VCOM amplitude
		SleepM(200);							// Dis-charge capacitor power voltage
		LCD_CtrlWrite_ILI9328(0x0010, 0x0082);	// SAP, BT[3:0], APE, AP, DSTB, SLP

		return	ret;
	}

	virtual int	Quit()
	{
		// 9.12 Power ON/OFF Sequence
		m_iRST	<< 0;

		return	Display_RGB565_spi::Quit();
	}

protected:
	virtual	int		TransferRGB(int x, int y, int cx, int cy, const uint8_t * image, int image_bytes )
	{
		int		ret;
		uint8_t	WriteDataCmd[3]	= { 0x70, 0x00, 0x22 };

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
			// ILI9328:	7.2.24. Horizontal and Vertical RAM Address Position (R50h, R51h, R52h, R53h)
			LCD_CtrlWrite_ILI9328( 0x0050, xs );
			LCD_CtrlWrite_ILI9328( 0x0051, xe );
			LCD_CtrlWrite_ILI9328( 0x0052, ys );
			LCD_CtrlWrite_ILI9328( 0x0053, ye );
	
			// ILI9328: 7.2.18. GRAM Horizontal/Vertical Address Set (R20h, R21h)
			LCD_CtrlWrite_ILI9328( 0x0020, xa );
			LCD_CtrlWrite_ILI9328( 0x0021, ya );
		}
		else
		{
			// ILI9328:	7.2.24. Horizontal and Vertical RAM Address Position (R50h, R51h, R52h, R53h)
			LCD_CtrlWrite_ILI9328( 0x0050, ys );
			LCD_CtrlWrite_ILI9328( 0x0051, ye );
			LCD_CtrlWrite_ILI9328( 0x0052, xs );
			LCD_CtrlWrite_ILI9328( 0x0053, xe );
	
			// ILI9328: 7.2.18. GRAM Horizontal/Vertical Address Set (R20h, R21h)
			LCD_CtrlWrite_ILI9328( 0x0020, ya );
			LCD_CtrlWrite_ILI9328( 0x0021, xa );
		}

		// ILI9328:	7.2.19. Write Data to GRAM (R22h)
		m_iDC	<< 0;
		m_iCS	<< 0;
		m_iSPI	<< WriteDataCmd;
		m_iCS	<< 1;

		m_iDC	<< 1;
		size_t	sz	= cx * cy * 2;
		while( 0 < sz )
		{
			uint8_t	data[4096];
			size_t	trans	= sizeof(data)-2 < sz ? sizeof(data)-2 : sz;

			data[0]	= 0x72;
			memcpy( &data[1], image, trans );

			m_iCS	<< 0;
			ret		= m_iSPI.write( data, 1 + trans );
			m_iCS	<< 1;

			image	+= trans;
			sz		-= trans;
		}

		return	ret;
	}

	void	LCD_CtrlWrite_ILI9328( int _cmd, int _data )
	{
		uint8_t	cmd[3]	= { 0x70, ((uint8_t*)&_cmd)[1],  ((uint8_t*)&_cmd)[0] };
		uint8_t	data[3]	= { 0x72, ((uint8_t*)&_data)[1], ((uint8_t*)&_data)[0] };
		
		m_iDC	<< 0;
		m_iCS	<< 0;
		m_iSPI	<< cmd;	
		m_iCS	<< 1;

		m_iDC	<< 1;
		m_iCS	<< 0;
		m_iSPI	<< data;
		m_iCS	<< 1;
	}

};
