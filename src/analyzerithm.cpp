#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include "phantomOP.hpp"
double han_window(const int i, size_t size)
{
	return 0.5 - 0.5 * cos(2.0 * M_PI * i / size);
}

double power(const double re, const double im)
{
	return re*re + im*im;
}

void find_peak3(const double* a, size_t size, int *max_idx)
{
	max_idx[0] = -1;
	max_idx[1] = -1;
	max_idx[2] = -1;
	double dy = 0;
	for (size_t i = 1; i < size; ++i)
	{
		double dy_pre = dy;
		dy = a[i] - a[i-1];
		if (dy_pre > 0 && dy <= 0)
		{
			if (max_idx[0] < 0 || a[i-1] > a[max_idx[0]])
			{
				max_idx[2] = max_idx[1];
				max_idx[1] = max_idx[0];
				max_idx[0] = i-1;
			}
			else if (max_idx[1] < 0 || a[i-1] > a[max_idx[1]])
			{
				max_idx[2] = max_idx[1];
				max_idx[1] = i-1;
			}
			else if (max_idx[2] < 0 || a[i-1] > a[max_idx[2]])
			{
				max_idx[2] = i-1;
			}
		}
	}
}

struct bpmdata {
        int bpm;
        double start_time;
        double start_beat;
        int cubase[2];
};


void analyzBPM(char* filename,int *bdatas){
	char c;
	char beforec = 0;
 	const int FRAME_LEN = 512;
  	double frame[FRAME_LEN];


	std::ifstream ifs(filename,std::ios::in | std::ios::binary);
	
	int ij;
	int dsize[4];
	while(1){
		ifs.read(&c,sizeof(char));
		if(beforec == 't' && c == 'a'){
			break;
		}else{
			beforec = c;
		}
        }

        for(ij=0;ij<4;ij++){
		ifs.read(&c,sizeof(char));
              	dsize[ij] = (int)(unsigned char)c;
        }

	long datasize = 0;

        for(ij=0;ij<4;ij++){
                datasize += (dsize[ij] - ((int)(dsize[ij]/16) * 16)) * pow(16.0,2.0 * ij);

                datasize += (int)(dsize[ij]/16) * pow(16.0,2.0 * ij + 1);
        }

  int N = datasize/sizeof(short) / 2 / FRAME_LEN;
  double *vol = new double[N];
//  ZeroMemory(vol, sizeof(double)*N);
	memset(vol,0,sizeof(double) * N);
//  printf("analyze time : %f sec\n", double(N) * FRAME_LEN / fmt->dwSamplesPerSec);

  unsigned long i = 0;
  int j = 0;
  int m = 0;
  while (i <= datasize/sizeof(short) && m < N)
  {
	ifs.read(&c,sizeof(char));
        frame[j] = (int)(unsigned char)c;
   // frame[j] = data[i];
    j++;
    if (j == FRAME_LEN)
    {
      // 音量(実効値)=sqrt(1/FRAME_LEN * Σframe[n]^2)
      double sum = 0;
      for (int n = 0; n < FRAME_LEN; n++)
      {
        sum += frame[n] * frame[n];
      }
      vol[m] = sqrt(sum / FRAME_LEN);
      m++;
      j = 0; // 次フレーム
    }
    i += 2;
  }

  // 音量差分(増加のみ)
  double *diff = new double[N]; // 音量差分
  diff[0] = vol[0];
  for (int i = 1; i < N; i++)
  {
    if (vol[i] - vol[i-1] > 0)
    {
      diff[i] = vol[i] - vol[i-1];
    } else {
      diff[i] = 0;
    }
  }

  //const double s = double(fmt->dwSamplesPerSec) / double(FRAME_LEN); // サンプリング周波数
	const double s = (double)44100/double(FRAME_LEN);
  // テンポ解析
  double *a = new double[240-60+1];
  double *b = new double[240-60+1];
  double *r = new double[240-60+1];
  for (int bpm = 60; bpm <= 240; bpm++)
  {
    double a_sum = 0;
    double b_sum = 0;
    double f = double(bpm) / 60;
    for (int n = 0; n < N; n++)
    {
      double win = han_window(n, N);
      a_sum += diff[n] * cos(2.0 * M_PI * f * n / s) * win;
      b_sum += diff[n] * sin(2.0 * M_PI * f * n / s) * win;
      // 注意：窓関数を使用しないと端の影響で誤差が出る
    }
    double a_tmp = a_sum / N;
    double b_tmp = b_sum / N;
    a[bpm-60] = a_tmp;
    b[bpm-60] = b_tmp;
    r[bpm-60] = sqrt(power(a_tmp, b_tmp));
  }

  // ピーク解析
  int peak_x[3];
  find_peak3(r, 240-60+1, peak_x);
  double start_n;
  double beat_gap;
  for (int idx = 0; idx < 3; idx++)
  {
    if (peak_x[idx] < 0)
    {
      printf("peakx < 0\n");
      break;
    }
    int peak_bpm = peak_x[idx] + 60;
	
	bdatas[idx] = peak_bpm;
    }
// 位相差
/*    double theta = atan2(b[peak_x[idx]], a[peak_x[idx]]);
    if (theta < 0)
    {
      theta += 2.0 * M_PI;
    }
    double peak_f = double(peak_bpm) / 60;
    double start_time = theta / (2.0 * M_PI * peak_f);
    double start_beat = theta / (2.0 * M_PI);
	bdatas[idx].start_time = start_time;
	bdatas[idx].start_beat = start_beat;
    double ajust_beat = (2.0 * M_PI - theta) / (2.0 * M_PI);
    int ajust_beat1 = int(ajust_beat*4) % 4;
    int ajust_beat2 = int(ajust_beat*4*120) % 120;
	bdatas[idx].cubase[0] = ajust_beat1 + 1;
	bdatas[idx].cubase[1] = ajust_beat2;
  }

*/
	
}
/*
int main(int argc,char** argv){

	if(argc < 2) return 0;

	struct bpmdata data[3];
	analyzBPM(argv[1],data);
	for (int i = 0; i<3; i++){
		printf("bpm:%d\tfirst_time:%f\tfirst_beat:%f\tcubase1:%d\tcubase2:%d\n",data[i].bpm,data[i].start_time,data[i].start_beat,data[i].cubase[0],data[i].cubase[1]);
	}
}*/
