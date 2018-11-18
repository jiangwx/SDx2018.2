/****************************************************************************
 *  Copyright (C) 2018 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#ifndef MODULES_DETECTION_CVTOOLBOX_H
#define MODULES_DETECTION_CVTOOLBOX_H

#include <vector>
#include <thread>
#include <mutex>
#include <iostream>
//opencv
#include "xf_headers.h"

//ros
//#include "image_transport/image_transport.h"
//#include "cv_bridge/cv_bridge.h"

//#include "messages/GimbalAngle.h"

//#include "modules/driver/camera/camera_param.h"
//#include "common/log.h"
//#include "common/game_info.h"


enum Color {
  BLUE = 0,
  RED = 1
};
/**
 * This class is a toolbox for RoboMaster detection.
 */
class CVToolbox {
 public:
  /**
   *  @brief The constructor of the CVToolbox.
   */
  CVToolbox(unsigned int buffer_size = 2,
            unsigned int camera_index = 0) :
      buffer_size_(buffer_size),
      camera_index_(camera_index) {

    //ros::NodeHandle nh;
    //gimbal_sub_ = nh.subscribe("gimbal", 1, &CVToolbox::ReceiveGimbal, this);

    //image_transport::ImageTransport it(nh);
    //unsigned long camera_num = camera_param_.GetCameraParam().size();  //rrts::driver::camera::CameraParam camera_param_;
    //subs_.resize(camera_num);                                          //std::vector<image_transport::Subscriber> subs_;

    unsigned int camera_num = 1;  //version1.0 we assume there is only one camera
    image_buffer_.resize(camera_num);                                          
    write_index_.resize(buffer_size);
    read_index_.resize(buffer_size);
    for (unsigned long i = 0; i < camera_num; i++) {
      image_buffer_[i].resize(buffer_size);
      write_index_[i] = 0;
      read_index_[i] = 1;
    }

    /*for (unsigned int i = 0; i < subs_.size(); i++) {
      std::string topic_name = "camera_" + std::to_string(i);
      subs_[i] = it.subscribe(topic_name, 20, boost::bind(&CVToolbox::ReceiveImg, this, _1, i));
    }*/
    for (unsigned int i = 0; i < camera_num; i++) {
        capture.open(i);
        ReceiveImg(i);
    }
  }
  

  void ReceiveImg(/*const sensor_msgs::ImageConstPtr &msg, */unsigned int camera_id) {
    cv::Mat image;
    capture >> image;
    image_buffer_[camera_id][write_index_[camera_id]] = image;
    lock_.lock();
    unsigned int tmp_index_ = write_index_[camera_id];
    write_index_[camera_id] = read_index_[camera_id];
    read_index_[camera_id] = tmp_index_;
    lock_.unlock();
  }
  /**
   * @brief Get next new image.
   * @param src_img Output image
   */
  void NextImage(cv::Mat &src_img, int camera_id) {
    ReceiveImg(camera_id);
    lock_.lock();
    image_buffer_.at(camera_id).at(read_index_.at(camera_id)).copyTo(src_img);
    lock_.unlock();
  }


  /**
   *  @brief Stop to read image.
   */
  void StopReadCamera() {
  }
  /**
   * @brief Highlight the blue or red region of the image.
   * @param image Input image ref
   * @return Single channel image
   */
  cv::Mat DistillationColor(const cv::Mat &src_img, Color color) {
    std::vector<cv::Mat> bgr;
    cv::split(src_img, bgr);
    if (color == Color::RED) {
      cv::Mat result_img;
      cv::subtract(bgr[2], bgr[1], result_img);
      return result_img;
    } else if (color == Color::BLUE) {
      cv::Mat result_img;
      cv::subtract(bgr[0], bgr[2], result_img);
      return result_img;
    }
  }
  /**
   * @brief The wrapper for function cv::findContours
   * @param binary binary image ref
   * @return Contours that found.
   */
  std::vector<std::vector<cv::Point> > FindContours(const cv::Mat &binary_img) {
    std::vector<std::vector<cv::Point> > contours;
    const int mode = CV_RETR_EXTERNAL;
    const int method = CV_CHAIN_APPROX_SIMPLE;
    cv::findContours(binary_img, contours, mode, method);
    return contours;
  }
  /**
   * @brief Draw rectangle.
   * @param img The image will be drew on
   * @param rect The target rectangle
   * @param color Rectangle color
   * @param thickness Thickness of the line
   */
  void DrawRotatedRect(const cv::Mat &img, const cv::RotatedRect &rect, const cv::Scalar &color, int thickness) {
    cv::Point2f vertex[4];

    rect.points(vertex);
    for (int i = 0; i < 4; i++)
      cv::line(img, vertex[i], vertex[(i + 1) % 4], color, thickness);
  }

  void DrawPoints(const cv::Mat &img, const std::vector<cv::Point2f> &points, const cv::Scalar &color, int thickness)
  {
    if (points.size() == 1) return;
    for (unsigned int i = 0; i < points.size(); i++)
      cv::line(img, points[i], points[(i + 1) % points.size()], color, thickness);
  }

  void DrawPoints(const cv::Mat &img, const std::vector<cv::Point2i> &points, const cv::Scalar &color, int thickness)
  {
    if (points.size() == 1) return;
    for (unsigned int i = 0; i < points.size(); i++)
      cv::line(img, points[i], points[(i + 1) % points.size()], color, thickness);
  }

  bool CheckPlain(cv::Mat img, std::vector<cv::Point> contour, float threshold = 40) {
    cv::Mat mean, stddev, mask;
    mask = cv::Mat::zeros(img.size(), CV_8U);
    cv::fillConvexPoly(mask, &contour[0], contour.size(), cv::Scalar(255));
    cv::meanStdDev(img, mean, stddev, mask);
    // cv::Scalar s = cv::sum(stddev);
    std::cout << " = " << stddev.at<cv::Vec4d>(0, 0)[0];
    return stddev.at<cv::Vec4d>(0, 0)[0] < threshold;
  }

  bool CalcCrossPoint(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3, cv::Point2f p4, cv::Point2f &cp)
  {
    cv::Point2f pt;
    // line1's cpmponent
    double X1 = p2.x - p1.x; //b1
    double Y1 = p2.y - p1.y; //a1
    // line2's cpmponent
    double X2 = p4.x - p3.x; //b2
    double Y2 = p4.y - p3.y; //a2
    // distance of 1,2
    double X21 = p3.x - p1.x;
    double Y21 = p3.y - p1.y;
    // determinant
    double D = Y1 * X2 - Y2 * X1; // a1b2-a2b1
    //
    if (D == 0)
      return false;
    // cross point
    pt.x = (X1 * X2 * Y21 + Y1 * X2 * p1.x - Y2 * X1 * p3.x) / D;
    // on screen y is down increased !
    pt.y = -(Y1 * Y2 * X21 + X1 * Y2 * p1.y - X2 * Y1 * p3.y) / D;

    // segments intersect.
    if ((abs(pt.x - p1.x - X1 / 2) <= abs(X1 / 2)) &&
        (abs(pt.y - p1.y - Y1 / 2) <= abs(Y1 / 2)) &&
        (abs(pt.x - p3.x - X2 / 2) <= abs(X2 / 2)) &&
        (abs(pt.y - p3.y - Y2 / 2) <= abs(Y2 / 2)))
    {
      cp = pt;
      return true;
    }
    return false;
  }

  void AdjustWhiteBalance(cv::Mat &img, cv::Mat &dst)
  {
    std::vector<cv::Mat> imageRGB;

    cv::split(img, imageRGB);

    double R, G, B;
    B = cv::mean(imageRGB[0])[0];
    G = cv::mean(imageRGB[1])[0];
    R = cv::mean(imageRGB[2])[0];

    double KR, KG, KB;
    KB = (R + G + B) / (3 * B);
    KG = (R + G + B) / (3 * G);
    KR = (R + G + B) / (3 * R);

    imageRGB[0] = imageRGB[0] * KB;
    imageRGB[1] = imageRGB[1] * KG;
    imageRGB[2] = imageRGB[2] * KR;

    cv::merge(imageRGB, dst);
  }

  void AdjustBrightness(cv::Mat &img, cv::Mat &dst)
  {
    // cv::equalizeHist(img, dst);
    std::vector<cv::Mat> imageRGB;

    cv::split(img, imageRGB);
    
    for(cv::Mat &cn : imageRGB) {
      cv::equalizeHist(cn, cn);
    }

    cv::merge(imageRGB, dst);
  }

private:

  unsigned int buffer_size_;

  std::vector<std::vector<cv::Mat> > image_buffer_;
  //rrts::driver::camera::CameraParam camera_param_;
  unsigned int camera_index_;
  std::vector<unsigned int> write_index_;
  std::vector<unsigned int> read_index_;
  std::mutex lock_;
  
  cv::VideoCapture capture;

  //messages::GimbalAngle gimbal_;

  //ros::Subscriber gimbal_sub_;
  //std::vector<image_transport::Subscriber> subs_;
};

#endif //MODULES_DETECTION_CVTOOLBOX_H
