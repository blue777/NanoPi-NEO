#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

int main( int argc, char * argv[] )
{
	int				fd;
	int				num			= 4;
	int				block_size	= 16;
	void*			base_addr	= 0;
	void*			base_addr2map;
	void*			map_addr;
	void*			map_addr_target;
	unsigned int	value;
	int				mode;
	
	if( argc < 3 )
	{
		printf( "reg r32 <StartAddress> <num=16>\n");
		printf( "reg w32 <Address> <data>\n");
		return  -1;
	}

	sscanf( argv[2], "%p", &base_addr );
	
	if( 0 == strcmp( argv[1], "r32" ) )
	{
		mode		= 'R';
		num			= 16;

		if( 3 < argc )
		{
			sscanf( argv[3], "%d", &num );
		}
		block_size	= num * 4;
	}
	else if( 0 == strcmp( argv[1], "w32") )
	{
		mode		= 'W';
		block_size	= 4;
		
		if( argc < 4 )
		{
			printf( "reg w32 <Address> <data>\n");
			return  -1;
		}
		
		sscanf( argv[3], "0x%x", &value );
	}

	fd	= open( "/dev/mem", O_RDWR | O_SYNC );
	if( fd == -1 )
	{
		printf( "ERROR! Cannot open /dev/mem\n" );
		return	-1;
	}


	base_addr2map	= (void*)(((size_t)base_addr) >> 12 << 12);
	block_size		+= (size_t)base_addr - (size_t)base_addr2map;

	map_addr	= mmap( NULL, block_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)base_addr2map );
	if( map_addr == MAP_FAILED )
	{
		printf("ERROR! Cannot map physical addr to virtual addr. %p\n", base_addr );
		return	-1;
	}

	map_addr_target	= (void*)((size_t)map_addr + (size_t)base_addr - (size_t)base_addr2map);

	close( fd );
	
	switch( mode )
	{
	case 'R':
		{
			uint32_t*	addr	= (uint32_t*)map_addr_target;
			int			i;
			
			for( i = 0; i < num;i++ )
			{
				printf( "%p : 0x%08x\r\n", &((uint32_t*)base_addr)[i], addr[i] );
			}
		}
		break;
		
	case 'W':
		{
			uint32_t*	addr	= (uint32_t*)map_addr_target;
			uint32_t	org		= addr[0];

			addr[0]	= value;
			
			msync( map_addr_target, 4, MS_SYNC );
			printf( "%p : 0x%08x --> w 0x%08x --> r 0x%08x\r\n", base_addr, org, value, addr[0] );
		}
		break;
	}

	munmap( map_addr, block_size );

	return	0;
}