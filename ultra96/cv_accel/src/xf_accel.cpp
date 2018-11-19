#include "xf_headers.h"
#include "xf_dilation_config.h"

void xf_dilate(cv::Mat &_src, cv::Mat &_dst)
{
	static xf::Mat<XF_8UC1, HEIGHT, WIDTH, NPPC> img_input(_src.rows,_src.cols);
	static xf::Mat<XF_8UC1, HEIGHT, WIDTH, NPPC> img_output(_src.rows,_src.cols);
	timeval start,end;

	gettimeofday(&start,NULL);
	img_input.copyTo(_src.data);
	gettimeofday(&end,NULL);
	printf(" img_input.copyTo(_src.data);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	gettimeofday(&start,NULL);
	dilation_accel(img_input,img_output);
	gettimeofday(&end,NULL);
	printf(" dilation_accel(img_input,img_output);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	gettimeofday(&start,NULL);
	_dst.data = img_output.copyFrom();
	//_dst.data = (unsigned char*)img_output.data;
	gettimeofday(&end,NULL);
	printf(" _dst.data = img_output.copyFrom();\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));
}
