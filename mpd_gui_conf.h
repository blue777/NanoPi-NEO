/******************************************************************************
	Copyright (C) 2017-2020 blue-7 (http://qiita.com/blue-7)

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

#pragma once

#include <map>
#include <string>
#include <fstream>


class mpd_gui_conf
{
public:

	typedef std::map<std::string, std::map<std::string, std::string>>		INIFILE;

	void	TrimWhites(std::string& str)
	{
		while (!str.empty() &&
			((str.front() == ' ') ||
			 (str.front() == '\t') ||
			 (str.front() == '\r') ||
			 (str.front() == '\n')))
		{
			str.erase(str.begin());
		}

		while (!str.empty() &&
			((str.back() == ' ') ||
			 (str.back() == '\t') ||
			 (str.back() == '\r') ||
			 (str.back() == '\n')))
		{
			str.pop_back();
		}
	}

	INIFILE	LoadIniFile(const char * pszFilePath)
	{
		INIFILE			configs;
		std::ifstream	ifs(pszFilePath);
		std::string		line;
		std::string		section;

		while (std::getline(ifs, line))
		{
			TrimWhites(line);

			if (!line.empty())
			{
				switch (line.front())
				{
				case '#':
					break;

				case '[':
					if (line.back() == ']')
					{
						section = std::string(line.begin() + 1, line.end() - 1);
					}
					break;

				default:
					if (!section.empty())
					{
						size_t	pos = line.find('=');
						if (std::string::npos != pos)
						{
							std::string	key = std::string(line.begin(), line.begin() + pos);
							std::string	value = std::string(line.begin() + pos + 1, line.end());

							TrimWhites(key);
							TrimWhites(value);

							if (!key.empty())
							{
								configs[section][key] = value;
							}
						}
					}
					break;
				}
			}
		}

		return	configs;
	};

	template<class T>
	void	SetValue(T& value, const std::string & str)
	{
		value = str;
	}

	template<class T>
	void	UpdateValue(INIFILE& conf, const char * section, const char * key, T& value)
	{
		auto	itS = conf.find(section);
		if (itS != conf.end())
		{
			auto	itK = (*itS).second.find(key);
			if (itK != (*itS).second.end())
			{
				SetValue(value, (*itK).second);
			}
		}
	}

public:
#define	CONFIG(SECTION,KEY,VALUE)		\
	SECTION.KEY	= VALUE;				\
	UpdateValue(conf,#SECTION,#KEY,SECTION.KEY);

	mpd_gui_conf()
	{
		INIFILE	conf = LoadIniFile("/etc/mpd_gui.conf");

		CONFIG(General, FontPath,		"/usr/share/fonts/truetype/takao-gothic/TakaoPGothic.ttf");
		CONFIG(General, FontDatePath,	"/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
		CONFIG(General, MusicRootPath,	"/mnt/");

		CONFIG(Battery, Empty,	10000);
		CONFIG(Battery, Full,	12600);

		CONFIG(GpioButtons, Left,	0);
		CONFIG(GpioButtons, Center,	2);
		CONFIG(GpioButtons, Right,	3);
		CONFIG(GpioButtons, Down,	203 );
		CONFIG(GpioButtons, Up,		198 );
	}

public:
	struct
	{
		std::string		FontPath;
		std::string		FontDatePath;
		std::string		MusicRootPath;
	} General;

	struct
	{
		int		Empty;
		int		Full;
	} Battery;

	struct
	{
		int		Left;
		int		Center;
		int		Right;
		int		Up;
		int		Down;
	} GpioButtons;

};


template<>
void	mpd_gui_conf::SetValue(int& value, const std::string & str)
{
	if (!str.empty())
	{
		value = std::atoi( str.c_str() );
	}
}

template<>
void	mpd_gui_conf::SetValue( float& value, const std::string & str)
{
	if (!str.empty())
	{
		value = std::atof(str.c_str());
	}
}



///////////////////////////////
//  Global variables
///////////////////////////////
mpd_gui_conf	g_settings;


///////////////////////////////
//  Compile settings
///////////////////////////////

//#define	VOLUME_CTRL_I2C_AK449x	1

#define	GPIO_BUTTON_PREV	g_settings.GpioButtons.Left
#define	GPIO_BUTTON_NEXT	g_settings.GpioButtons.Right
#define	GPIO_BUTTON_PLAY	g_settings.GpioButtons.Center

#define	MUSIC_ROOT_PATH		g_settings.General.MusicRootPath.c_str()
#define	FONT_PATH			g_settings.General.FontPath.c_str()
#define	FONT_DATE_PATH		g_settings.General.FontDatePath.c_str()

#define	BATTERY_EMPTY		g_settings.Battery.Empty
#define	BATTERY_FULL		g_settings.Battery.Full

#ifndef VOLUME_CTRL_I2C_AK449x
#define	VOLUME_CTRL_I2C_AK449x	0
#endif

#ifndef VOLUMIO
#define	VOLUMIO					0
#endif

#ifndef FEATURE_INA219
#define	FEATURE_INA219			1
#endif

#if VOLUMIO
#undef	VOLUME_CTRL_I2C_AK449x
#define	VOLUME_CTRL_I2C_AK449x	0
#endif


#define	MPD_HOST			"127.0.0.1"
#define	MPD_PORT			6600

#define	VOLUMIO_HOST		"127.0.0.1"
#define VOLUMIO_PORT		3000


int		AK449x_CH0			= 0x10;
int		AK449x_CH1			= 0x11;




