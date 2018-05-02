#ifndef	__DISPLAY_FBDEV_H_INCLUDED__
#define	__DISPLAY_FBDEV_H_INCLUDED__

#include <thread>
#include <vector>
#include "img_conv.h"

#include "display_if.h"

#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/omapfb.h>

//	if Console cursor is blinking..
//	echo 0 > /sys/class/vtconsole/vtcon1/bind


class Display_fbdev : public DisplayIF
{
public:
	Display_fbdev(
		const char * device_path = "/dev/fb0")
	{
		m_strDevPath	= device_path;
		m_nFD			= -1;
		m_pFrameBuffer	= NULL;
		
		memset( &m_tVarScreenInfo, 0, sizeof(m_tVarScreenInfo) );
		memset( &m_tFixScreenInfo, 0, sizeof(m_tFixScreenInfo) );
	}

	virtual	int	Init()
	{
		int		ret;
		int		fd;

		fd	= open( m_strDevPath.c_str(), O_RDWR );
		if( fd < 0 )
		{
			printf( "ERROR! Display_fbdev(%s)::Init(), open() ret %d.\n", m_strDevPath.c_str(),  fd  );
			return	-1;
		}

		ret	= ioctl( fd, FBIOGET_VSCREENINFO, &m_tVarScreenInfo );
		if( ret < 0 )
		{
			printf( "ERROR! Display_fbdev(%s)::Init(), ioctl() ret %d.\n", m_strDevPath.c_str(), ret );

			close( fd );
			return	-1;
		}

		ret	= ioctl( fd, FBIOGET_FSCREENINFO, &m_tFixScreenInfo );
		if( ret < 0 )
		{
			printf( "ERROR! Display_fbdev(%s)::Init(), ioctl() ret %d.\n", m_strDevPath.c_str(), ret );

			close( fd );
			return	-1;
		}
		
//		m_pFrameBuffer		= (uint8_t *)mmap( NULL, width * height * 4, PROT_READ, MAP_SHARED, m_nFD, 0 );
		m_pFrameBuffer		= (uint8_t *)mmap( NULL, m_tFixScreenInfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

		if( MAP_FAILED == m_pFrameBuffer )
		{
			printf( "ERROR! Display_fbdev(%s)::Init(), mmap() failed.\n", m_strDevPath.c_str() );

			m_pFrameBuffer	= NULL;
			close( fd );
			return	-1;			
		}
                 
		m_tDispSize.width	= m_tVarScreenInfo.xres_virtual;
		m_tDispSize.height	= m_tVarScreenInfo.yres_virtual;

		printf( "Display_fbdev(%s)::Init() %d x %d, %d bpp, stride=%d, RGB%d%d%d (%d-%d-%d)\n",
			m_strDevPath.c_str(),
			m_tDispSize.width,
			m_tDispSize.height,
			m_tVarScreenInfo.bits_per_pixel,
			m_tFixScreenInfo.line_length,
			m_tVarScreenInfo.red.length,
			m_tVarScreenInfo.green.length,
			m_tVarScreenInfo.blue.length,
			m_tVarScreenInfo.red.offset,
			m_tVarScreenInfo.green.offset,
			m_tVarScreenInfo.blue.offset );

/*
		{
			FILE*	fp	= fopen( "fb_dump.bin", "wb");
			if( fp != NULL )
			{
				fwrite( m_pFrameBuffer, 1, m_tFixScreenInfo.smem_len, fp );
				fclose( fp );
			}
		}
//*/

		return	0;
	}

	virtual int	DispClear()
	{
		if( m_pFrameBuffer != NULL )
		{
			memset( m_pFrameBuffer, 0, m_tFixScreenInfo.smem_len );
			msync( m_pFrameBuffer, m_tFixScreenInfo.smem_len, MS_SYNC );
		}
//		printf( "Display_RGB565_spi<%d,%d>::DispClear()\n", m_tDDRAM.width, m_tDDRAM.height );
//		return	TransferRGB565( 0, 0, m_tDispSize.width, m_tDispSize.height, m_iFrameBuf.data() );
		return	-1;
	}

	virtual int	DispOn()
	{
		return	0;
	}

	virtual int	DispOff()
	{
		return	0;
	}

	virtual int	Quit()
	{
		if( m_pFrameBuffer != NULL )
		{
			munmap( m_pFrameBuffer, m_tFixScreenInfo.smem_len );
			m_pFrameBuffer	= NULL;
		}
		return	0;
	}

	virtual	int	WriteImageBGRA( int x, int y, const uint8_t* image, int stride, int cx, int cy )
	{
		if( m_pFrameBuffer != NULL )
		{
			if( _CalcTransArea( x, y, image, stride, 4, cx, cy ) )
			{
				uint8_t	*	dst	= m_pFrameBuffer + m_tFixScreenInfo.line_length * y + x * (m_tVarScreenInfo.bits_per_pixel >> 3);
				
				switch( m_tVarScreenInfo.bits_per_pixel )
				{
				case 32:
					for( y = 0; y < cy; y++ )
					{
						memcpy( dst, image, cx * 4 );
						dst		+= m_tFixScreenInfo.line_length;
						image	+= stride;
					}
					return	0;
					
				case 16:
					ImageConvert::BGRA8888toRGB565L( image, stride, cx, cy, dst, m_tFixScreenInfo.line_length );
					return	0;
				}
			}
		}

		return	-1;
	}

	virtual	int WriteImageGRAY( int x, int y, const uint8_t* image, int stride, int cx, int cy )
	{
		if( _CalcTransArea( x, y, image, stride, 1, cx, cy ) )
		{
			uint8_t	*	dst	= m_pFrameBuffer + m_tFixScreenInfo.line_length * y + x * (m_tVarScreenInfo.bits_per_pixel >> 3);
			
			switch( m_tVarScreenInfo.bits_per_pixel )
			{
			case 32:
				for( y = 0; y < cy; y++ )
				{
					uint32_t*	dst_line	= (uint32_t*)dst;
					for( x = 0; x < cx; x++ )
					{
						uint32_t	value	= image[x];
						
						value	|= (value << 16) | (value << 8);
						value	|= 0xFF000000;
						
						dst_line[x]	= value;
					}

					dst		+= m_tFixScreenInfo.line_length;
					image	+= stride;
				}
				return	0;
				
			case 16:
				ImageConvert::GRAY8toRGB565L( image, stride, cx, cy, dst, m_tFixScreenInfo.line_length );
				return	0;
			}
		}
		
		return	-1;
	}
	
	virtual	void	Flush()
	{
		if( m_pFrameBuffer != NULL )
		{
			msync( m_pFrameBuffer, m_tFixScreenInfo.smem_len, MS_SYNC );
		}
	}
	
	virtual	int GetBPP()
	{
		return	m_tVarScreenInfo.bits_per_pixel;
	}

protected:
	std::string					m_strDevPath;
	int							m_nFD;
	struct fb_var_screeninfo	m_tVarScreenInfo;
	struct fb_fix_screeninfo	m_tFixScreenInfo;
	uint8_t*					m_pFrameBuffer;
};



#endif	//__DISPLAY_FBDEV_H_INCLUDED__
