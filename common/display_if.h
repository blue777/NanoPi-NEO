#ifndef __DISPALY_BASE_INCLUDED__
#define __DISPALY_BASE_INCLUDED__

#include <stdio.h>
#include <stdint.h>


class DisplayIF
{
public:
	class DispSize
	{
	public:
		DispSize()
		{
			width	= 0;
			height	= 0;
		}

		DispSize( int w, int h )
		{
			width	= w;
			height	= h;
		};

	public:
		uint16_t    width;
		uint16_t    height;
	};

public:
	DisplayIF(){};
	virtual ~DisplayIF(){};
	
	virtual int Init() = 0;
	virtual int DispClear()=0;
	virtual int DispOn()=0;
	virtual int DispOff()=0;
	virtual int Quit()=0;

	virtual	int WriteImageBGRA( int x, int y, const uint8_t* image, int stride, int cx, int cy )=0;
	virtual	int WriteImageGRAY( int x, int y, const uint8_t* image, int stride, int cx, int cy )=0;
	
	virtual	void Flush()
	{
	}

	const DispSize&    GetSize()
	{
		return  m_tDispSize;
	}
	
	virtual	int			GetBPP()=0;

protected:

	template<class PIXEL_PTR>
	static  PIXEL_PTR   GetLine( const uint8_t* base, int stride, int y )
	{
		return  (PIXEL_PTR)&base[stride * y];
	}

	bool    _CalcTransArea( int& x, int& y, const uint8_t* & image, int stride, int BytesPerPixel, int& cx, int& cy )
	{
		if( x < 0 )
		{
			image   -= x * BytesPerPixel;
			cx      += x;
			x       = 0;
		}

		if( m_tDispSize.width < (x + cx) )
		{
			cx  = m_tDispSize.width - x;
		}

		if( y < 0 )
		{
			image   -= y * stride;
			cy      += y;
			y       = 0;
		}

		if( m_tDispSize.height < (y + cy) )
		{
			cy  = m_tDispSize.height - y;
		}

		if( (cx <= 0) ||
			(cy <= 0) )
		{
			printf("ERROR: DisplayIF::_NormalizeTransArea(), No trans area.\n");   
			return  false;
		}
	  
		return  true;
	}


protected:
	DispSize     m_tDispSize;
};

#endif