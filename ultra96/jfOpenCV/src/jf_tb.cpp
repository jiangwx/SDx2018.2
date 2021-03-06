#include "jf_headers.h"

void dilate(cv::Mat& gray, cv::Mat& dilate)
{
	timeval start,end;
	static PIXEL* g=(PIXEL*)sds_alloc(sizeof(PIXEL)*gray.rows*gray.cols);
	gettimeofday(&start, NULL);
	memcpy(g, gray.data, sizeof(PIXEL)*gray.rows*gray.cols);
	jf_dilate(g, dilate.data, gray.rows, gray.cols);
	gettimeofday(&end, NULL);
	printf(" jf_dilate();\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));
}

void bgr2gray(cv::Mat& bgr, cv::Mat& gray)
{
	timeval start,end;
	gettimeofday(&start, NULL);
	static PIXEL* b=(PIXEL*)sds_alloc(sizeof(PIXEL)*bgr.rows*bgr.cols);
	static PIXEL* g=(PIXEL*)sds_alloc(sizeof(PIXEL)*bgr.rows*bgr.cols);
	static PIXEL* r=(PIXEL*)sds_alloc(sizeof(PIXEL)*bgr.rows*bgr.cols);
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

void threshold(cv::Mat& gray, cv::Mat& binary, PIXEL threshold, PIXEL maxval)
{
	timeval start,end;
	static PIXEL* g=(PIXEL*)sds_alloc(sizeof(PIXEL)*gray.rows*gray.cols);
	gettimeofday(&start, NULL);
	memcpy(g, gray.data, sizeof(PIXEL)*gray.rows*gray.cols);
	jf_threshold(g, binary.data, gray.rows, gray.cols, threshold, maxval);
	gettimeofday(&end, NULL);
	printf(" jf_threshold();\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

}

void pre_process(cv::Mat& bgr, cv::Mat& AND, PIXEL threshold, PIXEL maxval)
{

	timeval start,end;

	static PIXEL* b=(PIXEL*)sds_alloc(sizeof(PIXEL)*bgr.rows*bgr.cols);
	static PIXEL* g=(PIXEL*)sds_alloc(sizeof(PIXEL)*bgr.rows*bgr.cols);
	static PIXEL* r=(PIXEL*)sds_alloc(sizeof(PIXEL)*bgr.rows*bgr.cols);
	static PIXEL* color=(PIXEL*)sds_alloc(sizeof(PIXEL)*bgr.rows*bgr.cols);
	static PIXEL* bright=(PIXEL*)sds_alloc(sizeof(PIXEL)*bgr.rows*bgr.cols);


	gettimeofday(&start, NULL);
	cvMat2array(bgr, b, g, r);
	gettimeofday(&end, NULL);
	printf(" cvMat2array(bgr, b, g, r);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	gettimeofday(&start, NULL);
//#pragma SDS async(1)
	jf_color_pre(b, g, r, color, bgr.rows, bgr.cols, threshold, maxval);
//#pragma SDS async(2)
	jf_bright_pre(b, g, r, bright, bgr.rows, bgr.cols, threshold, maxval);
//#pragma SDS wait(1)
//#pragma SDS wait(2)
	gettimeofday(&end, NULL);
	printf(" jf_pre();\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	gettimeofday(&start, NULL);
	jf_and(color, bright, AND.data, bgr.rows, bgr.cols);
	gettimeofday(&end, NULL);
	printf(" jf_and();\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));
}

void distilate(cv::Mat &src, cv::Mat &dst)
{
	std::vector<cv::Mat> bgr;
	cv::split(src, bgr);
	cv::subtract(bgr[0], bgr[2], dst);
}

int main(int argc, char** argv)
{

	if(argc != 2)
	{
		fprintf(stderr,"Invalid Number of Arguments!\nUsage:\n");
		fprintf(stderr,"<Executable Name> <input image path> \n");
		return -1;
	}

	cv::Mat jf_in,jf_and;
	timeval start,end;

	jf_in = cv::imread(argv[1], 1);

	if (jf_in.data == NULL)
	{
		fprintf(stderr,"Cannot open image at %s\n", argv[1]);
		return 0;
	}
	jf_and.create(jf_in.rows,jf_in.cols,CV_8UC1);
	jf_and.data =(PIXEL*)sds_alloc(sizeof(PIXEL)*jf_in.rows*jf_in.cols);

	for(int i=0;i<10;i++)
	{
		pre_process(jf_in, jf_and, 200, 255);
	}
	cv::imwrite("jf_and.jpg", jf_and);

	//gettimeofday(&start, NULL);
	cv::Mat gray,bright,color,sub,binary,AND;
	cv::cvtColor(jf_in,gray,CV_BGR2GRAY);
	cv::threshold(gray,bright,200,255,CV_THRESH_BINARY);

	distilate(jf_in,sub);
	cv::threshold(sub,binary,200,255,CV_THRESH_BINARY);
	cv::Mat element=cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
	cv::dilate(binary,color,element,cv::Point(-1,-1),1);
	gettimeofday(&start, NULL);
	AND = color & bright;
	gettimeofday(&end, NULL);
	printf(" AND = color & bright; %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));
	//printf(" opencv pre-process took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));
	cv::imwrite("ocv_color.jpg", color);
	cv::imwrite("ocv_bright.jpg", bright);
	cv::imwrite("ocv_AND.jpg", AND);
	return 0;
}
