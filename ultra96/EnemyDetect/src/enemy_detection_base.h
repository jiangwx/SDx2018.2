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

#ifndef MODULES_PERCEPTION_DETECTION_DETECTION_ENEMY_DETECTION_BASE_H
#define MODULES_PERCEPTION_DETECTION_DETECTION_ENEMY_DETECTION_BASE_H

#include <vector>
#include "error_code.h"
#include "xf_headers.h"

/**
 *  This class describes the armor information, including maximum bounding box, vertex, standard deviation.
 */
class ArmorInfo
{
public:
  ArmorInfo(cv::RotatedRect armor_rect, std::vector<cv::Point2f> armor_vertex, float armor_stddev = 0.0)
  {
    rect = armor_rect;
    vertex = armor_vertex;
    stddev = armor_stddev;
  };

  ArmorInfo() {
    
  };

public:
  cv::RotatedRect rect;
  std::vector<cv::Point2f> vertex;
  float stddev;
};

class EnemyInfo {
 public:
  EnemyInfo(cv::Rect rect, ArmorInfo armor) : rect(rect), armor(armor) {};
  EnemyInfo() {};
 public:
  cv::Rect rect;
  ArmorInfo armor;
};

class EnemyDetectionBase {
 public:
  EnemyDetectionBase();
  virtual void LoadParam();
  virtual ErrorInfo DetectEnemies(std::vector<EnemyInfo> &enemies) = 0;
  virtual ErrorInfo DetectEnemies(const cv::Mat &img, std::vector<EnemyInfo> &enemies) = 0;
  virtual ~EnemyDetectionBase() = default;
};

#endif //MODULES_PERCEPTION_DETECTION_DETECTION_ENEMY_DETECTION_BASE_H
