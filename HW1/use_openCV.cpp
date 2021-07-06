#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace cv;
using namespace std;

int main() {

	//第一小題
	Mat image = imread("InputImage1.bmp",1);
	imwrite("InputImage1_out_opencv.bmp", image);

	//第二小題
	Mat image_rotate;
	flip(image, image_rotate, -1);
	imwrite("lena_rotation_opencv.bmp", image_rotate);

	//第三小題
	Mat temp;
	vector<Mat> channels;
	split(image_rotate, channels);
	for (int i = 0; i < image_rotate.rows; i++) {
		for (int j = 0; j < image_rotate.cols; j++) {
			temp = channels.at(0);
			channels.at(0) = channels.at(1);
			channels.at(1) = channels.at(2);
			channels.at(2) = temp;
		}
	}
	Mat mergedst;
	Mat mergesrc[3] = { channels[0] , channels[1] , channels[2] };
	merge(mergesrc,3, mergedst);
	imwrite("image_channel_interchange_opencv.bmp", mergedst);
	
	//bouns
	Mat lena1024_rotate;
	Mat lena1024 = imread("lena1024.bmp");
	flip(lena1024, lena1024_rotate, -1);
	imwrite("lena1024_rotation_opencv.bmp", lena1024_rotate);

	Mat lenaCropped_rotate;
	Mat lena_cropped = imread("lena_cropped.bmp");
	flip(lena_cropped, lenaCropped_rotate, -1);
	imwrite("lenaCropped_rotation_opencv.bmp", lenaCropped_rotate);

	system("pause");
	return 0;
}