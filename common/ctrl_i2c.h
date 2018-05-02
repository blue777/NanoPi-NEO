#ifndef __CTRL_I2C_H_INCLUDED__
#define __CTRL_I2C_H_INCLUDED__


#include <stdio.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


class ctrl_i2c
{
public:
    ctrl_i2c( const char * dev, int addr )
    {
        int     ret;

        m_i2c    = ::open( dev, O_RDWR );
        if( m_i2c < 0 )
        {
            printf("ERROR! ctrl_i2c() open( %s ) ret %d\n", dev, m_i2c );
            throw "ERROR: ctrl_i2c(), open() failed.";
        }

        ret     = ::ioctl( m_i2c, I2C_SLAVE, addr );
        if( ret < 0 )
        {
            printf("ERROR! ctrl_i2c() ioctl ret %d\n", ret );
            throw "ERROR: ctrl_i2c(), ioctl(I2C_SLAVE) failed.";
        }
    }

    bool    write( const unsigned char * data, int size )
    {
        int wrote   = ::write( m_i2c, data, size );
        
        if( size != wrote )
        {
            printf("ERROR! ctrl_i2c.write( *, %d ) ret %d\n", size, wrote );
            return  false;
        }
        
//      printf("Success. i2c_base.write( *, %d ) ret %d\n", size, wrote );
        return  true;
    }

    bool    read( unsigned char * data, int size )
    {
        int read   = ::read( m_i2c, data, size );
        
        if( size != read )
        { 
            printf("ERROR! ctrl_i2c.read( *, %d ) ret %d\n", size, read );
            return  false;
        }

        return  true;    
    }

private:
    int     m_i2c;
};

#endif
