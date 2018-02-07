
#include <vector>
#include <mutex>
#include <thread>
#include "common/string_util.h"

class CpuUsages
{
public:
	class CpuUsage
	{
	public:
		CpuUsage()
		{
			std::string	strVmStat	= StringUtil::GetTextFromExecResult( "vmstat 1 2" );
			std::string	strCpuFreq	= StringUtil::GetTextFromExecResult( "cpu_freq" );

			std::vector<std::string>	lines		= StringUtil::Split( strCpuFreq );
			int							cpu_count	= 0;

			m_nUsage	= 0;
			m_fTemp		= 0;
			m_fFreqM	= 0;	
			
			for( auto cpu : lines )
			{
				std::vector<std::string>	column	= StringUtil::Split( cpu, ' ', 4 );

				for( auto it : column )
				{
					if( 0 == it.find( "temp=" ) )
					{
						m_fTemp	+= std::stoi( it.substr(5) );
					}
					else if( 0 == it.find( "cur_freq=" ) )
					{
						m_fFreqM	+= std::stoi( it.substr(9) );
						cpu_count++;
					}
				}
			}
			
			std::vector<std::string>	fields		= StringUtil::SplitByWhites( StringUtil::Split( strVmStat )[3], 16 );

			m_nUsage	= 100 - std::stoi( fields[14] );
			m_fTemp		/= cpu_count * 1000;
			m_fFreqM	/= cpu_count * 1000;
		}

	public:		
		int		m_nUsage;
		float	m_fTemp;
		float	m_fFreqM;
	};

	CpuUsages( int nMaxHistoryCount=600 )
	{
		m_nMaxHistoryCount	= nMaxHistoryCount;
		m_isExecute			= true;
		m_iThread			= std::thread( ThreadLoop, this );
	}
	
	~CpuUsages()
	{
		m_isExecute	= false;
		m_iThread.join();
	}

	std::vector<CpuUsage>	GetUsages()
	{
		std::vector<CpuUsage>	iUsages;

		m_iMutex.lock();
		iUsages	= m_iUsages;
		m_iMutex.unlock();		
		
		return	iUsages;
	}

protected:
	void	Loop()
	{
		while( m_isExecute )
		{
			CpuUsage	iUsage;

			m_iMutex.lock();
			m_iUsages.push_back( iUsage );
			while( m_nMaxHistoryCount < m_iUsages.size() )
			{
				m_iUsages.erase( m_iUsages.begin() );
			}
			m_iMutex.unlock();
		}		
	}

	static	void	ThreadLoop( CpuUsages* piThis )
	{
		piThis->Loop();
	}
	
protected:
	bool					m_isExecute;
	std::thread				m_iThread;
	std::mutex				m_iMutex;
	int						m_nMaxHistoryCount;
	std::vector<CpuUsage>	m_iUsages;
};