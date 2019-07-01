
#include "draw_pattern.h"
#include "../common/display_st7789_spi.h"


int main( int argc, char* argv[] )
{
	Display_ST7789_IPS_spi	iDisp(270);

	return	DrawPattern( iDisp );
}
