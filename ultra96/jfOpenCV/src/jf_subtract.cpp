#include "jf_config.h"

static void jFsubtract(hls::stream<PIXEL4>& bgra, hls::stream<PIXEL>& sub, int rows, int cols)
{
	PIXEL4 _bgra;
	PIXEL _b,_r,_sub;
	for(ap_uint<12> row = 0; row < rows; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(ap_uint<12> col = 0; col < cols; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE II = 1
#pragma HLS loop_flatten off
			bgra >> _bgra;
			_b = _bgra.range(31, 24);
			_r = _bgra.range(15, 8);
			_sub = _b - _r ;
			sub << _sub;
		}
	}
}


void jf_subtract(PIXEL4* bgra, PIXEL* sub, int rows, int cols)
{
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	hls::stream<PIXEL4> _bgra;
	hls::stream<PIXEL> _sub;

read:
	for(ap_uint<12> i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(ap_uint<12> j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			_bgra.write(*(bgra + i*cols + j));
		}
	}

	jFsubtract(_bgra, _sub, rows, cols);

write:
	for(ap_uint<12> i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(ap_uint<12> j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			*(sub + i*cols + j) = _sub.read();
		}
	}
}
