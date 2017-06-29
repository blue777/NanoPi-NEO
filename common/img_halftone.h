

namespace ImageHalftoning
{
	class ErrDiffAdd
	{
	public:
		ErrDiffAdd( unsigned char * pbImage, int stride, int width, int height )
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
					unsigned char* data	= &m_pbImage[ m_nStride * y + x ];
					
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


	void ErrDiff_FloydSteinberg( unsigned char * image, int stride, int width, int height )
	{
		ErrDiffAdd	iEDA( image, stride, width, height );
		
		for( int y = 0; y < height; y++ )
		{
			unsigned char* line	= image + stride * y;

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


	void    ErrDiff_Burkes( unsigned char * image, int stride, int width, int height )
	{
		ErrDiffAdd	iEDA( image, stride, width, height );
		
		for( int y = 0; y < height; y++ )
		{
			unsigned char* line	= image + stride * y;

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


	void    ErrDiff_Stucki( unsigned char * image, int stride, int width, int height )
	{
		ErrDiffAdd	iEDA( image, stride, width, height );
		
		for( int y = 0; y < height; y++ )
		{
			unsigned char* line	= image + stride * y;

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


	void    ErrDiff_Atkinson( unsigned char * image, int stride, int width, int height )
	{
		ErrDiffAdd	iEDA( image, stride, width, height );
		
		for( int y = 0; y < height; y++ )
		{
			unsigned char* line	= image + stride * y;

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


	void PatternDither_2x2( unsigned char * image, int stride, int width, int height )
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
