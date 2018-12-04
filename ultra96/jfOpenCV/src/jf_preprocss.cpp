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
void jFthreshold(hls::stream<PIXEL>& gray, hls::stream<PIXEL>& binary, int rows, int cols, PIXEL threshold, PIXEL maxval)
{
	PIXEL _gray,_binary;
	for(int row = 0; row < rows; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int col = 0; col < cols; col++)
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

void jf_color_pre(PIXEL* b, PIXEL* g, PIXEL* r, PIXEL* dilate, int rows, int cols, PIXEL threshold, PIXEL maxval)
{
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	hls::stream<PIXEL> _b;
	hls::stream<PIXEL> _g;
	hls::stream<PIXEL> _r;
	hls::stream<PIXEL> _sub;
	hls::stream<PIXEL> _binary;
	hls::stream<PIXEL> _dilate;

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

	jFsubtract(_b, _r, _sub, rows, cols);
	jFthreshold(_sub, _binary, rows, cols, threshold, maxval);
	jFdilate(_binary, _dilate, rows, cols);

write:
	for(int i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			*(dilate + i*cols + j) = _dilate.read();
		}
	}
}

void jf_bright_pre(PIXEL* b, PIXEL* g, PIXEL* r, PIXEL* binary, int rows, int cols, PIXEL threshold, PIXEL maxval)
{
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW
	hls::stream<PIXEL> _b;
	hls::stream<PIXEL> _g;
	hls::stream<PIXEL> _r;
	hls::stream<PIXEL> _gray;
	hls::stream<PIXEL> _binary;

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
	jFthreshold(_gray, _binary, rows, cols, threshold, maxval);

write:
	for(int i=0; i<rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		for(int j=0; j<cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
			*(binary + i*cols + j) = _binary.read();
		}
	}
}