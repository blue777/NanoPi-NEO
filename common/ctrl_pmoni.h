//	g++ -Ofast -std=c++11 ina219.c -o ina219.o -lpthread `freetype-config --cflags` `freetype-config --libs`


#include "ctrl_i2c.h"

class ctrl_PowerMonitor
{
public:
	ctrl_PowerMonitor()
	{
		m_dShuntReg			= 0.002;
		m_dCalibExpected	= 1;
		m_dCalibMeasured	= 1;
	}

	void	SetShuntValue( double shuntreg, double expected = 1, double measured = 1)
	{
		// nomally, expected < measured
		m_dShuntReg			= shuntreg;
		m_dCalibExpected	= expected;
		m_dCalibMeasured	= measured;
	};

	virtual	double	GetA()
	{
		return	ReadShuntRaw() * GetAmpereOf1LSB();
	}

	virtual	double	GetV()
	{
		return	ReadVoltageRaw() * GetVoltageOf1LSB();
	}

	virtual	void	SetSamplingDuration( int msec )=0;
	
	virtual	double	GetShuntOf1LSB()=0;
	virtual	double	GetVoltageOf1LSB()=0;

	double	GetAmpereOf1LSB()
	{
		const double	s_reg	= m_dShuntReg * m_dCalibMeasured / m_dCalibExpected;

		return	GetShuntOf1LSB() / s_reg;
	}

protected:
	virtual	int16_t	ReadShuntRaw()=0;
	virtual	int16_t	ReadVoltageRaw()=0;

	

protected:
	double		m_dShuntReg;
	double		m_dCalibExpected;
	double		m_dCalibMeasured;
};


class PMoni_INA226 : public ctrl_PowerMonitor
{
public:
	PMoni_INA226( const char * dev = "/dev/i2c-0", int slave_addr = 0x44 ) : m_i2c( dev, slave_addr )
	{
		// reg = m_dShuntReg * m_dCalibMeasured / m_dCalibExpected
		m_dShuntReg			= 0.005;
		m_dCalibMeasured	= 0.1855;
		m_dCalibExpected	= 0.1574;
	}

	virtual	void	SetSamplingDuration( int msec )
	{
		int		avg_reg			= 0;
		int		avg_table[]		= { 1, 4, 16, 64, 128, 256, 512, 1024, 99999 };
		
		while( avg_table[avg_reg+1]*2 <= msec )
		{
			avg_reg++;
		}
		printf( "SetSamplingDuration req = %d, Average reg=%d, actual =%d\n", msec, avg_reg, avg_table[avg_reg] );
		
		uint8_t  w_data[3]	= { 0x00, (uint8_t)(0x41 | (avg_reg << 1)), 0x27 };
        m_i2c.write( w_data, sizeof(w_data) );
	}

	virtual	int16_t	ReadShuntRaw()
	{
		uint8_t	w_data[1]	= { 0x01 };
		uint8_t	r_data[2]	= { 0x00, 0x00 };

		// read shunt
		w_data[0]   = 0x01;
		m_i2c.write( w_data, sizeof(w_data) );
		m_i2c.read( r_data, sizeof(r_data) );

		return	(r_data[0] << 8) | r_data[1];
	}
	
	virtual	int16_t	ReadVoltageRaw()
	{
		uint8_t  w_data[1]	= { 0x01 };
		uint8_t  r_data[2]	= { 0x00, 0x00 };

		// read vbus
		w_data[0]   = 0x02;
		m_i2c.write( w_data, sizeof(w_data) );
		m_i2c.read( r_data, sizeof(r_data) );

		return	(r_data[0] << 8) | r_data[1];
	}

	virtual	double	GetShuntOf1LSB()
	{
		return	0.0000025;
	}

	virtual	double	GetVoltageOf1LSB()
	{
		return	0.00125;
	}

protected:
	ctrl_i2c	m_i2c;
};


class PMoni_INA219 : public ctrl_PowerMonitor
{
public:
	PMoni_INA219( const char * dev = "/dev/i2c-0", int slave_addr = 0x45 ) : m_i2c( dev, slave_addr )
	{
		// reg = m_dShuntReg * m_dCalibMeasured / m_dCalibExpected
		m_dShuntReg			= 0.005;
		m_dCalibMeasured	= 1.14;
		m_dCalibExpected	= 1.00;

		uint8_t  w_data[3]	= { 0x00, 0x07, 0xFF };
		if( !m_i2c.write( w_data, sizeof(w_data) ) )
		{
			throw	std::runtime_error("PMoni_INA219 i2c write failed");
		}
	}

	virtual	void	SetSamplingDuration( int msec )
	{
		int		avg_reg			= 0;
		int		avg_table[]		= { 1, 2, 4, 8, 17, 34, 68, 0x7FFFFFFF };
		
		while( avg_table[avg_reg] <= msec )
		{
			avg_reg++;
		}
		printf( "SetSamplingDuration req = %d, Average reg=%d, actual =%d\n", msec, avg_reg, avg_table[avg_reg] );

		uint8_t  w_data[3]	= { 0x00, 0x07, (uint8_t)(0x80 + avg_reg) };
		m_i2c.write( w_data, sizeof(w_data) );
	}

	double	GetShuntOf1LSB()
	{
		return 0.00001;
	}

	double	GetVoltageOf1LSB()
	{
		return	0.004;
	}

	virtual	int16_t	ReadShuntRaw()
	{
		uint8_t  		w_data[1]	= { 0x01 };
		uint8_t  		r_data[2]	= { 0x00, 0x00 };

		// read shunt
		w_data[0]   = 0x01;
		m_i2c.write( w_data, sizeof(w_data) );
		m_i2c.read( r_data, sizeof(r_data) );

		return	(r_data[0] << 8) | r_data[1];
	}

	virtual	int16_t	ReadVoltageRaw()
	{
		uint8_t 		w_data[1]	= { 0x02 };
		uint8_t 		r_data[2]	= { 0x00, 0x00 };

		// read vbus
		m_i2c.write( w_data, sizeof(w_data) );
		m_i2c.read( r_data, sizeof(r_data) );

		return	((r_data[0] << 8) | r_data[1]) >> 3;
	}

protected:
	ctrl_i2c	m_i2c;
};

