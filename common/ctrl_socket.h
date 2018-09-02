#ifndef	__CTRL_SOCKET_H_INCLUDED__
#define	__CTRL_SOCKET_H_INCLUDED__


#include <stdio.h>
#include <vector>

#ifdef WIN32
#include <stdint.h>
#include <WinSock2.h>
#pragma comment( lib, "ws2_32.lib" )
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif


class Socket
{
protected:

	class SocketInitializer
	{
	public:
		SocketInitializer()
		{
#ifdef WIN32
			int		retcode;
			WSADATA	tWsaData;

			retcode	=::WSAStartup( MAKEWORD(2,0), &tWsaData );
			if( 0 != retcode )
			{
				throw;
			}
#endif
		}

		~SocketInitializer()
		{
#ifdef WIN32
			::WSACleanup();
#endif
		}
	};

public:
	Socket( int domain=AF_INET, int type=SOCK_STREAM, int protocol=0 )
	{
		static	SocketInitializer	iSockInitializer;

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
#ifdef WIN32
			::closesocket( m_iSock );
#else
			::close( m_iSock );
#endif
			m_iSock	= -1;
		}
	}
	
	
	static	std::string	GetMyIpAddrString()
	{
		std::string		strMyIP	= "127.0.0.1";
		
		int		sock	= ::socket( AF_INET, SOCK_DGRAM, 0 ); 
		if( sock < 0 )
		{
			return	strMyIP;
		}

		struct sockaddr_in	tHost	= {0};
		tHost.sin_family		= AF_INET;
		tHost.sin_port			= htons( 80 );
		tHost.sin_addr.s_addr	= inet_addr("10.255.255.255");
		
		if( 0 != ::connect( sock, (struct sockaddr*)&tHost, sizeof(tHost) ) )
		{
#ifdef WIN32
			::closesocket( sock );
#else
			::close( sock );
#endif
			return	strMyIP;
		}

		struct sockaddr_in	tMyIP		= {0};
		socklen_t			tMyIpLen	= sizeof(tMyIP);
		if( 0 == ::getsockname( sock, (struct sockaddr*)&tMyIP, &tMyIpLen ) )
		{
			char			szBuf[64];
			unsigned char*	MyAddr	= (unsigned char*)&tMyIP.sin_addr.s_addr;
			
			sprintf(
				szBuf,
				"%d.%d.%d.%d",
				MyAddr[0],
				MyAddr[1],
				MyAddr[2],
				MyAddr[3] );

			strMyIP	= szBuf;
		}

#ifdef WIN32
		::closesocket( sock );
#else
		::close( sock );
#endif

		return	strMyIP;
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
		int	ret	= ::send( m_iSock, (const char*)pData, nDataLength, 0 );
		if( ret <= 0 )
		{
			printf( "ERROR: Socket::send(*,%d) ret %d\n", nDataLength, ret );
		}
		
		return	ret;
	}
	
	int	recv( uint8_t* pData, int BufferSize )
	{
#ifdef WIN32
		int	ret	= ::recv( m_iSock, (char*)pData, BufferSize, 0 );
#else
		int	ret	= ::read( m_iSock, pData, BufferSize );
#endif
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
	
	int		SetTimeout( int timeout=1000 )
	{
#ifdef WIN32
		setsockopt( m_iSock, SOL_TCP, TCP_USER_TIMEOUT, (char*)&timeout, sizeof(timeout) );
#else
		struct timeval	to;
		to.tv_sec	= timeout / 1000;
		to.tv_usec	= (timeout % 1000) * 1000;

		if( setsockopt( m_iSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&to, sizeof(to)) < 0 )
		{
			printf("[ERROR] setsockopt(RCV_TIMEO).\r\n");
			return	-1;
		}

		if( setsockopt( m_iSock, SOL_SOCKET, SO_SNDTIMEO, (char *)&to, sizeof(to)) < 0 )
		{
			printf("[ERROR] setsockopt(SND_TIMEO).\r\n");
			return	-1;
		}
#endif

		return	0;
	}

protected:
	int		m_iSock;
};


#endif	// __CTRL_SOCKET_H_INCLUDED__