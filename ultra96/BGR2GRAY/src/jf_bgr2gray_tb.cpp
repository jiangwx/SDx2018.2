#include "jf_headers.h"

void bgr2gray(cv::Mat& bgr, cv::Mat& gray)
{
	timeval start,end;
	gettimeofday(&start, NULL);
	static PIXEL* b=(PIXEL*)sds_alloc(sizeof(PIXEL)*WIDTH*HEIGHT);
	static PIXEL* g=(PIXEL*)sds_alloc(sizeof(PIXEL)*WIDTH*HEIGHT);
	static PIXEL* r=(PIXEL*)sds_alloc(sizeof(PIXEL)*WIDTH*HEIGHT);
	gettimeofday(&end, NULL);
	printf(" sds_alloc();\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	gettimeofday(&start, NULL);
	cvMat2array(bgr, b, g, r);
	gettimeofday(&end, NULL);
	printf(" cvMat2array(bgr, b, g, r);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	gettimeofday(&start, NULL);
	jf_rgb2gray(b, g, r, gray.data,bgr.rows,bgr.cols);
	gettimeofday(&end, NULL);
	printf(" jf_rgb2gray(b, g, r, gray.data,bgr.rows,bgr.cols);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));
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
	jf_in = cv::imread(argv[1], 1);
	if (jf_in.data == NULL)
	{
		fprintf(stderr,"Cannot open image at %s\n", argv[1]);
		return 0;
	}


	jf_out.create(jf_in.rows,jf_in.cols,CV_8UC1);
	jf_out.data=(PIXEL*)sds_alloc(sizeof(PIXEL)*WIDTH*HEIGHT);

	for(int i=0;i<10;i++)
	{
		bgr2gray(jf_in,jf_out);
	}

	cv::imwrite("jf_out.jpg", jf_out);

	gettimeofday(&start, NULL);
	cv::cvtColor(jf_in,jf_out,CV_BGR2GRAY);
	gettimeofday(&end, NULL);
	printf(" cv::cvtColor(jf_in,jf_out,CV_BGR2GRAY);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));
	cv::imwrite("out_ocv.jpg", jf_out);

	return 0;
}
