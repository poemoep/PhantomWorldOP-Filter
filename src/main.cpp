#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "phantomOP.hpp"

int main(int argc,char** argv){
	if(argc < 1){
		std::cout << "Input wmv video file as Argument" << std::endl;
		return -1;
	}

	
	if(to_wav(argv[1]) != 0){ 
		perror("to_wav");
		return -1;
	}

	char filename[] = "output.wav";
	int data[3];

	analyzBPM(filename,data);

	int i,count = 0;

	cv::Mat alpha;
	cv::Mat one;

//	for(i = 0;i < 3;i++) std::cout << data[i] << std::endl;

	for(i = 0;i < 3; i++){
		int flag = 0;

		cv::VideoCapture cap(argv[1]);

		cap >> one;
		sprintf(filename,"output%d.avi",i+1);
		cv::VideoWriter writer(filename,CV_FOURCC_DEFAULT,25,cv::Size(one.cols,one.rows),true);

	std::cout << filename << std::endl;		
		while(true){

		cv::Mat frame,output;
		if(flag == 0){
			frame = one;
			flag = 1;
		}else{
			cap >> frame;
		}

		if(frame.empty()) break;
		if(count == 0) alpha = createPix(frame.cols,frame.rows,15);
		count = (count + 1) % (int)( 30 * 60.0/data[i]);
		
		output = synthesis(frame,alpha);
		writer << output;
		
		}
	}
/*
	for(i = 0; i< 3;i++){
		sprintf(filename,"output%d.avi",i+1);
		if(to_avi(filename) != 0){
			perror("to_avi");
			return -1;
		}
	}
*/	return 0;
}
