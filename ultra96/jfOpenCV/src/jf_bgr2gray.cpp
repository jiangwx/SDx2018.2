#include "jf_config.h"

static void jFbgr2gray(hls::stream<PIXEL>& b, hls::stream<PIXEL>& g, hls::stream<PIXEL>& r, hls::stream<PIXEL>& gray, int rows, int cols)
{
	PIXEL _b,_g,_r,_gray;
	for(int row = 0; row < rows; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int col = 0; col < cols; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE II = 1
#pragma HLS loop_flatten off
			b >> _b;
			g >> _g;
			r >> _r;
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
	hls::stream<PIXEL> _b;
	hls::stream<PIXEL> _g;
	hls::stream<PIXEL> _r;
	hls::stream<PIXEL> _gray;

read:
	for(int i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			_bgra.write(*(bgra + i*cols + j));
			_b = _bgra.range(31, 24);
			_g = _bgra.range(23, 16);
			_r = _bgra.range(15, 8);

		}
	}

	jFbgr2gray(_b, _g, _r, _gray, rows, cols);

write:
	for(int i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			*(gray + i*cols + j) = _gray.read();
		}
	}
}
