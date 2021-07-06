#include <cstdio>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking/tracking.hpp>
#include<fstream>
#include<string>
#include<sstream>
#include<math.h>
#include <time.h>
#include <windows.h>
#include <stdio.h>
using namespace cv;
using namespace std;

double IoU_score2(Rect2d, Rect2d);

int main() {

	Ptr<Tracker> tracker;
	//trackerType: Boosting, MIL, KCF, TLD, MedianFlow, GOTURN, MOSSE, CSRT
	tracker = TrackerCSRT::create(); //選擇追蹤方式

	//讀ground Truth的txt檔案
	ifstream inf;
	inf.open("GroundTruth_2.txt");
	string sline;//每一行
	string out;
	string s1, s2, s3, s4;

	VideoCapture video("2.avi");

	getline(inf, sline); //讀掉第一排的標題
	getline(inf, sline); //讀取初始位置的x,y,w,h ground Truth的第一幀為初始位置
	istringstream sin(sline); //以空格為間隔讀取
	sin >> s1 >> s2 >> s3 >> s4; //讀出四個ground Truth
	Rect2d bbox(atoi(s1.c_str()), atoi(s2.c_str()), atoi(s3.c_str()), atoi(s4.c_str()));


	if (!video.isOpened()) {
		return -1;
	}
	Size videoSize = Size((int)video.get(CAP_PROP_FRAME_WIDTH), (int)video.get(CAP_PROP_FRAME_HEIGHT));

	Mat videoFrame;

	video >> videoFrame;
	//bbox = selectROI("video demo", videoFrame);//人工指定bounding box

	tracker->init(videoFrame, bbox); //初始化第一次追蹤
	printf("開始進行追蹤，按ESC結束程序\n");

	//VideoWriter writer;
	//writer.open("output_bonus.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, videoSize);
	double score; //暫存IoU
	double t_score = 0; //加總IoU
	int cap_num = 0; //計算總幀數
	int cap_num_no50 = 0; //計算IoU>50的幀數
	vector<double> IoU_all(1000); //存取每次的IoU
	clock_t t1, t2;
	double t_total = 0;//加總運算時間
	while (true) {
		t1 = clock();
		video >> videoFrame;

		if (videoFrame.empty()) {
			break;
		}
		tracker->update(videoFrame, bbox);//更新追蹤的bounding box
		rectangle(videoFrame, bbox, Scalar(0, 0, 255), 2, 1);//畫預測bounding box

		getline(inf, sline);
		istringstream sin(sline);
		sin >> s1 >> s2 >> s3 >> s4;
		//cout << s1 << " " << s2 << " " << s3 << " " << s4 << " " << "\n";
		Rect2d Gt(atoi(s1.c_str()), atoi(s2.c_str()), atoi(s3.c_str()), atoi(s4.c_str())); //Ground Truth
		rectangle(videoFrame, Gt, Scalar(255, 0, 0), 2, 1);//畫Ground Truth bounding box
		score = IoU_score2(bbox, Gt);
		if (score >= 0.5) {
			cap_num_no50++;
		}
		else
			printf("[50 down]");
		IoU_all[cap_num] = score;
		t_score += score;
		cap_num++;
		putText(videoFrame, '#' + to_string(cap_num + 1) + ':' + to_string(score), Point(10, 50), FONT_HERSHEY_COMPLEX, 1.5, cv::Scalar(255, 0, 255), 2, 8, 0);
		printf("#%d:%f\n", cap_num + 1, score);
		//if (cap_num % 90 == 0)
		//	imwrite("frame" + to_string(cap_num) + ".png", videoFrame);
		//writer.write(videoFrame);
		imshow("video demo", videoFrame);
		t2 = clock();
		t_total += (t2 - t1) / (double)(CLOCKS_PER_SEC);
		if (waitKey(30) == 27)
		{
			break;
		}
	}

	double av_score = t_score / (double)cap_num;
	printf("正確率:%f", 100 * cap_num_no50 / (double)cap_num);
	printf("平均IoU:%f\n", av_score);
	printf("平均執行時間:%f", t_total / (double)cap_num);

	//計數IoU
	vector<int> overlap(1000, 0);
	for (int k = 0; k < cap_num; k++) {
		int range = IoU_all[k] * 1000;
		//printf("%d,",range);
		overlap[range]++;
	}
	//CDF
	vector<double> overlap_cdf(1000, 0);
	for (int k = 0; k < 1000; k++) {
		for (int j = 0; j <= k; j++) {
			overlap_cdf[k] += overlap[j] / (double)cap_num;
		}
	}
	//畫CDF
	int CDF_H = 400;//縱軸的高度
	Mat cdfimage = Mat::zeros(CDF_H, 1000, CV_8UC1);
	for (int i = 0; i < 1000; i++)
		line(cdfimage, Point(i, CDF_H - 1), Point(i, (CDF_H - 1) - round(overlap_cdf[i] * (CDF_H - 1))), Scalar(255));

	imshow("CDF", cdfimage);
	waitKey(0);

	return 0;
}
double IoU_score2(Rect2d Pr, Rect2d Gt) {
	double Pr_area = Pr.width * Pr.height;
	double Gt_area = Gt.width * Gt.height;

	int w = max(min(Pr.x + Pr.width, Gt.x + Gt.width) - max(Pr.x, Gt.x), 0.0);
	int h = max(min(Pr.y + Pr.height, Gt.y + Gt.height) - max(Pr.y, Gt.y), 0.0);

	double Mix_area = w * h;

	double score = Mix_area / (double)(Pr_area + Gt_area - Mix_area);

	return score;
}