#include "jf_config.h"

static void jFand(hls::stream<PIXEL>& bright, hls::stream<PIXEL>& color, hls::stream<PIXEL>& AND, int rows, int cols)
{
	PIXEL _bright,_color,_and;
	for(int row = 0; row < rows; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int col = 0; col < cols; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE II = 1
#pragma HLS loop_flatten off
			bright >> _bright;
			color >> _color;
			if((_bright > 0)&&(_color > 0))
				_and = 255;
			else
				_and = 0;
			AND << _and;
		}
	}
}


void jf_and(PIXEL* bright, PIXEL* color, PIXEL* AND, int rows, int cols)
{
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	hls::stream<PIXEL> _bright;
	hls::stream<PIXEL> _color;
	hls::stream<PIXEL> _and;

read:
	for(int i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			_bright.write(*(bright + i*cols + j));
			_color.write(*(color + i*cols + j));
		}
	}

	jFand(_bright, _color, _and, rows, cols);

write:
	for(int i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			*(AND + i*cols + j) = _and.read();
		}
	}
}
