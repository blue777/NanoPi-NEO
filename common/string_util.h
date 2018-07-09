
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdio.h>

namespace StringUtil
{
	std::vector<std::string>	Split( const std::string& str, char sep='\n', size_t min_array=0 )
	{
		std::vector<std::string>	array;

		std::string			line;
		std::istringstream	stream(str);
		while( std::getline( stream, line, sep ))
		{
			array.push_back( line );
		}
		
		if( array.size() < min_array )
		{
			array.resize( min_array );
		}

		return	array;
	};

	std::vector<std::string>	Split( const std::string& str, const std::string& sep, size_t min_array=0 )
	{
		std::vector<std::string>	array;

		std::string::size_type		spos	= 0;
		std::string::size_type		fpos	= 0;
		
		fpos = str.find( sep, spos );
		while( fpos != std::string::npos )
		{
			array.push_back( str.substr( spos, fpos-spos ) );
			
			spos	= fpos + sep.size();
			fpos	= str.find( sep, spos );
		}

		if( spos+1 < str.size() )
		{
			array.push_back( str.substr( spos ) );
		}

		if( array.size() < min_array )
		{
			array.resize( min_array );
		}

		return	array;
	};

	std::vector<std::string>	SplitReverse( const std::string& str, const std::string& sep, size_t min_array=0 )
	{
		std::vector<std::string>	array;

		std::string::size_type		spos	= 0;
		std::string::size_type		fpos	= 0;
		
		fpos = str.find( sep, spos );
		while( fpos != std::string::npos )
		{
			array.insert( array.begin(), str.substr( spos, fpos-spos ) );
			
			spos	= fpos + sep.size();
			fpos	= str.find( sep, spos );
		}

		if( spos+1 < str.size() )
		{
			array.insert( array.begin(), str.substr( spos ) );
		}

		if( array.size() < min_array )
		{
			array.resize( min_array );
		}

		return	array;
	};

	std::vector<std::string>	SplitByWhites( const std::string& str, size_t min_array=0 )
	{
		std::vector<std::string>	array;

		for( size_t s = 0; s < str.size(); s++ )
		{
			size_t	e;

			// skip whites
			for( ; (str[s] == ' ') && (s < str.size()); s++ );
			
			// find text end
			for( e = s; (str[e] != ' ') && (e < str.size()); e++ );
			
			array.push_back( str.substr( s, e-s+1 ) );
			
			s	= e;
		}
		
		if( array.size() < min_array )
		{
			array.resize( min_array );
		}

		return	array;
	};
		
	std::string	GetTextFromFile( const char * pszFilePath )
	{
		std::ifstream	ifs( pszFilePath );
		
		if( !ifs.fail() )
		{
			return	std::string(
				(std::istreambuf_iterator<char>(ifs)),
				std::istreambuf_iterator<char>() );
		}
		
		return	std::string();
	};

#ifndef WIN32
	std::string	GetTextFromExecResult( const char * pszCommand )
	{
		std::string	result;
		FILE*		fp	= popen( pszCommand, "r");

		if( NULL != fp )
		{
			char		szBuf[256];
			
			while( NULL != fgets( szBuf, sizeof(szBuf), fp ) )
			{
				result	+= szBuf;
			}

			pclose(fp);
		}

		return	result;
	};
#endif
	
	
};