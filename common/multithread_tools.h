

#include <thread>
#include <vector>

typedef unsigned long	( * THREAD_FUNCTION)( void * param );

class CMultiThreadTools
{
public:
	static	unsigned int	GetProcessorCount()
	{
		return	std::thread::hardware_concurrency();
	}

	static	void	DoExecute(
						int		nThreadCount,
						void *	pParam,
						THREAD_FUNCTION	pFunc )
	{
		if( nThreadCount == 0 )
		{
			nThreadCount	= GetProcessorCount();
		}

		if( 1 < nThreadCount )
		{
#ifndef _DEBUG
			std::vector<std::thread>	iThreads(nThreadCount-1);
			
			for( auto& thread : iThreads )
			{
				thread	= std::thread(pFunc,pParam);
			}

			pFunc(pParam);

			for( auto& thread : iThreads )
			{
				thread.join();
			}
#else
			for( int i = 0; i < nThreadCount; i++ )
			{
				pFunc( pParam );
			}
#endif
		}
		else
		{
			pFunc( pParam );
		}
	}

	template<typename Func>
	static	void	DoExecute( int nThreadCount, Func& func)
	{
		DoExecute(
			nThreadCount,
			(void*)&func,
			[](void* param)->unsigned long
			{
				(*(Func*)param)();
				return	0;
			} );
	};
};
