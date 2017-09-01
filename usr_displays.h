
#include <vector>

#include "common/display_st7735_spi.h"
#include "common/display_ili9341_spi.h"
#include "common/display_ili9328_spi.h"
#include "common/display_ili9225_spi.h"
#include "common/display_ssd1306_i2c.h"

std::vector<DisplayIF*>	GetUsrDisplays()
{
	std::vector<DisplayIF*>	iDisplays;

//	iDisplays.push_back( new Display_SSD1306_i2c(180,0) );
//	iDisplays.push_back( new Display_SSD1306_i2c(180,2) );	// for SH1306

//	iDisplays.push_back( new Display_ILI9341_spi_TM24(270) );
//	iDisplays.push_back( new Display_ILI9341_spi_TM22(270) );
//	iDisplays.push_back( new Display_ILI9341_spi_TM22(0) );
	iDisplays.push_back( new Display_ILI9328_spi_TM22(270));
//	iDisplays.push_back( new Display_ILI9225_spi(270) );
//	iDisplays.push_back( new Display_ST7735_spi(90) );

	return	iDisplays;
}

