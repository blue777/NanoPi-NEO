
#include "draw_pattern.h"
#include "../common/display_fbdev.h"


int main( int argc, char* argv[] )
{
	const char* fbdev	= 1 < argc ? argv[1] : "/dev/fb1";
	
	Display_fbdev			iDisp( fbdev );

	return	DrawPattern( iDisp );
}
