
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
