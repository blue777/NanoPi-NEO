#ifndef __CTRL_SPI_H_INCLUDED__
#define	__CTRL_SPI_H_INCLUDED__

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>


#define	SPIBASE_MAX_TRANS_SIZE	64
#define	SPIBASE_BIT_PER_WORD	8

class ctrl_spi
{
public:
	ctrl_spi( unsigned int spi_speed = 1000000, unsigned char spi_mode = SPI_MODE_0, const char * dev = "/dev/spidev0.0" )
	{
		unsigned char	spi_bitsPerWord	= SPIBASE_BIT_PER_WORD;
		int     ret;

		m_nSpiSpeed			= spi_speed;

		m_spi    = ::open( dev, O_RDWR );
		if( m_spi < 0 )
		{
			printf("ERROR! ctrl_spi() - open( %s ) ret %d\n", dev, m_spi );
			throw "ERROR: ctrl_spi::ctrl_spi(), open() failed.";
		}

		ret     = ::ioctl( m_spi, SPI_IOC_WR_MODE, &spi_mode );
		if( ret < 0 )
		{
			printf("ERROR! ctrl_spi() - ioctl( SPI_IOC_WR_MODE = %d ) ret %d\n", spi_mode, ret );
			throw "ERROR: ctrl_spi::ctrl_spi(), ioctl(SPI_IOC_WR_MODE) failed.";
		}

		ret     = ::ioctl( m_spi, SPI_IOC_RD_MODE, &spi_mode );
		if( ret < 0 )
		{
			printf("ERROR! ctrl_spi() - ioctl( SPI_IOC_RD_MODE = %d ) ret %d\n", spi_mode, ret );
			throw "ERROR: ctrl_spi::ctrl_spi(), ioctl(SPI_IOC_RD_MODE) failed.";
		}

		ret     = ::ioctl( m_spi, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord );
		if( ret < 0 )
		{
			printf("ERROR! ctrl_spi() - ioctl( SPI_IOC_WR_BITS_PER_WORD = %d ) ret %d\n", spi_bitsPerWord, ret );
			throw "ERROR: ctrl_spi::ctrl_spi(), ioctl(SPI_IOC_WR_BITS_PER_WORD) failed.";
		}

		ret     = ::ioctl( m_spi, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord );
		if( ret < 0 )
		{
			printf("ERROR! ctrl_spi() - ioctl( SPI_IOC_RD_BITS_PER_WORD = %d ) ret %d\n", spi_bitsPerWord, ret );
			throw "ERROR: ctrl_spi::ctrl_spi(), ioctl(SPI_IOC_RD_BITS_PER_WORD) failed.";
		}

		ret     = ::ioctl( m_spi, SPI_IOC_WR_MAX_SPEED_HZ, &m_nSpiSpeed );
		if( ret < 0 )
		{
			printf("ERROR! ctrl_spi() - ioctl( SPI_IOC_WR_MAX_SPEED_HZ = %d ) ret %d\n", m_nSpiSpeed, ret );
			throw "ERROR: ctrl_spi::ctrl_spi(), ioctl(SPI_IOC_WR_MAX_SPEED_HZ) failed.";
		}
		ret     = ::ioctl( m_spi, SPI_IOC_RD_MAX_SPEED_HZ, &m_nSpiSpeed );
		if( ret < 0 )
		{
			printf("ERROR! ctrl_spi() - ioctl( SPI_IOC_RD_MAX_SPEED_HZ = %d ) ret %d\n", m_nSpiSpeed, ret );
			throw "ERROR: ctrl_spi::ctrl_spi(), ioctl(SPI_IOC_RD_MAX_SPEED_HZ) failed.";
		}
		
		printf("Success. ctrl_spi(), Initialized with SPI_IOC_RD_MAX_SPEED_HZ = %d\n", m_nSpiSpeed );
		memset( m_tParam, 0, sizeof(m_tParam) );
	}
	
	~ctrl_spi()
	{
		if( 0 <= m_spi )
		{
			int	ret;
			
			ret	= ::close( m_spi );
			if( ret < 0 )
			{
				printf("ERROR! ~ctrl_spi() - close()) ret %d\n",  ret );
			}
		}
	}
	
	int	write( const uint8_t * data, int size )
	{
		while( 0 < size )
		{
			int		count	= 0;
			int		ret;

			while(	(0 < size) &&
					(count < (sizeof(m_tParam)/sizeof(m_tParam[0])) ))
			{
				int	trans	= size < SPIBASE_MAX_TRANS_SIZE ? size : SPIBASE_MAX_TRANS_SIZE; 
				
				m_tParam[count].tx_buf			= (size_t)data;
				m_tParam[count].rx_buf			= 0;
				m_tParam[count].len				= trans;
				m_tParam[count].speed_hz		= m_nSpiSpeed;
				m_tParam[count].delay_usecs		= 0;
				m_tParam[count].bits_per_word	= SPIBASE_BIT_PER_WORD;
				m_tParam[count].cs_change		= 0;
				
				size	-= trans;
				data	+= trans;
				count++;
			}

			ret = ioctl( m_spi, SPI_IOC_MESSAGE(count), m_tParam );
			if( ret < 0 ) 
			{
				printf("ERROR! write() - ioctl(SPI_IOC_MESSAGE) ret %d\n",  ret );
				return	ret;
			}
		}

		return	0;
	}

	int	read( const uint8_t * cmd, int cmd_len, uint8_t * rcv, int size )
	{
		int		ret;

		m_tParam[0].tx_buf			= (size_t)cmd;
		m_tParam[0].rx_buf			= 0;
		m_tParam[0].len				= cmd_len;
		m_tParam[0].speed_hz		= m_nSpiSpeed;
		m_tParam[0].delay_usecs		= 0;
		m_tParam[0].bits_per_word	= SPIBASE_BIT_PER_WORD;
		m_tParam[0].cs_change		= 0;
		
		m_tParam[1].tx_buf			= 0;
		m_tParam[1].rx_buf			= (size_t)rcv;
		m_tParam[1].len				= size;
		m_tParam[1].speed_hz		= m_nSpiSpeed;
		m_tParam[1].delay_usecs		= 0;
		m_tParam[1].bits_per_word	= SPIBASE_BIT_PER_WORD;
		m_tParam[1].cs_change		= 0;
		
		ret = ioctl( m_spi, SPI_IOC_MESSAGE(2), m_tParam );
		if( ret < 0 ) 
		{
			printf("ERROR! read() - ioctl(SPI_IOC_MESSAGE) ret %d\n",  ret );
			return	ret;
		}

		return	0;
	}

    void    operator << ( uint8_t i )
    {
    	write( &i, 1 );
    }
	
	template<size_t size>
	void	operator << ( const uint8_t (&data)[size] )
	{
		write( data, size );
	}

protected:
	int 						m_spi;
	unsigned int				m_nSpiSpeed;
	struct spi_ioc_transfer		m_tParam[64];
};


#endif	// __CTRL_SPI_H_INCLUDED__