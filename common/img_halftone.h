#ifndef	__IMG_HALFTONE_H_INCLUDED__
#define	__IMG_HALFTONE_H_INCLUDED__

#include <stdint.h>
#include <math.h>
#include <vector>

namespace ImageHalftoning
{
	class ErrDiffAdd
	{
	public:
		ErrDiffAdd( uint8_t * pbImage, int stride, int width, int height )
		{
			m_pbImage	= pbImage;
			m_nStride	= stride;
			m_nWidth	= width;
			m_nHeight	= height;
		}

		void	operator ()( int x, int y, int value )
		{
			if( (0 <= y) && (y < m_nHeight) )
			{
				if( (0 <= x) && (x < m_nWidth) )
				{
					uint8_t* data	= &m_pbImage[ m_nStride * y + x ];
					
					value	+= data[0];
					
					data[0]	= 0 == (value >> 8) ? value : (value >> 8) < 0 ? 0 : 255;
				}
			}
		}

	protected:
		unsigned char*	m_pbImage;
		int				m_nStride;
		int				m_nWidth;
		int				m_nHeight;
	};


	void ErrDiff_FloydSteinberg( uint8_t * image, int stride, int width, int height )
	{
		ErrDiffAdd	iEDA( image, stride, width, height );
		
		for( int y = 0; y < height; y++ )
		{
			uint8_t* line	= image + stride * y;

			for( int x = 0; x < width; x++ )
			{
				int		c	= line[x];
				int		e	= c > 127 ? c - 255 : c;
				
				line[x]	= c > 127 ? 255 : 0;

				//	-		*		7/16
				//	3/16	5/16	1/16
				iEDA( x+1, y+0, e * 7 / 16 );
		
				iEDA( x-1, y+1, e * 3 / 16 );
				iEDA( x+0, y+1, e * 5 / 16 );
				iEDA( x+1, y+1, e * 1 / 16 );
			}
		}
	}


	void    ErrDiff_Burkes( uint8_t* image, int stride, int width, int height )
	{
		ErrDiffAdd	iEDA( image, stride, width, height );
		
		for( int y = 0; y < height; y++ )
		{
			uint8_t* line	= image + stride * y;

			for( int x = 0; x < width; x++ )
			{
				int		c	= line[x];
				int		e	= c > 127 ? c - 255 : c;
				
				line[x]	= c > 127 ? 255 : 0;

				//	-		-		*		4/16	2/16
				//	1/16	2/16	4/16	2/16	1/16
				iEDA( x+1, y+0, e * 4 / 16 );
				iEDA( x+2, y+0, e * 2 / 16 );
		
				iEDA( x-2, y+1, e * 1 / 16 );
				iEDA( x-1, y+1, e * 2 / 16 );
				iEDA( x+0, y+1, e * 4 / 16 );
				iEDA( x+1, y+1, e * 2 / 16 );
				iEDA( x+2, y+1, e * 1 / 16 );
			}
		}
	}


	void    ErrDiff_Stucki( uint8_t * image, int stride, int width, int height )
	{
		ErrDiffAdd	iEDA( image, stride, width, height );
		
		for( int y = 0; y < height; y++ )
		{
			uint8_t* line	= image + stride * y;

			for( int x = 0; x < width; x++ )
			{
				int		c	= line[x];
				int		e	= c > 127 ? c - 255 : c;
				
				line[x]	= c > 127 ? 255 : 0;

				//	-		-		*		8/42	4/42
				//	2/42	4/42	8/42	4/42	2/42
				//	1/42	2/42	4/42	2/42	1/42
				iEDA( x+1, y+0, e * 8 / 42 );
				iEDA( x+2, y+0, e * 4 / 42 );
		
				iEDA( x-2, y+1, e * 2 / 42 );
				iEDA( x-1, y+1, e * 4 / 42 );
				iEDA( x+0, y+1, e * 8 / 42 );
				iEDA( x+1, y+1, e * 4 / 42 );
				iEDA( x+2, y+1, e * 2 / 42 );
		
				iEDA( x-2, y+2, e * 1 / 42 );
				iEDA( x-1, y+2, e * 2 / 42 );
				iEDA( x+0, y+2, e * 4 / 42 );
				iEDA( x+1, y+2, e * 2 / 42 );
				iEDA( x+2, y+2, e * 1 / 42 );
			}
		}
	}


	void    ErrDiff_Atkinson( uint8_t * image, int stride, int width, int height )
	{
		ErrDiffAdd	iEDA( image, stride, width, height );
		
		for( int y = 0; y < height; y++ )
		{
			uint8_t* line	= image + stride * y;

			for( int x = 0; x < width; x++ )
			{
				int		c	= line[x];
				int		e	= c > 127 ? c - 255 : c;
				
				line[x]	= c > 127 ? 255 : 0;

				//	-		-		*		1/8		1/8
				//	-		1/8		1/8		1/8		-
				//	-		-		1/8		-		-
				iEDA( x+1, y+0, e * 1 / 8 );
				iEDA( x+2, y+0, e * 1 / 8 );
		
				iEDA( x-1, y+1, e * 1 / 8 );
				iEDA( x+0, y+1, e * 1 / 8 );
				iEDA( x+1, y+1, e * 1 / 8 );

				iEDA( x+0, y+2, e * 1 / 8 );
			}
		}
	}


	void    ErrDiff_LinearFloydSteinberg( uint8_t * image, int stride, int width, int height )
	{
		int					shift	= 20;
		int					gamma[256];
		std::vector<int>	data(width*height);

		for( int i = 0; i < 256; i++ )
		{
			gamma[i]	= (int)(pow( i / 255.0, 2.2 ) * ((1 << shift) - 1));
		}
		
		for( int y = 0; y < height; y++ )
		{
			uint8_t*	src_line	= image + stride * y;
			int *		dst_line	= &data.data()[ width * y ];
			int			x			= 0;

			for( ; (x+4) <= width; x += 4 )
			{
				dst_line[x+0]	= gamma[ src_line[x+0] ];
				dst_line[x+1]	= gamma[ src_line[x+1] ];
				dst_line[x+2]	= gamma[ src_line[x+2] ];
				dst_line[x+3]	= gamma[ src_line[x+3] ];
			}

			for( ; x < width; x++ )
			{
				dst_line[x+0]	= gamma[ src_line[x+0] ];
			}
		}


		for( int y = 0; y < height; y++ )
		{
			int*		line		= &data.data()[ width * y ];
			uint8_t*	dst_line	= image + stride * y;

			for( int x = 0; x < width; x++ )
			{
				int		c	= line[x];
				int		e	= (1 << (shift-1)) <= c ? c - ((1 << shift) - 1) :  c;
				
				dst_line[x]	= (1 << (shift-1)) <= c ? 255 : 0;

				//	-		*		7/16
				//	3/16	5/16	1/16
				if( (x+1) < width )	line[x+1]	+= e * 7 / 16;

				if( (y+1) < height )
				{
					int*	lineN	= line + width;

					if( 0 <= (x-1) )	lineN[x-1]	+= e * 3 / 16;
										lineN[x  ]	+= e * 5 / 16;
					if( (x+2) < width ) lineN[x+1]	+= e * 1 / 16;
				}
			}
		}
	}
	
	void    ErrDiff_LinearStucki( uint8_t * image, int stride, int width, int height )
	{
		int					shift	= 20;
		int					gamma[256];
		std::vector<int>	data(width*height);

		for( int i = 0; i < 256; i++ )
		{
			gamma[i]	= (int)(pow( i / 255.0, 2.2 ) *((1 << shift) - 1));
		}
		
		for( int y = 0; y < height; y++ )
		{
			uint8_t*	src_line	= image + stride * y;
			int *		dst_line	= &data.data()[ width * y ];
			int			x			= 0;

			for( ; (x+4) <= width; x += 4 )
			{
				dst_line[x+0]	= gamma[ src_line[x+0] ];
				dst_line[x+1]	= gamma[ src_line[x+1] ];
				dst_line[x+2]	= gamma[ src_line[x+2] ];
				dst_line[x+3]	= gamma[ src_line[x+3] ];
			}

			for( ; x < width; x++ )
			{
				dst_line[x+0]	= gamma[ src_line[x+0] ];
			}
		}


		for( int y = 0; y < height; y++ )
		{
			int*		line		= &data.data()[ width * y ];
			uint8_t*	dst_line	= image + stride * y;

			for( int x = 0; x < width; x++ )
			{
				int		c	= line[x];
				int		e	= (1 << (shift-1)) <= c ? c - ((1 << shift) - 1) :  c;
				
				dst_line[x]	= (1 << (shift-1)) <= c ? 255 : 0;

				//	-		-		*		8/42	4/42
				//	2/42	4/42	8/42	4/42	2/42
				//	1/42	2/42	4/42	2/42	1/42
				if( (x+1) < width )	line[x+1]	+= e * 8 / 42;
				if( (x+2) < width ) line[x+2]	+= e * 4 / 42;

				if( (y+1) < height )
				{
					int*	lineN	= line + width;

					if( 0 <= (x-2) )	lineN[x-2]	+= e * 2 / 42;
					if( 0 <= (x-1) )	lineN[x-1]	+= e * 4 / 42;
										lineN[x  ]	+= e * 8 / 42;
					if( (x+2) < width ) lineN[x+1]	+= e * 4 / 42;
					if( (x+2) < width ) lineN[x+2]	+= e * 2 / 42;
				}

				if( (y+2) < height )
				{
					int*	lineN	= line + width * 2;

					if( 0 <= (x-2) )	lineN[x-2]	+= e * 1 / 42;
					if( 0 <= (x-1) )	lineN[x-1]	+= e * 2 / 42;
										lineN[x  ]	+= e * 4 / 42;
					if( (x+2) < width ) lineN[x+1]	+= e * 2 / 42;
					if( (x+2) < width ) lineN[x+2]	+= e * 1 / 42;
				}
			}
		}
	}


	void PatternDither_2x2( uint8_t * image, int stride, int width, int height )
	{
		for( int y = 0; (y+2) <= height; y += 2)
		{
			for( int x = 0; (x+2) <= width; x += 2 )
			{
				int	ave	=
						image[ (y+0) * stride + x+0 ] +
						image[ (y+0) * stride + x+1 ] +
						image[ (y+1) * stride + x+0 ] +
						image[ (y+1) * stride + x+1 ] ;
				
				switch( ave >> 7 )
				{
				default:
				case 0:
				case 1:
					image[ (y+0) * stride + x+0 ]	= 0;
					image[ (y+0) * stride + x+1 ]	= 0;
					image[ (y+1) * stride + x+0 ]	= 0;
					image[ (y+1) * stride + x+1 ]	= 0;
					break;
	
				case 2:
				case 3:
				case 4:
					image[ (y+0) * stride + x+0 ]	= 255;
					image[ (y+0) * stride + x+1 ]	= 0;
					image[ (y+1) * stride + x+0 ]	= 0;
					image[ (y+1) * stride + x+1 ]	= 0;
					break;
	
				case 5:
					image[ (y+0) * stride + x+0 ]	= 255;
					image[ (y+0) * stride + x+1 ]	= 0;
					image[ (y+1) * stride + x+0 ]	= 0;
					image[ (y+1) * stride + x+1 ]	= 255;
					break;
	
				case 6:
					image[ (y+0) * stride + x+0 ]	= 255;
					image[ (y+0) * stride + x+1 ]	= 255;
					image[ (y+1) * stride + x+0 ]	= 0;
					image[ (y+1) * stride + x+1 ]	= 255;
					break;
	
				case 7:
					image[ (y+0) * stride + x+0 ]	= 255;
					image[ (y+0) * stride + x+1 ]	= 255;
					image[ (y+1) * stride + x+0 ]	= 255;
					image[ (y+1) * stride + x+1 ]	= 255;
					break;
				}
				
			} 
		}
	}
};


#endif	// __IMG_HALFTONE_H_INCLUDED__