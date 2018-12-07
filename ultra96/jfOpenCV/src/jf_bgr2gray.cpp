#include "jf_config.h"

static void jFbgr2gray(hls::stream<PIXEL4>& bgra, hls::stream<PIXEL>& gray, int rows, int cols)
{
	PIXEL4 _bgra;
	PIXEL _b,_g,_r,_gray;
	for(ap_uint<12>  row = 0; row < rows; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(ap_uint<12>  col = 0; col < cols; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE II = 1
#pragma HLS loop_flatten off
			bgra >> _bgra;
			_b = _bgra.range(31, 24);
			_g = _bgra.range(23, 16);
			_r = _bgra.range(15, 8);
			_gray = 0.299*_r + 0.587*_g + 0.114*_b;
			gray << _gray;
		}
	}
}


void jf_rgb2gray(PIXEL4* bgra, PIXEL* gray, int rows, int cols)
{
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	hls::stream<PIXEL4> _bgra;
	hls::stream<PIXEL> _gray;

read:
	for(ap_uint<12>  i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(ap_uint<12>  j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			_bgra.write(*(bgra + i*cols + j));
		}
	}

	jFbgr2gray(_bgra, _gray, rows, cols);

write:
	for(ap_uint<12>  i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(ap_uint<12>  j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			*(gray + i*cols + j) = _gray.read();
		}
	}
}
