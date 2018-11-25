#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#undef __ARM_NEON__
#undef __ARM_NEON
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#define __ARM_NEON__
#define __ARM_NEON
#include "sds_lib.h"
#include "jf_bgr2gray_config.h"

void bgr2gray(cv::Mat& bgr, cv::Mat& gray)
{
	jf_rgb2gray(bgr.data,bgr.data + 1,bgr.data + 2,gray.data,bgr.rows,bgr.cols);
}

int main(int argc, char** argv)
{

	if(argc != 2)
	{
		fprintf(stderr,"Invalid Number of Arguments!\nUsage:\n");
		fprintf(stderr,"<Executable Name> <input image path> \n");
		return -1;
	}

	cv::Mat jf_in,jf_out;
	timeval start,end;

	// reading in the color image
	jf_in.data=(uint8*)sds_alloc(sizeof(uint8)*3*WIDTH*HEIGHT);
	jf_out.data=(uint8*)sds_alloc(sizeof(uint8)*WIDTH*HEIGHT);
	jf_in = cv::imread(argv[1], 1);
	printf("img step %d\n",(int)jf_in.step);
	if (jf_in.data == NULL)
	{
		fprintf(stderr,"Cannot open image at %s\n", argv[1]);
		return 0;
	}

	jf_out.create(jf_in.rows,jf_in.cols,CV_8UC1);
	//cv::cvtColor(jf_in,jf_out,CV_BGR2GRAY);
	bgr2gray(jf_in,jf_out);
	cv::imwrite("out_ocv.jpg", jf_out);

	return 0;
}
