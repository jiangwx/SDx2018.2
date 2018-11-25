
#include "jf_bgr2gray_config.h"


void jFbgr2gray(hls::stream<uint8>& b, hls::stream<uint8>& g, hls::stream<uint8>& r, hls::stream<uint8>& gray, int rows, int cols)
{
	uint8 _b,_g,_r,_gray;
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


void jf_rgb2gray(uint8* b, uint8* g, uint8* r, uint8* gray, int rows, int cols)
{
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	hls::stream<uint8> _b;
	hls::stream<uint8> _g;
	hls::stream<uint8> _r;
	hls::stream<uint8> _gray;

read_rgb:
	for(int i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			_b.write(*(b + i*cols*3+j*3));
			_g.write(*(g + i*cols*3+j*3));
			_r.write(*(r + i*cols*3+j*3));
		}
	}

	jFbgr2gray(_b, _g, _r, _gray, rows, cols);

write_gray:
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
