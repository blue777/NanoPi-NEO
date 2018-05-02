#ifndef	__IMG_CONV_H_INCLUDED__
#define	__IMG_CONV_H_INCLUDED__

#include <stdint.h>

#define	IMAGE_GET_LINE(base,stride,y)		((void*)(&((uint8_t*)(base))[ stride * y]))

namespace ImageConvert
{
	template<class PIXEL_PTR>
	static  PIXEL_PTR   GetLine( PIXEL_PTR base, int stride, int y )
	{
		const uint8_t*	base8	= (const uint8_t*)base;
		return  (PIXEL_PTR)&base8[stride * y];
	}
	
	void	BGRA8888toGRAY8( const uint8_t* pSrcImage, int nSrcStride, int cx, int cy, uint8_t * pDstImage, int nDstStride )
	{
		for( int y = 0; y < cy; y++ )
		{
			const uint32_t* pSrcLine	= GetLine((const uint32_t*)pSrcImage,nSrcStride,y);
			uint8_t*		pDstLine	= GetLine(pDstImage,nDstStride,y);
			int				x			= 0;

			for( ; (x+4) <= cx; x +=4 )
			{
				const uint8_t*	s	= (const uint8_t*)&pSrcLine[x];

				pDstLine[x+0]	= (s[ 0] * 4732 + s[ 1] * 46871 + s[ 2] * 13933) >> 16;
				pDstLine[x+1]	= (s[ 4] * 4732 + s[ 5] * 46871 + s[ 6] * 13933) >> 16;
				pDstLine[x+2]	= (s[ 8] * 4732 + s[ 9] * 46871 + s[10] * 13933) >> 16;
				pDstLine[x+3]	= (s[12] * 4732 + s[13] * 46871 + s[14] * 13933) >> 16;
			}
			
			for( ; x < cx; x++ )
			{
				const uint8_t*	s	= (const uint8_t*)&pSrcLine[x];

				pDstLine[x+0]	= (s[ 0] * 4732 + s[ 1] * 46871 + s[ 2] * 13933) >> 16;
			}
		}
	}

	void	BGRA8888toRGB565( const uint8_t* pSrcImage, int nSrcStride, int cx, int cy, uint8_t * pDstImage, int nDstStride )
	{
		for( int y = 0; y < cy; y++ )
		{
			const uint32_t* pSrcLine	= GetLine((const uint32_t*)pSrcImage,nSrcStride,y);
			uint8_t*		pDstLine	= GetLine(pDstImage,nDstStride,y);
			int				x			= 0;

			for( ; (x+8) <= cx; x += 8 )
			{
				uint8_t*	d	= &pDstLine[x*2];
				uint32_t	s0	= pSrcLine[x+0];
				uint32_t	s1	= pSrcLine[x+1];
				uint32_t	s2	= pSrcLine[x+2];
				uint32_t	s3	= pSrcLine[x+3];
				uint32_t	s4	= pSrcLine[x+4];
				uint32_t	s5	= pSrcLine[x+5];
				uint32_t	s6	= pSrcLine[x+6];
				uint32_t	s7	= pSrcLine[x+7];
	
				d[0]	= (0xF8 & (s0 >> 16)) | (0x07 & (s0 >> 13));
				d[1]	= (0xE0 & (s0 >>  5)) | (0x1F & (s0 >>  3));
				d[2]	= (0xF8 & (s1 >> 16)) | (0x07 & (s1 >> 13));
				d[3]	= (0xE0 & (s1 >>  5)) | (0x1F & (s1 >>  3));
				d[4]	= (0xF8 & (s2 >> 16)) | (0x07 & (s2 >> 13));
				d[5]	= (0xE0 & (s2 >>  5)) | (0x1F & (s2 >>  3));
				d[6]	= (0xF8 & (s3 >> 16)) | (0x07 & (s3 >> 13));
				d[7]	= (0xE0 & (s3 >>  5)) | (0x1F & (s3 >>  3));
	
				d[ 8]	= (0xF8 & (s4 >> 16)) | (0x07 & (s4 >> 13));
				d[ 9]	= (0xE0 & (s4 >>  5)) | (0x1F & (s4 >>  3));
				d[10]	= (0xF8 & (s5 >> 16)) | (0x07 & (s5 >> 13));
				d[11]	= (0xE0 & (s5 >>  5)) | (0x1F & (s5 >>  3));
				d[12]	= (0xF8 & (s6 >> 16)) | (0x07 & (s6 >> 13));
				d[13]	= (0xE0 & (s6 >>  5)) | (0x1F & (s6 >>  3));
				d[14]	= (0xF8 & (s7 >> 16)) | (0x07 & (s7 >> 13));
				d[15]	= (0xE0 & (s7 >>  5)) | (0x1F & (s7 >>  3));
			}
	
			for( ; x < cx; x++ )
			{
				uint8_t*	d	= &pDstLine[x*2];
				uint32_t	s0	= pSrcLine[x+0];
	
				d[0]	= (0xF8 & (s0 >> 16)) | (0x07 & (s0 >> 13));
				d[1]	= (0xE0 & (s0 >>  5)) | (0x1F & (s0 >>  3));
			}
		}
	}

	void	BGRA8888toRGB888( const uint8_t* pSrcImage, int nSrcStride, int cx, int cy, uint8_t * pDstImage, int nDstStride )
	{
		for( int y = 0; y < cy; y++ )
		{
			const uint32_t* pSrcLine	= GetLine((const uint32_t*)pSrcImage,nSrcStride,y);
			uint8_t*		pDstLine	= GetLine(pDstImage,nDstStride,y);
			int				x			= 0;

			for( ; (x+4) <= cx; x += 4 )
			{
				uint32_t*	d	= (uint32_t*)&pDstLine[x*3];
				uint32_t	s0	= pSrcLine[x+0];
				uint32_t	s1	= pSrcLine[x+1];
				uint32_t	s2	= pSrcLine[x+2];
				uint32_t	s3	= pSrcLine[x+3];
	
				d[ 0]	= (0x000000FF & (s0 >> 16)) | (0x0000FF00 & (s0 >>  0))  | (0x00FF0000 & (s0 << 16)) | (0xFF000000 & (s1 <<  8));
				d[ 1]	= (0x000000FF & (s1 >>  8)) | (0x0000FF00 & (s1 <<  8))  | (0x00FF0000 & (s2 <<  0)) | (0xFF000000 & (s2 << 16));
				d[ 2]	= (0x000000FF & (s2 >>  0)) | (0x0000FF00 & (s3 >>  8))  | (0x00FF0000 & (s3 <<  8)) | (0xFF000000 & (s1 << 24));
			}
	
			for( ; x < cx; x++ )
			{
				uint8_t*	d	= &pDstLine[x*3];
				uint32_t	s0	= pSrcLine[x+0];
	
				d[0]	= 0xFF & (s0 >> 16);
				d[1]	= 0xFF & (s0 >>  8);
				d[2]	= 0xFF & (s0 >>  0);
			}
		}
	}
	
	void	BGRA8888toRGB565L( const uint8_t* pSrcImage, int nSrcStride, int cx, int cy, uint8_t * pDstImage, int nDstStride )
	{
		for( int y = 0; y < cy; y++ )
		{
			const uint32_t* pSrcLine	= GetLine((const uint32_t*)pSrcImage,nSrcStride,y);
			uint8_t*		pDstLine	= GetLine(pDstImage,nDstStride,y);
			int				x			= 0;

			for( ; (x+8) <= cx; x += 8 )
			{
				uint16_t*	d	= (uint16_t*)&pDstLine[x*2];
				uint32_t	s0	= pSrcLine[x+0];
				uint32_t	s1	= pSrcLine[x+1];
				uint32_t	s2	= pSrcLine[x+2];
				uint32_t	s3	= pSrcLine[x+3];
				uint32_t	s4	= pSrcLine[x+4];
				uint32_t	s5	= pSrcLine[x+5];
				uint32_t	s6	= pSrcLine[x+6];
				uint32_t	s7	= pSrcLine[x+7];
	
				d[0]	= (0xF800 & (s0 >> 8)) | (0x07E0 & (s0 >> 5)) | (0x001F & (s0 >> 3));
				d[1]	= (0xF800 & (s1 >> 8)) | (0x07E0 & (s1 >> 5)) | (0x001F & (s1 >> 3));
				d[2]	= (0xF800 & (s2 >> 8)) | (0x07E0 & (s2 >> 5)) | (0x001F & (s2 >> 3));
				d[3]	= (0xF800 & (s3 >> 8)) | (0x07E0 & (s3 >> 5)) | (0x001F & (s3 >> 3));
				d[4]	= (0xF800 & (s4 >> 8)) | (0x07E0 & (s4 >> 5)) | (0x001F & (s4 >> 3));
				d[5]	= (0xF800 & (s5 >> 8)) | (0x07E0 & (s5 >> 5)) | (0x001F & (s5 >> 3));
				d[6]	= (0xF800 & (s6 >> 8)) | (0x07E0 & (s6 >> 5)) | (0x001F & (s6 >> 3));
				d[7]	= (0xF800 & (s7 >> 8)) | (0x07E0 & (s7 >> 5)) | (0x001F & (s7 >> 3));
			}
	
			for( ; x < cx; x++ )
			{
				uint16_t*	d	= (uint16_t*)&pDstLine[x*2];
				uint32_t	s0	= pSrcLine[x+0];
	
				d[0]	= (0xF800 & (s0 >> 8)) | (0x07E0 & (s0 >> 5)) | (0x001F & (s0 >> 3));
			}
		}
	}

	void	GRAY8toRGB565( const uint8_t* pSrcImage, int nSrcStride, int cx, int cy, uint8_t * pDstImage, int nDstStride )
	{
		for( int y = 0; y < cy; y++ )
		{
			const uint8_t*	pSrcLine	= GetLine(pSrcImage,nSrcStride,y);
			uint8_t*		pDstLine	= GetLine(pDstImage,nDstStride,y);
			int				x			= 0;

			for( ; (x+8) <= cx; x += 8 )
			{
				uint8_t*	d	= &pDstLine[x*2];
				uint8_t		s0	= pSrcLine[x+0];
				uint8_t		s1	= pSrcLine[x+1];
				uint8_t		s2	= pSrcLine[x+2];
				uint8_t		s3	= pSrcLine[x+3];
				uint8_t		s4	= pSrcLine[x+4];
				uint8_t		s5	= pSrcLine[x+5];
				uint8_t		s6	= pSrcLine[x+6];
				uint8_t		s7	= pSrcLine[x+7];

				d[0]	= (0xF8 & (s0 >>  0)) | (0x07 & (s0 >>  5));
				d[1]	= (0xE0 & (s0 <<  3)) | (0x1F & (s0 >>  3));
				d[2]	= (0xF8 & (s1 >>  0)) | (0x07 & (s1 >>  5));
				d[3]	= (0xE0 & (s1 <<  3)) | (0x1F & (s1 >>  3));
				d[4]	= (0xF8 & (s2 >>  0)) | (0x07 & (s2 >>  5));
				d[5]	= (0xE0 & (s2 <<  3)) | (0x1F & (s2 >>  3));
				d[6]	= (0xF8 & (s3 >>  0)) | (0x07 & (s3 >>  5));
				d[7]	= (0xE0 & (s3 <<  3)) | (0x1F & (s3 >>  3));

				d[ 8]	= (0xF8 & (s4 >>  0)) | (0x07 & (s4 >>  5));
				d[ 9]	= (0xE0 & (s4 <<  3)) | (0x1F & (s4 >>  3));
				d[10]	= (0xF8 & (s5 >>  0)) | (0x07 & (s5 >>  5));
				d[11]	= (0xE0 & (s5 <<  3)) | (0x1F & (s5 >>  3));
				d[12]	= (0xF8 & (s6 >>  0)) | (0x07 & (s6 >>  5));
				d[13]	= (0xE0 & (s6 <<  3)) | (0x1F & (s6 >>  3));
				d[14]	= (0xF8 & (s7 >>  0)) | (0x07 & (s7 >>  5));
				d[15]	= (0xE0 & (s7 <<  3)) | (0x1F & (s7 >>  3));
			}

			for( ; x < cx; x++ )
			{
				uint8_t*	d	= &pDstLine[x*2];
				uint8_t		s0	= pSrcLine[x+0];
	
				d[0]	= (0xF8 & (s0 >>  0)) | (0x07 & (s0 >>  5));
				d[1]	= (0xE0 & (s0 <<  3)) | (0x1F & (s0 >>  3));
			}
		}
	}

	void	GRAY8toRGB888( const uint8_t* pSrcImage, int nSrcStride, int cx, int cy, uint8_t * pDstImage, int nDstStride )
	{
		for( int y = 0; y < cy; y++ )
		{
			const uint8_t*	pSrcLine	= GetLine(pSrcImage,nSrcStride,y);
			uint8_t*		pDstLine	= GetLine(pDstImage,nDstStride,y);
			int				x			= 0;

			for( ; (x+4) <= cx; x += 4 )
			{
				uint32_t*	d	= (uint32_t*)&pDstLine[x*3];
				uint8_t		s0	= pSrcLine[x+0];
				uint8_t		s1	= pSrcLine[x+1];
				uint8_t		s2	= pSrcLine[x+2];
				uint8_t		s3	= pSrcLine[x+3];

				d[0]	= (s0 << 0) | (s0 << 8) | (s0 << 16) | (s1 << 24);
				d[1]	= (s1 << 0) | (s1 << 8) | (s2 << 16) | (s2 << 24);
				d[2]	= (s2 << 0) | (s3 << 8) | (s3 << 16) | (s3 << 24);
			}

			for( ; x < cx; x++ )
			{
				uint8_t*	d	= &pDstLine[x*3];
				uint8_t		s0	= pSrcLine[x+0];
	
				d[0]	= s0;
				d[1]	= s0;
				d[2]	= s0;
			}
		}
	}
	
	void	GRAY8toRGB565L( const uint8_t* pSrcImage, int nSrcStride, int cx, int cy, uint8_t * pDstImage, int nDstStride )
	{
		for( int y = 0; y < cy; y++ )
		{
			const uint8_t*	pSrcLine	= GetLine(pSrcImage,nSrcStride,y);
			uint8_t*		pDstLine	= GetLine(pDstImage,nDstStride,y);
			int				x			= 0;

			for( ; (x+8) <= cx; x += 8 )
			{
				uint16_t*	d	= (uint16_t*)&pDstLine[x*2];
				uint8_t		s0	= pSrcLine[x+0];
				uint8_t		s1	= pSrcLine[x+1];
				uint8_t		s2	= pSrcLine[x+2];
				uint8_t		s3	= pSrcLine[x+3];
				uint8_t		s4	= pSrcLine[x+4];
				uint8_t		s5	= pSrcLine[x+5];
				uint8_t		s6	= pSrcLine[x+6];
				uint8_t		s7	= pSrcLine[x+7];

				d[0]	= (0xF800 & (s0 << 8)) | (0x07E0 & (s0 << 3)) | (0x001F & (s0 >> 3));
				d[1]	= (0xF800 & (s1 << 8)) | (0x07E0 & (s1 << 3)) | (0x001F & (s1 >> 3));
				d[2]	= (0xF800 & (s2 << 8)) | (0x07E0 & (s2 << 3)) | (0x001F & (s2 >> 3));
				d[3]	= (0xF800 & (s3 << 8)) | (0x07E0 & (s3 << 3)) | (0x001F & (s3 >> 3));
				d[4]	= (0xF800 & (s4 << 8)) | (0x07E0 & (s4 << 3)) | (0x001F & (s4 >> 3));
				d[5]	= (0xF800 & (s5 << 8)) | (0x07E0 & (s5 << 3)) | (0x001F & (s5 >> 3));
				d[6]	= (0xF800 & (s6 << 8)) | (0x07E0 & (s6 << 3)) | (0x001F & (s6 >> 3));
				d[7]	= (0xF800 & (s7 << 8)) | (0x07E0 & (s7 << 3)) | (0x001F & (s7 >> 3));
			}

			for( ; x < cx; x++ )
			{
				uint16_t*	d	= (uint16_t*)&pDstLine[x*2];
				uint8_t		s0	= pSrcLine[x+0];

				d[0]	= (0xF800 & (s0 << 8)) | (0x07E0 & (s0 << 3)) | (0x001F & (s0 >> 3));
			}
		}
	}
};

#endif	// __IMG_CONV_H_INCLUDED__