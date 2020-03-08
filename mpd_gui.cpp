/******************************************************************************
	Copyright (C) 2017-2018 blue-7 (http://qiita.com/blue-7)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

/*

Required external packages and compile command:

Ubuntu + mpd:

	apt install libcv-dev libopencv-dev opencv-doc fonts-takao-pgothic libtag1-dev

	g++ -Ofast -pthread -std=c++11 mpd_gui.cpp -o mpd_gui.cpp.o `pkg-config --cflags opencv` `pkg-config --libs opencv` `freetype-config --cflags` `freetype-config --libs` `taglib-config --cflags` `taglib-config --libs`

Ubuntu + mpd NasPiDac

	apt install libcv-dev libopencv-dev opencv-doc fonts-takao-pgothic libtag1-dev

	g++ -Ofast -pthread -std=c++11 -DVOLUME_CTRL_I2C_AK449x=1 mpd_gui.cpp -o mpd_gui.cpp.o `pkg-config --cflags opencv` `pkg-config --libs opencv` `freetype-config --cflags` `freetype-config --libs` `taglib-config --cflags` `taglib-config --libs`

Volumio(Debian):

	apt install libcv-dev libopencv-dev opencv-doc fonts-takao-gothic libtag1-dev

	g++ -Ofast -pthread -std=c++11 -DVOLUMIO=1 mpd_gui.cpp -o mpd_gui.cpp.o `pkg-config --cflags opencv` `pkg-config --libs opencv` `freetype-config --cflags` `freetype-config --libs` `taglib-config --cflags` `taglib-config --libs` -mfpu=neon

*/

#include <string.h>
#include <map>
#include <string>
#include <ctime>
#include <opencv2/opencv.hpp>

#include "common/perf_log.h"
#include "common/img_font.h"
#include "common/ctrl_socket.h"
#include "common/ctrl_http.h"
#include "common/string_util.h"
#include "common/ctrl_socket.h"
#include "common/ctrl_pmoni.h"
#include "CoverArtExtractor.h"

#include "usr_displays.h"
#include "mpd_gui_conf.h"


#include "common/ctrl_i2c.h"


#if VOLUME_CTRL_I2C_AK449x
#include "common/ctrl_i2c.h"
#endif




class MusicController
{
public:

	static	std::map<std::string,std::string>	SplitMpdStatus( std::string &str )
	{
		std::map<std::string,std::string>	iInfo;
		std::string			line;
		std::istringstream	stream(str);
		while( std::getline( stream, line ))
		{
			std::string::size_type	pos	= line.find( ":" );
			if( pos != std::string::npos )
			{
				std::string	field( line, 0, pos );
				std::string	value( line, pos+2 );
				
				iInfo[field]	= value;
			}
		}
		
		return	iInfo;
	}

	static	void	PlayPause()
	{
#if VOLUMIO
		std::vector<uint8_t>	iData;

		Http::Get( VOLUMIO_HOST, VOLUMIO_PORT, "/api/v1/getstate", iData );

		std::string		str( (char*)iData.data(), iData.size() );

		size_t			pos		= str.find( "\"status\":" );
		int				value	= 0;

		if( pos != std::string::npos )
		{
			pos		+= 9;
			size_t	posE	= str.find( ',', pos );
			
			std::string	status( str.substr( pos, posE - pos ) );

			if( std::string::npos != status.find( "play" ) )
			{
				Http::Get( VOLUMIO_HOST, VOLUMIO_PORT, "/api/v1/commands/?cmd=pause", iData );
			}
			else
			{
				Http::Get( VOLUMIO_HOST, VOLUMIO_PORT, "/api/v1/commands/?cmd=play", iData );
			}
		}
#else
		Socket		iSock;
		std::string	str;

		iSock.connect( MPD_HOST, MPD_PORT );
		
		iSock	>> str;	// DummyRead
		iSock	<< "status\n";
		iSock	>> str;

		std::map<std::string,std::string>	iInfo	= SplitMpdStatus( str );

		if( iInfo["state"] == "play" )
		{
			iSock	<< "pause\n";
			iSock	>> str;	// dummy read
		}
		else
		{
			iSock	<< "play\n";
			iSock	>> str;	// dummy read
		}
#endif
	}

	static	std::string	VolumeCtrl( int offset )
	{
#if VOLUME_CTRL_I2C_AK449x			
		unsigned char	dataL[2]	= { 0x03, 0x00 };
		unsigned char	dataR[2]	= { 0x04, 0x00 };
		int				value		= 0;

		{
			ctrl_i2c		iic( "/dev/i2c-0",AK449x_CH0 );
			iic.write( dataL, 1 );
			iic.read( &dataL[1], 1 );

			value		= dataL[1];

			if( 0 < offset )
			{
				value	+= offset;
				value	= 135 < value ? value <= 255 ? value : 255 : 135;
			}
			else if( offset < 0 )
			{
				value	+= offset;
				value	= 135 <= value ? value : 0;
			}

			dataL[1]	= value;
			dataR[1]	= value;
			
			iic.write( dataL, 2 );
			iic.write( dataR, 2 );
		}

		{
			ctrl_i2c		iicR( "/dev/i2c-0",AK449x_CH1 );
			iicR.write( dataL, 2 );
			iicR.write( dataR, 2 );
		}

		if( 0 < value )
		{
			float		db_value	= (value - 255) * 0.5f;
			char		szBuf[64];
			sprintf( szBuf, "%.1f dB", db_value );

			return	szBuf;
		}		
		else
		{
			return	"- ∁EdB";
		}
#else
		
#if VOLUMIO
		std::vector<uint8_t>	iData;

		Http::Get( VOLUMIO_HOST, VOLUMIO_PORT, "/api/v1/getstate", iData );

		std::string		str( (char*)iData.data(), iData.size() );

		size_t			vol_pos	= str.rfind( ",\"volume\":" );
		int				value	= 0;

		if( vol_pos != std::string::npos )
		{
			try
			{
				char		szBuf[256];

				vol_pos	+= 10;
				value	= std::stoi( &str[vol_pos] );

				value	+= offset;
				value	= 0 <= value ? value <= 100 ? value : 100 : 0;

				sprintf( szBuf, "%d", value );

				str	= "/api/v1/commands/?cmd=volume&volume=";
				str	+= szBuf;

				Http::Get( VOLUMIO_HOST, VOLUMIO_PORT, str.c_str(), iData );
				
				return	szBuf;
			}
			catch( std::exception &e )
			{
			}
			
			return	"n/a";
		}
		else
		{
			return	"n/a";
		}
#else
		Socket		iSock;
		std::string	str;

		iSock.connect( MPD_HOST, MPD_PORT );
		
		iSock	>> str;	// DummyRead
		iSock	<< "status\n";
		iSock	>> str;

		std::map<std::string,std::string>	iInfo	= SplitMpdStatus( str );
		int									value	= std::stoi( iInfo["volume"] );

		if( 0 <= value )
		{
			value	+= offset;
			value	= 0 <= value ? value <= 100 ? value : 100 : 0;

			{
				char		szBuf[256];
				std::string	str;

				sprintf( szBuf, "setvol %d\n", value );

				iSock	<< szBuf;
				iSock	>> str;	// Dummy read

				sprintf( szBuf, "%d", value );
				return	szBuf;
			}
		}
		else
		{
			return	"n/a";
		}
#endif
#endif
	}


	static	void	PlayNextSong()
	{
#if VOLUMIO
		std::vector<uint8_t>	iData;

		Http::Get( VOLUMIO_HOST, VOLUMIO_PORT, "/api/v1/commands/?cmd=next", iData );
#else
		Socket		iSock;
		std::string	str;

		iSock.connect( MPD_HOST, MPD_PORT );
		iSock	<< "next\n";
		iSock	>> str;
#endif
	}

	static	void	PlayPrevSong()
	{
#if VOLUMIO
		std::vector<uint8_t>	iData;

		Http::Get( VOLUMIO_HOST, VOLUMIO_PORT, "/api/v1/commands/?cmd=prev", iData );
#else
		Socket		iSock;
		std::string	str;

		iSock.connect( MPD_HOST, MPD_PORT );
		iSock	<< "previous\n";
		iSock	>> str;
#endif
	}
};




void	Draw( cv::Mat& dst, int x, int y, cv::Mat& src )
{
	const uint8_t*	src_image	= src.data;
	int				src_stride	= src.step;
	uint8_t*		dst_image	= dst.data;
	int				dst_stride	= dst.step;
	
	if( (src_image != NULL) && 
		(dst_image != NULL) )
	{
		int				cx			= src.cols;
		int				cy			= src.rows;
	
		if( x < 0 )
		{
			src_image   -= x * src.elemSize();
			cx			+= x;
			x			= 0;
		}
	
		if( dst.cols < (x + cx) )
		{
			cx  = dst.cols - x;
		}
	
		if( y < 0 )
		{
			src_image	-= y * src_stride;
			cy			+= y;
			y			= 0;
		}
	
		if( dst.rows < (y + cy) )
		{
			cy  = dst.rows - y;
		}
	
		if( (0 < cx) && (0 < cy) )
		{
			dst_image	+= y * dst_stride + x * dst.elemSize();
			
			for( int r = 0; r < cy; r++ )
			{
				memcpy( dst_image, src_image, cx * src.elemSize() );
				src_image	+= src_stride;
				dst_image	+= dst_stride;
			}
		}
	}
}



class DrawAreaIF
{
public:
	DrawAreaIF( DisplayIF& iDisplay, int x, int y, int cx, int cy ) :
		m_iDisp( iDisplay )
	{
		m_nRectX		= x;
		m_nRectY		= y;
		m_nRectWidth	= cx;
		m_nRectHeight	= cy;
		
		m_iAreaImage	= cv::Mat::zeros( cy, cx, CV_8UC1 );
	}

	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )=0;
	
	virtual	void	Reset()
	{
		m_nCurrent	= "";
	}

protected:
	DisplayIF&	m_iDisp;
	int			m_nRectX;
	int			m_nRectY;
	int			m_nRectWidth;
	int			m_nRectHeight;
	std::string	m_nCurrent;
	cv::Mat		m_iAreaImage;
};

class DrawArea_ScrollImage : public DrawAreaIF
{
public:
	enum	DRAW_STYLE
	{
		DRAW_ALIGN_LEFT		= 0x0,
		DRAW_ALIGN_RIGHT	= 0x1,
		DRAW_ALIGN_CENTER	= 0x2,
		DRAW_VALIGN_TOP		= 0x0,
		DRAW_VALIGN_BOTTOM	= 0x4,
		DRAW_VALIGN_CENTER	= 0x8,
	};

	DrawArea_ScrollImage( DisplayIF& iDisplay, int x, int y, int cx, int cy, unsigned int nDrawStyle = 0 ) :
		DrawAreaIF( iDisplay, x, y, cx, cy )
	{
		m_nDrawStyle		= nDrawStyle;
		m_iScrlTick.x		= 0;	// 0< : Right to left scroll, <0 : Left to right scroll.
		m_iScrlTick.y		= 0;	// 0< : Bottom to top scroll, <0 : Top to bottom scroll.
		m_iScrlCurrentPos.x	= 0;	// LeftTop
		m_iScrlCurrentPos.y	= 0;	// LeftTop
	}
	
	void	SetScrollImage( cv::Mat& image, int ScrlTickX=1, int ScrlTickY=0 )
	{
		int		x			= 0;
		int		y			= 0;

		m_iScrlTick.x		= 0;
		m_iScrlTick.y		= 0;
		m_iScrlImage		= image;

		// Horizon
		if( m_nDrawStyle & DRAW_ALIGN_CENTER )
		{
			m_iScrlCurrentPos.x	= (m_iAreaImage.cols - m_iScrlImage.cols) / 2;
		}
		else if( m_nDrawStyle & DRAW_ALIGN_RIGHT )
		{
			m_iScrlCurrentPos.x	= m_iAreaImage.cols - m_iScrlImage.cols;
		}
		else
		{
			m_iScrlCurrentPos.x	= 0;
		}
		x	= m_iScrlCurrentPos.x;

		if( (ScrlTickX != 0) &&
			(m_iAreaImage.cols < m_iScrlImage.cols) )
		{
			m_iScrlTick.x	= ScrlTickX;
			if( 0 < ScrlTickX )	
			{
				// Right to left scroll
				// Force left align
				// 0 ~ m_iAreaImage.cols : static
				// -m_iAreaImage.cols ~ -1 : scroll right to left
				m_iScrlCurrentPos.x	= m_iAreaImage.cols;
				x					= 0;
			}
			else if( ScrlTickX < 0 )
			{
				// Left to right scroll
				// Force right align
				// -m_iAreaImage.cols ~ 0 : static	
				// 1 ~ m_iAreaImage.cols : scroll left to right
				m_iScrlCurrentPos.x	= -m_iAreaImage.cols;
				x					= m_iScrlImage.cols - m_iAreaImage.cols;
			}
		}

		// Vertical
		if( m_nDrawStyle & DRAW_VALIGN_CENTER )
		{
			m_iScrlCurrentPos.y	= (m_iAreaImage.rows - m_iScrlImage.rows) / 2;
		}
		else if( m_nDrawStyle & DRAW_VALIGN_BOTTOM )
		{
			m_iScrlCurrentPos.y	= m_iAreaImage.rows - m_iScrlImage.rows;
		}
		else
		{
			m_iScrlCurrentPos.y	= 0;
		}
		y	= m_iScrlCurrentPos.y;

		if( (ScrlTickY != 0) &&
			(m_iAreaImage.rows < m_iScrlImage.rows) )
		{
			m_iScrlTick.y	= ScrlTickY;
			if( 0 < ScrlTickY )
			{
				// Bottom to top scroll
				// Force top align
				// 0 ~ m_iAreaImage.rows : static
				// -m_iAreaImage.rows ~ -1 : scroll bottom to top
				m_iScrlCurrentPos.y	= m_iAreaImage.rows;
				y					= 0;
			}
			else if( ScrlTickY < 0 )
			{
				// Top to bottom scroll
				// -m_iAreaImage.rows ~ 0 : static	
				// 1 ~ m_iAreaImage.rows : scroll top to bottom
				m_iScrlCurrentPos.y	= -m_iAreaImage.rows;
				y					= m_iScrlImage.rows - m_iAreaImage.rows;
			}
			y	= 0;
		}

		// Draw initial image
		m_iAreaImage	= cv::Mat::zeros( m_iAreaImage.rows, m_iAreaImage.cols, m_iScrlImage.type() );
		Draw( m_iAreaImage, x, y, m_iScrlImage );

		switch( m_iAreaImage.type() )
		{
		case CV_8UC4:
			m_iDisp.WriteImageBGRA(
				m_nRectX,
				m_nRectY,
				m_iAreaImage.data,
				m_iAreaImage.step,
				m_iAreaImage.cols,
				m_iAreaImage.rows );
			break;

		case CV_8UC1:
			m_iDisp.WriteImageGRAY(
				m_nRectX,
				m_nRectY,
				m_iAreaImage.data,
				m_iAreaImage.step,
				m_iAreaImage.cols,
				m_iAreaImage.rows );
			break;
		}
	}
	
	bool	UpdateScroll()
	{
		int		x			= m_iScrlCurrentPos.x;
		int		y			= m_iScrlCurrentPos.y;

		if( m_iScrlTick.x != 0 )
		{
			m_iScrlCurrentPos.x	-= m_iScrlTick.x;

			if( 0 < m_iScrlTick.x )
			{
				if( m_iScrlCurrentPos.x < -m_iScrlImage.cols )
				{
					m_iScrlCurrentPos.x	= m_iScrlImage.cols;
				}
				x	= m_iScrlCurrentPos.x;
				if( 0 < x )
				{
					x	= 0;
				}
			}
			else
			{
				if( m_iAreaImage.cols < m_iScrlCurrentPos.x )
				{
					m_iScrlCurrentPos.x	= - m_iScrlImage.cols;
				}
				x	= m_iScrlCurrentPos.x;
				if( x < 0 )
				{
					x	= 0;
				}
				
				x	+= m_iScrlImage.cols - m_iAreaImage.cols;
			}
		}

		if( m_iScrlTick.y != 0 )
		{
			m_iScrlCurrentPos.y	-= m_iScrlTick.y;

			if( 0 < m_iScrlTick.y )
			{
				if( m_iScrlCurrentPos.y < -m_iScrlImage.rows )
				{
					m_iScrlCurrentPos.y	= m_iScrlImage.rows;
				}
				y	= m_iScrlCurrentPos.y;
				if( 0 < y )
				{
					y	= 0;
				}
			}
			else
			{
				if( m_iAreaImage.rows < m_iScrlCurrentPos.y )
				{
					m_iScrlCurrentPos.y	= - m_iScrlImage.rows;
				}
				y	= m_iScrlCurrentPos.y;
				if( y < 0 )
				{
					y	= 0;
				}
				
				y	+= m_iScrlImage.rows - m_iAreaImage.rows;
			}
		}

		if( (m_iScrlTick.x != 0) ||
			(m_iScrlTick.y != 0) )
		{
			m_iAreaImage	= cv::Mat::zeros( m_iAreaImage.rows, m_iAreaImage.cols, m_iAreaImage.type() );
			Draw( m_iAreaImage, x, y, m_iScrlImage );

			switch( m_iAreaImage.type() )
			{
			case CV_8UC4:
				m_iDisp.WriteImageBGRA(
					m_nRectX,
					m_nRectY,
					m_iAreaImage.data,
					m_iAreaImage.step,
					m_iAreaImage.cols,
					m_iAreaImage.rows );
				return	true;
				break;
	
			case CV_8UC1:
				m_iDisp.WriteImageGRAY(
					m_nRectX,
					m_nRectY,
					m_iAreaImage.data,
					m_iAreaImage.step,
					m_iAreaImage.cols,
					m_iAreaImage.rows );
				return	true;
				break;
			}
		}

		return	false;
	}
	
protected:
	unsigned int	m_nDrawStyle;
	cv::Mat			m_iScrlImage;
	cv::Point2i		m_iScrlTick;
	cv::Point2i		m_iScrlCurrentPos;
};


class DrawArea_ScrollText : public DrawArea_ScrollImage
{
public:
	DrawArea_ScrollText( DisplayIF& iDisplay, int x, int y, int cx, int cy, unsigned int nDrawStyle = 0, int rotate=0 ) :
		DrawArea_ScrollImage( iDisplay, x, y, cx, cy ),
		m_iFont( FONT_PATH, ((rotate % 180) == 0) ? m_nRectHeight : m_nRectWidth )
	{
		m_str			= "";
		m_color			= 0xFFFFFFFF;
		m_scroll_tick	= 1;
		m_isSelected	= false;

		m_nRotate		= rotate;
		m_nDrawStyle	= 0;
		
		switch( m_nRotate )
		{
		case 0:
			m_nDrawStyle	= nDrawStyle;
			break;
			
		case 90:
			if( nDrawStyle & DRAW_ALIGN_RIGHT )			m_nDrawStyle	|= DRAW_VALIGN_TOP;
			else if( nDrawStyle & DRAW_ALIGN_CENTER )	m_nDrawStyle	|= DRAW_VALIGN_CENTER;
			else										m_nDrawStyle	|= DRAW_VALIGN_BOTTOM;

			if( nDrawStyle & DRAW_VALIGN_TOP )			m_nDrawStyle	|= DRAW_ALIGN_LEFT;
			else if( nDrawStyle & DRAW_VALIGN_CENTER )	m_nDrawStyle	|= DRAW_ALIGN_CENTER;
			else										m_nDrawStyle	|= DRAW_ALIGN_RIGHT;
			
			break;

		case 180:
			if( nDrawStyle & DRAW_ALIGN_RIGHT )			m_nDrawStyle	|= DRAW_ALIGN_LEFT;
			else if( nDrawStyle & DRAW_ALIGN_CENTER )	m_nDrawStyle	|= DRAW_ALIGN_CENTER;
			else										m_nDrawStyle	|= DRAW_ALIGN_RIGHT;

			if( nDrawStyle & DRAW_VALIGN_TOP )			m_nDrawStyle	|= DRAW_VALIGN_BOTTOM;
			else if( nDrawStyle & DRAW_VALIGN_CENTER )	m_nDrawStyle	|= DRAW_VALIGN_CENTER;
			else										m_nDrawStyle	|= DRAW_VALIGN_TOP;
			
			break;

		case 270:
			if( nDrawStyle & DRAW_ALIGN_RIGHT )			m_nDrawStyle	|= DRAW_VALIGN_BOTTOM;
			else if( nDrawStyle & DRAW_ALIGN_CENTER )	m_nDrawStyle	|= DRAW_VALIGN_CENTER;
			else										m_nDrawStyle	|= DRAW_VALIGN_TOP;

			if( nDrawStyle & DRAW_VALIGN_TOP )			m_nDrawStyle	|= DRAW_ALIGN_RIGHT;
			else if( nDrawStyle & DRAW_VALIGN_CENTER )	m_nDrawStyle	|= DRAW_ALIGN_CENTER;
			else										m_nDrawStyle	|= DRAW_ALIGN_LEFT;
			
			break;
		}
	}

	void	SetSelect( bool isSelect )
	{
		m_isSelected		= !m_isSelected;
		SetScrollText( m_str, m_color, m_scroll_tick );
	}
	
	bool	IsSelected()
	{
		return	m_isSelected;
	}

	void	SetScrollText( const std::string& str, uint32_t color, int tick=1 )
	{
		int	l,t,r,b,w,h;
		m_iFont.CalcRect( l,t,r,b, str.c_str() );

		switch( m_nRotate )
		{
		case 0:
		case 180:
			w	= r;
			h	= m_nRectHeight;
			break;
		
		case 90:
		case 270:
			w	= r;
			h	= m_nRectWidth;
			break;
		}

		cv::Mat		image;
		if( 1 < m_iDisp.GetBPP() )
		{
			image	= cv::Mat::zeros( h, w, CV_8UC4 );
			m_iFont.DrawTextBGRA(
				0,
				0,
				str.c_str(),
				color,
				image.data,
				image.step,
				image.cols, 
				image.rows );
		}
		else
		{
			cv::Mat	gray	= cv::Mat::zeros( h, w, CV_8UC1 );

			m_iFont.DrawTextGRAY(
				0,
				0,
				str.c_str(),
				255,
				gray.data,
				gray.step,
				gray.cols,
				gray.rows );
			
			cv::threshold( gray, gray, 128, 255, CV_THRESH_BINARY );
			cv::cvtColor( gray, image, CV_GRAY2BGRA );
		}
		
		if( m_isSelected )
		{
			cv::bitwise_not( image, image );
		}
		
		switch( m_nRotate )
		{
		case 0:
			SetScrollImage( image, tick, 0 );
			break;
			
		case 180:
			cv::flip( image, image, -1 );
			SetScrollImage( image, -tick, 0 );
			break;
			
		case 90:
			cv::transpose(image, image);
			cv::flip(image, image, 0);
			SetScrollImage( image, 0, -tick );
			break;

		case 270:
			cv::transpose(image, image);
			cv::flip(image, image, 1);
			SetScrollImage( image, 0, tick );
			break;
		}

		// Update member variables
		m_str			= str;
		m_color			= color;
		m_scroll_tick	= tick;
	}

protected:

	std::string	m_str;
	uint32_t	m_color;
	int			m_scroll_tick;
	bool		m_isSelected;

	int			m_nRotate;
	ImageFont	m_iFont;
};

class DrawArea_STR : public DrawArea_ScrollText
{
public:
	DrawArea_STR( const std::string& tag, uint32_t color, DisplayIF& iDisplay, int x, int y, int cx, int cy, int isRightAlign=false, int rotate=0 ) :
		DrawArea_ScrollText( iDisplay, x, y, cx, cy, 0 < isRightAlign ? DRAW_ALIGN_RIGHT : isRightAlign < 0 ? DRAW_ALIGN_CENTER : DRAW_ALIGN_LEFT, rotate )
	{
		m_strTag		= tag;
		m_nColor		= color;
	}

	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		auto		it	= map.find( m_strTag );
		std::string	str	= it != map.end() ? (*it).second : "";

		if( m_nCurrent != str )
		{
			m_nCurrent		= str;

			SetScrollText( m_nCurrent, m_nColor );
		}
		else
		{
			UpdateScroll();
		}
	}
	
protected:
	std::string	m_strTag;
	uint32_t	m_nColor;
};


class DrawArea_StaticText : public DrawArea_ScrollText
{
public:
	DrawArea_StaticText( const std::string& text, uint32_t color, DisplayIF& iDisplay, int x, int y, int cx, int cy, bool isRightAlign=false ) :
		DrawArea_ScrollText( iDisplay, x, y, cx, cy, isRightAlign ? DRAW_ALIGN_RIGHT : DRAW_ALIGN_LEFT )
	{
		m_strText		= text;
		m_nColor		= color;
	}

	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		std::string	str	= m_strText;

		if( m_nCurrent != str )
		{
			m_nCurrent		= str;

			SetScrollText( m_nCurrent, m_nColor );
		}
		else
		{
			UpdateScroll();
		}
	}
	
protected:
	std::string	m_strText;
	uint32_t	m_nColor;
};



class DrawArea_MyIpAddr : public DrawArea_ScrollText
{
public:
	DrawArea_MyIpAddr( uint32_t color, DisplayIF& iDisplay, int x, int y, int cx, int cy, bool isRightAlign=false ) :
		DrawArea_ScrollText( iDisplay, x, y, cx, cy, isRightAlign ? DRAW_ALIGN_RIGHT : DRAW_ALIGN_LEFT )
	{
		m_strText		= Socket::GetMyIpAddrString();
		m_nColor		= 0xFFFFFFFF;
		m_iLastChecked	= std::chrono::high_resolution_clock::now();
	}

	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		std::chrono::high_resolution_clock::time_point   current	= std::chrono::high_resolution_clock::now();
        double	elapsed = std::chrono::duration_cast<std::chrono::seconds>(current-m_iLastChecked).count();

        if( 10 <= elapsed )
        {
        	m_iLastChecked	= current;
 			m_strText		= Socket::GetMyIpAddrString();
 			
 			if( m_strText == "127.0.0.1" )
 			{
 				m_nColor	= 0xFFFF8080;
 			}
 			else
 			{
 				m_nColor	= 0xFFFFFFFF;
 			}
 		}
 		
		if( m_nCurrent != m_strText )
		{
			m_nCurrent	= m_strText;

			SetScrollText( m_nCurrent, m_nColor );
		}
		else
		{
			UpdateScroll();
		}
	}
	
protected:
   	std::chrono::high_resolution_clock::time_point	m_iLastChecked;
   	std::string										m_strText;
   	uint32_t										m_nColor;
};



class DrawArea_Battery : public DrawArea_ScrollText
{
public:

	void	UpdateBatteryStatus()
	{
		if( m_piPowerMon != NULL )
		{
			double	v	= m_piPowerMon->GetV();
			double	a	= m_piPowerMon->GetA();

			char	szBuf[128];
			sprintf( szBuf, "%.3fV %.3fA", v, a );

			m_strText	= szBuf;
			m_nColor	= BATTERY_EMPTY <= (v * 1000) ? 0xFFFFFFFF : 0xFFFF0000;


	/*
			FILE*	fp	= fopen( "/media/nas/Battery.csv","a");
			if( fp != NULL )
			{
				std::chrono::high_resolution_clock::time_point   current	= std::chrono::high_resolution_clock::now();
		        double	elapsed = std::chrono::duration_cast<std::chrono::seconds>(current-m_iStarted).count();

				std::time_t 	t	= std::time(nullptr);
				std::tm*		tl	= std::localtime(&t);
		
				fprintf( fp, "%02d:%02d:%02d, %.3f, %.3f\r\n", tl->tm_hour, tl->tm_min, tl->tm_sec, elapsed, value );
				fclose(fp);
			}
	*/
		}
	}


	DrawArea_Battery( DisplayIF& iDisplay, int x, int y, int cx, int cy, bool isRightAlign=false ) :
		DrawArea_ScrollText( iDisplay, x, y, cx, cy, isRightAlign ? DRAW_ALIGN_RIGHT : DRAW_ALIGN_LEFT )
	{
		try
		{
			m_piPowerMon	= new PMoni_INA219;
		}
		catch( std::exception &e )
		{
			printf( "DrawArea_Battery, %s\n", e.what() );
			m_piPowerMon	= NULL;
		}

		m_iStarted		= std::chrono::high_resolution_clock::now();

		UpdateBatteryStatus();
		m_iLastChecked	= std::chrono::high_resolution_clock::now();
	}

	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		std::chrono::high_resolution_clock::time_point   current	= std::chrono::high_resolution_clock::now();
        double	elapsed = std::chrono::duration_cast<std::chrono::seconds>(current-m_iLastChecked).count();

        if( 1 <= elapsed )
        {
        	m_iLastChecked	= current;

			UpdateBatteryStatus();
 		}
 		
		if( m_nCurrent != m_strText )
		{
			m_nCurrent	= m_strText;

			SetScrollText( m_nCurrent, m_nColor );
		}
		else
		{
			UpdateScroll();
		}
	}
	
protected:
	ctrl_PowerMonitor*								m_piPowerMon;
	std::chrono::high_resolution_clock::time_point	m_iStarted;
	
	std::chrono::high_resolution_clock::time_point	m_iLastChecked;
	std::string										m_strText;
	uint32_t										m_nColor;
};



class DrawArea_BatteryIcon : public DrawAreaIF
{
public:
	void	UpdateBatteryStatus()
	{
		m_iAreaImage	= cv::Mat::zeros( m_nRectHeight, m_nRectWidth, CV_8UC4 );

		if( m_piPowerMon != NULL )
		{
			const int	icon_cx	= 16;
			const int	icon_cy	= 32;

			const int	l		= (m_nRectWidth  - (icon_cx * m_nScale)) / 2;
			const int	t		= (m_nRectHeight - (icon_cy * m_nScale)) / 2;
			const int	s		= m_nScale;
			const int	r		= l + (s - 1);
			const int	b		= t + (s - 1);

			const int	mV		= (int)(1000 * m_piPowerMon->GetV());
			const int	mA		= (int)(1000 * m_piPowerMon->GetA());
			char		szBuf[128];
			
			cv::Scalar	color_frame		= cv::Scalar(192,192,192,255);
//			cv::Scalar	color_cell		= color_frame;
			cv::Scalar	color_cell		= cv::Scalar(  0,192,  0,255);

			cv::Scalar	color_red		= cv::Scalar(  0,  0,255,255);
			cv::Scalar	color_yellow	= cv::Scalar(  0,255,255,255 );
			
			sprintf( szBuf, "%d mV", mV );
			m_strText	= szBuf;

			// Draw battery frame
			cv::rectangle( m_iAreaImage,cv::Point2i(  1*s+l, 4*s+t), cv::Point2i( 14*s+r,30*s+b), color_frame, CV_FILLED );
			cv::rectangle( m_iAreaImage,cv::Point2i(  4*s+l, 1*s+t), cv::Point2i( 11*s+r, 3*s+b), color_frame, CV_FILLED );

			cv::rectangle( m_iAreaImage,cv::Point2i(  2*s+l, 5*s+t), cv::Point2i( 13*s+r,29*s+b), cv::Scalar(  0,  0,  0,255),CV_FILLED );
			cv::rectangle( m_iAreaImage,cv::Point2i(  5*s+l, 2*s+t), cv::Point2i( 10*s+r, 4*s+b), cv::Scalar(  0,  0,  0,255),CV_FILLED );

			if( BATTERY_EMPTY <= mV )
			{
				// Empty=3.9, Full=4.1
				// 3.9 - 4.1	--> 4 (Green x4)
				// 3.7 - 3.9	--> 3 (Green x3)
				// 3.5 - 3.7	--> 2 (Green x2)
				// 3.3 - 3.5	--> 1 (Green x1)
				// 3.1 - 3.3	--> 0 (Red   x1)
				int	count	= (mV - BATTERY_EMPTY) * 5 / (BATTERY_FULL - BATTERY_EMPTY);

				if( 4 <= count ) cv::rectangle( m_iAreaImage,cv::Point2i(  3*s+l, 6*s+t), cv::Point2i( 12*s+r,10*s+b), color_cell, CV_FILLED );
				if( 3 <= count ) cv::rectangle( m_iAreaImage,cv::Point2i(  3*s+l,12*s+t), cv::Point2i( 12*s+r,16*s+b), color_cell, CV_FILLED );
				if( 2 <= count ) cv::rectangle( m_iAreaImage,cv::Point2i(  3*s+l,18*s+t), cv::Point2i( 12*s+r,22*s+b), color_cell, CV_FILLED );
				if( 1 <= count ) cv::rectangle( m_iAreaImage,cv::Point2i(  3*s+l,24*s+t), cv::Point2i( 12*s+r,28*s+b), color_cell, CV_FILLED );
				if( 0 == count ) cv::rectangle( m_iAreaImage,cv::Point2i(  3*s+l,24*s+t), cv::Point2i( 12*s+r,28*s+b), color_red, CV_FILLED );
			}
			else
			{
//				cv::rectangle( m_iAreaImage,cv::Point2i(  3*s+l, 6*s+t), cv::Point2i( 12*s+r, 28*s+b), color_red, CV_FILLED );
				cv::rectangle( m_iAreaImage,cv::Point2i(  3*s+l,28*s+t), cv::Point2i( 12*s+r, 28*s+b), color_red, CV_FILLED );
			}
			
			if( mA < 0 )
			{
				cv::Point2i		chg1[3];
				cv::Point2i		chg2[3];
				
				chg1[0]	= cv::Point2i(  4*s+l,19*s+b );
				chg1[1]	= cv::Point2i(  8*s+r, 7*s+t );
				chg1[2]	= cv::Point2i(  8*s+r,19*s+b );
				cv::fillConvexPoly( m_iAreaImage, chg1, 3, color_yellow );

				chg2[0]	= cv::Point2i(  7*s+l,15*s+t );
				chg2[1]	= cv::Point2i( 11*s+r,15*s+t );
				chg2[2]	= cv::Point2i(  7*s+l,27*s+b );
				cv::fillConvexPoly( m_iAreaImage, chg2, 3, color_yellow );
			}
			
//			cv::imwrite( "/root/BatteryIcon.png", m_iAreaImage );
		}
	}

	DrawArea_BatteryIcon( DisplayIF& iDisplay, int x, int y, int cx, int cy ) :
		DrawAreaIF( iDisplay, x, y, cx, cy )
	{
		try
		{
			m_piPowerMon	= new PMoni_INA219;
		}
		catch( std::exception &e )
		{
			printf( "DrawArea_Battery, %s\n", e.what() );
			m_piPowerMon	= NULL;
		}
		
		m_nScale		= std::max( 1, cy / 32 );

		m_iStarted		= std::chrono::high_resolution_clock::now();

		UpdateBatteryStatus();
		m_iLastChecked	= std::chrono::high_resolution_clock::now();
	}

	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		std::chrono::high_resolution_clock::time_point   current	= std::chrono::high_resolution_clock::now();
		double	elapsed = std::chrono::duration_cast<std::chrono::seconds>(current-m_iLastChecked).count();

		if( 5 <= elapsed )
		{
			m_iLastChecked	= current;

			UpdateBatteryStatus();
 		}
 		
		if( m_nCurrent != m_strText )
		{
			m_nCurrent	= m_strText;
			m_iDisp.WriteImageBGRA( m_nRectX, m_nRectY, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );
		}
	}

	static	bool IsValid()
	{
		try
		{
			ctrl_PowerMonitor*	piPowerMon	= new PMoni_INA219;
			delete	piPowerMon;
			return	true;
		}
		catch( std::exception &e )
		{
		}
		return	false;
	}
	
protected:
	ctrl_PowerMonitor*								m_piPowerMon;
	std::chrono::high_resolution_clock::time_point	m_iStarted;
	
	std::chrono::high_resolution_clock::time_point	m_iLastChecked;
	std::string										m_strText;
	int												m_nScale;
};



class DrawArea_PlayPos: public DrawAreaIF
{
public:
	DrawArea_PlayPos( DisplayIF& iDisplay, int x, int y, int cx, int cy ) : DrawAreaIF( iDisplay, x, y, cx, cy ){};

	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		auto	itT			= map.find( "Time" );
		auto	itE			= map.find( "elapsed" );

		float	duration	= itT != map.end() ? std::stof( (*itT).second ) : 1;
		float	elapsed		= itE != map.end() ? std::stof( (*itE).second ) : 0;

		m_iAreaImage	= cv::Mat::zeros( m_nRectHeight, m_nRectWidth, CV_8UC4 );

		if( 3 <= m_nRectHeight )
		{
			int		w			= (m_iAreaImage.cols - 2) * elapsed / duration;
			
			cv::rectangle(
				m_iAreaImage,
				cv::Point2i(0,0),
				cv::Point2i(m_iAreaImage.cols-1, m_iAreaImage.rows-1),
				cv::Scalar(128,128,128,255),
				1 );

			cv::rectangle(
				m_iAreaImage,
				cv::Point2i(1,1),
				cv::Point2i(1+w, m_iAreaImage.rows-2),
				cv::Scalar(255,255,255,255),	// B,G,R,A
				CV_FILLED );
		}
		else
		{
			int		w			= (m_iAreaImage.cols-1) * elapsed / duration;
			
			cv::rectangle(
				m_iAreaImage,
				cv::Point2i(0,0),
				cv::Point2i(w, m_iAreaImage.rows-1),
				cv::Scalar(255,255,255,255),	// B,G,R,A
				CV_FILLED );
		}

		m_iDisp.WriteImageBGRA( m_nRectX, m_nRectY, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );
	}
};


class DrawArea_CoverImage: public DrawAreaIF
{
public:
	DrawArea_CoverImage( DisplayIF& iDisplay, int x, int y, int cx, int cy ) : DrawAreaIF( iDisplay, x, y, cx, cy ){};

	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		auto		it	= map.find( "file" );
		std::string	str	= it != map.end() ? (*it).second : "";
		
		if( m_nCurrent != str )
		{
			m_nCurrent	= str;

			// Create frame
			cv::Mat	image	= cv::Mat::zeros( m_nRectHeight, m_nRectWidth, CV_8UC4 );
			cv::Mat	cover;

			cv::rectangle(
				image,
				cv::Point2i(0,0),
				cv::Point2i(image.cols-1, image.rows-1),
				cv::Scalar(255,255,255,255),
				1 );

			// Load music file
			if( 0 != str.find("http://") )
			{
				std::string					path		= MUSIC_ROOT_PATH + str;
				std::vector<unsigned char>	coverArt	= ExtractCoverArt( path.c_str() );
	
				if( !coverArt.empty() )
				{
					cover	= cv::imdecode( coverArt, cv::IMREAD_COLOR );
				}
	
				if( cover.empty() )
				{
					path	= path.substr(0, path.rfind('/')+1 );
					path	+= "front.jpg";
	
					cover	= cv::imread( path );
				}
			}

			if( !cover.empty() )
			{
				cv::cvtColor( cover, cover, cv::COLOR_BGR2BGRA );
				cv::resize( cover, cover, cv::Size(image.cols-2,image.rows-2), 0, 0, cv::INTER_AREA );
				cover.copyTo( image( cv::Rect( 1, 1, cover.cols, cover.rows ) ) );
			}
			else
			{
				const char*	pszText	= "NoImage";
				int			cx, cy, l,t,r,b;
				ImageFont	iFont( FONT_PATH, image.rows / 4 );
				
				iFont.CalcRect( l,t,r,b, pszText );
				cx	= r-l;
				cy	= b-t;

				iFont.DrawTextBGRA(
					(image.cols-cx)/2-l,
					(image.rows-cy)/2-t,
					pszText,
					0xFFFFFFFF,
					image.data, 
					image.step,
					image.cols,
					image.rows );
			}

			m_iDisp.WriteImageBGRA( m_nRectX, m_nRectY, image.data, image.step, image.cols, image.rows );
		}
	}
};


class DrawArea_CpuTemp: public DrawAreaIF
{
public:
	DrawArea_CpuTemp( DisplayIF& iDisplay, int x, int y, int cx, int cy, bool isRightAlign=false ) : 
		DrawAreaIF( iDisplay, x, y, cx, cy ),
		m_iFont( FONT_PATH, m_nRectHeight )
	{
		m_isRightAlign	= isRightAlign;
	};

	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		int			x = 0;
		char		buf[128];
		float		cpuTemp	= 0;
		
		try
		{
			cpuTemp	= std::stof( StringUtil::GetTextFromFile( "/sys/class/thermal/thermal_zone0/temp") ) / 1000.0f;
		}
		catch( std::exception& ex )
		{
			printf( "EXCEPTION! Get CpuTemp :  %s\n", ex.what() );
		}
	
		sprintf( buf, u8"cpu %.1f C", cpuTemp );

		if( m_isRightAlign )
		{
			int	l,t,r,b;
			m_iFont.CalcRect( l,t,r,b, buf );

			x	= m_nRectWidth - r;
		}

		m_iAreaImage	= cv::Mat::zeros( m_nRectHeight, m_nRectWidth, CV_8UC1 );
		m_iFont.DrawTextGRAY( x, 0, buf, 255, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );
		m_iDisp.WriteImageGRAY( m_nRectX, m_nRectY, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );
	}

protected:
	ImageFont	m_iFont;
	bool		m_isRightAlign;
};


class DrawArea_Date: public DrawAreaIF
{
public:
	DrawArea_Date( DisplayIF& iDisplay, int x, int y, int cx, int cy ) : 
		DrawAreaIF( iDisplay, x, y, cx, cy ),
		m_iFont( FONT_DATE_PATH, m_nRectHeight, false )
	{
		int	l,t,r,b;
		m_iFont.CalcRect( l,t,r,b, "0000/00/00" );
		
		m_nOffsetX	= 0;//(cx - (r-l) + 1) / 2 - l;
		m_nOffsetY	= (cy - (b-t) + 1) / 2 - t;
	};
	
	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		char			buf[32];
		std::time_t 	t	= std::time(nullptr);
		std::tm*		tl	= std::localtime(&t);

		sprintf( buf, "%04d/%02d/%02d", 1900 + tl->tm_year, 1 + tl->tm_mon, tl->tm_mday );

		if( m_nCurrent != buf )
		{
			m_nCurrent	= buf;

			m_iAreaImage	= cv::Mat::zeros( m_nRectHeight, m_nRectWidth, CV_8UC1 );
			m_iFont.DrawTextGRAY( m_nOffsetX, m_nOffsetY, buf, 255, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );
			m_iDisp.WriteImageGRAY( m_nRectX, m_nRectY, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );
		}
	}

protected:
	ImageFont	m_iFont;
	int			m_nOffsetX;
	int			m_nOffsetY;
};


class DrawArea_TimeOLD: public DrawAreaIF
{
public:
	DrawArea_TimeOLD( DisplayIF& iDisplay, int x, int y, int cx, int cy ) : 
		DrawAreaIF( iDisplay, x, y, cx, cy ),
		m_iFont( FONT_DATE_PATH, m_nRectHeight, false )
	{
		int	l,t,r,b;
		m_iFont.CalcRect( l,t,r,b, "88:88" );
		
		m_nOffsetX	= (cx - (r-l)) / 2 - l;		
		m_nOffsetY	= (cy - (b-t)) / 2 - t;
	};
	
	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		char			buf[32];
		std::time_t 	t	= std::time(nullptr);
		std::tm*		tl	= std::localtime(&t);

		if( tl->tm_sec & 1 )
			sprintf( buf, "%02d %02d", tl->tm_hour, tl->tm_min );
		else
			sprintf( buf, "%02d:%02d", tl->tm_hour, tl->tm_min );

		if( m_nCurrent != buf )
		{
			m_nCurrent	= buf;

			m_iAreaImage	= cv::Mat::zeros( m_nRectHeight, m_nRectWidth, CV_8UC1 );
			m_iFont.DrawTextGRAY( m_nOffsetX, m_nOffsetY, buf, 255, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );
			m_iDisp.WriteImageGRAY( m_nRectX, m_nRectY, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );
		}
	}

protected:
	ImageFont	m_iFont;
	int			m_nOffsetX;
	int			m_nOffsetY;
};


class DrawArea_Time: public DrawAreaIF
{
public:
	DrawArea_Time( DisplayIF& iDisplay, int x, int y, int cx, int cy ) : 
		DrawAreaIF( iDisplay, x, y, cx, cy ),
		m_iFont( FONT_DATE_PATH, m_nRectHeight, false )
	{
		int	l,t,r,b;

		m_iFont.CalcRect( l,t,r,b, "88:88" );
		m_nOffsetY		= (cy - (b-t)) / 2 - t;

		m_iFont.CalcRect( l,t,r,b, ":" );
		m_nColonWidth	= r;
		m_nColonOffsetX	= - r / 2;
		printf("colon %d, %d, %d, %d\n", l, t, r, b );

		m_iAreaImage	= cv::Mat::zeros( m_nRectHeight, m_nRectWidth, CV_8UC1 );
		Reset();
	};

	virtual	void	Reset()
	{
		m_nLastHour		= -1;
		m_nLastMinute	= -1;
		m_nLastSecond	= -1;
	}

	virtual	void	UpdateInfo( std::map<std::string,std::string>& map )
	{
		char			buf[32];
		std::time_t 	t	= std::time(nullptr);
		std::tm*		tl	= std::localtime(&t);

		if( (m_nLastHour != tl->tm_hour) ||
			(m_nLastMinute != tl->tm_min))
		{
			int	ColonLeft	= (m_nRectWidth - m_nColonWidth) / 2;
			int	l,t,r,b;

			m_iAreaImage	= cv::Mat::zeros( m_nRectHeight, m_nRectWidth, CV_8UC1 );

			// hour
			sprintf( buf, "%02d", tl->tm_hour );
			m_iFont.CalcRect( l,t,r,b, buf );
			m_iFont.DrawTextGRAY( ColonLeft - r, m_nOffsetY, buf, 255, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );

			// colon
			if( (tl->tm_sec & 1) == 0 )
			{
				sprintf( buf, ":" );
				m_iFont.DrawTextGRAY( (m_nRectWidth + m_nColonOffsetX * 2)/ 2, m_nOffsetY, buf, 255, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );
			}

			// minute
			sprintf( buf, "%02d", tl->tm_min );
			m_iFont.DrawTextGRAY( ColonLeft + m_nColonWidth, m_nOffsetY, buf, 255, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );

			// Transfer image
			m_iDisp.WriteImageGRAY( m_nRectX, m_nRectY, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );

			// Update variables
			m_nLastHour		= tl->tm_hour;
			m_nLastMinute	= tl->tm_min;
			m_nLastSecond	= tl->tm_sec;
		}
		else if( m_nLastSecond != tl->tm_sec )
		{
			int	ColonLeft	= (m_nRectWidth - m_nColonWidth) / 2;
			
			// Clear image
			cv::rectangle( m_iAreaImage, cv::Point(ColonLeft,0), cv::Point(ColonLeft+m_nColonWidth, m_nRectHeight), cv::Scalar(0), CV_FILLED);
			
			if( (tl->tm_sec & 1) == 0 )
			{
				sprintf( buf, ":" );
				m_iFont.DrawTextGRAY( (m_nRectWidth + m_nColonOffsetX * 2)/ 2, m_nOffsetY, buf, 255, m_iAreaImage.data, m_iAreaImage.step, m_iAreaImage.cols, m_iAreaImage.rows );
			}

			// Transfer image
			m_iDisp.WriteImageGRAY( m_nRectX + ColonLeft, m_nRectY, m_iAreaImage.data + ColonLeft, m_iAreaImage.step, m_nColonWidth, m_iAreaImage.rows );

			// Update variables
			m_nLastSecond	= tl->tm_sec;
		}
	}

protected:
	ImageFont	m_iFont;
	int			m_nLastHour;
	int			m_nLastMinute;
	int			m_nLastSecond;

	int			m_nColonWidth;
	int			m_nColonOffsetX;
	int			m_nOffsetY;
};


class MpdGui
{
public:
	MpdGui()
	{
		m_iDisplays		= GetUsrDisplays();
		m_eDisplayMode	= DISPLAY_MODE_NONE;

		m_isVolumeCtrlMode		= false;
		m_isButtonNextPressed	= false;
		m_isButtonPrevPressed	= false;
	}
	
	bool	Initialize()
	{
		for( auto it : m_iDisplays )
		{
			if( 0 != it->Init() )
			{
				return	false;
			}
	
			it->DispClear();
			it->DispOn();
	
			SetupLayout_SongInfo( m_iDrawAreasSongInfo, it );
			SetupLayout_Idle( m_iDrawAreasIdle, it );
			SetupLayout_Volume( m_iDrawAreasVolume, it );
		}
		
		////////////////////////////////////
		// Gpio interrupt
		////////////////////////////////////
		SetupButtons();
		
		return	true;
	}

	void	Loop()
	{
		m_iGpioIntCtrl.ThreadStart();
	
		////////////////////////////////////
		// Connect to MPD
		////////////////////////////////////
		DISPLAY_MODE	ePrevDisplayMode;

		ePrevDisplayMode	= DISPLAY_MODE_NONE;
		m_eDisplayMode		= DISPLAY_MODE_NONE;
		m_isVolumeCtrlMode	= false;

		while( 1 )
		{
			std::map<std::string,std::string>	iInfo;
	
			///////////////////////////////////////////
			// Receive current playback info.
			///////////////////////////////////////////
			{
				std::string	strStatus;
		
				try
				{
					Socket		iSock;
					std::string	strCurrentSong;

					if( 0 == iSock.connect( MPD_HOST, MPD_PORT ) )
					{
						iSock	>> strStatus;	// DummyRead

						iSock	<< "currentsong\n";
						iSock	>> strCurrentSong;
						iSock	<< "status\n";
						iSock	>> strStatus;

						strStatus	+= strCurrentSong;
					}
				}
				catch( const char * reason )
				{
					printf( "EXCEPTION!! : %s\n", reason );
					printf( "%s\n", strStatus.c_str() );
					strStatus	= "";
				}

				iInfo		= MusicController::SplitMpdStatus( strStatus );

				if( iInfo.find("state") == iInfo.end() )
				{
					iInfo["state"]	= "???";
				}

				if( iInfo.find("Date") != iInfo.end() )
				{
					iInfo["Date"]	= iInfo["Date"].substr( 0, 4 );
				}
				
				auto	itF	= iInfo.find("file");
				if( itF != iInfo.end() )
				{
					if( iInfo.find("Title") == iInfo.end() )
					{
						std::vector<std::string>	names	= StringUtil::SplitReverse( (*itF).second, "/", 3 );
	
						iInfo["Title"]	= names[0];
						iInfo["Album"]	= names[1];
						iInfo["Artist"]	= names[2];
					}
					else if( 0 == (*itF).second.find("http://") )
					{
						std::vector<std::string>	names	= StringUtil::Split( iInfo["Title"], " - ", 3 );
						
						iInfo["Artist"]	= names[0];
						iInfo["Title"]	= names[1];
						iInfo["Album"]	= names[2];
						
						iInfo["Album"]	= iInfo["Name"];
					}
				}
			}

			if( iInfo["state"] == "play" )
			{
				m_eDisplayMode	= DISPLAY_MODE_SONGINFO;
			}
			else
			{
				m_eDisplayMode	= DISPLAY_MODE_IDLE;
			}

			if( !m_isVolumeCtrlMode )
			{
				if( m_isButtonNextPressed )
				{
					double elapsed = std::chrono::duration_cast<std::chrono::seconds>(
										std::chrono::system_clock::now() -
										m_iButtonNextPressedTime).count();

					if( 0.3 <= elapsed )
					{
						m_isVolumeCtrlMode	= true;
					}
				}
				
				if( m_isButtonPrevPressed )
				{
					double elapsed = std::chrono::duration_cast<std::chrono::seconds>(
										std::chrono::system_clock::now() -
										m_iButtonPrevPressedTime).count();

					if( 0.3 <= elapsed )
					{
						m_isVolumeCtrlMode	= true;
					}
				}
			}
			
			if( m_isVolumeCtrlMode )
			{
				m_eDisplayMode	= DISPLAY_MODE_VOLUME;
				
				iInfo["volume"]	= MusicController::VolumeCtrl( 
										(m_isButtonNextPressed ? 1 : 0) +
										(m_isButtonPrevPressed ? -1 : 0) );
			}

			if( m_eDisplayMode != ePrevDisplayMode )
			{
				for( auto it : m_iDisplays )		
				{
					it->DispClear();
				}
				
				for( auto it : m_iDrawAreasSongInfo )
				{
					it->Reset();
				}
	
				for( auto it : m_iDrawAreasIdle )
				{
					it->Reset();
				}
	
				for( auto it : m_iDrawAreasVolume )
				{
					it->Reset();
				}

				ePrevDisplayMode	= m_eDisplayMode;
			}

			switch( m_eDisplayMode )
			{
			case DISPLAY_MODE_SONGINFO:
				for( auto it : m_iDrawAreasSongInfo )
				{
					it->UpdateInfo( iInfo );
				}
	
				for( auto it : m_iDisplays )		
				{
					it->Flush();
				}
	
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				break;
				
			case DISPLAY_MODE_IDLE:
				for( auto it : m_iDrawAreasIdle )
				{
					it->UpdateInfo( iInfo );
				}
	
				for( auto it : m_iDisplays )		
				{
					it->Flush();
				}
	
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				break;

			case DISPLAY_MODE_VOLUME:
				for( auto it : m_iDrawAreasVolume )
				{
					it->UpdateInfo( iInfo );
				}
	
				for( auto it : m_iDisplays )		
				{
					it->Flush();
				}
	
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				break;
			}
		}
	
		m_iGpioIntCtrl.ThreadStop();
		return;
	}

protected:
	static	void	SetupLayout_SongInfo( std::vector<DrawAreaIF*>& iDrawAreas, DisplayIF* it )
	{
		int		x, y, csz, big, med, sml, ind;
		int		cx	= it->GetSize().width;
		int		cy	= it->GetSize().height;
		
		uint32_t	white	= 0xFFFFFFFF;
		uint32_t	blue	= 0xFF0095D8;
	
		if( cy < cx )
		{
			if( 80 < cy )
			{
				int	m	= 2;
				big	= 48 * cy / 240; 
				med	= 36 * cy / 240; 
				sml	= 28 * cy / 240;
				ind	=  4 * cy / 240;
		
				x	= m;
				y	= m;
				iDrawAreas.push_back( new DrawArea_STR( "Title",	blue,	*it,	x,	y,			cx - 2*x,	big	) );	y += big + 2;
				iDrawAreas.push_back( new DrawArea_PlayPos(					*it,	x,	y,			cx - 2*x,	ind ) );	y += ind + 2;
				iDrawAreas.push_back( new DrawArea_STR( "Artist",	white,	*it,	x,	y,			cx - 2*x,	med	) );	y += med + 2;
		
				x	= m * 2;
				csz	= cy - y - m * 2;
				iDrawAreas.push_back( new DrawArea_CoverImage(				*it,	x,	y,			csz,		csz	) );	x += csz + 8;
				iDrawAreas.push_back( new DrawArea_STR( "Album",	white,	*it,	x,	y,			cx - x,		med	) );	y += med + 2;
				iDrawAreas.push_back( new DrawArea_STR( "Date",		white,	*it,	x,	y,			cx - x,		med	) );	y += med + 2;
				iDrawAreas.push_back( new DrawArea_STR( "audio",	white,	*it,	x,	y,			cx - x,		sml	) );	y += sml;

#if FEATURE_INA219
				int	tiny	= (cy - y) / 2;
				iDrawAreas.push_back( new DrawArea_CpuTemp(					*it,	x,	cy-tiny*2,	cx - x,		tiny ) );
				iDrawAreas.push_back( new DrawArea_Battery(					*it,	x,	cy-tiny,	cx - x,		tiny ) );
#else
				iDrawAreas.push_back( new DrawArea_CpuTemp(					*it,	x,	cy-sml,		cx - x,		sml ) );
#endif
			}
			else if( 32 < cy )
			{
				big	= 18 * cy / 64; 
				sml	= 12 * cy / 64;
				med	= (cy - big - sml - 2) / 2;
		
				x	= 0;
				y	= 0;
				iDrawAreas.push_back( new DrawArea_STR( "Title",	white,	*it,	x,	y,			cx,		big ) );	y += big;
				iDrawAreas.push_back( new DrawArea_PlayPos(					*it,	x,	y,			cx,		  1 ) );	y +=   1 + 1;
		
				csz	= it->GetSize().height - y;
				iDrawAreas.push_back( new DrawArea_CoverImage(				*it,	x,	y,			csz,	csz  ));
				
				x	+= csz + 4;
		
				iDrawAreas.push_back( new DrawArea_STR( "Artist",	white,	*it,	x,	y,			cx - x,	med ) );	y += med;
				iDrawAreas.push_back( new DrawArea_STR( "Album",	white,	*it,	x,	y,			cx - x,	med ) );	y += med;
				iDrawAreas.push_back( new DrawArea_STR( "audio",	white,	*it,	x,	cy-sml,		cx - x,	sml ) );
			}
			else
			{
				big	= 16 * cy / 32;
				med	= cy - big - 2; 
		
				x	= 0;
				y	= 0;
				iDrawAreas.push_back( new DrawArea_STR( "Title",	white,	*it,	x,	y,			cx,		big ) );	y += big;
				iDrawAreas.push_back( new DrawArea_PlayPos(					*it,	x,	y,			cx,		  1 ) );	y +=   1 + 1;
				iDrawAreas.push_back( new DrawArea_STR( "Artist",	white,	*it,	x,	y,			cx,		med ) );	y += med;
			}
		}
		else if( cx == cy )
		{
//*
			// Normal
			int	m	= 2;
			big	= 36 * cy / 240; // 48
			med	= 32 * cy / 240; // 36
			sml	= 20 * cy / 240;
			ind	=  6 * cy / 240;
	
			x	= m;
			y	= m;

//			if( DrawArea_BatteryIcon::IsValid() )
//			{
//				iDrawAreas.push_back( new DrawArea_BatteryIcon(	*it, cx - 16, 0, 16, big ) );
//				iDrawAreas.push_back( new DrawArea_STR( "Title",	blue,	*it,	x,	y,			cx - 2*x - 16,	big,	false	) );	y += big + 2;
//			}
//			else
//			{
				iDrawAreas.push_back( new DrawArea_STR( "Title",	blue,	*it,	x,	y,			cx - 2*x,	big,	false	) );	y += big + 2;
//			}

			iDrawAreas.push_back( new DrawArea_PlayPos(					*it,	x,	y,			cx - 2*x,	ind				) );	y += ind + 2;
			iDrawAreas.push_back( new DrawArea_STR( "Album",	white,	*it,	x,	y,			cx - 2*x,	med,	false	) );	y += med + 2;
			iDrawAreas.push_back( new DrawArea_STR( "Artist",	white,	*it,	x,	y,			cx - 2*x,	med,	false	) );	y += med + 2;
	
			x	= m * 2;
			csz	= cy - y - m * 2;
			iDrawAreas.push_back( new DrawArea_CoverImage(				*it,	x,	y,			csz,		csz	) );	x += csz + 8;

			iDrawAreas.push_back( new DrawArea_STR( "Date",		white,	*it,	x,	y,				cx - x,		med	) );	y += med + 2;
			iDrawAreas.push_back( new DrawArea_CpuTemp(					*it,	x,	cy-sml*2-m*2,	cx - x,		sml ) );	y += med + 2;
			iDrawAreas.push_back( new DrawArea_STR( "audio",	white,	*it,	x,	cy-sml-m,		cx - x,		sml	) );
//*/
/*
			int	m	= 0;
			big	= 32 * cy / 240; // 48
			med	= 28 * cy / 240; // 36
			sml	= 20 * cy / 240;
			ind	=  4 * cy / 240;
	
			x	= m;
			y	= m;
			iDrawAreas.push_back( new DrawArea_STR( "Title",	blue,	*it,	x,	y,			cx - 2*x,	big,	false	) );	y += big + 2;
			iDrawAreas.push_back( new DrawArea_PlayPos(					*it,	x,	y,			cx - 2*x,	ind				) );	y += ind + 2;
			iDrawAreas.push_back( new DrawArea_STR( "Album",	white,	*it,	x,	y,			cx - 2*x,	med,	false	) );	y += med + 2;
			iDrawAreas.push_back( new DrawArea_STR( "Artist",	white,	*it,	x,	y,			cx - 2*x,	med,	false	) );	y += med + 2;
	
			x	= m * 2;
			csz	= cy - y - m * 2;
			iDrawAreas.push_back( new DrawArea_CoverImage(				*it,	x,	y,			csz,		csz	) );	x += csz + 8;

//			iDrawAreas.push_back( new DrawArea_STR( "Date",		white,	*it,	x,	y,				cx - x,		med	) );	y += med + 2;
//			iDrawAreas.push_back( new DrawArea_CpuTemp(					*it,	x,	cy-sml*2-m*2,	cx - x,		sml ) );	y += med + 2;
//			iDrawAreas.push_back( new DrawArea_STR( "audio",	white,	*it,	x,	cy-sml-m,		cx - x,		sml	) );
			iDrawAreas.push_back( new DrawArea_STR( "audio",	white,	*it,	x,	y,				sml,		csz, true, 270	) );
//*/
			
/*
			// Centering
			int	m	= 0;
			big	= 32 * cy / 240; // 48
			med	= 28 * cy / 240; // 36
			sml	= 24 * cy / 240;
			ind	=  4 * cy / 240;
	
			x	= m;
			y	= m;
			iDrawAreas.push_back( new DrawArea_STR( "Title",	blue,	*it,	x,	y,			cx - 2*x,	big,	-1	) );	y += big + 2;
			iDrawAreas.push_back( new DrawArea_PlayPos(					*it,	x,	y,			cx - 2*x,	ind			) );	y += ind + 2;
			iDrawAreas.push_back( new DrawArea_STR( "Artist",	white,	*it,	x,	y,			cx - 2*x,	med,	-1	) );	y += med + 2;
//			iDrawAreas.push_back( new DrawArea_STR( "Album",	white,	*it,	x,	y,			cx - 2*x,	med,	-1	) );	y += med + 2;
	
			x	= m * 2;
			csz	= cy - y - m * 2;	
			x	= (cx - csz) / 2;

			iDrawAreas.push_back( new DrawArea_STR( "Album",	white,	*it,	x - sml - 4,	y,	sml,	cy - y,	true, 270	) );
			iDrawAreas.push_back( new DrawArea_CoverImage(				*it,	x,				y,	csz,	csz	) );
			iDrawAreas.push_back( new DrawArea_STR( "audio",	white,	*it,	x + csz + 4,	y,	sml,	cy - y, true, 270	) );

//			iDrawAreas.push_back( new DrawArea_STR( "Date",		white,	*it,	x,	y,				cx - x,		med	) );	y += med + 2;
//			iDrawAreas.push_back( new DrawArea_CpuTemp(					*it,	x,	cy-sml*2-m*2,	cx - x,		sml ) );	y += med + 2;
//			iDrawAreas.push_back( new DrawArea_STR( "audio",	white,	*it,	x,	cy-sml-m,		cx - x,		sml	) );
//*/		
		
		}
		else
		{
			int	m	= 2;
			big	= 36 * cy / 320; 
			med	= 28 * cy / 320; 
			sml	= 24 * cy / 320;
			ind	=  4 * cy / 320;
	
			x	= m;
			y	= m;
			iDrawAreas.push_back( new DrawArea_STR( "Title",	blue,	*it,	x,	y,			cx - 2*x,	big, false	) );	y += big + 2;
			iDrawAreas.push_back( new DrawArea_PlayPos(					*it,	x,	y,			cx - 2*x,	ind			) );	y += ind + 2;
			iDrawAreas.push_back( new DrawArea_STR( "Artist",	white,	*it,	x,	y,			cx - 2*x,	med, true	) );	y += med + 2;
	
			csz	= cy - y - 4 - med - 2 - sml;
			csz	= csz < cx ? csz : cx;
			
			iDrawAreas.push_back( new DrawArea_CoverImage(				*it,	(cx-csz)/2,	y,	csz,		csz			));	y += csz + 4;
			iDrawAreas.push_back( new DrawArea_STR( "Album",	white,	*it,	x,	y,			cx - 2*x,	med, false	) );	y += med + 2;
			iDrawAreas.push_back( new DrawArea_STR( "audio",	white,	*it,	x,	cy-sml,		cx - 2*x,	sml, true	) );
		}
	}

	static	void	SetupLayout_Volume( std::vector<DrawAreaIF*>& iDrawAreas, DisplayIF* it )
	{
		int		m	= 4;
		int		cx	= it->GetSize().width;
		int		cy	= it->GetSize().height;
		
		uint32_t	white	= 0xFFFFFFFF;
		int		oy	= 0;
		
		if( cx <= cy )
		{
			int	h	= cx * 2 / 3;
			oy	= (cy - h) / 2;
			cy	= h;
		}

#ifdef VOLUME_CTRL_I2C_AK449x
		iDrawAreas.push_back( new DrawArea_StaticText(	"volume",	white,	*it,	m,	oy,					cx - 2 * m, cy * 6 / 16 ) );
		iDrawAreas.push_back( new DrawArea_STR( 		"volume",	white,	*it,	0,	oy + cy * 6 / 16,	cx        , cy * 6 / 16, true ) );
#else
		iDrawAreas.push_back( new DrawArea_StaticText(	"volume",	white,	*it,	m,	oy,					cx - 2 * m, cy *  6 / 16 ) );
		iDrawAreas.push_back( new DrawArea_STR( 		"volume",	white,	*it,	0,	oy + cy * 6 / 16,	cx - 2 * m, cy * 10 / 16, true ) );
#endif
	}

	static	void	SetupLayout_Idle( std::vector<DrawAreaIF*>& iDrawAreas, DisplayIF* it )
	{
		int				font_height	= it->GetSize().height;
		char			buf[256];
		int				cx, cy;
		int				x, y, top;
	
		{
			ImageFont	iFont( FONT_DATE_PATH, font_height, false );
			int			l,t,r,b;
	
			iFont.CalcRect( l,t,r,b, "88:88" );
			cx	= r;
			cy	= b-t;
	
			if( it->GetSize().width < cx )
			{
				cy			= font_height * it->GetSize().width / cx;
				cx			= it->GetSize().width;

				cy			= cy * 9 / 10;
				font_height	= cy;
			}
	
	//		printf("%d x %d, %d\n", cx, cy, font_height );
		}	

		// draw time
		{
			x	= (it->GetSize().width  - cx) / 2;
			y	= (it->GetSize().height - cy) / 2;
	
			iDrawAreas.push_back( new DrawArea_Time( *it,	x,	y,	cx,	cy ) );
	
			top	= y;
			y	+= cy;
		}

		x	= 4;
	
		// draw date
		cy	= font_height * 4 / 10;
		cy	= cy < top ? cy : top;
		iDrawAreas.push_back( new DrawArea_Date(	*it,	x,	top - cy,	it->GetSize().width - x,	cy ) );
	
		cy	= font_height * 4 / 10;
		cy	= y+cy < it->GetSize().height ? cy : it->GetSize().height - y;
		
		cy	= cy * 2 / 3;

		// ip addr
		iDrawAreas.push_back( new DrawArea_MyIpAddr(	0xFFFFFFFF,	*it,	0,	y,	it->GetSize().width - x,	cy,	true ) );	y	+= cy;

		// cpu temp
		iDrawAreas.push_back( new DrawArea_CpuTemp( *it,	0,	y,	it->GetSize().width - x,	cy,	true ) );	y	+= cy;

#if FEATURE_INA219
		// battery voltage
		iDrawAreas.push_back( new DrawArea_Battery(	*it,	0,	y,	it->GetSize().width - x,	cy,	true ) );	y	+= cy;
//		iDrawAreas.push_back( new DrawArea_BatteryIcon(	*it, it->GetSize().width - 16, 0, 16, 32 ) );
#endif
	}

	void	SetupButtons()
	{
		m_iGpioIntCtrl.RegistPin(
			GPIO_BUTTON_PREV,
			[&]( int value )
			{
				if( 0 == value )
				{
					printf( "OnButtonPrev_Up()\n");

					if( m_isVolumeCtrlMode == 0 )
					{
						MusicController::PlayPrevSong();
					}

					m_isButtonPrevPressed		= false;
					m_isVolumeCtrlMode			= false;
					printf( "OnButtonPrev_Up() Leave\n");
				}
				else
				{
					printf( "OnButtonPrev_Down()\n");
					m_isVolumeCtrlMode			= false;
					m_iButtonPrevPressedTime	= std::chrono::system_clock::now();
					m_isButtonPrevPressed		= true;
					printf( "OnButtonPrev_Down() Leave\n");
				}
			});
	
		m_iGpioIntCtrl.RegistPin(
			GPIO_BUTTON_NEXT,
			[&]( int value )
			{
				if( 0 == value )
				{
					printf( "OnButtonNext_Up()\n");

					if( !m_isVolumeCtrlMode )
					{
						MusicController::PlayNextSong();
					}

					m_isButtonNextPressed		= false;
					m_isVolumeCtrlMode			= false;

					printf( "OnButtonNext_Up() LEAVE\n");
				}
				else
				{
					printf( "OnButtonNext_Down()\n");
					m_isVolumeCtrlMode			= false;
					m_iButtonNextPressedTime	= std::chrono::system_clock::now();
					m_isButtonNextPressed		= true;
					printf( "OnButtonNext_Down() LEAVE\n");
				}
			});
	
		m_iGpioIntCtrl.RegistPin(
			GPIO_BUTTON_PLAY,
			[]( int value )
			{
				if( 1 == value )
				{
					MusicController::PlayPause();
				}
			});
	}
	
	enum DISPLAY_MODE
	{
		DISPLAY_MODE_NONE	= 0,
		DISPLAY_MODE_IDLE,
		DISPLAY_MODE_SONGINFO,
		DISPLAY_MODE_VOLUME,
	};

protected:
	DISPLAY_MODE				m_eDisplayMode;
	std::vector<DisplayIF*>		m_iDisplays;
	std::vector<DrawAreaIF*>	m_iDrawAreasSongInfo;
	std::vector<DrawAreaIF*>	m_iDrawAreasIdle;
	std::vector<DrawAreaIF*>	m_iDrawAreasVolume;
	GpioInterruptCtrl			m_iGpioIntCtrl;

	bool									m_isVolumeCtrlMode;
	bool									m_isButtonNextPressed;
	bool									m_isButtonPrevPressed;
	std::chrono::system_clock::time_point	m_iButtonNextPressedTime;
	std::chrono::system_clock::time_point	m_iButtonPrevPressedTime;
};



int	main( int argc, char* argv[] )
{
	MpdGui	iMPD;
	
	if( 1 < argc )
	{
		if( 0 == strcmp( argv[1], "zishan" ) )
		{
			printf( "MODE = ZiShanDSD\n");
			AK449x_CH0	= 0x12;
			AK449x_CH1	= 0x10;
		}
	}
	
	if( !iMPD.Initialize() )
	{
		return	-1;
	}

	iMPD.Loop();
	return	0;
}
