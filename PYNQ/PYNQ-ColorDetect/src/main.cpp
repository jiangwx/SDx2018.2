#include "xf_headers.h"
#include "xf_cvt_color_config.h"
#include "xf_colordetect_config.h"
using namespace std;

int main(int, char**)
{
    cv::Mat cv_yuyv,cv_gray;
    cv::Size S0(WIDTH,HEIGHT);
    cv_yuyv.create(S0,CV_8UC4);
    cv_gray.create(S0,CV_8UC1);
    static xf::Mat<XF_16UC1, HEIGHT, WIDTH, XF_NPPC1> xf_yuyv(HEIGHT,WIDTH);
	static xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> xf_rgba(HEIGHT,WIDTH);

	sds_utils::perf_counter cap_ctr,cv2xf_ctr,xf2cv_ctr,yuyv2rgba_ctr,colordetect_ctr,imshow_ctr,bound_ctr;

    //--- INITIALIZE VIDEOCAPTURE
    cv::VideoCapture cap;
    cap.open(0);
    int deviceID = 0;
    int apiID = cv::CAP_ANY;
    cap.open(deviceID + apiID);
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
    cap.set(CV_CAP_PROP_MODE,CV_CAP_MODE_YUYV);

    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
         << "Press any key to terminate" << endl;

	unsigned char * high_thresh = (unsigned char *)sds_alloc_non_cacheable(9* sizeof(unsigned char));
	unsigned char * low_thresh = (unsigned char *)sds_alloc_non_cacheable(9* sizeof(unsigned char));
	low_thresh[0] = 22;
	low_thresh[1] = 150;
	low_thresh[2] = 60;

	high_thresh[0] = 38;
	high_thresh[1] = 255;
	high_thresh[2] = 255;

	low_thresh[3] = 38;
	low_thresh[4] = 150;
	low_thresh[5] = 60;

	high_thresh[3] = 75;
	high_thresh[4] = 255;
	high_thresh[5] = 255;

	low_thresh[6] = 160;
	low_thresh[7] = 150;
	low_thresh[8] = 60;

	high_thresh[6] = 179;
	high_thresh[7] = 255;
	high_thresh[8] = 255;

	static xf::Mat<XF_8UC4,HEIGHT,WIDTH,XF_NPPC1> imgInput(HEIGHT,WIDTH);
	static xf::Mat<XF_8UC4,HEIGHT,WIDTH,XF_NPPC1> hsvimage(HEIGHT,WIDTH);
	static xf::Mat<XF_8UC1,HEIGHT,WIDTH,XF_NPPC1> imgrange(HEIGHT,WIDTH);
	static xf::Mat<XF_8UC1,HEIGHT,WIDTH,XF_NPPC1> imgerode1(HEIGHT,WIDTH);
	static xf::Mat<XF_8UC1,HEIGHT,WIDTH,XF_NPPC1> imgdilate1(HEIGHT,WIDTH);
	static xf::Mat<XF_8UC1,HEIGHT,WIDTH,XF_NPPC1> imgdilate2(HEIGHT,WIDTH);
	static xf::Mat<XF_8UC1,HEIGHT,WIDTH,XF_NPPC1> imgOutput(HEIGHT,WIDTH);

	for (;;)
    {
		cap_ctr.start();
		cap.read(cv_yuyv);
		cap_ctr.stop();

		cv2xf_ctr.start();
		xf_yuyv.copyTo((unsigned short int*)cv_yuyv.data);
		cv2xf_ctr.stop();

		yuyv2rgba_ctr.start();
		cvtcolor_yuyv2rgba(xf_yuyv,xf_rgba);
		yuyv2rgba_ctr.stop();

		colordetect_ctr.start();
		colordetect_accel(xf_rgba,hsvimage,imgrange,imgerode1,imgdilate1,imgdilate2,imgOutput, low_thresh, high_thresh);
		colordetect_ctr.stop();

		xf2cv_ctr.start();
		cv_gray.data=imgOutput.copyFrom();
		xf2cv_ctr.stop();

		bound_ctr.start();
		cv::Rect rect = cv::boundingRect(cv_gray);
		printf("rectangle center position x = %d, y = %d\n ", (rect.tl().x +  rect.br().x)/2, (rect.tl().y +  rect.br().y)/2);
		bound_ctr.stop();

        if (cv::waitKey(5) >= 0) break;
    }

    printf("average cap cycles = %llu\n",cap_ctr.avg_cpu_cycles());
    printf("average cv2xf_ctr cycles = %llu\n",cv2xf_ctr.avg_cpu_cycles());
    printf("average yuyv2rgba cycles = %llu\n",yuyv2rgba_ctr.avg_cpu_cycles());
    printf("average colordetect cycles = %llu\n",colordetect_ctr.avg_cpu_cycles());
    printf("average xf2cv_ctr cycles = %llu\n",xf2cv_ctr.avg_cpu_cycles());
    printf("average imshow cycles = %llu\n",imshow_ctr.avg_cpu_cycles());
    printf("average bounding cycles = %llu\n",bound_ctr.avg_cpu_cycles());

    return 0;
}
