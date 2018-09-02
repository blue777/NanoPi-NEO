#ifndef	__CTRL_HTTP_H_INCLUDED__
#define	__CTRL_HTTP_H_INCLUDED__

#include "ctrl_socket.h"

class Http
{
public:
	static	int		Get( const char * pszHostName, int port, const char * URI, std::vector<uint8_t>& iRecv, std::vector<std::string>* piHeader=NULL, int timeout=1000 )
	{
		Socket		iSock;
		std::string	str;
		int			ret;
		
		iSock.SetTimeout( timeout );

		ret		= iSock.connect( pszHostName, port );
		if( 0 != ret )
		{
			return	ret;
		}

		str		= "GET ";
		str		+= URI;
		str		+= " HTTP/1.1\r\n";
		str		+= "HOST: ";
		str		+= pszHostName;
		str		+= "\r\n\r\n";
		
		ret		= iSock.send( (const uint8_t*)str.c_str(), str.size() );
		if( ret < str.size() )
		{
			return	-1;
		}

		iRecv.resize( 63356 );

		ret		= iSock.recv( iRecv.data(), iRecv.size() );
		if( ret <= 0 )
		{
			return	-1;
		}
		
		// Read HTTP header
		std::string	strHttpResult;
		{
			size_t	i	= 0;
			size_t	s	= 0;
			for( ; i < iRecv.size(); i++ )
			{
				if( iRecv[i] == '\n' )
				{
					size_t	e	= i;

					while( (s < e) && isspace(iRecv[s]) )	s++;
					while( (s < e) && isspace(iRecv[e-1]) )	e--;
					
					if( 0 < (e-s) )
					{
						std::string	line( (const char*)&iRecv[s], e - s );
						
						if( strHttpResult.empty() )
						{
							strHttpResult	= line;
						}
						
						if( piHeader != NULL )
						{
							piHeader->push_back( line );
						}
						
						s	= i + 1;
					}
					else
					{
						// End of header section
						i++;
						iRecv.erase( iRecv.begin(), iRecv.begin() + i );
						break;
					}
				}
			}
		}

		int	http_result	= std::stoi( std::string( strHttpResult, strHttpResult.find( ' ' ) + 1 ) );
		if( http_result == 200 )
		{
			return	0;
		}

		printf( "ERRRO: %s\n", strHttpResult.c_str() );
		return	http_result;
	}
};

#endif	// __CTRL_HTTP_H_INCLUDED__
