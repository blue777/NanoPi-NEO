

//#define ENABLE_SSE2
//#define ENABLE_AVX
//#define ENABLE_AVX_FMA
//#define ENABLE_AVX2_FMA
#define ENABLE_NEON



#include <vector>
#include <stdint.h>
#include <string.h>
#include <chrono>
#include <atomic>
#include <iostream>
#include "common/multithread_tools.h"


#define	M44(base,row,col)	base[col*4+row]



template<class T>
void	MultMatrix_ForC( T result[16], const T base[16], const T mult[16] )
{
	for( int r = 0; r < 4; r++ )
	{
		for( int c = 0; c < 4; c++ )
		{
			T	temp	= 0;

			for( int i = 0; i < 4; i++ )
			{
				temp	+= M44(base,r,i) * M44(mult,i,c);
			}

			M44(result,r,c)	= temp;
		}
	}
}

template<class T>
void	MultMatrix_ForR( T result[16], const T base[16], const T mult[16] )
{
	for( int c = 0; c < 4; c++ )
	{
		for( int r = 0; r < 4; r++ )
		{
			T	temp	= 0;

			for( int i = 0; i < 4; i++ )
			{
				temp	+= M44(base,r,i) * M44(mult,i,c);
			}

			M44(result,r,c)	= temp;
		}
	}
}

template<class T>
void	MultMatrix_ExpC( T result[16], const T base[16], const T mult[16] )
{
	M44(result,0,0)	= M44(base,0,0) * M44(mult,0,0) + M44(base,0,1) * M44(mult,1,0) + M44(base,0,2) * M44(mult,2,0) + M44(base,0,3) * M44(mult,3,0);
	M44(result,1,0)	= M44(base,1,0) * M44(mult,0,0) + M44(base,1,1) * M44(mult,1,0) + M44(base,1,2) * M44(mult,2,0) + M44(base,1,3) * M44(mult,3,0);
	M44(result,2,0)	= M44(base,2,0) * M44(mult,0,0) + M44(base,2,1) * M44(mult,1,0) + M44(base,2,2) * M44(mult,2,0) + M44(base,2,3) * M44(mult,3,0);
	M44(result,3,0)	= M44(base,3,0) * M44(mult,0,0) + M44(base,3,1) * M44(mult,1,0) + M44(base,3,2) * M44(mult,2,0) + M44(base,3,3) * M44(mult,3,0);

	M44(result,0,1)	= M44(base,0,0) * M44(mult,0,1) + M44(base,0,1) * M44(mult,1,1) + M44(base,0,2) * M44(mult,2,1) + M44(base,0,3) * M44(mult,3,1);
	M44(result,1,1)	= M44(base,1,0) * M44(mult,0,1) + M44(base,1,1) * M44(mult,1,1) + M44(base,1,2) * M44(mult,2,1) + M44(base,1,3) * M44(mult,3,1);
	M44(result,2,1)	= M44(base,2,0) * M44(mult,0,1) + M44(base,2,1) * M44(mult,1,1) + M44(base,2,2) * M44(mult,2,1) + M44(base,2,3) * M44(mult,3,1);
	M44(result,3,1)	= M44(base,3,0) * M44(mult,0,1) + M44(base,3,1) * M44(mult,1,1) + M44(base,3,2) * M44(mult,2,1) + M44(base,3,3) * M44(mult,3,1);

	M44(result,0,2)	= M44(base,0,0) * M44(mult,0,2) + M44(base,0,1) * M44(mult,1,2) + M44(base,0,2) * M44(mult,2,2) + M44(base,0,3) * M44(mult,3,2);
	M44(result,1,2)	= M44(base,1,0) * M44(mult,0,2) + M44(base,1,1) * M44(mult,1,2) + M44(base,1,2) * M44(mult,2,2) + M44(base,1,3) * M44(mult,3,2);
	M44(result,2,2)	= M44(base,2,0) * M44(mult,0,2) + M44(base,2,1) * M44(mult,1,2) + M44(base,2,2) * M44(mult,2,2) + M44(base,2,3) * M44(mult,3,2);
	M44(result,3,2)	= M44(base,3,0) * M44(mult,0,2) + M44(base,3,1) * M44(mult,1,2) + M44(base,3,2) * M44(mult,2,2) + M44(base,3,3) * M44(mult,3,2);

	M44(result,0,3)	= M44(base,0,0) * M44(mult,0,3) + M44(base,0,1) * M44(mult,1,3) + M44(base,0,2) * M44(mult,2,3) + M44(base,0,3) * M44(mult,3,3);
	M44(result,1,3)	= M44(base,1,0) * M44(mult,0,3) + M44(base,1,1) * M44(mult,1,3) + M44(base,1,2) * M44(mult,2,3) + M44(base,1,3) * M44(mult,3,3);
	M44(result,2,3)	= M44(base,2,0) * M44(mult,0,3) + M44(base,2,1) * M44(mult,1,3) + M44(base,2,2) * M44(mult,2,3) + M44(base,2,3) * M44(mult,3,3);
	M44(result,3,3)	= M44(base,3,0) * M44(mult,0,3) + M44(base,3,1) * M44(mult,1,3) + M44(base,3,2) * M44(mult,2,3) + M44(base,3,3) * M44(mult,3,3);
}

template<class T>
void	MultMatrix_ExpR( T result[16], const T base[16], const T mult[16] )
{
	M44(result,0,0)	= M44(base,0,0) * M44(mult,0,0) + M44(base,0,1) * M44(mult,1,0) + M44(base,0,2) * M44(mult,2,0) + M44(base,0,3) * M44(mult,3,0);
	M44(result,0,1)	= M44(base,0,0) * M44(mult,0,1) + M44(base,0,1) * M44(mult,1,1) + M44(base,0,2) * M44(mult,2,1) + M44(base,0,3) * M44(mult,3,1);
	M44(result,0,2)	= M44(base,0,0) * M44(mult,0,2) + M44(base,0,1) * M44(mult,1,2) + M44(base,0,2) * M44(mult,2,2) + M44(base,0,3) * M44(mult,3,2);
	M44(result,0,3)	= M44(base,0,0) * M44(mult,0,3) + M44(base,0,1) * M44(mult,1,3) + M44(base,0,2) * M44(mult,2,3) + M44(base,0,3) * M44(mult,3,3);

	M44(result,1,0)	= M44(base,1,0) * M44(mult,0,0) + M44(base,1,1) * M44(mult,1,0) + M44(base,1,2) * M44(mult,2,0) + M44(base,1,3) * M44(mult,3,0);
	M44(result,1,1)	= M44(base,1,0) * M44(mult,0,1) + M44(base,1,1) * M44(mult,1,1) + M44(base,1,2) * M44(mult,2,1) + M44(base,1,3) * M44(mult,3,1);
	M44(result,1,2)	= M44(base,1,0) * M44(mult,0,2) + M44(base,1,1) * M44(mult,1,2) + M44(base,1,2) * M44(mult,2,2) + M44(base,1,3) * M44(mult,3,2);
	M44(result,1,3)	= M44(base,1,0) * M44(mult,0,3) + M44(base,1,1) * M44(mult,1,3) + M44(base,1,2) * M44(mult,2,3) + M44(base,1,3) * M44(mult,3,3);

	M44(result,2,0)	= M44(base,2,0) * M44(mult,0,0) + M44(base,2,1) * M44(mult,1,0) + M44(base,2,2) * M44(mult,2,0) + M44(base,2,3) * M44(mult,3,0);
	M44(result,2,1)	= M44(base,2,0) * M44(mult,0,1) + M44(base,2,1) * M44(mult,1,1) + M44(base,2,2) * M44(mult,2,1) + M44(base,2,3) * M44(mult,3,1);
	M44(result,2,2)	= M44(base,2,0) * M44(mult,0,2) + M44(base,2,1) * M44(mult,1,2) + M44(base,2,2) * M44(mult,2,2) + M44(base,2,3) * M44(mult,3,2);
	M44(result,2,3)	= M44(base,2,0) * M44(mult,0,3) + M44(base,2,1) * M44(mult,1,3) + M44(base,2,2) * M44(mult,2,3) + M44(base,2,3) * M44(mult,3,3);

	M44(result,3,0)	= M44(base,3,0) * M44(mult,0,0) + M44(base,3,1) * M44(mult,1,0) + M44(base,3,2) * M44(mult,2,0) + M44(base,3,3) * M44(mult,3,0);
	M44(result,3,1)	= M44(base,3,0) * M44(mult,0,1) + M44(base,3,1) * M44(mult,1,1) + M44(base,3,2) * M44(mult,2,1) + M44(base,3,3) * M44(mult,3,1);
	M44(result,3,2)	= M44(base,3,0) * M44(mult,0,2) + M44(base,3,1) * M44(mult,1,2) + M44(base,3,2) * M44(mult,2,2) + M44(base,3,3) * M44(mult,3,2);
	M44(result,3,3)	= M44(base,3,0) * M44(mult,0,3) + M44(base,3,1) * M44(mult,1,3) + M44(base,3,2) * M44(mult,2,3) + M44(base,3,3) * M44(mult,3,3);
}


#ifdef ENABLE_NEON
#include <arm_neon.h>
void	MultMatrixF_NEON( float result[16], const float base[16], const float mult[16] )
{
	float32x4_t	c0, c1, c2, c3;
	float32x4_t	r0, r1, r2, r3;

	c0	= vld1q_f32( &mult[0] );
	c1	= vld1q_f32( &mult[4] );
	c2	= vld1q_f32( &mult[8] );
	c3	= vld1q_f32( &mult[12] );

	// column 0
	r0	= vmulq_n_f32( c0, base[0] ); 
	r1	= vmulq_n_f32( c1, base[1] ); 
	r2	= vmulq_n_f32( c2, base[2] ); 
	r3	= vmulq_n_f32( c3, base[3] ); 
	
	r0	= vaddq_f32( r0, r1 );
	r2	= vaddq_f32( r2, r3 );
	r0	= vaddq_f32( r0, r2 );

	vst1q_f32( &result[0], r0 );

	// column 1
	r0	= vmulq_n_f32( c0, base[4] ); 
	r1	= vmulq_n_f32( c1, base[5] ); 
	r2	= vmulq_n_f32( c2, base[6] ); 
	r3	= vmulq_n_f32( c3, base[7] ); 
	
	r0	= vaddq_f32( r0, r1 );
	r2	= vaddq_f32( r2, r3 );
	r0	= vaddq_f32( r0, r2 );

	vst1q_f32( &result[4], r0 );

	// column 2
	r0	= vmulq_n_f32( c0, base[8] ); 
	r1	= vmulq_n_f32( c1, base[9] ); 
	r2	= vmulq_n_f32( c2, base[10] ); 
	r3	= vmulq_n_f32( c3, base[11] ); 
	
	r0	= vaddq_f32( r0, r1 );
	r2	= vaddq_f32( r2, r3 );
	r0	= vaddq_f32( r0, r2 );

	vst1q_f32( &result[8], r0 );

	// column 3
	r0	= vmulq_n_f32( c0, base[12] ); 
	r1	= vmulq_n_f32( c1, base[13] ); 
	r2	= vmulq_n_f32( c2, base[14] ); 
	r3	= vmulq_n_f32( c3, base[15] ); 
	
	r0	= vaddq_f32( r0, r1 );
	r2	= vaddq_f32( r2, r3 );
	r0	= vaddq_f32( r0, r2 );

	vst1q_f32( &result[12], r0 );
}

void	MultMatrixF_NEON_type2( float result[16], const float base[16], const float mult[16] )
{
	float32x4_t	c0, c1, c2, c3;
	float32x4_t	r0, r1, r2, r3;

	c0	= vld1q_f32( &mult[0] );
	c1	= vld1q_f32( &mult[4] );
	c2	= vld1q_f32( &mult[8] );
	c3	= vld1q_f32( &mult[12] );

	// column 0
	r0	= vmulq_n_f32(     c0, base[0] ); 
	r0	= vmlaq_n_f32( r0, c1, base[1] );
	r0	= vmlaq_n_f32( r0, c2, base[2] );
	r0	= vmlaq_n_f32( r0, c3, base[3] );

	vst1q_f32( &result[0], r0 );

	// column 1
	r0	= vmulq_n_f32(     c0, base[4] ); 
	r0	= vmlaq_n_f32( r0, c1, base[5] );
	r0	= vmlaq_n_f32( r0, c2, base[6] );
	r0	= vmlaq_n_f32( r0, c3, base[7] );

	vst1q_f32( &result[4], r0 );

	// column 2
	r0	= vmulq_n_f32(     c0, base[8] ); 
	r0	= vmlaq_n_f32( r0, c1, base[9] );
	r0	= vmlaq_n_f32( r0, c2, base[10] );
	r0	= vmlaq_n_f32( r0, c3, base[11] );

	vst1q_f32( &result[8], r0 );

	// column 3
	r0	= vmulq_n_f32(     c0, base[12] ); 
	r0	= vmlaq_n_f32( r0, c1, base[13] );
	r0	= vmlaq_n_f32( r0, c2, base[14] );
	r0	= vmlaq_n_f32( r0, c3, base[15] );

	vst1q_f32( &result[12], r0 );
}

void	CopyMatrixD_NEON( double dst[16], const double src[16] )
{
	uint64_t*		d	= (uint64_t*)dst;
	const uint64_t*	s	= (const uint64_t*)src;
	
	vst1q_u64( &d[ 0], vld1q_u64( &s[ 0] ) );
	vst1q_u64( &d[ 2], vld1q_u64( &s[ 2] ) );
	vst1q_u64( &d[ 4], vld1q_u64( &s[ 4] ) );
	vst1q_u64( &d[ 6], vld1q_u64( &s[ 6] ) );
	vst1q_u64( &d[ 8], vld1q_u64( &s[ 8] ) );
	vst1q_u64( &d[10], vld1q_u64( &s[10] ) );
	vst1q_u64( &d[12], vld1q_u64( &s[12] ) );
	vst1q_u64( &d[14], vld1q_u64( &s[14] ) );
}

#endif

#ifdef ENABLE_SSE2
#include <intrin.h>
void	MultMatrixF_SSE2( float result[16], const float base[16], const float mult[16] )
{
	__m128	xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;

	xmm4	= _mm_loadu_ps( &mult[0] );
	xmm5	= _mm_loadu_ps( &mult[4] );
	xmm6	= _mm_loadu_ps( &mult[8] );
	xmm7	= _mm_loadu_ps( &mult[12] );

	// column0
	xmm0	= _mm_load1_ps( &base[0] );
	xmm1	= _mm_load1_ps( &base[1] );
	xmm2	= _mm_load1_ps( &base[2] );
	xmm3	= _mm_load1_ps( &base[3] );

	xmm0	= _mm_mul_ps( xmm0, xmm4 );
	xmm1	= _mm_mul_ps( xmm1, xmm5 );
	xmm2	= _mm_mul_ps( xmm2, xmm6 );
	xmm3	= _mm_mul_ps( xmm3, xmm7 );

	xmm0	= _mm_add_ps( xmm0, xmm1 );
	xmm2	= _mm_add_ps( xmm2, xmm3 );
	xmm0	= _mm_add_ps( xmm0, xmm2 );

	_mm_storeu_ps( &result[0], xmm0 );

	// column1
	xmm0	= _mm_load1_ps( &base[4] );
	xmm1	= _mm_load1_ps( &base[5] );
	xmm2	= _mm_load1_ps( &base[6] );
	xmm3	= _mm_load1_ps( &base[7] );

	xmm0	= _mm_mul_ps( xmm0, xmm4 );
	xmm1	= _mm_mul_ps( xmm1, xmm5 );
	xmm2	= _mm_mul_ps( xmm2, xmm6 );
	xmm3	= _mm_mul_ps( xmm3, xmm7 );

	xmm0	= _mm_add_ps( xmm0, xmm1 );
	xmm2	= _mm_add_ps( xmm2, xmm3 );
	xmm0	= _mm_add_ps( xmm0, xmm2 );

	_mm_storeu_ps( &result[4], xmm0 );

	// column2
	xmm0	= _mm_load1_ps( &base[8] );
	xmm1	= _mm_load1_ps( &base[9] );
	xmm2	= _mm_load1_ps( &base[10] );
	xmm3	= _mm_load1_ps( &base[11] );

	xmm0	= _mm_mul_ps( xmm0, xmm4 );
	xmm1	= _mm_mul_ps( xmm1, xmm5 );
	xmm2	= _mm_mul_ps( xmm2, xmm6 );
	xmm3	= _mm_mul_ps( xmm3, xmm7 );

	xmm0	= _mm_add_ps( xmm0, xmm1 );
	xmm2	= _mm_add_ps( xmm2, xmm3 );
	xmm0	= _mm_add_ps( xmm0, xmm2 );

	_mm_storeu_ps( &result[8], xmm0 );

	// column3
	xmm0	= _mm_load1_ps( &base[12] );
	xmm1	= _mm_load1_ps( &base[13] );
	xmm2	= _mm_load1_ps( &base[14] );
	xmm3	= _mm_load1_ps( &base[15] );

	xmm0	= _mm_mul_ps( xmm0, xmm4 );
	xmm1	= _mm_mul_ps( xmm1, xmm5 );
	xmm2	= _mm_mul_ps( xmm2, xmm6 );
	xmm3	= _mm_mul_ps( xmm3, xmm7 );

	xmm0	= _mm_add_ps( xmm0, xmm1 );
	xmm2	= _mm_add_ps( xmm2, xmm3 );
	xmm0	= _mm_add_ps( xmm0, xmm2 );

	_mm_storeu_ps( &result[12], xmm0 );
}

void	MultMatrixD_SSE2( double result[16], const double base[16], const double mult[16] )
{
	__m128d	xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	__m128d	xmm08, xmm09, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;


	xmm08	= _mm_loadu_pd( &mult[0] );
	xmm09	= _mm_loadu_pd( &mult[2] );
	xmm10	= _mm_loadu_pd( &mult[4] );
	xmm11	= _mm_loadu_pd( &mult[6] );
	xmm12	= _mm_loadu_pd( &mult[8] );
	xmm13	= _mm_loadu_pd( &mult[10] );
	xmm14	= _mm_loadu_pd( &mult[12] );
	xmm15	= _mm_loadu_pd( &mult[14] );

	// column0
	xmm0	= _mm_load1_pd( &base[0] );
	xmm2	= _mm_load1_pd( &base[1] );
	xmm4	= _mm_load1_pd( &base[2] );
	xmm6	= _mm_load1_pd( &base[3] );

	xmm1	= _mm_mul_pd( xmm0, xmm09 );
	xmm3	= _mm_mul_pd( xmm2, xmm11 );
	xmm5	= _mm_mul_pd( xmm4, xmm13 );
	xmm7	= _mm_mul_pd( xmm6, xmm15 );
	xmm0	= _mm_mul_pd( xmm0, xmm08 );
	xmm2	= _mm_mul_pd( xmm2, xmm10 );
	xmm4	= _mm_mul_pd( xmm4, xmm12 );
	xmm6	= _mm_mul_pd( xmm6, xmm14 );

	xmm0	= _mm_add_pd( xmm0, xmm4 );
	xmm1	= _mm_add_pd( xmm1, xmm5 );
	xmm2	= _mm_add_pd( xmm2, xmm6 );
	xmm3	= _mm_add_pd( xmm3, xmm7 );
	xmm0	= _mm_add_pd( xmm0, xmm2 );
	xmm1	= _mm_add_pd( xmm1, xmm3 );

	_mm_storeu_pd( &result[0], xmm0 );
	_mm_storeu_pd( &result[2], xmm1 );


	// column1
	xmm0	= _mm_load1_pd( &base[4] );
	xmm2	= _mm_load1_pd( &base[5] );
	xmm4	= _mm_load1_pd( &base[6] );
	xmm6	= _mm_load1_pd( &base[7] );

	xmm1	= _mm_mul_pd( xmm0, xmm09 );
	xmm3	= _mm_mul_pd( xmm2, xmm11 );
	xmm5	= _mm_mul_pd( xmm4, xmm13 );
	xmm7	= _mm_mul_pd( xmm6, xmm15 );
	xmm0	= _mm_mul_pd( xmm0, xmm08 );
	xmm2	= _mm_mul_pd( xmm2, xmm10 );
	xmm4	= _mm_mul_pd( xmm4, xmm12 );
	xmm6	= _mm_mul_pd( xmm6, xmm14 );

	xmm0	= _mm_add_pd( xmm0, xmm4 );
	xmm1	= _mm_add_pd( xmm1, xmm5 );
	xmm2	= _mm_add_pd( xmm2, xmm6 );
	xmm3	= _mm_add_pd( xmm3, xmm7 );
	xmm0	= _mm_add_pd( xmm0, xmm2 );
	xmm1	= _mm_add_pd( xmm1, xmm3 );

	_mm_storeu_pd( &result[4], xmm0 );
	_mm_storeu_pd( &result[6], xmm1 );

	// column2
	xmm0	= _mm_load1_pd( &base[8] );
	xmm2	= _mm_load1_pd( &base[9] );
	xmm4	= _mm_load1_pd( &base[10] );
	xmm6	= _mm_load1_pd( &base[11] );

	xmm1	= _mm_mul_pd( xmm0, xmm09 );
	xmm3	= _mm_mul_pd( xmm2, xmm11 );
	xmm5	= _mm_mul_pd( xmm4, xmm13 );
	xmm7	= _mm_mul_pd( xmm6, xmm15 );
	xmm0	= _mm_mul_pd( xmm0, xmm08 );
	xmm2	= _mm_mul_pd( xmm2, xmm10 );
	xmm4	= _mm_mul_pd( xmm4, xmm12 );
	xmm6	= _mm_mul_pd( xmm6, xmm14 );

	xmm0	= _mm_add_pd( xmm0, xmm4 );
	xmm1	= _mm_add_pd( xmm1, xmm5 );
	xmm2	= _mm_add_pd( xmm2, xmm6 );
	xmm3	= _mm_add_pd( xmm3, xmm7 );
	xmm0	= _mm_add_pd( xmm0, xmm2 );
	xmm1	= _mm_add_pd( xmm1, xmm3 );

	_mm_storeu_pd( &result[8], xmm0 );
	_mm_storeu_pd( &result[10], xmm1 );

	// column3
	xmm0	= _mm_load1_pd( &base[12] );
	xmm2	= _mm_load1_pd( &base[13] );
	xmm4	= _mm_load1_pd( &base[14] );
	xmm6	= _mm_load1_pd( &base[15] );

	xmm1	= _mm_mul_pd( xmm0, xmm09 );
	xmm3	= _mm_mul_pd( xmm2, xmm11 );
	xmm5	= _mm_mul_pd( xmm4, xmm13 );
	xmm7	= _mm_mul_pd( xmm6, xmm15 );
	xmm0	= _mm_mul_pd( xmm0, xmm08 );
	xmm2	= _mm_mul_pd( xmm2, xmm10 );
	xmm4	= _mm_mul_pd( xmm4, xmm12 );
	xmm6	= _mm_mul_pd( xmm6, xmm14 );

	xmm0	= _mm_add_pd( xmm0, xmm4 );
	xmm1	= _mm_add_pd( xmm1, xmm5 );
	xmm2	= _mm_add_pd( xmm2, xmm6 );
	xmm3	= _mm_add_pd( xmm3, xmm7 );
	xmm0	= _mm_add_pd( xmm0, xmm2 );
	xmm1	= _mm_add_pd( xmm1, xmm3 );

	_mm_storeu_pd( &result[12], xmm0 );
	_mm_storeu_pd( &result[14], xmm1 );
}
#endif


#ifdef ENABLE_AVX
#include <intrin.h>

void	MultMatrixD_AVX( double result[16], const double base[16], const double mult[16] )
{
	__m256d	ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7;

	ymm4	= _mm256_loadu_pd( &mult[0] );
	ymm5	= _mm256_loadu_pd( &mult[4] );
	ymm6	= _mm256_loadu_pd( &mult[8] );
	ymm7	= _mm256_loadu_pd( &mult[12] );

	// column0
	ymm0	= _mm256_broadcast_sd( &base[0] );
	ymm1	= _mm256_broadcast_sd( &base[1] );
	ymm2	= _mm256_broadcast_sd( &base[2] );
	ymm3	= _mm256_broadcast_sd( &base[3] );
	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm1	= _mm256_mul_pd( ymm1, ymm5 );
	ymm2	= _mm256_mul_pd( ymm2, ymm6 );
	ymm3	= _mm256_mul_pd( ymm3, ymm7 );

	ymm0	= _mm256_add_pd( ymm0, ymm1 );
	ymm2	= _mm256_add_pd( ymm2, ymm3 );
	ymm0	= _mm256_add_pd( ymm0, ymm2 );
	_mm256_storeu_pd( &result[0], ymm0 );

	// column1
	ymm0	= _mm256_broadcast_sd( &base[4] );
	ymm1	= _mm256_broadcast_sd( &base[5] );
	ymm2	= _mm256_broadcast_sd( &base[6] );
	ymm3	= _mm256_broadcast_sd( &base[7] );
	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm1	= _mm256_mul_pd( ymm1, ymm5 );
	ymm2	= _mm256_mul_pd( ymm2, ymm6 );
	ymm3	= _mm256_mul_pd( ymm3, ymm7 );

	ymm0	= _mm256_add_pd( ymm0, ymm1 );
	ymm2	= _mm256_add_pd( ymm2, ymm3 );
	ymm0	= _mm256_add_pd( ymm0, ymm2 );
	_mm256_storeu_pd( &result[4], ymm0 );

	// column2
	ymm0	= _mm256_broadcast_sd( &base[8] );
	ymm1	= _mm256_broadcast_sd( &base[9] );
	ymm2	= _mm256_broadcast_sd( &base[10] );
	ymm3	= _mm256_broadcast_sd( &base[11] );
	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm1	= _mm256_mul_pd( ymm1, ymm5 );
	ymm2	= _mm256_mul_pd( ymm2, ymm6 );
	ymm3	= _mm256_mul_pd( ymm3, ymm7 );

	ymm0	= _mm256_add_pd( ymm0, ymm1 );
	ymm2	= _mm256_add_pd( ymm2, ymm3 );
	ymm0	= _mm256_add_pd( ymm0, ymm2 );
	_mm256_storeu_pd( &result[8], ymm0 );

	// column3
	ymm0	= _mm256_broadcast_sd( &base[12] );
	ymm1	= _mm256_broadcast_sd( &base[13] );
	ymm2	= _mm256_broadcast_sd( &base[14] );
	ymm3	= _mm256_broadcast_sd( &base[15] );
	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm1	= _mm256_mul_pd( ymm1, ymm5 );
	ymm2	= _mm256_mul_pd( ymm2, ymm6 );
	ymm3	= _mm256_mul_pd( ymm3, ymm7 );

	ymm0	= _mm256_add_pd( ymm0, ymm1 );
	ymm2	= _mm256_add_pd( ymm2, ymm3 );
	ymm0	= _mm256_add_pd( ymm0, ymm2 );
	_mm256_storeu_pd( &result[12], ymm0 );
}
#endif


#ifdef ENABLE_AVX_FMA
#include <intrin.h>

void	MultMatrixD_AVX_FMA( double result[16], const double base[16], const double mult[16] )
{
	__m256d	ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7;

	ymm4	= _mm256_loadu_pd( &mult[0] );
	ymm5	= _mm256_loadu_pd( &mult[4] );
	ymm6	= _mm256_loadu_pd( &mult[8] );
	ymm7	= _mm256_loadu_pd( &mult[12] );

	// column0
	ymm0	= _mm256_broadcast_sd( &base[0] );
	ymm1	= _mm256_broadcast_sd( &base[1] );
	ymm2	= _mm256_broadcast_sd( &base[2] );
	ymm3	= _mm256_broadcast_sd( &base[3] );

	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm2	= _mm256_mul_pd( ymm2, ymm6 );
	ymm0	= _mm256_fmadd_pd( ymm1, ymm5, ymm0 );
	ymm2	= _mm256_fmadd_pd( ymm3, ymm7, ymm2 );
	ymm0	= _mm256_add_pd( ymm0, ymm2 );

	_mm256_storeu_pd( &result[0], ymm0 );

	// column1
	ymm0	= _mm256_broadcast_sd( &base[4] );
	ymm1	= _mm256_broadcast_sd( &base[5] );
	ymm2	= _mm256_broadcast_sd( &base[6] );
	ymm3	= _mm256_broadcast_sd( &base[7] );

	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm2	= _mm256_mul_pd( ymm2, ymm6 );
	ymm0	= _mm256_fmadd_pd( ymm1, ymm5, ymm0 );
	ymm2	= _mm256_fmadd_pd( ymm3, ymm7, ymm2 );
	ymm0	= _mm256_add_pd( ymm0, ymm2 );

	_mm256_storeu_pd( &result[4], ymm0 );

	// column2
	ymm0	= _mm256_broadcast_sd( &base[8] );
	ymm1	= _mm256_broadcast_sd( &base[9] );
	ymm2	= _mm256_broadcast_sd( &base[10] );
	ymm3	= _mm256_broadcast_sd( &base[11] );

	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm2	= _mm256_mul_pd( ymm2, ymm6 );
	ymm0	= _mm256_fmadd_pd( ymm1, ymm5, ymm0 );
	ymm2	= _mm256_fmadd_pd( ymm3, ymm7, ymm2 );
	ymm0	= _mm256_add_pd( ymm0, ymm2 );

	_mm256_storeu_pd( &result[8], ymm0 );

	// column3
	ymm0	= _mm256_broadcast_sd( &base[12] );
	ymm1	= _mm256_broadcast_sd( &base[13] );
	ymm2	= _mm256_broadcast_sd( &base[14] );
	ymm3	= _mm256_broadcast_sd( &base[15] );

	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm2	= _mm256_mul_pd( ymm2, ymm6 );
	ymm0	= _mm256_fmadd_pd( ymm1, ymm5, ymm0 );
	ymm2	= _mm256_fmadd_pd( ymm3, ymm7, ymm2 );
	ymm0	= _mm256_add_pd( ymm0, ymm2 );

	_mm256_storeu_pd( &result[12], ymm0 );
}


void	MultMatrixD_AVX_FMA_type2( double result[16], const double base[16], const double mult[16] )
{
	__m256d	ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7;

	ymm4	= _mm256_loadu_pd( &mult[0] );
	ymm5	= _mm256_loadu_pd( &mult[4] );
	ymm6	= _mm256_loadu_pd( &mult[8] );
	ymm7	= _mm256_loadu_pd( &mult[12] );

	// column0
	ymm0	= _mm256_broadcast_sd( &base[0] );
	ymm1	= _mm256_broadcast_sd( &base[1] );
	ymm2	= _mm256_broadcast_sd( &base[2] );
	ymm3	= _mm256_broadcast_sd( &base[3] );

	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm0	= _mm256_fmadd_pd( ymm1, ymm5, ymm0 );
	ymm0	= _mm256_fmadd_pd( ymm2, ymm6, ymm0 );
	ymm0	= _mm256_fmadd_pd( ymm3, ymm7, ymm0 );

	_mm256_storeu_pd( &result[0], ymm0 );

	// column1
	ymm0	= _mm256_broadcast_sd( &base[4] );
	ymm1	= _mm256_broadcast_sd( &base[5] );
	ymm2	= _mm256_broadcast_sd( &base[6] );
	ymm3	= _mm256_broadcast_sd( &base[7] );

	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm0	= _mm256_fmadd_pd( ymm1, ymm5, ymm0 );
	ymm0	= _mm256_fmadd_pd( ymm2, ymm6, ymm0 );
	ymm0	= _mm256_fmadd_pd( ymm3, ymm7, ymm0 );

	_mm256_storeu_pd( &result[4], ymm0 );

	// column2
	ymm0	= _mm256_broadcast_sd( &base[8] );
	ymm1	= _mm256_broadcast_sd( &base[9] );
	ymm2	= _mm256_broadcast_sd( &base[10] );
	ymm3	= _mm256_broadcast_sd( &base[11] );

	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm0	= _mm256_fmadd_pd( ymm1, ymm5, ymm0 );
	ymm0	= _mm256_fmadd_pd( ymm2, ymm6, ymm0 );
	ymm0	= _mm256_fmadd_pd( ymm3, ymm7, ymm0 );

	_mm256_storeu_pd( &result[8], ymm0 );

	// column3
	ymm0	= _mm256_broadcast_sd( &base[12] );
	ymm1	= _mm256_broadcast_sd( &base[13] );
	ymm2	= _mm256_broadcast_sd( &base[14] );
	ymm3	= _mm256_broadcast_sd( &base[15] );

	ymm0	= _mm256_mul_pd( ymm0, ymm4 );
	ymm0	= _mm256_fmadd_pd( ymm1, ymm5, ymm0 );
	ymm0	= _mm256_fmadd_pd( ymm2, ymm6, ymm0 );
	ymm0	= _mm256_fmadd_pd( ymm3, ymm7, ymm0 );

	_mm256_storeu_pd( &result[12], ymm0 );
}
#endif

template<class T>
bool	CalcInverse( T result[16], const T src[16] )
{
	T	b00		= M44(src,1,1) * (M44(src,2,2) * M44(src,3,3) - M44(src,3,2) * M44(src,2,3))
				+ M44(src,1,2) * (M44(src,2,3) * M44(src,3,1) - M44(src,3,3) * M44(src,2,1))
				+ M44(src,1,3) * (M44(src,2,1) * M44(src,3,2) - M44(src,3,1) * M44(src,2,2));	//14
	T	b10		= M44(src,1,2) * (M44(src,2,0) * M44(src,3,3) - M44(src,3,0) * M44(src,2,3))
				+ M44(src,1,3) * (M44(src,2,2) * M44(src,3,0) - M44(src,3,2) * M44(src,2,0))
				+ M44(src,1,0) * (M44(src,2,3) * M44(src,3,2) - M44(src,3,3) * M44(src,2,2));
	T	b20		= M44(src,1,3) * (M44(src,2,0) * M44(src,3,1) - M44(src,3,0) * M44(src,2,1))
				+ M44(src,1,0) * (M44(src,2,1) * M44(src,3,3) - M44(src,3,1) * M44(src,2,3))
				+ M44(src,1,1) * (M44(src,2,3) * M44(src,3,0) - M44(src,3,3) * M44(src,2,0));
	T	b30		= M44(src,1,0) * (M44(src,2,2) * M44(src,3,1) - M44(src,3,2) * M44(src,2,1))
				+ M44(src,1,1) * (M44(src,2,0) * M44(src,3,2) - M44(src,3,0) * M44(src,2,2))
				+ M44(src,1,2) * (M44(src,2,1) * M44(src,3,0) - M44(src,3,1) * M44(src,2,0));

	T	b01		= M44(src,2,1) * (M44(src,0,2) * M44(src,3,3) - M44(src,3,2) * M44(src,0,3))
				+ M44(src,2,2) * (M44(src,0,3) * M44(src,3,1) - M44(src,3,3) * M44(src,0,1))
				+ M44(src,2,3) * (M44(src,0,1) * M44(src,3,2) - M44(src,3,1) * M44(src,0,2));
	T	b11		= M44(src,2,2) * (M44(src,0,0) * M44(src,3,3) - M44(src,3,0) * M44(src,0,3))
				+ M44(src,2,3) * (M44(src,0,2) * M44(src,3,0) - M44(src,3,2) * M44(src,0,0))
				+ M44(src,2,0) * (M44(src,0,3) * M44(src,3,2) - M44(src,3,3) * M44(src,0,2));
	T	b21		= M44(src,2,3) * (M44(src,0,0) * M44(src,3,1) - M44(src,3,0) * M44(src,0,1))
				+ M44(src,2,0) * (M44(src,0,1) * M44(src,3,3) - M44(src,3,1) * M44(src,0,3))
				+ M44(src,2,1) * (M44(src,0,3) * M44(src,3,0) - M44(src,3,3) * M44(src,0,0));
	T	b31		= M44(src,2,0) * (M44(src,0,2) * M44(src,3,1) - M44(src,3,2) * M44(src,0,1))
				+ M44(src,2,1) * (M44(src,0,0) * M44(src,3,2) - M44(src,3,0) * M44(src,0,2))
				+ M44(src,2,2) * (M44(src,0,1) * M44(src,3,0) - M44(src,3,1) * M44(src,0,0));

	T	b02		= M44(src,3,1) * (M44(src,0,2) * M44(src,1,3) - M44(src,1,2) * M44(src,0,3))
				+ M44(src,3,2) * (M44(src,0,3) * M44(src,1,1) - M44(src,1,3) * M44(src,0,1))
				+ M44(src,3,3) * (M44(src,0,1) * M44(src,1,2) - M44(src,1,1) * M44(src,0,2));
	T	b12		= M44(src,3,2) * (M44(src,0,0) * M44(src,1,3) - M44(src,1,0) * M44(src,0,3))
				+ M44(src,3,3) * (M44(src,0,2) * M44(src,1,0) - M44(src,1,2) * M44(src,0,0))
				+ M44(src,3,0) * (M44(src,0,3) * M44(src,1,2) - M44(src,1,3) * M44(src,0,2));
	T	b22		= M44(src,3,3) * (M44(src,0,0) * M44(src,1,1) - M44(src,1,0) * M44(src,0,1))
				+ M44(src,3,0) * (M44(src,0,1) * M44(src,1,3) - M44(src,1,1) * M44(src,0,3))
				+ M44(src,3,1) * (M44(src,0,3) * M44(src,1,0) - M44(src,1,3) * M44(src,0,0));
	T	b32		= M44(src,3,0) * (M44(src,0,2) * M44(src,1,1) - M44(src,1,2) * M44(src,0,1))
				+ M44(src,3,1) * (M44(src,0,0) * M44(src,1,2) - M44(src,1,0) * M44(src,0,2))
				+ M44(src,3,2) * (M44(src,0,1) * M44(src,1,0) - M44(src,1,1) * M44(src,0,0));

	T	b03		= M44(src,0,1) * (M44(src,2,2) * M44(src,1,3) - M44(src,2,3) * M44(src,1,2))
				+ M44(src,0,2) * (M44(src,2,3) * M44(src,1,1) - M44(src,2,1) * M44(src,1,3))
				+ M44(src,0,3) * (M44(src,2,1) * M44(src,1,2) - M44(src,2,2) * M44(src,1,1));
	T	b13		= M44(src,0,2) * (M44(src,2,0) * M44(src,1,3) - M44(src,2,3) * M44(src,1,0))
				+ M44(src,0,3) * (M44(src,2,2) * M44(src,1,0) - M44(src,2,0) * M44(src,1,2))
				+ M44(src,0,0) * (M44(src,2,3) * M44(src,1,2) - M44(src,2,2) * M44(src,1,3));
	T	b23		= M44(src,0,3) * (M44(src,2,0) * M44(src,1,1) - M44(src,2,1) * M44(src,1,0))
				+ M44(src,0,0) * (M44(src,2,1) * M44(src,1,3) - M44(src,2,3) * M44(src,1,1))
				+ M44(src,0,1) * (M44(src,2,3) * M44(src,1,0) - M44(src,2,0) * M44(src,1,3));
	T	b33		= M44(src,0,0) * (M44(src,2,2) * M44(src,1,1) - M44(src,2,1) * M44(src,1,2))
				+ M44(src,0,1) * (M44(src,2,0) * M44(src,1,2) - M44(src,2,2) * M44(src,1,0))
				+ M44(src,0,2) * (M44(src,2,1) * M44(src,1,0) - M44(src,2,0) * M44(src,1,1));

	T	detA	= M44(src,0,0) * b00
				+ M44(src,0,1) * b10
				+ M44(src,0,2) * b20
				+ M44(src,0,3) * b30;	// 7

	if( detA != 0 )
	{
		M44(result,0,0)	= b00 / detA; 
		M44(result,1,0)	= b10 / detA; 
		M44(result,2,0)	= b20 / detA;
		M44(result,3,0)	= b30 / detA;
		M44(result,0,1)	= b01 / detA; 
		M44(result,1,1)	= b11 / detA; 
		M44(result,2,1)	= b21 / detA;
		M44(result,3,1)	= b31 / detA;
		M44(result,0,2)	= b02 / detA; 
		M44(result,1,2)	= b12 / detA; 
		M44(result,2,2)	= b22 / detA;
		M44(result,3,2)	= b32 / detA;
		M44(result,0,3)	= b03 / detA; 
		M44(result,1,3)	= b13 / detA; 
		M44(result,2,3)	= b23 / detA;
		M44(result,3,3)	= b33 / detA;

		return	true;
	}

	return	false;
}


#ifdef ENABLE_AVX2_FMA
#include <intrin.h>

bool	CalcInverseD_AVX2_FMA( double result[16], const double src[16] )
{
	__m256d	c0	= _mm256_loadu_pd( &src[ 0] );
	__m256d	c1	= _mm256_loadu_pd( &src[ 4] );
	__m256d	c2	= _mm256_loadu_pd( &src[ 8] );
	__m256d	c3	= _mm256_loadu_pd( &src[12] );

	__m256d	r0, r1, r2, r3;

	{
		// transpose
		__m256d	L01	= _mm256_unpacklo_pd( c0, c1 );
		__m256d	H01	= _mm256_unpackhi_pd( c0, c1 );
		__m256d	L23	= _mm256_unpacklo_pd( c2, c3 );
		__m256d	H23	= _mm256_unpackhi_pd( c2, c3 );

		r0	= _mm256_permute2f128_pd( L01, L23, 0x20 );
		r1	= _mm256_permute2f128_pd( H01, H23, 0x20 );
		r2	= _mm256_permute2f128_pd( L01, L23, 0x31 );
		r3	= _mm256_permute2f128_pd( H01, H23, 0x31 );
	}

	__m256d	rc0	=
		_mm256_fmadd_pd(
			_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 0, 3, 2, 1 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 2, 0, 0, 2 )),
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 1, 1, 3, 3 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 2, 0, 0, 2 )),
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 1, 1, 3, 3 )) ) ),

		_mm256_fmadd_pd(
			_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 1, 0, 3, 2 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 0, 1, 2, 3 )),
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 2, 3, 0, 1 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 0, 1, 2, 3 )),
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 2, 3, 0, 1 )) ) ),

		_mm256_mul_pd(
			_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 2, 1, 0, 3 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 1, 3, 3, 1 )),
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 0, 0, 2, 2 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 1, 3, 3, 1 )),
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 0, 0, 2, 2 )) ) ) ) ) );

	__m256d	rc1	=
		_mm256_fmadd_pd(
			_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 0, 3, 2, 1 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 2, 0, 0, 2 )),
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 1, 1, 3, 3 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 2, 0, 0, 2 )),
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 1, 1, 3, 3 )) ) ),

		_mm256_fmadd_pd(
			_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 1, 0, 3, 2 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 0, 1, 2, 3 )),
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 2, 3, 0, 1 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 0, 1, 2, 3 )),
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 2, 3, 0, 1 )) ) ),

		_mm256_mul_pd(
			_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 2, 1, 0, 3 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 1, 3, 3, 1 )),
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 0, 0, 2, 2 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 1, 3, 3, 1 )),
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 0, 0, 2, 2 )) ) ) ) ) );

	__m256d	rc2	=
		_mm256_fmadd_pd(
			_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 0, 3, 2, 1 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 2, 0, 0, 2 )),
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 1, 1, 3, 3 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 2, 0, 0, 2 )),
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 1, 1, 3, 3 )) ) ),

		_mm256_fmadd_pd(
			_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 1, 0, 3, 2 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 0, 1, 2, 3 )),
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 2, 3, 0, 1 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 0, 1, 2, 3 )),
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 2, 3, 0, 1 )) ) ),

		_mm256_mul_pd(
			_mm256_permute4x64_pd( r3, _MM_SHUFFLE( 2, 1, 0, 3 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 1, 3, 3, 1 )),
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 0, 0, 2, 2 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 1, 3, 3, 1 )),
				_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 0, 0, 2, 2 )) ) ) ) ) );


	__m256d	rc3	=
		_mm256_fmadd_pd(
			_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 0, 3, 2, 1 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 2, 0, 0, 2 )),
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 1, 1, 3, 3 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 2, 0, 0, 2 )),
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 1, 1, 3, 3 )) ) ),

		_mm256_fmadd_pd(
			_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 1, 0, 3, 2 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 0, 1, 2, 3 )),
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 2, 3, 0, 1 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 0, 1, 2, 3 )),
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 2, 3, 0, 1 )) ) ),

		_mm256_mul_pd(
			_mm256_permute4x64_pd( r0, _MM_SHUFFLE( 2, 1, 0, 3 )),
			_mm256_fmsub_pd(
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 1, 3, 3, 1 )),
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 0, 0, 2, 2 )),
			_mm256_mul_pd(
				_mm256_permute4x64_pd( r1, _MM_SHUFFLE( 1, 3, 3, 1 )),
				_mm256_permute4x64_pd( r2, _MM_SHUFFLE( 0, 0, 2, 2 )) ) ) ) ) );



	__m256d	detA		= _mm256_mul_pd( r0, rc0 );
	__m256d	detS		= _mm256_permute2f128_pd( detA, detA, 0x23 );
	detA	= _mm256_hadd_pd( detA, detS );
	detA	= _mm256_hadd_pd( detA, detA );

	if( 0 != detA.m256d_f64[0] )//_mm256_testz_pd( detA, _mm256_setzero_pd() ) )
	{
		rc0	= _mm256_div_pd( rc0, detA );
		rc1	= _mm256_div_pd( rc1, detA );
		rc2	= _mm256_div_pd( rc2, detA );
		rc3	= _mm256_div_pd( rc3, detA );

		_mm256_storeu_pd( &result[ 0], rc0 ); 
		_mm256_storeu_pd( &result[ 4], rc1 ); 
		_mm256_storeu_pd( &result[ 8], rc2 ); 
		_mm256_storeu_pd( &result[12], rc3 ); 

		return	true;
	}

	return	false;
}
#endif

template<class T>
void	CopyMatrix_memcpy( T dst[16], const T src[16] )
{
	memcpy( dst, src, sizeof(T) * 16 );
}

template<class T>
void	CopyMatrix_Expand( T dst[16], const T src[16] )
{
	dst[ 0]	= src[ 0];
	dst[ 1]	= src[ 1];
	dst[ 2]	= src[ 2];
	dst[ 3]	= src[ 3];
	dst[ 4]	= src[ 4];
	dst[ 5]	= src[ 5];
	dst[ 6]	= src[ 6];
	dst[ 7]	= src[ 7];
	dst[ 8]	= src[ 8];
	dst[ 9]	= src[ 9];
	dst[10]	= src[10];
	dst[11]	= src[11];
	dst[12]	= src[12];
	dst[13]	= src[13];
	dst[14]	= src[14];
	dst[15]	= src[15];
}


#ifdef ENABLE_SSE2
void	CopyMatrixD_SSE2( double dst[16], const double src[16] )
{
	_mm_storeu_pd( &dst[ 0], _mm_loadu_pd( &src[ 0] ) );
	_mm_storeu_pd( &dst[ 2], _mm_loadu_pd( &src[ 2] ) );
	_mm_storeu_pd( &dst[ 4], _mm_loadu_pd( &src[ 4] ) );
	_mm_storeu_pd( &dst[ 6], _mm_loadu_pd( &src[ 6] ) );
	_mm_storeu_pd( &dst[ 8], _mm_loadu_pd( &src[ 8] ) );
	_mm_storeu_pd( &dst[10], _mm_loadu_pd( &src[10] ) );
	_mm_storeu_pd( &dst[12], _mm_loadu_pd( &src[12] ) );
	_mm_storeu_pd( &dst[14], _mm_loadu_pd( &src[14] ) );
}
#endif


#ifdef ENABLE_AVX
void	CopyMatrixD_AVX( double dst[16], const double src[16] )
{
	_mm256_storeu_pd( &dst[ 0], _mm256_loadu_pd( &src[ 0] ) );
	_mm256_storeu_pd( &dst[ 4], _mm256_loadu_pd( &src[ 4] ) );
	_mm256_storeu_pd( &dst[ 8], _mm256_loadu_pd( &src[ 8] ) );
	_mm256_storeu_pd( &dst[12], _mm256_loadu_pd( &src[12] ) );
}
#endif

template<class T>
void	ShowMatrix( const T m[16] )
{
	printf( "%9.3f, %9.3f, %9.3f, %9.3f\r\n", M44(m,0,0), M44(m,0,1), M44(m,0,2), M44(m,0,3) );
	printf( "%9.3f, %9.3f, %9.3f, %9.3f\r\n", M44(m,1,0), M44(m,1,1), M44(m,1,2), M44(m,1,3) );
	printf( "%9.3f, %9.3f, %9.3f, %9.3f\r\n", M44(m,2,0), M44(m,2,1), M44(m,2,2), M44(m,2,3) );
	printf( "%9.3f, %9.3f, %9.3f, %9.3f\r\n", M44(m,3,0), M44(m,3,1), M44(m,3,2), M44(m,3,3) );
}


#define	PAIR_MLTD				std::pair<void (*)(double result[16], const double base[16], const double mult[16]),const char*>
#define	MAKE_MLTD_PAIR(func)	PAIR_MLTD(func,#func)

#define	PAIR_MLTF				std::pair<void (*)(float result[16], const float base[16], const float mult[16]),const char*>
#define	MAKE_MLTF_PAIR(func)	PAIR_MLTF(func,#func)

#define	PAIR_INVD				std::pair<bool (*)(double result[16], const double base[16]),const char*>
#define	MAKE_INVD_PAIR(func)	PAIR_INVD(func,#func)

#define	PAIR_INVF				std::pair<bool (*)(float result[16], const float base[16]),const char*>
#define	MAKE_INVF_PAIR(func)	PAIR_INVF(func,#func)

#define	PAIR_CPYD				std::pair<void (*)(double result[16], const double base[16]),const char*>
#define	MAKE_CPYD_PAIR(func)	PAIR_CPYD(func,#func)





int main()
{
	int     duration	= 3;
	int 	dwThreads	= CMultiThreadTools::GetProcessorCount();


	double	baseD[]  = {	1,	2,	6,	7,
							3,	5,	8,	13,
							4,	9,	12,	14,
							10,	11,	15,	16 };

	double  multD[]  = {	-0.048821549,	0.075757576,	-0.257575758,	0.185185185,
							-0.38047138,	-0.03030303,	0.303030303,	-0.074074074,
							0.417508418,	-0.303030303,	0.03030303	,	0.037037037,
							-0.099326599,	0.257575758,	-0.075757576,	-0.037037037 };

	float	baseF[]  = {	1,	2,	6,	7,
							3,	5,	8,	13,
							4,	9,	12,	14,
							10,	11,	15,	16 };

	float  multF[]  = {	-0.048821549,	0.075757576,	-0.257575758,	0.185185185,
							-0.38047138,	-0.03030303,	0.303030303,	-0.074074074,
							0.417508418,	-0.303030303,	0.03030303	,	0.037037037,
							-0.099326599,	0.257575758,	-0.075757576,	-0.037037037 };


	std::vector<PAIR_MLTD>		iMltD;
	std::vector<PAIR_MLTF>		iMltF;

	std::vector<PAIR_INVD>		iInvD;
	std::vector<PAIR_INVF>		iInvF;

	std::vector<PAIR_CPYD>		iCpyD;

	iMltF.push_back(MAKE_MLTF_PAIR(MultMatrix_ForC<float>));
	iMltF.push_back(MAKE_MLTF_PAIR(MultMatrix_ForR<float>));
	iMltF.push_back(MAKE_MLTF_PAIR(MultMatrix_ExpC<float>));
	iMltF.push_back(MAKE_MLTF_PAIR(MultMatrix_ExpR<float>));

	iMltD.push_back(MAKE_MLTD_PAIR(MultMatrix_ForC<double>));
	iMltD.push_back(MAKE_MLTD_PAIR(MultMatrix_ForR<double>));
	iMltD.push_back(MAKE_MLTD_PAIR(MultMatrix_ExpC<double>));
	iMltD.push_back(MAKE_MLTD_PAIR(MultMatrix_ExpR<double>));

	iInvF.push_back(MAKE_INVF_PAIR(CalcInverse<float>));
	iInvD.push_back(MAKE_INVD_PAIR(CalcInverse<double>));

	iCpyD.push_back(MAKE_CPYD_PAIR(CopyMatrix_memcpy<double>));
	iCpyD.push_back(MAKE_CPYD_PAIR(CopyMatrix_Expand<double>));

#ifdef ENABLE_NEON
	iMltF.push_back(MAKE_MLTF_PAIR(MultMatrixF_NEON));
	iMltF.push_back(MAKE_MLTF_PAIR(MultMatrixF_NEON_type2));
	iCpyD.push_back(MAKE_CPYD_PAIR(CopyMatrixD_NEON));
#endif

#ifdef ENABLE_SSE2
	iMltF.push_back(MAKE_MLTF_PAIR(MultMatrixF_SSE2));
	iMltD.push_back(MAKE_MLTD_PAIR(MultMatrixD_SSE2));
	iCpyD.push_back(MAKE_CPYD_PAIR(CopyMatrixD_SSE2));
#endif

#ifdef ENABLE_AVX
	iMltD.push_back(MAKE_MLTD_PAIR(MultMatrixD_AVX));
	iCpyD.push_back(MAKE_CPYD_PAIR(CopyMatrixD_AVX));
#endif

#ifdef ENABLE_AVX_FMA
	iMltD.push_back(MAKE_MLTD_PAIR(MultMatrixD_AVX_FMA));
	iMltD.push_back(MAKE_MLTD_PAIR(MultMatrixD_AVX_FMA_type2));
#endif

#ifdef ENABLE_AVX2_FMA
	iInvD.push_back(MAKE_INVD_PAIR(CalcInverseD_AVX2_FMA));
#endif


//*
	for (auto it = iMltD.begin(); it != iMltD.end(); it++ )
	{
		double	result[16]	= {-1};

		(*it).first( result, baseD, multD );
		printf( "----- %s ------------\n", (*it).second );
		ShowMatrix( result );
	}

	for (auto it = iMltF.begin(); it != iMltF.end(); it++ )
	{
		float	result[16]	= {-1};

		(*it).first( result, baseF, multF );
		printf( "----- %s ------------\n", (*it).second );
		ShowMatrix( result );
	}

	for (auto it = iInvD.begin(); it != iInvD.end(); it++ )
	{
		double	result[16]	= {-1};

		(*it).first( result, multD );
		printf( "----- %s ------------\n", (*it).second );
		ShowMatrix( result );
	}
//*/

	printf("| Function name                | Single Thread | Multi  Thread |Performance(MT) |MT/ST |\n");
	printf("|:-----------------------------|--------------:|--------------:|---------------:|-----:|\n");

	// Dest = src, src
	for (auto it = iMltF.begin(); it != iMltF.end(); it++)
	{
	    std::chrono::system_clock::time_point   EndTime;
        std::atomic<uint64_t>                   TotalCount;
        std::atomic<uint64_t>                   TotalTime;
        double									opS,opM;

		auto	func		= [&]()
		{
			size_t	                                nCount		= 0;
			size_t	                                nLoop		= 100000;
			std::chrono::system_clock::time_point   _EndTime	= EndTime;

			auto			dss_func	= (*it).first;
			const float * 	_base	= baseF;
			const float *	_mult	= multF;
			float			result[16];

        	std::chrono::high_resolution_clock::time_point	st,et;
        	
        	st  = std::chrono::high_resolution_clock::now();
			while( std::chrono::system_clock::now() < _EndTime )
			{
				for( size_t i = 0; i < nLoop; i++ )
				{
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );

					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
				}

				nCount += 10;
			}
        	et  = std::chrono::high_resolution_clock::now();

            TotalCount  += nCount * (uint64_t)nLoop;
			TotalTime   += std::chrono::duration_cast<std::chrono::nanoseconds>(et-st).count();
		};


		TotalCount	= 0;
		TotalTime	= 0;
		EndTime     = std::chrono::system_clock::now() + std::chrono::seconds(duration);
		func();
		opS			= TotalCount.load() * 1000000000.0 / TotalTime.load();


		TotalCount	= 0;
		TotalTime	= 0;
		EndTime     = std::chrono::system_clock::now() + std::chrono::seconds(duration);
		CMultiThreadTools::DoExecute( dwThreads, func );
		opM			= TotalCount.load() * 1000000000.0 * dwThreads / TotalTime.load();


		printf( "|%-30s|%7.1f M mul/s|%7.1f M mul/s|%9.3f Gflops|%4.0f %%|\n",
			(*it).second,
			opS / 1000000,
			opM / 1000000,
			opM * 112 / 1000000000,
			opM * 100 / opS );
	}

	// double dst = src , src
	for (auto it = iMltD.begin(); it != iMltD.end(); it++)
	{
	    std::chrono::system_clock::time_point   EndTime;
        std::atomic<uint64_t>                   TotalCount;
        std::atomic<uint64_t>                   TotalTime;
        double									opS, opM;

		auto	func		= [&]()
		{
			size_t	                                nCount		= 0;
			size_t	                                nLoop		= 100000;
			std::chrono::system_clock::time_point   _EndTime	= EndTime;

			auto			dss_func	= (*it).first;
			const double * 	_base	= baseD;
			const double *	_mult	= multD;
			double			result[16];

        	std::chrono::high_resolution_clock::time_point	st,et;
        	
        	st  = std::chrono::high_resolution_clock::now();
			while( std::chrono::system_clock::now() < _EndTime )
			{
				for( size_t i = 0; i < nLoop; i++ )
				{
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );

					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
					dss_func( result, _base, _mult );
				}

				nCount += 10;
			}
        	et  = std::chrono::high_resolution_clock::now();

            TotalCount  += nCount * (uint64_t)nLoop;
			TotalTime   += std::chrono::duration_cast<std::chrono::nanoseconds>(et-st).count();
		};


		TotalCount	= 0;
		TotalTime	= 0;
		EndTime     = std::chrono::system_clock::now() + std::chrono::seconds(duration);
		func();
		opS			= TotalCount.load() * 1000000000.0 / TotalTime.load();


		TotalCount	= 0;
		TotalTime	= 0;
		EndTime     = std::chrono::system_clock::now() + std::chrono::seconds(duration);
		CMultiThreadTools::DoExecute( dwThreads, func );
		opM			= TotalCount.load() * 1000000000.0 * dwThreads / TotalTime.load();


		printf( "|%-30s|%7.1f M mul/s|%7.1f M mul/s|%9.3f Gflops|%4.0f %%|\n",
			(*it).second,
			opS / 1000000,
			opM / 1000000,
			opM * 112 / 1000000000,
			opM * 100 / opS );
	}


	// float inverse
	for (auto it = iInvF.begin(); it != iInvF.end(); it++)
	{
	    std::chrono::system_clock::time_point   EndTime;
        std::atomic<uint64_t>                   TotalCount;
        std::atomic<uint64_t>                   TotalTime;
        double									opS, opM;

		auto	func		= [&]()
		{
			size_t	                                nCount		= 0;
			size_t	                                nLoop		= 100000;
			std::chrono::system_clock::time_point   _EndTime	= EndTime;

			auto			ds_func	= (*it).first;
			const float * 	_base	= baseF;
			float			result[16];

        	std::chrono::high_resolution_clock::time_point	st,et;
        	
        	st  = std::chrono::high_resolution_clock::now();
			while( std::chrono::system_clock::now() < _EndTime )
			{
				for( size_t i = 0; i < nLoop; i++ )
				{
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );

					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
				}

				nCount += 10;
			}
        	et  = std::chrono::high_resolution_clock::now();

            TotalCount  += nCount * (uint64_t)nLoop;
			TotalTime   += std::chrono::duration_cast<std::chrono::nanoseconds>(et-st).count();
		};


		TotalCount	= 0;
		TotalTime	= 0;
		EndTime     = std::chrono::system_clock::now() + std::chrono::seconds(duration);
		func();
		opS			= TotalCount.load() * 1000000000.0 / TotalTime.load();


		TotalCount	= 0;
		TotalTime	= 0;
		EndTime     = std::chrono::system_clock::now() + std::chrono::seconds(duration);
		CMultiThreadTools::DoExecute( dwThreads, func );
		opM			= TotalCount.load() * 1000000000.0 * dwThreads / TotalTime.load();


		printf( "|%-30s|%7.1f M inv/s|%7.1f M inv/s|%9.3f Gflops|%4.0f %%|\n",
			(*it).second,
			opS / 1000000,
			opM / 1000000,
			opM * 247 / 1000000000,
			opM * 100 / opS );
	}

	// double inverser
	for (auto it = iInvD.begin(); it != iInvD.end(); it++)
	{
	    std::chrono::system_clock::time_point   EndTime;
        std::atomic<uint64_t>                   TotalCount;
        std::atomic<uint64_t>                   TotalTime;
        double									opS, opM;

		auto	func		= [&]()
		{
			size_t	                                nCount		= 0;
			size_t	                                nLoop		= 100000;
			std::chrono::system_clock::time_point   _EndTime	= EndTime;

			auto			ds_func	= (*it).first;
			const double * 	_base	= baseD;
			const double *	_mult	= multD;
			double			result[16];

        	std::chrono::high_resolution_clock::time_point	st,et;
        	
        	st  = std::chrono::high_resolution_clock::now();
			while( std::chrono::system_clock::now() < _EndTime )
			{
				for( size_t i = 0; i < nLoop; i++ )
				{
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );

					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
				}

				nCount += 10;
			}
        	et  = std::chrono::high_resolution_clock::now();

            TotalCount  += nCount * (uint64_t)nLoop;
			TotalTime   += std::chrono::duration_cast<std::chrono::nanoseconds>(et-st).count();
		};


		TotalCount	= 0;
		TotalTime	= 0;
		EndTime     = std::chrono::system_clock::now() + std::chrono::seconds(duration);
		func();
		opS			= TotalCount.load() * 1000000000.0 / TotalTime.load();


		TotalCount	= 0;
		TotalTime	= 0;
		EndTime     = std::chrono::system_clock::now() + std::chrono::seconds(duration);
		CMultiThreadTools::DoExecute( dwThreads, func );
		opM			= TotalCount.load() * 1000000000.0 * dwThreads / TotalTime.load();


		printf( "|%-30s|%7.1f M inv/s|%7.1f M inv/s|%9.3f Gflops|%4.0f %%|\n",
			(*it).second,
			opS / 1000000,
			opM / 1000000,
			opM * 247 / 1000000000,
			opM * 100 / opS );
	}
	
	// double copy
	for (auto it = iCpyD.begin(); it != iCpyD.end(); it++)
	{
	    std::chrono::system_clock::time_point   EndTime;
        std::atomic<uint64_t>                   TotalCount;
        std::atomic<uint64_t>                   TotalTime;
        double									opS,opM;

		auto	func		= [&]()
		{
			size_t	                                nCount		= 0;
			size_t	                                nLoop		= 100000;
			std::chrono::system_clock::time_point   _EndTime	= EndTime;

			auto			ds_func	= (*it).first;
			const double * 	_base	= baseD;
			const double *	_mult	= multD;
			double			result[16];

        	std::chrono::high_resolution_clock::time_point	st,et;
        	
        	st  = std::chrono::high_resolution_clock::now();
			while( std::chrono::system_clock::now() < _EndTime )
			{
				for( size_t i = 0; i < nLoop; i++ )
				{
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );

					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
					ds_func( result, _base );
				}

				nCount += 10;
			}
        	et  = std::chrono::high_resolution_clock::now();

            TotalCount  += nCount * (uint64_t)nLoop;
			TotalTime   += std::chrono::duration_cast<std::chrono::nanoseconds>(et-st).count();
		};


		TotalCount	= 0;
		TotalTime	= 0;
		EndTime     = std::chrono::system_clock::now() + std::chrono::seconds(duration);
		func();
		opS			= TotalCount.load() * 1000000000.0 / TotalTime.load();


		TotalCount	= 0;
		TotalTime	= 0;
		EndTime     = std::chrono::system_clock::now() + std::chrono::seconds(duration);
		CMultiThreadTools::DoExecute( dwThreads, func );
		opM			= TotalCount.load() * 1000000000.0 * dwThreads / TotalTime.load();


		printf( "|%-30s|%7.1f M cpy/s|%7.1f M cpy/s|%9.3f GB/sec|%4.0f %%|\n",
			(*it).second,
			opS / 1000000,
			opM / 1000000,
			opM * 8 * 16 / 1000000000,
			opM * 100 / opS );
	}


	printf( "fin" );
//	getchar();
    return 0;
}

