
#include "draw_pattern.h"
#include "../common/display_st7789_spi.h"


int main( int argc, char* argv[] )
{
	int	rotate	= 2 <= argc ? atoi( argv[1] ) : 0;

	Display_ST7789_IPS_240x240_spi	iDisp(rotate);

	return	DrawPattern( iDisp );
}
