#include <string>
#include <iostream>

#include "xf_headers.h"

using namespace cv;
using namespace std;
using namespace cv::ml;

class SVMPredictor
{
  public:
    SVMPredictor();

    void LordParam();

    bool Predict(cv::Mat patch);

  private:
    cv::Ptr<cv::ml::SVM> svm_model_;
    std::string model_name_;
    int resize_height_;
    int resize_width_;
};



SVMPredictor::SVMPredictor()
{
    LordParam();
}

void SVMPredictor::LordParam() {

    model_name_ = "blue.xml";
    resize_height_ = 48;
    resize_width_ = 64;

    svm_model_ = cv::ml::StatModel::load<cv::ml::SVM>(model_name_);
    if(svm_model_.empty())  cout<< "Cannot load svm model " << model_name_<<endl;
}

bool SVMPredictor::Predict(cv::Mat patch)
{
    Mat resize_image;
    timeval start,end;
    vector<float> descriptors;
    HOGDescriptor hog(Size(32, 32), Size(16, 16), Size(8, 8), Size(8, 8), 9);
    resize(patch, resize_image, Size(resize_width_, resize_height_));
	gettimeofday(&start,NULL);
    hog.compute(resize_image, descriptors);
	gettimeofday(&end,NULL);
	printf("hog.compute(resize_image, descriptors);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

    float pre_result = svm_model_->predict(descriptors);
    return pre_result;
}
