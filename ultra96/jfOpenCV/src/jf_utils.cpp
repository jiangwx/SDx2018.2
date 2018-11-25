#include "jf_headers.h"

void cvMat2array(cv::Mat src, PIXEL* b, PIXEL* g, PIXEL* r)
{
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			b[i * src.cols+ j] = (PIXEL)src.data[i*src.cols*3 + j*3];
			g[i * src.cols+ j] = (PIXEL)src.data[i*src.cols*3 + j*3 + 1];
			r[i * src.cols+ j] = (PIXEL)src.data[i*src.cols*3 + j*3 + 2];
		}
	}
}
