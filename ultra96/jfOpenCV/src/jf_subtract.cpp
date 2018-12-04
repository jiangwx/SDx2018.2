#include "jf_config.h"

static void jFsubtract(hls::stream<PIXEL>& b, hls::stream<PIXEL>& r, hls::stream<PIXEL>& sub, int rows, int cols)
{
	PIXEL _b,_r,_sub;
	for(int row = 0; row < rows; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int col = 0; col < cols; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE II = 1
#pragma HLS loop_flatten off
			b >> _b;
			r >> _r;
			_sub = _b - _r ;
			sub << _sub;
		}
	}
}


void jf_subtract(PIXEL* b, PIXEL* r, PIXEL* sub, int rows, int cols)
{
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	hls::stream<PIXEL> _b;
	hls::stream<PIXEL> _r;
	hls::stream<PIXEL> _sub;

read:
	for(int i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			_b.write(*(b + i*cols + j));
			_r.write(*(r + i*cols + j));
		}
	}

	jFsubtract(_b, _r, _sub, rows, cols);

write:
	for(int i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			*(sub + i*cols + j) = _sub.read();
		}
	}
}
