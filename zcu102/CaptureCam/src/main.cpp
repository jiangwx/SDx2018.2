#include "xf_headers.h"
#include "xf_cvt_color_config.h"
using namespace std;

int main(int, char**)
{
    cv::Mat cv_yuyv,cv_rgba,cv_bgr;
	cv::Size S0(WIDTH,HEIGHT);
	cv_rgba.create(S0,CV_8UC4);
    static xf::Mat<XF_16UC1, HEIGHT, WIDTH, XF_NPPC1> xf_yuyv(HEIGHT,WIDTH);
	static xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> xf_rgba(HEIGHT,WIDTH);

	sds_utils::perf_counter cap_ctr,cv2xf_ctr,xf2cv_ctr,yuyv2rgba_ctr,imshow_ctr;

	
    //--- INITIALIZE VIDEOCAPTURE
    cv::VideoCapture cap;
    cap.open(0);
    // OR advance usage: select any API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    cap.open(deviceID + apiID);
    // check if we succeeded
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
    cv::namedWindow("USB Camera Grab",0);
    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
    	cap_ctr.start();
        cap.read(cv_yuyv);
        cap_ctr.stop();

        cv2xf_ctr.start();
		xf_yuyv.copyTo((unsigned short int*)cv_yuyv.data);
		cv2xf_ctr.stop();

		yuyv2rgba_ctr.start();
		cvtcolor_yuyv2rgba(xf_yuyv,xf_rgba);
		yuyv2rgba_ctr.stop();

		xf2cv_ctr.start();
		cv_rgba.data=xf_rgba.copyFrom();
		xf2cv_ctr.stop();

		imshow_ctr.start();
        cv::imshow("USB Camera Grab", cv_rgba);
        imshow_ctr.stop();
        if (cv::waitKey(5) >= 0)
            break;
    }
    printf("average cap cycles = %llu\n",cap_ctr.avg_cpu_cycles());
    printf("average cv2xf_ctr cycles = %llu\n",cv2xf_ctr.avg_cpu_cycles());
    printf("average yuyv2rgba cycles = %llu\n",yuyv2rgba_ctr.avg_cpu_cycles());
    printf("average xf2cv_ctr cycles = %llu\n",xf2cv_ctr.avg_cpu_cycles());
    printf("average imshow cycles = %llu\n",imshow_ctr.avg_cpu_cycles());

    return 0;
}
