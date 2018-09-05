
#include <vector>
#include <stdint.h>
#include "../common/display_if.h"

int	DrawPattern( DisplayIF& iDisp )
{
	std::vector<uint32_t>	buff;
	int						x,y,cx,cy,v;
	int						ret;
	int						loop	= 2;

	////////////////////////////////////////////////////////////////
	// DisplayIF::Init()
	////////////////////////////////////////////////////////////////
	ret	= iDisp.Init();
	if( 0 != ret )
	{
		printf( "[ERROR] iDisp.Init() ret %d\n", ret );
		return	ret;
	}

	ret	= iDisp.DispOn();
	if( 0 != ret )
	{
		printf( "[ERROR] iDisp.DispOn() ret %d\n", ret );
		return	ret;
	}
	
	ret	= iDisp.DispClear();
	if( 0 != ret )
	{
		printf( "[ERROR] iDisp.DispClear() ret %d\n", ret );
		return	ret;
	}

	cx	= iDisp.GetSize().width;
	cy	= iDisp.GetSize().height;
	buff.resize( cx * cy );

	while( 0 < loop )
	{
		loop--;
		
		// Gradation
		for( y = 0; y < cy; y++ )
		{
			uint32_t*	line	= &buff.data()[ y * cx] ;
			x	= 0;
	
			for( ; x < cx*1/4; x++ )
			{
				v		= y * 256 / cy;
				line[x]	= 0xFF000000 | (v << 16);
			}
			for( ; x < cx*2/4; x++ )
			{
				v		= y * 256 / cy;
				line[x]	= 0xFF000000 | (v << 8);
			}
			for( ; x < cx*3/4; x++ )
			{
				v		= y * 256 / cy;
				line[x]	= 0xFF000000 | (v << 0);
			}
			for( ; x < cx*4/4; x++ )
			{
				v		= y * 256 / cy;
				line[x]	= 0xFF000000 | (v << 16) | (v << 8) | (v << 0);
			}
		}
		iDisp.WriteImageBGRA( 0, 0, (uint8_t*)buff.data(), cx * 4, cx, cy );
		printf( "Test: RGBg gradation. OK ? Hit any key to execute next pattern.\n");
		getchar();

		// Gradation
		for( y = 0; y < cy; y++ )
		{
			uint32_t*	line	= &buff.data()[ y * cx] ;
			x	= 0;

			for( ; x < cx*1/2; x++ )
			{
				v		= 1 & (x + y) ? 0xFF : 0x00;
				line[x]	= 0xFF000000 | (v << 16) | (v << 8) | v;
			}
			for( ; x < cx*2/2; x++ )
			{
				v		= ((y * 256 / cy) << 0);
				line[x]	= 0xFF000000 | (v << 16) | (v << 8) | v;
			}
		}
		iDisp.WriteImageBGRA( 0, 0, (uint8_t*)buff.data(), cx * 4, cx, cy );
		printf( "Test: Gray gradation. OK ? Hit any key to execute next pattern.\n");
		getchar();

		// Red
		for( y = 0; y < cy; y++ )
		{
			uint32_t*	line	= &buff.data()[ y * cx] ;
	
			for( x = 0; x < cx/2; x++ )
			{
				v		= y * 256 / cy;
				line[x]	= 0xFF000000 | (v << 16);
			}
			for( x = cx/2; x < cx; x++ )
			{
				v		= y * 256 / cy;
				line[x]	= 0xFF000000 | (0xFF << 16) | (v << 8) | (v << 0);
			}
		}
		iDisp.WriteImageBGRA( 0, 0, (uint8_t*)buff.data(), cx * 4, cx, cy );
		printf( "Test: Red gradation. OK ? Hit any key to execute next pattern.\n");
		getchar();
	
		// Green
		for( y = 0; y < cy; y++ )
		{
			uint32_t*	line	= &buff.data()[ y * cx] ;
	
			for( x = 0; x < cx/2; x++ )
			{
				v		= y * 256 / cy;
				line[x]	= 0xFF000000 | (v << 8);
			}
			for( x = cx/2; x < cx; x++ )
			{
				v		= y * 256 / cy;
				line[x]	= 0xFF000000 | (v << 16) | (0xFF << 8) | (v << 0);
			}
		}
		iDisp.WriteImageBGRA( 0, 0, (uint8_t*)buff.data(), cx * 4, cx, cy );
		printf( "Test: Green gradation. OK ? Hit any key to execute next pattern.\n");
		getchar();

		// Blue
		for( y = 0; y < cy; y++ )
		{
			uint32_t*	line	= &buff.data()[ y * cx] ;
	
			for( x = 0; x < cx/2; x++ )
			{
				v		= y * 256 / cy;
				line[x]	= 0xFF000000 | (v << 0);
			}
			for( x = cx/2; x < cx; x++ )
			{
				v		= y * 256 / cy;
				line[x]	= 0xFF000000 | (v << 16) | (v << 8) | (0xFF << 0);
			}
		}
		iDisp.WriteImageBGRA( 0, 0, (uint8_t*)buff.data(), cx * 4, cx, cy );
		printf( "Test: Blue gradation. OK ? Hit any key to execute next pattern.\n");
		getchar();

		// Bit
		for( y = 0; y < cy; y++ )
		{
			uint32_t*	line	= &buff.data()[ y * cx ];
	
			for( x = 0; x < cx; x++ )
			{
				line[x]	= 1 << (y * 32 / cy);
			}
		}
		iDisp.WriteImageBGRA( 0, 0, (uint8_t*)buff.data(), cx * 4, cx, cy );
		printf( "Test: Bit 0 - 31. OK ? Hit any key to execute next pattern.\n");
		getchar();
	}

	ret	= iDisp.DispOff();
	if( 0 != ret )
	{
		printf( "[ERROR] iDisp.DispOff() ret %d\n", ret );
		return	ret;
	}

	ret	= iDisp.Quit();
	if( 0 != ret )
	{
		printf( "[ERROR] iDisp.Quit() ret %d\n", ret );
		return	ret;
	}

	return  0;
}