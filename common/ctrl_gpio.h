#ifndef __CTRL_GPIO_H_INCLUDED__
#define	__CTRL_GPIO_H_INCLUDED__


//	cd /sys/class/gpio

//	echo 200 > /sys/class/gpio/export
//	echo out > /sys/class/gpio/gpio200/direction
//	echo 1 > /sys/class/gpio/gpio200/value
//	echo 0 > /sys/class/gpio/gpio200/value
//	echo 200 > /sys/class/gpio/unexport

#include	<fstream>
#include	<iostream>


class GpioOut
{
public:
    GpioOut(int pin)
    {
    	m_nPin	= pin;

        if( 0 <= m_nPin )
        {
            std::string PinName = std::to_string(m_nPin);
    
            m_strPath	 = "/sys/class/gpio/gpio" + PinName + "/";
            
            std::ofstream( "/sys/class/gpio/export" ) << PinName;
            std::ofstream( m_strPath+"direction" ) << "out";
            std::ofstream( m_strPath+"value" ) << "0";
        }
    }

    ~GpioOut()
    {
        if( 0 <= m_nPin )
        {
            std::ofstream( m_strPath+"value" ) << "0";
            std::ofstream( "/sys/class/gpio/unexport") << std::to_string(m_nPin);
        }
    }

    void    operator << ( int i )
    {
        if( !m_strPath.empty() )
        {
            std::ofstream( m_strPath+"value" ) << std::to_string(i);
        }
    }
    
protected:
	int			m_nPin;
    std::string m_strPath;
};


class GpioIn
{
public:
    GpioIn(int pin)
    {
        std::string PinName = std::to_string(pin);

    	m_nPin		= pin;
        m_strPath	 = "/sys/class/gpio/gpio" + PinName + "/";
        
        std::ofstream( "/sys/class/gpio/export" ) << PinName;
        std::ofstream( m_strPath+"direction" ) << "in";
    }

    ~GpioIn()
    {
        std::ofstream( "/sys/class/gpio/unexport")  << std::to_string(m_nPin);
    }

    void    operator >> ( int& i )
    {
    	std::string	buf;

		std::getline( std::ifstream( m_strPath+"value" ), buf );

		i	= std::stoi(buf );
    }
    
protected:
	int			m_nPin;
    std::string m_strPath;
};

#endif	//__CTRL_GPIO_H_INCLUDED__
