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

#include "jf_config.h"

void cvMat2array(cv::Mat src, PIXEL* b, PIXEL* g, PIXEL* r);

#define WIDTH 	1280
#define HEIGHT	720
