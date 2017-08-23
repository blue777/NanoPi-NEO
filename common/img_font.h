#ifndef	__IMG_FONT_H_INCLUDED__
#define __IMG_FONT_H_INCLUDED__

//	`freetype-config --cflags` `freetype-config --libs`


//	https://www.freetype.org/freetype2/docs/reference/ft2-index.html
//	https://www.freetype.org/freetype2/docs/tutorial/example1.c
//	https://www.freetype.org/freetype2/docs/tutorial/example2.cpp

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <stdint.h>
#include <string>
#include <locale>
#include <codecvt>

class	ImageFont
{
public:
	ImageFont( const char* filename, uint32_t height, bool isFitHeight=true )
	{
		FT_Error	error;

		m_nFontHeight	= height;
		m_piLibrary		= NULL;
		m_piFace		= NULL;

		error	= FT_Init_FreeType( &m_piLibrary );
		if( 0 != error )
		{
			throw;
		}

		error	= FT_New_Face( m_piLibrary, filename, 0, &m_piFace );
		if( 0 != error )
		{
			throw;
		}
		
		if( isFitHeight )
		{
			int		cx, cy;

			error	= FT_Set_Pixel_Sizes( m_piFace, 0, m_nFontHeight);
			if( 0 != error )
			{
				throw;
			}

			if( 0 == CalcSize( cx, cy, "_gjpqy({[" ) )
			{
				m_nFontHeight	= (m_nFontHeight * m_nFontHeight * 4 + cy) / (cy * 4);
			}
			else
			{
				throw;
			}

			error	= FT_Set_Pixel_Sizes( m_piFace, 0, m_nFontHeight );
			if( 0 != error )
			{
				throw;
			}
			
/*
			if( 0 == CalcSize( cx, cy, "_gjpqy({[" ) )
			{
				printf( "%d --> %d\n", height, cy );
			}
//*/
		}
		else
		{
			error	= FT_Set_Pixel_Sizes( m_piFace, 0, m_nFontHeight );
			if( 0 != error )
			{
				throw;
			}
		}
	}
	

	~ImageFont()
	{
		FT_Done_Face( m_piFace );
		FT_Done_FreeType( m_piLibrary );
	}

	int	CalcSize( int& width, int& height, const char* str )
	{
		std::u32string	u32str	= std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str);

		return	CalcSize( width, height, u32str );
	}

	int	CalcSize( int& width, int& height, const std::u32string& u32str )
	{
		FT_Error		error;
		FT_GlyphSlot	slot	= m_piFace->glyph;
		FT_Matrix		matrix	= { 1 << 16, 0, 0, 1 << 16 };
		FT_Vector		pen		= { 0, 0 };

		width	= 0;
		height	= 0;

		for( size_t i = 0; i < u32str.size(); i++ )
		{
			FT_Set_Transform( m_piFace, &matrix, &pen );

			error	= FT_Load_Char( m_piFace, u32str[i], FT_LOAD_RENDER );
			if( 0 == error )
			{
				int	font_height	= m_nFontHeight - slot->bitmap_top + slot->bitmap.rows;

				if( height < font_height )
				{
					height	= font_height;
				}

				pen.x	+= slot->advance.x;
				pen.y	+= slot->advance.y;
			}
		}
		
		width	= pen.x >> 6;
		return	0;
	}
	
	int DrawTextGRAY( int x, int y, const char* str, uint8_t color, uint8_t * image, int stride, int cx, int cy )
	{
		std::u32string	u32str	= std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str);

		return	DrawTextGRAY( x, y, u32str, color, image, stride, cx, cy );
	}

	int DrawTextGRAY( int x, int y, const std::u32string& u32str, uint8_t color, uint8_t * image, int stride, int cx, int cy )
	{
		FT_Error		error;
		FT_GlyphSlot	slot	= m_piFace->glyph;
		FT_Matrix		matrix	= { 1 << 16, 0, 0, 1 << 16 };
		FT_Vector		pen		= { 0, 0 };

		for( size_t i = 0; i < u32str.size(); i++ )
		{
			FT_Set_Transform( m_piFace, &matrix, &pen );

			error	= FT_Load_Char( m_piFace, u32str[i], FT_LOAD_RENDER );
			if( 0 == error )
			{
				int	bmp_cy	= slot->bitmap.rows;
				int	pos_y	= y + m_nFontHeight - slot->bitmap_top;
				int	rs		= 0 <= pos_y ? 0 : -pos_y;
				int	re		= (pos_y + bmp_cy) <= cy ? bmp_cy : (cy - pos_y);

				int	bmp_cx	= slot->bitmap.width;
				int	pos_x	= x + slot->bitmap_left;
				int	cs		= 0 <= pos_x ? 0 : -pos_x;
				int	ce		= (pos_x + bmp_cx) <= cx ? bmp_cx : (cx - pos_x);

				for( int r = rs; r < re; r++ )
				{
					uint8_t *	src_line	= &slot->bitmap.buffer[ bmp_cx * r ];
					uint8_t *	dst_line	= &image[ stride * (pos_y + r) + pos_x ];
					int 		c			= cs;

					for( ; c < ce; c++ )
					{
						int32_t	a0	= src_line[c+0];

						if( 0 < a0 )
						{
							int32_t	d0	= dst_line[c+0];
							a0	+= a0 >> 7;
							dst_line[c+0] = (uint8_t)( d0 + (((color - d0) * a0) >> 8) );
						}
					}
				}

				pen.x	+= slot->advance.x;
				pen.y	+= slot->advance.y;
			}
		}
		
		return	0;
	}

	int DrawTextBGRA( int x, int y, const char* str, uint32_t color, uint8_t * image, int stride, int cx, int cy )
	{
		std::u32string	u32str	= std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str);

		return	DrawTextBGRA( x, y, u32str, color, image, stride, cx, cy );
	}
	
	int DrawTextBGRA( int x, int y, const std::u32string& u32str, uint32_t color, uint8_t * image, int stride, int cx, int cy )
	{
		FT_Error		error;
		FT_GlyphSlot	slot	= m_piFace->glyph;
		FT_Matrix		matrix	= { 1 << 16, 0, 0, 1 << 16 };
		FT_Vector		pen		= { 0, 0 };
		uint32_t		alpha	= (color >> 24) + (color >> 31);

		for( size_t i = 0; i < u32str.size(); i++ )
		{
			FT_Set_Transform( m_piFace, &matrix, &pen );

			error	= FT_Load_Char( m_piFace, u32str[i], FT_LOAD_RENDER );
			if( 0 == error )
			{
				int	bmp_cy	= slot->bitmap.rows;
				int	pos_y	= y + m_nFontHeight - slot->bitmap_top;
				int	rs		= 0 <= pos_y ? 0 : -pos_y;
				int	re		= (pos_y + bmp_cy) <= cy ? bmp_cy : (cy - pos_y);

				int	bmp_cx	= slot->bitmap.width;
				int	pos_x	= x + slot->bitmap_left;
				int	cs		= 0 <= pos_x ? 0 : -pos_x;
				int	ce		= (pos_x + bmp_cx) <= cx ? bmp_cx : (cx - pos_x);

				for( int r = rs; r < re; r++ )
				{
					uint8_t *	src_line	= &slot->bitmap.buffer[ bmp_cx * r ];
					uint8_t *	dst_line	= &image[ stride * (pos_y + r) + (pos_x * 4) ];
					int 		c			= cs;

					for( ; c < ce; c++ )
					{
						int32_t	a	= src_line[c+0];
						
						if( 0 < a )
						{
							uint8_t*	clr	= (uint8_t*)&color;
							int32_t		d0	= dst_line[c*4+0];
							int32_t		d1	= dst_line[c*4+1];
							int32_t		d2	= dst_line[c*4+2];
							int32_t		d3	= dst_line[c*4+3];
	
							a	+= a >> 7;
							a	*= alpha;
	
							dst_line[c*4+0] = (uint8_t)( ((d0 << 16) + (clr[0] - d0) * a) >> 16 );
							dst_line[c*4+1] = (uint8_t)( ((d1 << 16) + (clr[1] - d1) * a) >> 16 );
							dst_line[c*4+2] = (uint8_t)( ((d2 << 16) + (clr[2] - d2) * a) >> 16 );
							dst_line[c*4+3] = (uint8_t)( ((d3 << 16) + (clr[3] - d3) * a) >> 16 );
						}
					}
				}

				pen.x	+= slot->advance.x;
				pen.y	+= slot->advance.y;
			}
		}
		
		return	0;
	}

protected:
	int				m_nFontHeight;
	FT_Library		m_piLibrary;
	FT_Face			m_piFace;
};

#endif	// __IMG_FONT_H_INCLUDED__