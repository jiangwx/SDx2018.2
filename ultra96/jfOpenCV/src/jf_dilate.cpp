#include "jf_config.h"

static PIXEL dilate_kernel(PIXEL WB[3][3])
{
#pragma HLS INLINE off
	PIXEL array[9]={WB[0][0], WB[0][1], WB[0][2],
					WB[1][0], WB[1][1], WB[1][2],
					WB[2][0], WB[2][1], WB[2][2]};
#pragma HLS ARRAY_PARTITION variable = array complete dim = 1

	PIXEL max = array[0];
	for (ap_uint<4> c = 1 ; c < 9; c++)
	{
#pragma HLS unroll
		if(array[c] > max)
		{
			max = array[c];
		}
	}
	return max;
}

static void jFdilate(hls::stream<PIXEL>& gray, hls::stream<PIXEL>& dilate, int rows, int cols)
{
	PIXEL _dilate;

	PIXEL LineBuffer[3][WIDTH];
#pragma HLS ARRAY_PARTITION variable=LineBuffer complete dim=1

	PIXEL WindowBuffer[3][3];
#pragma HLS ARRAY_PARTITION variable=WindowBuffer complete dim=0

	ap_uint<13> row, col;
	ap_uint<2> lb_r_i;
	ap_uint<2> top, mid, btm;//linebuffer row index

	for(col = 0; col < cols; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS pipeline
		LineBuffer[0][col] = 0;
		gray >> LineBuffer[1][col];
	}

	lb_r_i = 2;
	for(row = 1; row < rows + 1; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		if(lb_r_i == 2)
		{
			top = 0; mid = 1; btm = 2;
		}
		else if(lb_r_i == 0)
		{
			top = 1; mid = 2; btm = 0;
		}
		else if(lb_r_i == 1)
		{
			top = 2; mid = 0; btm = 1;
		}

		WindowBuffer[top][0] = WindowBuffer[top][1] = 0;
		WindowBuffer[mid][0] = WindowBuffer[top][1] = 0;
		WindowBuffer[btm][0] = WindowBuffer[top][1] = 0;

		for(col = 0; col < cols; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS pipeline
			if(row < rows)
				LineBuffer[btm][col] = gray.read();
			else
				LineBuffer[btm][col] = 0;

			WindowBuffer[0][2] = LineBuffer[top][col];
			WindowBuffer[1][2] = LineBuffer[mid][col];
			WindowBuffer[2][2] = LineBuffer[btm][col];
			_dilate = dilate_kernel(WindowBuffer);
			WindowBuffer[0][0] = WindowBuffer[0][1];
			WindowBuffer[1][0] = WindowBuffer[1][1];
			WindowBuffer[2][0] = WindowBuffer[2][1];
			WindowBuffer[0][1] = WindowBuffer[0][2];
			WindowBuffer[1][1] = WindowBuffer[1][2];
			WindowBuffer[2][1] = WindowBuffer[2][2];

			dilate << _dilate;

			lb_r_i++;
			if(lb_r_i == 3) lb_r_i = 0;
		}
	}
}


void jf_dilate(PIXEL* gray, PIXEL* dilate, int rows, int cols)
{
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	hls::stream<PIXEL> _gray;
	hls::stream<PIXEL> _dilate;
	ap_uint<13> row, col;
read:
	for(row=0; row<rows; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(col=0; col<cols; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			_gray.write(*(gray + row*cols + col));
		}
	}

	jFdilate(_gray, _dilate, rows, cols);

write:
	for(row=0; row<rows; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(col=0; col<cols; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			*(dilate + row*cols + col) = _dilate.read();
		}
	}
}

