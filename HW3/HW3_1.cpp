#define _CRT_SECURE_NO_DEPRECATE
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <cmath>

#define PI 3.14159265359
using namespace std;
using namespace cv;
void FishEye_Unwarp(Mat);

int main()
{
	//Image Read
	Mat distortion = imread("distortion.bmp", 1);
	FishEye_Unwarp(distortion);


	

	return 0;
}
void FishEye_Unwarp(Mat I) {
	int Hd = I.rows, Wd = I.cols;
	int i0 = Hd / 2, j0 = Wd / 2;
	double rd, ru, theta;
	double omega = 0.0036;
	//找到轉換後的大小Hu與Wu
	int Imax = 0, Jmax = 0;
	for (int i = 0; i < Hd; i++) {
		for (int j = 0; j < Wd; j++) {
			rd = sqrt(pow(i - i0, 2) + pow(j - j0, 2));
			theta = atan((i - i0) / (double)(j - j0));
			ru = tan(rd * omega) / (2 * tan(omega / 2));
			Imax = ru * sin(theta) > Imax ? round(ru * sin(theta)) : Imax;
			Jmax = ru * cos(theta) > Jmax ? round(ru * cos(theta)) : Jmax;
		}
	}
	int iu_0 = Imax, ju_0 = Jmax;
	int Hu = 2 * Imax, Wu = 2 * Jmax;
	int r = 20; //Hu,Wu的縮小比例
	int nHu = Hu / r, nWu = Wu / r;
	printf("轉換後的大小Hu:%d,Wu,%d，縮小%d倍\n", Hu, Wu,r);

	//建立一張強度為(128,128,128)的圖
	Mat UnI(nHu, nWu, CV_8UC3);
	for (int niu = 0; niu < nHu; niu++) {
		for (int nju = 0; nju < nWu; nju++) {
			UnI.at<Vec3b>(niu, nju)[0] = 128;
			UnI.at<Vec3b>(niu, nju)[1] = 128;
			UnI.at<Vec3b>(niu, nju)[2] = 128;
		}
	}

	int iu, ju, id, jd;
	for (int niu = 0; niu < nHu; niu++) {
		for (int nju = 0; nju < nWu; nju++) {
			iu = r * niu, ju = r * nju;
			ru = sqrt(pow(iu - iu_0, 2) + pow(ju - ju_0, 2));
			theta = atan2((iu - iu_0), (ju - ju_0));
			rd = (1 / omega) * atan(2 * ru * tan(omega / 2));
			id = round(rd * sin(theta)) + i0;
			jd = round(rd * cos(theta)) + j0;
			if (id >= 0 && id < Hd && jd >= 0 && jd < Wd) {
				UnI.at<Vec3b>(niu, nju)[0] = I.at<Vec3b>(id, jd)[0];
				UnI.at<Vec3b>(niu, nju)[1] = I.at<Vec3b>(id, jd)[1];
				UnI.at<Vec3b>(niu, nju)[2] = I.at<Vec3b>(id, jd)[2];
			}
		}
	}
	imshow("result", UnI);

	int w = 1000, h = 800;
	Mat s_UnI(h, w, CV_8UC3);
	int iu_start = Imax - h / 2, ju_start = Jmax - w / 2;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			iu = i + iu_start;
			ju = j + ju_start;
			ru = sqrt(pow(iu - Imax, 2) + pow(ju - Jmax, 2));
			theta = atan2((iu - Imax), (ju - Jmax));
			rd = (1 / omega) * atan(2 * ru * tan(omega / 2));
			id = round(rd * sin(theta)) + i0;
			jd = round(rd * cos(theta)) + j0;
			if (id >= 0 && id < Hd && jd >= 0 && jd < Wd) {
				s_UnI.at<Vec3b>(i, j)[0] = I.at<Vec3b>(id, jd)[0];
				s_UnI.at<Vec3b>(i, j)[1] = I.at<Vec3b>(id, jd)[1];
				s_UnI.at<Vec3b>(i, j)[2] = I.at<Vec3b>(id, jd)[2];
			}
		}
	}
	imshow("result2", s_UnI);

	waitKey(0);
}
