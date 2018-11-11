#include "lenet5.h"

#define TEST 1
char synset[10]={'0','1','2','3','4','5','6','7','8','9'};
sds_utils::perf_counter hw_ctr,pre_process;

layer input = { "data", 28,28,1,28,28,1,0,0,0};

int main(int argc, char **argv)
{
	char* image_path = argv[1];
	int label;

	cv::Mat src_img;
	src_img=cv::imread(argv[1]);
	cv::Mat resized_img;
	cv::Mat morphologyExed_img;
	cv::Mat gray_img;
	cv::Mat binarized_img;

	pre_process.start();
	cv::resize(src_img, resized_img, cv::Size(IMG_W, IMG_H), (0, 0), (0, 0), cv::INTER_LINEAR);
	cv::morphologyEx(resized_img, morphologyExed_img, cv::MORPH_OPEN, cv::Mat::ones(2, 2, CV_8U));
	cv::Mat inversed_img = cv::Scalar(255, 255, 255) - morphologyExed_img;
	cv::cvtColor(inversed_img, gray_img, cv::COLOR_BGR2GRAY);
	cv::threshold(gray_img, binarized_img, 150, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
	pre_process.stop();
	printf("pre-process cost %llu cycles\n", pre_process.avg_cpu_cycles());
	//cv::Mat resized_binarized_img;
	//cv::resize(binarized_img, resized_binarized_img, cv::Size(IMG_W, IMG_H), (0, 0), (0, 0), cv::INTER_LINEAR);

	DTYPE *blob = (DTYPE *)sds_alloc(sizeof(DTYPE)*IMG_W*IMG_H);
	cvMat2array(binarized_img,blob);
	Lenet5_init();
	load_weight();
	hw_ctr.start();
	label=Lenet5(blob);
	hw_ctr.stop();
	printf("the number is %d\n", label);

}
