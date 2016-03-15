#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "phantomOP.hpp"
cv::Mat synthesis(cv::Mat main, cv::Mat input_alpha){
	int i,j,k;
	cv::Mat pic = cv::Mat(main.size(),CV_8UC3);

	for(i = 0;i < input_alpha.rows;i++){
		for(j = 0;j < input_alpha.cols;j++){
			cv::Vec4b px = input_alpha.at<cv::Vec4b>(i,j);
			if(px[3] == 0){
				pic.at<cv::Vec3b>(i,j) = main.at<cv::Vec3b>(i,j);
			}else{
				cv::Vec3b m = main.at<cv::Vec3b>(i,j);
				cv::Vec3b p = pic.at<cv::Vec3b>(i,j);
				float alpha = (float)px[3]/255.0;
				for(k = 0;k < 3;k++) p[k] = (1.0 - alpha) * m[k] + alpha * px[k];
				pic.at<cv::Vec3b>(i,j) = p;
			}
		}
	}
	


  // 表示
	return pic;
}
/*
int main(int argc,char** argv){

	cv::Mat bg = cv::imread(argv[1],1);
	cv::Mat al = cv::imread(argv[2],-1);

	cv::Mat output;
	output = synthesis(bg,al);
  	cv::imwrite("blend.png",output);
}*/
