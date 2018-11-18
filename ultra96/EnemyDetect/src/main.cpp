#include "xf_headers.h"
#include "cv_toolbox.h"
#include "enemy_detection_base.h"
#include "svm_predictor.h"

#include <stdio.h>
#include <iostream>


CVToolbox cv_toolbox_;
ErrorInfo error_info_;
SVMPredictor svm_predictor_;

/**
***GlobalVaribles***
**/
cv::Mat src_img_;
cv::Mat gray_img_;

cv::Mat show_lights_before_filter_;
cv::Mat show_lights_after_filter_;
cv::Mat show_armors_before_filter_;
cv::Mat show_armors_after_filter_;
cv::Mat show_enemies_merged_;

bool enable_debug_=0;
bool use_svm_=1;

//Filterlights
float light_min_aspect_ratio_=1.4;
float light_max_aspect_ratio_=10.0;
float light_regular_area_=100.0;
float light_min_area_=5.0;
float light_max_area_=5000.0;
float light_max_angle_=30.0;
float light_max_angle_diff_=25.0;

//Filterarmor
float armor_max_angle_=30.0;
float armor_min_area_=50.0;
float armor_max_area_=20000.0;
float armor_max_area_diff_=6.0;
float armor_min_aspect_ratio_=0.6;
float armor_max_aspect_ratio_=6.0;
float armor_max_pixel_val_=150.0;
float armor_max_stddev_=40;

/**
light_min_aspect_ratio:1.4
light_max_aspect_ratio:10.0
light_regular_area:100.0
light_min_area:5.0
light_max_area:5000.0
light_max_angle:30.0
light_max_angle_diff:25.0

armor_max_angle:30.0
armor_min_area:50.0
armor_max_area:20000.0
armor_max_area_diff:6.0
armor_min_aspect_ratio:0.6
armor_max_aspect_ratio:6.0
armor_max_pixel_val:150.0
armor_max_stddev:40
**/

Color Enemy_Color=Color::BLUE;


void drawText(cv::Mat &image);
ErrorInfo DetectEnemies(const cv::Mat &img,std::vector<EnemyInfo>&enemies);
void DetectLights(const cv::Mat &src,std::vector<cv::RotatedRect >&lights);
void FilterLights(std::vector<cv::RotatedRect >&lights);
void CalArmorInfo(std::vector<cv::Point2f >&armor_points,
cv::RotatedRect left_light,
cv::RotatedRect right_light);
void PossibleArmors(const std::vector<cv::RotatedRect >&lights,std::vector<ArmorInfo>&armors);
void FilterArmors(std::vector<ArmorInfo>&armors);
void PossibleEnemies(const std::vector<ArmorInfo>&armors,std::vector<EnemyInfo>&enemies);


int main()
{

	std::cout<<"Built with OpenCV"<<CV_VERSION<<std::endl;
	std::vector<EnemyInfo>enemies;
	timeval start,end;
	for(;;)
	{
		gettimeofday(&start,NULL);
		cv_toolbox_.NextImage(src_img_,0);
		printf("img height = %d, img width = %d\n",src_img_.rows,src_img_.cols);
		if(src_img_.empty())break;
		gettimeofday(&end,NULL);
		printf("cv_toolbox_.NextImage(src_img_,0);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

		gettimeofday(&start,NULL);
		DetectEnemies(src_img_,enemies);
		gettimeofday(&end,NULL);
		printf("DetectEnemies(src_img_,enemies);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

		gettimeofday(&start,NULL);
		drawText(show_armors_after_filter_);
		cv::imshow("Sample",show_armors_after_filter_);
		gettimeofday(&end,NULL);
		printf("drawText(show_armors_after_filter_);\n cv::imshow(Sample,show_armors_after_filter_);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

		if(cv::waitKey(10)>=0)break;
	}
	return 0;
}


void drawText(cv::Mat &image)
{
	cv::putText(image,"EnemyDetection!",cv::Point(20,50),cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(255,255,255),1,cv::LINE_AA);//linethicknessandtype
}





void DetectLights(const cv::Mat &src,std::vector<cv::RotatedRect >&lights)
{
	printf("----------------------------------void DetectLights()----------------------------------\n");
	cv::Mat light=cv_toolbox_.DistillationColor(src,Enemy_Color);
	cv::Mat binary_brightness_img;
	cv::Mat binary_color_img;
	cv::Mat binary_light_img;
	timeval start,end;
	//TODO(noah.guo):param
	gettimeofday(&start,NULL);
	cv::threshold(gray_img_,binary_brightness_img,200,255,CV_THRESH_BINARY);
	gettimeofday(&end,NULL);
	printf("cv::threshold(gray_img_,binary_brightness_img,200,255,CV_THRESH_BINARY);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	//TODO(noah.guo):param
	float thresh;
	if(Enemy_Color==Color::BLUE)
		thresh=90;
	else
		thresh=50;
	gettimeofday(&start,NULL);
	cv::threshold(light,binary_color_img,thresh,255,CV_THRESH_BINARY);
	gettimeofday(&end,NULL);
	printf("cv::threshold(light,binary_color_img,thresh,255,CV_THRESH_BINARY);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	gettimeofday(&start,NULL);
	cv::Mat element=cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
	gettimeofday(&end,NULL);
	printf("cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	gettimeofday(&start,NULL);
	cv::dilate(binary_color_img,binary_color_img,element,cv::Point(-1,-1),1);
	gettimeofday(&end,NULL);
	printf("cv::dilate(binary_color_img,binary_color_img,element,cv::Point(-1,-1),1);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	binary_light_img=binary_color_img&binary_brightness_img;
	if(enable_debug_)
		cv::imshow("binary_light_img",binary_light_img);

	gettimeofday(&start,NULL);
	std::vector<std::vector<cv::Point>>contours_light=cv_toolbox_.FindContours(binary_light_img);
	gettimeofday(&end,NULL);
	printf("cv_toolbox_.FindContours(binary_light_img);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	gettimeofday(&start,NULL);
	std::vector<std::vector<cv::Point>>contours_brightness=cv_toolbox_.FindContours(binary_brightness_img);
	gettimeofday(&end,NULL);
	printf("cv_toolbox_.FindContours(binary_brightness_img);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

	lights.reserve(contours_brightness.size());
	//TODO:Tobeoptimized
	std::vector<int>is_processes(contours_brightness.size());
	for(unsigned int i=0;i<contours_light.size();++i)
	{
		for(unsigned int j=0;j<contours_brightness.size();++j)
		{
			if(!is_processes[j])
			{
				if(cv::pointPolygonTest(contours_brightness[j],contours_light[i][0],true)>=0.0)
				{
					gettimeofday(&start,NULL);
					cv::RotatedRect single_light=cv::minAreaRect(contours_brightness[j]);
					gettimeofday(&end,NULL);
					printf("cv::minAreaRect(contours_brightness[j]);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

					lights.push_back(single_light);
					if(enable_debug_)
					cv_toolbox_.DrawRotatedRect(show_lights_before_filter_,single_light,cv::Scalar(100),2);
					is_processes[j]=true;
					break;
				}
			}
		}
	}
	if(enable_debug_){
		std::cout<<"DetectLights"<<std::endl;
		cv::imshow("lights_before_filter",show_lights_before_filter_);
	}
}


void FilterLights(std::vector<cv::RotatedRect >&lights)
{
	std::vector<cv::RotatedRect >rects;
	rects.reserve(lights.size());

	for(auto &light:lights)
	{
		float angle=0.0f;

		cv::Point2f points[4];
		light.points(points);
		std::sort(points,points+4,[](const cv::Point2f &p1,const cv::Point2f &p2){return p1.x<p2.x;});
		float height=cv::norm(points[0]-points[1]);
		float tan=(points[0].x==points[1].x)?1000:((points[0].y-points[1].y)/(float )(points[0].x-points[1].x));

		float width;
		if((points[0].y>points[1].y)==(points[2].y>points[3].y))
			width=cv::norm(points[0]-points[2]);
		else
			width=cv::norm(points[0]-points[3]);

		if(std::abs(tan)>1.5)
		{
			light.size.height=height;
			light.size.width=width;
			light.angle=std::atan(tan)*180/CV_PI+90;
			if(light.angle>=180)
			light.angle-=180;
		}
		else
		{
			light.size.height=width;
			light.size.width=height;
			light.angle=std::atan(tan)*180/CV_PI;
		}
		float light_aspect_ratio=light.size.height/(float )light.size.width;
		angle=std::abs(light.angle)>=45?90-std::abs(light.angle):std::abs(light.angle);
		//angle=std::abs(angle)>=45?90-std::abs(angle):std::abs(angle);

		if((light_aspect_ratio<light_max_aspect_ratio_||light.size.area()<light_regular_area_)&&light_aspect_ratio>light_min_aspect_ratio_&&angle<light_max_angle_&&light.size.area()>=light_min_area_&&light.size.area()<=light_max_area_)
		{
			rects.push_back(light);
			if(enable_debug_)
				cv_toolbox_.DrawRotatedRect(show_lights_after_filter_,light,cv::Scalar(100),2);
		}
	}
	if(enable_debug_)
	{
		std::cout<<"FilterLights"<<std::endl;
		cv::imshow("lights_after_filter",show_lights_after_filter_);
	}
	lights=rects;
}





void CalArmorInfo(std::vector<cv::Point2f >&armor_points,cv::RotatedRect left_light,cv::RotatedRect right_light)
{
	cv::Point2f left_points[4],right_points[4];
	left_light.points(left_points);
	right_light.points(right_points);
	cv::Point2f right_lu,right_ld,lift_ru,lift_rd;
	std::sort(left_points,left_points+4,[](const cv::Point2f &p1,const cv::Point2f &p2){return p1.x<p2.x;});
	std::sort(right_points,right_points+4,[](const cv::Point2f &p1,const cv::Point2f &p2){return p1.x<p2.x;});
	if(right_points[0].y<right_points[1].y)
	{
		right_lu=right_points[0];
		right_ld=right_points[1];
	}
	else
	{
		right_lu=right_points[1];
		right_ld=right_points[0];
	}

	if(left_points[2].y<left_points[3].y)
	{
		lift_ru=left_points[2];
		lift_rd=left_points[3];
	}
	else
	{
		lift_ru=left_points[3];
		lift_rd=left_points[2];
	}
	armor_points.push_back(lift_ru);
	armor_points.push_back(right_lu);
	armor_points.push_back(right_ld);
	armor_points.push_back(lift_rd);
}

void PossibleArmors(const std::vector<cv::RotatedRect >&lights,std::vector<ArmorInfo>&armors)
{
	for(const auto &light1:lights){
	for(const auto &light2:lights){
	auto edge1=std::minmax(light1.size.width,light1.size.height);
	auto edge2=std::minmax(light2.size.width,light2.size.height);
	float lights_dis=cv::norm(light1.center-light2.center);
	float center_angle=
	std::atan((light1.center.y-light2.center.y)/(light1.center.x-light2.center.x))*180/CV_PI;

	cv::RotatedRect rect;
	rect.angle=static_cast<float >(center_angle);
	rect.center.x=(light1.center.x+light2.center.x)/2;
	rect.center.y=(light1.center.y+light2.center.y)/2;
	float armor_width=static_cast<float >(lights_dis)+std::max(edge1.first,edge2.first);
	float armor_height=std::max<float >(edge1.second,edge2.second);

	rect.size.width=armor_width;
	rect.size.height=armor_height;

	float ratio=rect.size.width/(float )rect.size.height;
	auto area_dif=std::minmax(light1.size.area(),light2.size.area());

	//std::cout<<"light:"<<light1.angle<<","<<light2.angle<<std::endl;

	if(std::abs(((int)(light1.angle-light2.angle)%180+180)%180)<light_max_angle_diff_&&
	area_dif.second<=armor_max_area_diff_*area_dif.first&&
	std::abs(center_angle)<armor_max_angle_&&
	ratio<armor_max_aspect_ratio_&&
	ratio>armor_min_aspect_ratio_&&
	rect.size.area()>armor_min_area_&&
	rect.size.area()<armor_max_area_)
	{

	rect.size.height*=2.4;
	rect.size.width*=1.2;

	if(light1.center.x<light2.center.x){
	std::vector<cv::Point2f >armor_points;
	CalArmorInfo(armor_points,light1,light2);
	armors.emplace_back(ArmorInfo(rect,armor_points));
	if(enable_debug_)
	cv_toolbox_.DrawRotatedRect(show_armors_before_filter_,rect,cv::Scalar(100),2);
	armor_points.clear();
	}else{
	std::vector<cv::Point2f >armor_points;
	CalArmorInfo(armor_points,light2,light1);
	armors.emplace_back(ArmorInfo(rect,armor_points));
	if(enable_debug_)
	cv_toolbox_.DrawRotatedRect(show_armors_before_filter_,rect,cv::Scalar(100),2);
	armor_points.clear();
	}
	}
	}
	}
	if(enable_debug_){
	std::cout<<"PossibleArmors"<<std::endl;
	cv::imshow("armors_before_filter",show_armors_before_filter_);
	}
}


void FilterArmors(std::vector<ArmorInfo>&armors){

if(!use_svm_)return ;

std::vector<ArmorInfo>filtered_armors;

for(auto &armor:armors)
{
//std::cout<<"IDLE"<<std::endl;
cv::Rect roi=armor.rect.boundingRect();

if(0<=roi.x&&0<=roi.width&&roi.x+roi.width<=src_img_.cols&&0<=roi.y&&0<=roi.height&&roi.y+roi.height<=src_img_.rows){
cv::imwrite("dataset/"+std::to_string(cv::getTickCount())+".jpg",src_img_(roi));

//cv::Mat trans_mat=cv::getPerspectiveTransform(armor_iter->vertex,base_points);
//cv::Mat trans_out;
//cv::warpPerspective(src_img_,trans_out,trans_mat,cv::Size(base_points[2].x,base_points[2].y));

//cv::imwrite("dataset/"+std::to_string(cv::getTickCount())+".jpg",trans_out);

if(svm_predictor_.Predict(src_img_(roi)))
{
filtered_armors.emplace_back(armor);
//if(enable_debug_)
cv_toolbox_.DrawRotatedRect(show_armors_after_filter_,armor.rect,cv::Scalar(255,255,255),2);
//armor_iter++;
//continue;
}
//armors.erase(armor_iter);
}
armors=filtered_armors;
}

if(enable_debug_)
cv::imshow("armors_after_filter",show_armors_after_filter_);
}


void PossibleEnemies(const std::vector<ArmorInfo>&armors,std::vector<EnemyInfo>&enemies){
enemies.clear();
for(const auto &armor:armors){
enemies.emplace_back(EnemyInfo(cv::boundingRect(armor.vertex),armor));
}

return ;
}

ErrorInfo DetectEnemies(const cv::Mat &img,std::vector<EnemyInfo>&enemies)
{
	std::vector<cv::RotatedRect> lights;
	std::vector<ArmorInfo> armors;
	//staticcv::VideoWritervw("test.avi",CV_FOURCC_DEFAULT,24,cv::Size(480,640),true);

	timeval start,end;
	//TODO(noah.guo):param


	src_img_=img.clone();
	show_armors_after_filter_=src_img_.clone();
	if(!src_img_.empty())
	{
		printf("Begin to detect enemy!\n\r");
		gettimeofday(&start,NULL);
		cv::cvtColor(src_img_,gray_img_,CV_BGR2GRAY);
		gettimeofday(&end,NULL);
		printf("cv::cvtColor(src_img_,gray_img_,CV_BGR2GRAY);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));
		if(enable_debug_){
			show_lights_before_filter_=src_img_.clone();
			show_lights_after_filter_=src_img_.clone();
			show_armors_before_filter_=src_img_.clone();
			show_armors_after_filter_=src_img_.clone();
			show_enemies_merged_=src_img_.clone();
			cv::waitKey(1);
		}

		gettimeofday(&start,NULL);
		DetectLights(src_img_,lights);
		gettimeofday(&end,NULL);
		printf("DetectLights(src_img_,lights);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

		gettimeofday(&start,NULL);
		FilterLights(lights);
		gettimeofday(&end,NULL);
		printf("FilterLights(lights);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

		gettimeofday(&start,NULL);
		PossibleArmors(lights,armors);
		gettimeofday(&end,NULL);
		printf("PossibleArmors(lights,armors);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

		gettimeofday(&start,NULL);
		FilterArmors(armors);
		gettimeofday(&end,NULL);
		printf("FilterArmors(armors);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

		gettimeofday(&start,NULL);
		PossibleEnemies(armors,enemies);
		gettimeofday(&end,NULL);
		printf("PossibleEnemies(armors,enemies);\n took %lu us\n",(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec));

		//vw<<show_armors_after_filter_;
		lights.clear();
		armors.clear();
	}
	else
	{
		printf("Waiting for run camera driver...");
	}

return error_info_;
}




