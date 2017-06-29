
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


class i2c_base
{
public:
    i2c_base( const char * dev, int addr )
    {
        int     ret;

        m_i2c    = ::open( dev, O_RDWR );
        if( m_i2c < 0 )
        {
            printf("ERROR! i2c_base() open( %s ) ret %d\n", dev, m_i2c );
            throw;
        }

        ret     = ::ioctl( m_i2c, I2C_SLAVE, addr );
        if( ret < 0 )
        {
            printf("ERROR! i2c_base() ioctl ret %d\n", ret );
            throw;
        }
    }

    bool    write( const unsigned char * data, int size )
    {
        int wrote   = ::write( m_i2c, data, size );
        
        if( size != wrote )
        {
            printf("ERROR! i2c_base.write( *, %d ) ret %d\n", size, wrote );
            return  false;
        }
        
//        printf("Success. i2c_base.write( *, %d ) ret %d\n", size, wrote );
        return  true;
    }

private:
    int     m_i2c;
};