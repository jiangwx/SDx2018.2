#include "jf_headers.h"

void cvMat2array(cv::Mat src, PIXEL4* bgr)
{
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			bgr[i * src.cols+ j].range(31, 24) = (PIXEL)src.data[i*src.cols*3 + j*3];
			bgr[i * src.cols+ j].range(23, 16)  = (PIXEL)src.data[i*src.cols*3 + j*3 + 1];
			bgr[i * src.cols+ j].range(15, 8)  = (PIXEL)src.data[i*src.cols*3 + j*3 + 2];
			bgr[i * src.cols+ j].range(7, 0)  = (PIXEL)0;
		}
	}
}
