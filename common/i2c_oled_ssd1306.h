#include "i2c_base.h"

class I2C_OLED_SSD1306 : public i2c_base
{
public:
    I2C_OLED_SSD1306() :
        i2c_base("/dev/i2c-0",0x3C)
    {
    }
    
    bool    WriteCmd( unsigned char cmd )
    {
        unsigned char data[2];

        data[0] = 0x00; // Command Mode
        data[1] = cmd;

        return  write( data, 2 );
    }
    
    bool    WriteImage( unsigned char * image, int stride )
    {
        unsigned char  data[1+128*64/8];
        
        data[0] = 0x40; // Data Mode

        for( int y = 0; y < 64; y += 8 )
        {
            for( int x = 0; x < 128; x += 4 )
            {
                unsigned char * src = &image[ (stride * y) + x];
                unsigned char * dst = &data[ 1 + y / 8 * 128 + x ];

                dst[0] =    ((src[ 0 * stride + 0 ] >> 7) << 0) |
                            ((src[ 1 * stride + 0 ] >> 7) << 1) |
                            ((src[ 2 * stride + 0 ] >> 7) << 2) |
                            ((src[ 3 * stride + 0 ] >> 7) << 3) |
                            ((src[ 4 * stride + 0 ] >> 7) << 4) |
                            ((src[ 5 * stride + 0 ] >> 7) << 5) |
                            ((src[ 6 * stride + 0 ] >> 7) << 6) |
                            ((src[ 7 * stride + 0 ] >> 7) << 7);

				dst[1] =    ((src[ 0 * stride + 1 ] >> 7) << 0) |
                            ((src[ 1 * stride + 1 ] >> 7) << 1) |
                            ((src[ 2 * stride + 1 ] >> 7) << 2) |
                            ((src[ 3 * stride + 1 ] >> 7) << 3) |
                            ((src[ 4 * stride + 1 ] >> 7) << 4) |
                            ((src[ 5 * stride + 1 ] >> 7) << 5) |
                            ((src[ 6 * stride + 1 ] >> 7) << 6) |
                            ((src[ 7 * stride + 1 ] >> 7) << 7);

                dst[2] =    ((src[ 0 * stride + 2 ] >> 7) << 0) |
                            ((src[ 1 * stride + 2 ] >> 7) << 1) |
                            ((src[ 2 * stride + 2 ] >> 7) << 2) |
                            ((src[ 3 * stride + 2 ] >> 7) << 3) |
                            ((src[ 4 * stride + 2 ] >> 7) << 4) |
                            ((src[ 5 * stride + 2 ] >> 7) << 5) |
                            ((src[ 6 * stride + 2 ] >> 7) << 6) |
                            ((src[ 7 * stride + 2 ] >> 7) << 7);

                dst[3] =    ((src[ 0 * stride + 3 ] >> 7) << 0) |
                            ((src[ 1 * stride + 3 ] >> 7) << 1) |
                            ((src[ 2 * stride + 3 ] >> 7) << 2) |
                            ((src[ 3 * stride + 3 ] >> 7) << 3) |
                            ((src[ 4 * stride + 3 ] >> 7) << 4) |
                            ((src[ 5 * stride + 3 ] >> 7) << 5) |
                            ((src[ 6 * stride + 3 ] >> 7) << 6) |
                            ((src[ 7 * stride + 3 ] >> 7) << 7);
			}
        }

        return  write( data, sizeof(data));
    }

    bool    Init()
    {
        printf( "I2C_OLED_SSD1306::Init()\n");

		WriteCmd(0xAE); // #display off
		
		WriteCmd(0x00); // #set lower column address
		WriteCmd(0x10); // #set higher column address
		
		WriteCmd(0x40); // #set display start line
		
		WriteCmd(0xB0); // #set page address
		
		WriteCmd(0x81);
		WriteCmd(0xFF); // #0~255????????????????
		
		WriteCmd(0xA1); // #set segment remap
		
		WriteCmd(0xA6); // #normal / reverse
		
		WriteCmd(0xA8); // #multiplex ratio
		WriteCmd(0x3F); // #duty = 1/64
		
		WriteCmd(0xC8); // #Com scan direction
		
		WriteCmd(0xD3); // #set display offset
		WriteCmd(0x00); // 
		
		WriteCmd(0xD5); // #set osc division
		WriteCmd(0x80); // 
		
		WriteCmd(0xD9); // #set pre-charge period
		WriteCmd(0xF1); // 
		
		WriteCmd(0xDA); // #set COM pins
		WriteCmd(0x12); // 
		
		WriteCmd(0xDB); // #set vcomh
		WriteCmd(0x40); // 
		
		WriteCmd(0x8D); // #set charge pump enable
		WriteCmd(0x14); // 

    	WriteCmd(0x20); // set addressing mode
    	WriteCmd(0x00); // set horizontal addressing mode
	}
    
    bool    DisplayOn()
    {
        printf( "I2C_OLED_SSD1306::DisplayOn()\n");
        WriteCmd(0xAF);   // #display ON   
    }
};
