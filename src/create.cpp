#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
#include "phantomOP.hpp"
using namespace cv;
using namespace std;

#define WIDTH 16
#define HIGHT 9


Mat createPix(int w,int h,int times){
	int i,j;
	Mat main = Mat(Size(w,h),CV_8UC3,Scalar(0,0,0));
	Mat alpha = Mat(main.size(),CV_8UC3);
// ランダムピクセル化

	random_device rd;	//デバイス依存の乱数
	mt19937 mt(rd());	//rdを使ってメルセンヌツイスター初期化
	uniform_int_distribution<int> wpix(0,WIDTH - 1);
	uniform_int_distribution<int> hpix(0,HIGHT - 1);
	uniform_int_distribution<int> clr(0,255);

	int l = w / 16;
		cvtColor(main,alpha,CV_RGB2RGBA);

	for(int num = 0; num < times;num++){
		int startx = wpix(mt);
		int starty = hpix(mt);
		int r = clr(mt);
		int g = clr(mt);
		int b = clr(mt);
	
//	cout << startx << "," << starty << endl;
		
		
		for (i = starty * l;i < starty * l + l; i++) {
		        for (j = startx * l; j < startx * l + l; j++) {

//				if(i > alpha.rows || j > alpha.cols) continue;
				Vec4b pix = alpha.at<Vec4b>(i,j);
				pix[0] = b;
				pix[1] = g;
				pix[2] = r;
				alpha.at<Vec4b>(i,j) = pix;
        		}
    		}
	}

	for (i = 0; i < alpha.cols; i++) {
        	for (j = 0; j < alpha.rows; j++) {
        	    Vec4b px = alpha.at<Vec4b>(j, i);
        	    if (px[0] + px[1] + px[2] != 0) {
        	        px[3] = 150;
        	        alpha.at<Vec4b>(j, i) = px;
        	    }else{
        	        px[3] = 0;
        	        alpha.at<Vec4b>(j, i) = px;
               	    }
        	}
    	}	


	return alpha;
		
}
/*
int main(){
	Mat m;
	m = createPix(640,360,10);
	imwrite("test.png",m);
	return 0;
}*/
