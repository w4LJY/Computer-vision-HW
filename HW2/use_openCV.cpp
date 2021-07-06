#define _CRT_SECURE_NO_DEPRECATE
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <stdio.h>
#include <iostream>

#define PI 3.14159265359
using namespace std;
using namespace cv;

void finger_num(Mat, Mat, Mat);
void finger_hr(Mat, Mat,Mat);

int main222()
{

	//讀取影像
	Mat hand = imread("hand.bmp", 1);
	//轉灰階
	Mat Ghand;
	cvtColor(hand, Ghand, COLOR_BGR2GRAY);
	//二值化
	Mat Bhand;
	threshold(Ghand, Bhand, 230, 255, THRESH_BINARY);

	//黑白反轉
	bitwise_not(Bhand, Bhand);

	//手部的ConnectedComponents
	/*	nccomps:共幾類
		labels:原圖大小的標記圖
		stats:大小nccomps x 5,Mat<int>型態,依序存儲x,y,width,height,area
		centroids:大小nccomps x 2,Mat<double>型態,對應重心的x,y座標
	*/
	Mat labels, stats, centroids;
	
	
	int i, nccomps = connectedComponentsWithStats(Bhand, labels, stats, centroids);
	for (int i = 1; i < stats.rows; i++) {
		int x = stats.at<int>(i, 0);
		int y = stats.at<int>(i, 1);
		int width = stats.at<int>(i, 2);
		int height = stats.at<int>(i, 3);
		rectangle(hand, Point(x,y), Point(x + width,y+height),Scalar(0,0,255),2);
		circle(hand, Point(centroids.at<double>(i, 0), centroids.at<double>(i, 1)), 2, Scalar(0,0,255), FILLED);
		
	}
	
	
	
	
	//---形態學取出手指---//
	Mat dst;
	//erode mask=方型27x27
	Mat elementa = getStructuringElement(MORPH_RECT, Size(27, 27));
	erode(Bhand, dst, elementa);

	//dilate mask=橢圓27x27
	Mat elementb = getStructuringElement(MORPH_ELLIPSE, Size(27, 27));
	dilate(dst, dst, elementb);

	//灰階手指-形態學結果
	dst = Bhand - dst;

	//openning mask=橢圓15x15
	Mat element2 = getStructuringElement(MORPH_ELLIPSE, Size(15, 15));
	morphologyEx(dst, dst, MORPH_OPEN, element2);



	//手指的ConnectedComponents
	Mat labels2, stats2, centroids2;
	int j, nccomps2 = connectedComponentsWithStats(dst, labels2, stats2, centroids2);
	for (int i = 1; i < stats2.rows; i++) {
		int x = stats2.at<int>(i, 0);
		int y = stats2.at<int>(i, 1);
		int width = stats2.at<int>(i, 2);
		int height = stats2.at<int>(i, 3);
		rectangle(hand, Point(x, y), Point(x + width, y + height), Scalar(255, 0, 0), 2);
		circle(hand, Point(centroids2.at<double>(i, 0), centroids2.at<double>(i, 1)), 2, Scalar(255, 0, 0), FILLED);
		
	}
	
	
	
	//畫出手指，算出手指長與角度
	finger_hr(stats2,dst,hand);
	

	//畫出每隻手比的數字
	finger_num(stats, centroids2, hand);
	

	
	imshow("original", hand);
	waitKey(0);

	return 0;
}
void finger_num(Mat stats, Mat centroids2, Mat image) {

	vector<int> H_f(stats.rows, 0);
	for (int n = 1; n < centroids2.rows; n++) {
		int x = int(centroids2.at<double>(n, 0));
		int y = int(centroids2.at<double>(n, 1));
		for (int i = 1; i < stats.rows; i++) {
			int a = stats.at<int>(i,0);
			int b = stats.at<int>(i, 1);
			int c = stats.at<int>(i, 0) + stats.at<int>(i, 2);
			int d = stats.at<int>(i, 1) + stats.at<int>(i, 3);
			
			if (x > a && x<c && y>b && y < d)
				H_f[i]++;
		}
	}
	for (int i = 1; i < H_f.size(); i++) {
		int a = stats.at<int>(i, 0);
		int b = stats.at<int>(i, 1);
		int c = stats.at<int>(i, 0) + stats.at<int>(i, 2);
		int d = stats.at<int>(i, 1) + stats.at<int>(i, 3);
		putText(image, to_string(H_f[i]), Point((a + c) / (float)2, (b + d) / (float)2), 0, 1, Scalar(0, 0, 0), 3);
	}

}
void finger_hr(Mat stats2, Mat Gimage, Mat image) {

	vector<vector<int>> long_axis(stats2.rows, vector<int>(4));
	vector<int> long_length(stats2.rows);
	vector<double> long_orientation(stats2.rows);

	for (int i = 1; i < stats2.rows; i++) {
		int s1 = stats2.at<int>(i, 0);
		int s0 = stats2.at<int>(i, 1);
		int s3 = stats2.at<int>(i, 0) + stats2.at<int>(i, 2)-1;
		int s2 = stats2.at<int>(i, 1) + stats2.at<int>(i, 3)-1;

		int axis1 = abs(s2 - s0), axis2 = abs(s3 - s1);

		int a1_1 = 0, a2_1 = 0, b1_1 = 0, b2_1 = 0;
		int a1_2 = 0, a2_2 = 0, b1_2 = 0, b2_2 = 0;
		int back = 0, back2 = 0;
		if (axis1 >= axis2) {
			for (int a = 0; a < axis2; a++) {
				int Istate_1 = Gimage.at<uchar>(s0, s1 + a);
				if (Istate_1 != back) {
					a1_1 = s0;
					a2_1 = s1 + a;
					break;
				}
				else
					back = Istate_1;
			}
			for (int a = 0; a < axis2; a++) {
				int Istate_2 = Gimage.at<uchar>(s2, s1 + a);
				if (Istate_2 != back2) {
					a1_2 = s2;
					a2_2 = s1 + a;
					break;
				}
				else
					back2 = Istate_2;
			}
			for (int a = 0; a < axis2; a++) {
				int Istate_1 = Gimage.at<uchar>(s0, s3 - a);
				if (Istate_1 != back) {
					b1_1 = s0;
					b2_1 = s3 - a;
					break;
				}
				else
					back = Istate_1;
			}
			for (int a = 0; a < axis2; a++) {
				int Istate_2 = Gimage.at<uchar>(s2, s3 - a);
				if (Istate_2 != back2) {
					b1_2 = s2;
					b2_2 = s3 - a;
					break;
				}
				else
					back2 = Istate_2;
			}
		}
		else {
			for (int a = 0; a < axis1; a++) {
				int Istate_1 = Gimage.at<uchar>(s0 + a, s1);

				if (Istate_1 != back) {
					a1_1 = s0 + a;
					a2_1 = s1;
					break;
				}
				else
					back = Istate_1;
			}
			for (int a = 0; a < axis1; a++) {
				int Istate_2 = Gimage.at<uchar>(s0 + a, s3);
				if (Istate_2 != back2) {
					a1_2 = s0 + a;
					a2_2 = s3;
					break;
				}
				else
					back2 = Istate_2;
			}
			for (int a = 0; a < axis1; a++) {
				int Istate_1 = Gimage.at<uchar>(s2 - a, s1);
				if (Istate_1 != back) {
					b1_1 = s2 - a;
					b2_1 = s1;
					break;
				}
				else
					back = Istate_1;
			}
			for (int a = 0; a < axis1; a++) {
				int Istate_2 = Gimage.at<uchar>(s2 - a, s3);
				if (Istate_2 != back2) {
					b1_2 = s2 - a;
					b2_2 = s3;
					break;
				}
				else
					back2 = Istate_2;
			}
		}
		long_axis[i][0] = int((a1_1 + b1_1) / float(2));
		long_axis[i][1] = int((a2_1 + b2_1) / float(2));
		long_axis[i][2] = int((a1_2 + b1_2) / float(2));
		long_axis[i][3] = int((a2_2 + b2_2) / float(2));
		long_length[i] = sqrt(pow(long_axis[i][0] - long_axis[i][2], 2) + pow(long_axis[i][1] - long_axis[i][3], 2));
		int orientation_x = abs(long_axis[i][0] - long_axis[i][2]);
		int orientation_y = abs(long_axis[i][1] - long_axis[i][3]);
		long_orientation[i] = atan(orientation_x / (float)orientation_y) * 180 / PI;
		
		line(image, Point(long_axis[i][1], long_axis[i][0]), Point(long_axis[i][3], long_axis[i][2]), Scalar(0, 255, 0), 1);
		
	}
}