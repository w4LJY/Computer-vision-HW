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
Mat creat_image(Mat);
void Projective_mapping(Mat, Mat);
int det(int, int, int, int);

int main()
{
	//Image Read
	Mat left = imread("left.bmp", 1);
	Mat right = imread("right.bmp", 1);

	Mat bI = creat_image(left);
	imshow("", bI);
	Projective_mapping(right, bI);

	waitKey(0);

	return 0;
}
Mat creat_image(Mat I) {
	Mat bI = Mat::zeros(480, 900, CV_8UC3);
	int x0 = 80, y0 = 0;

	for (int x = 0; x < I.rows; x++)
		for (int y = 0; y < I.cols; y++)
			bI.at<Vec3b>(x + x0, y + y0) = I.at<Vec3b>(x, y);
	return bI;
}
void Projective_mapping(Mat rightI, Mat bI) {
	//###  Inverse Warping  ###//
	int x0, y0, x1, y1, x2, y2, x3, y3;
	int dx1, dy1, dx2, dy2, sx, sy;
	//------case 1-----//
	double A, B, C, D, E, F, G, H, I;
	double aa, bb, cc, dd, ee, ff, gg, hh, ii;
	//x0, y0, x1, y1, x2, y2, x3, y3 左上角開始逆時鐘
	x0 = 130;
	y0 = 250;
	x1 = 470;
	y1 = 310;
	x2 = 475;
	y2 = 900;
	x3 = 0;
	y3 = 770;
	//dx1, dy1, dx2, dy2, sx, sy
	dx1 = x1 - x2;
	dy1 = y1 - y2;
	dx2 = x3 - x2;
	dy2 = y3 - y2;
	sx = x0 - x1 + x2 - x3;
	sy = y0 - y1 + y2 - y3;
	//aa,bb,cc,dd,ee,ff,gg,hh,ii
	gg = det(sx, sy, dx2, dy2) / (double)det(dx1, dy1, dx2, dy2);
	hh = det(dx1, dy1, sx, sy) / (double)det(dx1, dy1, dx2, dy2);
	ii = 1;
	aa = x1 - x0 + gg * x1;
	bb = x3 - x0 + hh * x3;
	cc = x0;
	dd = y1 - y0 + gg * y1;
	ee = y3 - y0 + hh * y3;
	ff = y0;
	//A, B, C, D, E, F, G, H, I
	A = ee * ii - ff * hh;
	B = cc * hh - bb * ii;
	C = bb * ff - cc * ee;
	D = ff * gg - dd * ii;
	E = aa * ii - cc * gg;
	F = cc * dd - aa * ff;
	G = dd * hh - ee * gg;
	H = bb * gg - aa * hh;
	I = aa * ee - bb * dd;
	//------case 2-----//
	double a, b, c, d, e, f, g, h, i;
	//x0, y0, x1, y1, x2, y2, x3, y3 左上角開始逆時鐘
	x0 = 0;
	y0 = 0;
	x1 = rightI.rows;
	y1 = 0;
	x2 = rightI.rows;
	y2 = rightI.cols;
	x3 = 0;
	y3 = rightI.cols;
	//dx1, dy1, dx2, dy2, sx, sy
	dx1 = x1 - x2;
	dy1 = y1 - y2;
	dx2 = x3 - x2;
	dy2 = y3 - y2;
	sx = x0 - x1 + x2 - x3;
	sy = y0 - y1 + y2 - y3;
	//a, b, c, d, e, f, g, h, i
	g = det(sx, sy, dx2, dy2) / (double)det(dx1, dy1, dx2, dy2);
	h = det(dx1, dy1, sx, sy) / (double)det(dx1, dy1, dx2, dy2);
	i = 1;
	a = x1 - x0 + g * x1;
	b = x3 - x0 + h * x3;
	c = x0;
	d = y1 - y0 + g * y1;
	e = y3 - y0 + h * y3;
	f = y0;
	
	//------case 3-----//
	//Transition matrix
	double tA, tB, tC, tD, tE, tF, tG, tH, tI;
	tA = A * a + D * b + G * c;
	tB = B * a + E * b + H * c;
	tC = C * a + F * b + I * c;
	tD = A * d + D * e + G * f;
	tE = B * d + E * e + H * f;
	tF = C * d + F * e + I * f;
	tG = A * g + D * h + G * i;
	tH = B * g + E * h + H * i;
	tI = C * g + F * h + I * i;

	int x, y;
	for (int X = 0; X < bI.rows; X++) {
		for (int Y = 0; Y < bI.cols; Y++) {
			x = (tA*X+tB*Y+tC)/(tG*X+tH*Y+tI);
			y = (tD*X+tE*Y+tF)/(tG*X+tH*Y+tI);
			if (x >= 0 && x < rightI.rows && y >= 0 && y < rightI.cols) {
				bI.at<Vec3b>(X, Y)[0] = rightI.at<Vec3b>(x, y)[0];
				bI.at<Vec3b>(X, Y)[1] = rightI.at<Vec3b>(x, y)[1];
				bI.at<Vec3b>(X, Y)[2] = rightI.at<Vec3b>(x, y)[2];
			}
		}
	}
	imshow("", bI);

}
int det(int a, int b, int c, int d) {
	return a * d - b * c;
}
