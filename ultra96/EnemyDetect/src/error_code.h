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

#ifndef COMMON_ERROR_CODE_H
#define COMMON_ERROR_CODE_H



#include <string>

enum ErrorCode{
   OK = 0,
   Error = 1
};


class ErrorInfo {

 public:
  ErrorInfo():error_code_(ErrorCode::OK),error_msg_(""){};
  ErrorInfo(ErrorCode error_code, const std::string &error_msg=""):error_code_(error_code),error_msg_(error_msg){};

  ErrorInfo& operator= ( const ErrorInfo& error_info ) {
    if (&error_info != this) {
      error_code_ = error_info.error_code_;
      error_msg_ = error_info.error_msg_;
    }
    return *this;
  }

  static ErrorInfo OK(){
    return ErrorInfo();
  }

  ErrorCode error_code() const { return error_code_;};
  const std::string &error_msg() const { return error_msg_; }

  void SetErrorCode () {

  }

  bool IsOK() const{
    return (error_code_ == ErrorCode::OK);
  }

  ~ErrorInfo()= default;

 private:
  ErrorCode error_code_;
  std::string error_msg_;


};

#endif //COMMON_ERROR_CODE_H
