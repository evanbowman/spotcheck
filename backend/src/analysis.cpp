#include "analysis.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
// threshold for canny edge detection
const int THRESH = 100;

cv::RNG rng(12345);




int find_background(cv::Mat & src, cv::Mat & mask) {
  long sum = 0;
  int  quant = 0;
  for(int i=0; i < mask.rows ; ++i){
    for( int j = 0; j < mask.cols; ++j){
      if(mask.at<unsigned char>(i,j)==0){
          sum += src.at<unsigned char>(i,j);
          quant++;
      }
    }
  }
  return sum / quant;
}

int find_area(cv::Mat & src, cv::Mat & mask) {
  int  quant = 0;
  for(int i=0; i < mask.rows ; ++i){
    for( int j = 0; j < mask.cols; ++j){
      if(mask.at<unsigned char>(i,j)>0){
          quant++;
      }
    }
  }
  return quant;
}

unsigned char find_max_height(cv::Mat & src, cv::Mat & mask, int bgHeight) {
  unsigned char max = 0;
  for(int i=0; i < mask.rows ; ++i){
    for( int j = 0; j < mask.cols; ++j){
      if(mask.at<unsigned char>(i,j)>0){
          if(max < mask.at<unsigned char>(i,j)){
            max = mask.at<unsigned char>(i,j)-bgHeight;
          }
      }
    }
  }
  return max;
}

unsigned char find_min_height(cv::Mat & src, cv::Mat & mask, int bgHeight) {
  unsigned char min = 255;
  for(int i=0; i < mask.rows ; ++i){
    for( int j = 0; j < mask.cols; ++j){
      if(mask.at<unsigned char>(i,j)>0){
          if(min > mask.at<unsigned char>(i,j)){
            min = mask.at<unsigned char>(i,j)-bgHeight;
          }
      }
    }
  }
  return min;
}

long find_volume(cv::Mat & src, cv::Mat & mask, int bgHeight){
  long volume = 0;

  for(int i=0; i < mask.rows ; ++i){
    for(int j = 0; j < mask.cols; ++j){
      if(mask.at<unsigned char>(i,j)>0){
          volume += src.at<unsigned char>(i,j) - bgHeight;
      }
    }
  }

  return volume;
}

long find_average_height(cv::Mat & src, cv::Mat & mask, int bgHeight){
  long volume = 0;
  long quant  = 0;

  for(int i=0; i < mask.rows ; ++i){
    for(int j = 0; j < mask.cols; ++j){
      if(mask.at<unsigned char>(i,j)>0){
          volume += src.at<unsigned char>(i,j) - bgHeight;
          quant++;
      }
    }
  }

  return volume/quant;
}

void find_circularity(cv::Mat & mask){

  //Do canny to get the edges of the mask,
  cv::Mat canny_output;
  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::Canny(mask,canny_output, THRESH, THRESH*2, 3);

  //find countours
  cv::findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );

  std::cout << "#found curves" <<contours.size() << std::endl;




  // auto perimeter = cv::cvArcLength();






  /////////

}
