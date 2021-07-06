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
void world_Unwarp(Mat);
double cot(double);

int main()
{
	//Image Read
	Mat road = imread("road.bmp", 1);
	world_Unwarp(road);
	waitKey(0);

	return 0;
}
void world_Unwarp(Mat I) {
	int m = I.rows, n = I.cols;
	int x, y;
	double alpha = 15 * PI / 180, dx = 4, dy = -10, dz = 5, gamma0 = 0, theta0 = 0.025;
	//找到轉換後的大小Hu與Wu
	int Xmax = 0, Ymax = 0;
	int Xmin = 9999999, Ymin = 9999999;

	int uHorizon = ceil((m - 1) * (-theta0 + alpha) / (double)(2 * alpha));

	for (int u = uHorizon; u < m; u++) {
		for (int v = 0; v < n; v++) {
			x = round(dz * cot(theta0 - alpha + u * ((2 * alpha) / (double)(m - 1))) * sin(gamma0 - alpha + v * ((2 * alpha) / (double)(n - 1))) + dx);
			y = round(dz * cot(theta0 - alpha + u * ((2 * alpha) / (double)(m - 1))) * cos(gamma0 - alpha + v * ((2 * alpha) / (double)(n - 1))) + dy);
			Xmax = x > Xmax ? x : Xmax;
			Ymax = y > Ymax ? y : Ymax;
			Xmin = x < Xmin ? x : Xmin;
			Ymin = y < Ymin ? y : Ymin;
		}
	}
	int M = Xmax - Xmin, N = Ymax - Ymin;
	int r = 50; //Hu,Wu的縮小比例
	int nM = M / r, nN = N / r;
	printf("轉換後的大小Hu:%d,Wu,%d縮小%d倍\n", M, N,r);


	//建立一張強度為(128,128,128)的圖
	Mat UnI(nM, nN, CV_8UC3);
	for (int nx = 0; nx < nM; nx++) {
		for (int ny = 0; ny < nN; ny++) {
			UnI.at<Vec3b>(nx, ny)[0] = 128;
			UnI.at<Vec3b>(nx, ny)[1] = 128;
			UnI.at<Vec3b>(nx, ny)[2] = 128;
		}
	}
	

	int iI, jI;
	int u, v;
	for (int nx = 0; nx < nM; nx++) {
		for (int ny = 0; ny < nN; ny++) {
			x = r * nx + Xmin, y = r * ny;
			u = round(((m - 1) / (double)(2 * alpha)) * (atan(dz * sin(atan((x - dx) / (double)(y - dy))) / (double)(x - dx)) - theta0 + alpha));
			v = round(((n - 1) / (double)(2 * alpha)) * (atan((x - dx) / (double)(y - dy)) - gamma0 + alpha));
			if (u >= uHorizon && u < m && v >= 0 && v < n) {
				UnI.at<Vec3b>(nx, ny)[0] = I.at<Vec3b>(u, v)[0];
				UnI.at<Vec3b>(nx, ny)[1] = I.at<Vec3b>(u, v)[1];
				UnI.at<Vec3b>(nx, ny)[2] = I.at<Vec3b>(u, v)[2];
			}
		}
	}
	imshow("result", UnI);

	int w = 800, h = 400;
	Mat s_UnI(h, w, CV_8UC3);
	int x_start = (M/2) - h / 2, y_start = 0;
	int nx, ny;

	for (int x = 0; x < h; x++) {
		for (int y = 0; y < w; y++) {
			nx = x + x_start + Xmin;
			ny = y + y_start;
			u = round(((m - 1) / (double)(2 * alpha)) * (atan(dz * sin(atan((nx - dx) / (double)(ny - dy))) / (double)(nx - dx)) - theta0 + alpha));
			v = round(((n - 1) / (double)(2 * alpha)) * (atan((nx - dx) / (double)(ny - dy)) - gamma0 + alpha));
			if (u >= uHorizon && u < m && v >= 0 && v < n) {
				s_UnI.at<Vec3b>(x, y)[0] = I.at<Vec3b>(u, v)[0];
				s_UnI.at<Vec3b>(x, y)[1] = I.at<Vec3b>(u, v)[1];
				s_UnI.at<Vec3b>(x, y)[2] = I.at<Vec3b>(u, v)[2];
			}
		}
	}
	imshow("result2", s_UnI);
}
double cot(double x) {
	return 1 / tan(x);
}