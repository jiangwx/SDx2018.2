
#include "jf_config.h"

static void jFthreshold(hls::stream<PIXEL>& gray, hls::stream<PIXEL>& binary, int rows, int cols, PIXEL threshold, PIXEL maxval)
{
	PIXEL _gray,_binary;
	for(ap_uint<12> row = 0; row < rows; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(ap_uint<12> col = 0; col < cols; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE II = 1
#pragma HLS loop_flatten off
			gray >> _gray;
			_binary = _gray > threshold ? maxval : 0  ;
			binary << _binary;
		}
	}
}


void jf_threshold(PIXEL* gray, PIXEL* binary,int rows, int cols, PIXEL threshold, PIXEL maxval)
{
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	hls::stream<PIXEL> _gray;
	hls::stream<PIXEL> _binary;
read:
	for(ap_uint<12> i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(ap_uint<12> j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			_gray.write(*(gray + i*cols + j));
		}
	}

	jFthreshold(_gray, _binary, rows, cols, threshold, maxval);

write:
	for(ap_uint<12> i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(ap_uint<12> j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			*(binary + i*cols + j) = _binary.read();

		}
	}
}
