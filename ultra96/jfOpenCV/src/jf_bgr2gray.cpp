
#include "jf_config.h"

void jFbgr2gray(hls::stream<PIXEL>& b, hls::stream<PIXEL>& g, hls::stream<PIXEL>& r, hls::stream<PIXEL>& gray, int rows, int cols)
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


void jf_rgb2gray(PIXEL* b, PIXEL* g, PIXEL* r, PIXEL* gray, int rows, int cols)
{
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
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
			_b.write(*(b + i*cols + j));
			_g.write(*(g + i*cols + j));
			_r.write(*(r + i*cols + j));
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
