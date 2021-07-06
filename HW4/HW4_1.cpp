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

double IoU_score(Rect2d, Rect2d);

int main() {

	Ptr<Tracker> tracker;
	//trackerType: Boosting, MIL, KCF, TLD, MedianFlow, GOTURN, MOSSE, CSRT
	tracker = TrackerMedianFlow::create(); //��ܰl�ܤ覡

	ifstream inf;
	inf.open("GroundTruth_1.txt");
	string sline;//�C�@��
	string out;
	string s1, s2, s3, s4;

	VideoCapture video("1.avi");
	
	getline(inf, sline); //Ū���Ĥ@�ƪ����D
	getline(inf, sline); //Ū����l��m��x,y,w,h
	istringstream sin(sline); //�H�Ů欰���jŪ��
	sin >> s1 >> s2 >> s3 >> s4;
	Rect2d bbox(atoi(s1.c_str()), atoi(s2.c_str()), atoi(s3.c_str()), atoi(s4.c_str()));


	if (!video.isOpened()) {
		return -1;
	}
	Size videoSize = Size((int)video.get(CAP_PROP_FRAME_WIDTH), (int)video.get(CAP_PROP_FRAME_HEIGHT));

	Mat videoFrame;

	video >> videoFrame;
	//bbox = selectROI("video demo", videoFrame);//�H�u���wbounding box

	

	tracker->init(videoFrame, bbox); //��l�ƲĤ@���l��
	printf("�}�l�i��l�ܡA��ESC�����{��\n");

	//VideoWriter writer;
	//writer.open("output1.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, videoSize);
	double score;
	double t_score = 0;
	int cap_num = 0;
	int cap_num_no50 = 0;
	vector<double> IoU_all(1000);
	clock_t t1, t2;
	double t_total = 0;
	while (true) {
		t1 = clock();
		video >> videoFrame;

		if (videoFrame.empty()) {
			break;
		}
		tracker->update(videoFrame, bbox);//��s�l��
		rectangle(videoFrame, bbox, Scalar(0, 0, 255), 2, 1);//�ebounding box
		
		getline(inf, sline);
		istringstream sin(sline);
		sin >> s1 >> s2 >> s3 >> s4;
		//cout << s1 << " " << s2 << " " << s3 << " " << s4 << " " << "\n";
		Rect2d Gt(atoi(s1.c_str()), atoi(s2.c_str()), atoi(s3.c_str()), atoi(s4.c_str()));
		rectangle(videoFrame, Gt, Scalar(255, 0, 0), 2, 1);//�ebounding box
		score = IoU_score(bbox, Gt);
		if (score >= 0.5) {
			cap_num_no50++;
		}
		else
			printf("[50 down]");
		IoU_all[cap_num] = score;
		t_score += score;
		cap_num++;
		putText(videoFrame,'#'+to_string(cap_num+1)+':'+to_string(score),Point(10, 50), FONT_HERSHEY_COMPLEX, 1.5, cv::Scalar(255,0,255), 2 ,8, 0);
		printf("#%d:%f\n", cap_num+1,score);
		
		//writer.write(videoFrame);
		imshow("video demo", videoFrame);
		/*if (cap_num % 90 == 0)
			imwrite("frame"+ to_string(cap_num)+".png", videoFrame);*/
		t2 = clock();
		t_total += (t2 - t1) / (double)(CLOCKS_PER_SEC);
		if (waitKey(30) == 27)
		{
			break;
		}
	}
	
	double av_score = t_score / (double)cap_num;
	printf("���T�v:%f", 100 * cap_num_no50 / (double)cap_num);
	printf("����IoU:%f\n", av_score);
	printf("��������ɶ�:%f", t_total / (double)cap_num);

	vector<int> overlap(1000,0);
	for (int k = 0; k < cap_num; k++) {
		int range = IoU_all[k]*1000;
		//printf("%d,",range);
		overlap[range]++;
	}
	vector<double> overlap_cdf(1000,0);
	for (int k = 0; k < 1000; k++) {
		for (int j = 0; j <= k; j++) {
			overlap_cdf[k] += overlap[j] / (double)cap_num;
		}
	}
	int CDF_H = 400;//�a�b������
	Mat cdfimage = Mat::zeros(CDF_H, 1000, CV_8UC1);
	for (int i = 0; i < 1000; i++)
		line(cdfimage, Point(i, CDF_H - 1), Point(i, (CDF_H - 1) - round(overlap_cdf[i] * (CDF_H - 1))), Scalar(255));
	
	imshow("CDF", cdfimage);
	//imwrite("cdf_1avi.png", cdfimage);
	waitKey(0);

	return 0;
}
double IoU_score(Rect2d Pr, Rect2d Gt) {
	double Pr_area = Pr.width * Pr.height;
	double Gt_area = Gt.width * Gt.height;

	int w = max(min(Pr.x + Pr.width, Gt.x + Gt.width)-max(Pr.x,Gt.x),0.0);
	int h = max(min(Pr.y + Pr.height, Gt.y + Gt.height) - max(Pr.y, Gt.y),0.0);

	double Mix_area = w * h;
	
	double score = Mix_area / (double)(Pr_area + Gt_area - Mix_area);
	
	return score;
}