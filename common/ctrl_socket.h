#ifndef	__CTRL_SOCKET_H_INCLUDED__
#define	__CTRL_SOCKET_H_INCLUDED__


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


class Socket
{
public:
	Socket( int domain=AF_INET, int type=SOCK_STREAM, int protocol=0 )
	{
		m_iSock	= ::socket( domain, type, protocol );
		if( m_iSock < 0 )
		{
			printf("socket::socket(), socket ret %d\n", m_iSock );
			throw;
		}
	}
	
	~Socket()
	{
		if( 0 <= m_iSock )
		{
			::close( m_iSock );
			m_iSock	= -1;
		}
	}
	
	int	connect( const char* pszHostName, int port )
	{
		struct sockaddr_in	tHost	= {0};

		tHost.sin_family		= AF_INET;
		tHost.sin_port			= htons( port );
		tHost.sin_addr.s_addr	= inet_addr(pszHostName);
		
		if( tHost.sin_addr.s_addr == 0xFFFFFFFF )
		{
			struct hostent*	ptHost	= ::gethostbyname(pszHostName);
			
			if( NULL == ptHost )
			{
				printf("ERROR! socket::connect(%s,%d) host not found.\n", pszHostName, port );
				return	-1;
			}

			unsigned int**	ppuiAddr	= (unsigned int **)ptHost->h_addr_list;
			if( NULL == *ppuiAddr )
			{
				printf("ERROR! socket::connect(%s,%d) system error?\n", pszHostName, port );
				return	-1;
			}

			tHost.sin_addr.s_addr	= *(*ppuiAddr);
		}

		if( 0 != ::connect( m_iSock, (struct sockaddr*)&tHost, sizeof(tHost) ) )
		{
			printf("ERROR! socket::connect(%s,%d) Connection refused.\n", pszHostName, port );
			return	-1;
		}
		
		return	0;
	}
	
	int	send( const uint8_t* pData, int nDataLength )
	{
		int	ret	= ::write( m_iSock, pData, nDataLength );
		if( ret <= 0 )
		{
			printf( "ERROR: Socket::send(*,%d) ret %d\n", nDataLength, ret );
		}
		
		return	ret;
	}
	
	int	recv( uint8_t* pData, int BufferSize )
	{
		int	ret	= ::read( m_iSock, pData, BufferSize );
		if( ret <= 0 )
		{
			printf( "ERROR: Socket::recv(*,%d) ret %d\n", BufferSize, ret );
		}

		return	ret;		
	}

    void    operator << ( const std::string& str )
    {
    	int	ret	= send( (const uint8_t*)str.data(), str.size() );
     	if( ret <= 0 )
    	{
    		throw;
    	}
    }

    void    operator >> ( std::string& str )
    {
    	std::vector<uint8_t>	buf(16384);

    	int	ret	= recv( buf.data(), buf.size() );
     	if( ret <= 0 )
    	{
    		throw;
    	}

		str	= std::string( (const char*)buf.data(), ret );
    }

protected:
	int		m_iSock;
};


#endif	// __CTRL_SOCKET_H_INCLUDED__